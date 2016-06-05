#include <stdio.h>
#include <DAU.h>
#include <libgen.h>
#include <sys/time.h>
long interval(struct timeval *begtime,struct timeval *endtime)
{
long ret;
        ret=endtime->tv_sec-begtime->tv_sec;
        ret*=1000000;
        ret += endtime->tv_usec - begtime->tv_usec;
        return ret;
}


#define trans_begin(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANBEGIN)
#define trans_rollback(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANROLLBACK)
#define trans_commit(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANCOMMIT)

int unloadfile(T_SQL_Connect *SQL_Connect,char *tablename,FILE *ifd,FILE *ofd,int Pflg,char *where,int buflen)
{
char *p;
DAU _DAU;
int num,ret,cur;
struct timeval begtime,endtime;

	ShowLog(5,"unloadfile:entry");
	ret=DAU_init(&_DAU,SQL_Connect,tablename,0,0);
	if(ret) {
		ShowLog(1,"unloadfile:DAU_init tabname=%s,ret=%d",tablename,ret);
		return -1;
	}
	ret=DAU_prepare(&_DAU,where);
//	sqlo_set_prefetch_rows(_DAU.cursor,1000);
	ShowLog(5,"unldfile:ret=%d,stmt=%s",ret,where);
	if(ret) {
		ShowLog(1,"unloadfile:err=%d,%s",ret,where);
		return ret;
	}
	cur=_DAU.cursor;
	ret=1;
	num=0;
	gettimeofday(&begtime,0);
	while(p=___SQL_Fetch(SQL_Connect,cur,&ret)) {
//		JSON_OBJECT json=json_object_new_object();
//		net_dispack(_DAU.srm.rec,p,_DAU.srm.tp);
//		net_pack(where,_DAU.srm.rec,_DAU.srm.tp);
		fputs(p,ofd);
		fputc('\n',ofd);
		free(p);
//		DAU_toJSON(&_DAU,json,0);
//		p=json_object_to_json_string(json);	
//		json_object_put(json);
		num+=ret;
	}
	gettimeofday(&endtime,0);
	DAU_free(&_DAU);
	ShowLog(2,"unload %s:rows=%d,time=%d",tablename,num,(int)interval(&begtime,&endtime));
	return 0;
}

int loadfile(T_SQL_Connect *SQL_Connect,char *tablename,FILE *ifd,FILE *ofd,int Pflg,char *buf,int buflen)
{
char *p,tabn[512];
DAU _DAU;
int rows,ret;
int upd,loss;
char *tabname;
int num;
struct timeval begtime,endtime;
INT64 now;

	ShowLog(5,"loadfile:entry");
	tabname=tablename;
	if(tabname) {
		ret=DAU_init(&_DAU,SQL_Connect,tabname,0,0);
		if(ret) {
			ShowLog(1,"loadfile:DAU_init tabname=%s,ret=%d",tabname,ret);
			return -1;
		}
	}
	upd=loss=0;
	num=0;
	gettimeofday(&begtime,0);
	for(rows=0;!ferror(ifd);rows++) {
		fgets(buf,buflen,ifd);
		if(feof(ifd)) break;
/*
		TRIM(buf);
		if(!*buf) {
			rows--;
			continue;
		}
		if(!tabname) {
			p=skipblk(buf);
			ret=sscanf(p,"TABNAME=%s",tabn);
			if(ret<1) {
				ShowLog(1,"Can Not find TABLE Name!");
				return FORMATERR;
			}
			ret=DAU_init(&_DAU,SQL_Connect,tabn,0,0);
			if(ret) {
				ShowLog(1,"loadfile:DAU_init tabname=%s",tabn);
				return -1;
			}
			tabname=tabn;
			continue;
		}
*/
		ret=DAU_dispack(&_DAU,buf);
		if(!(++num %10000)) {
			now=now_usec();
			ret=trans_commit(SQL_Connect);
ShowLog(5,"loadsth:%d commit TIMEVAL=%lld,ret=%d",num,now_usec()-now,ret);
		}
		*buf=0;
		ret=bind_ins(&_DAU,buf);
		if(ret) {
			if(SQL_Connect->Errno == DUPKEY) {
				if(!Pflg) {//如果没有-P选项
					*buf=0;
					ret=update_by_PK(&_DAU,buf);
					if(ret>0) {
						upd+=ret;
					}
					else {
						ShowLog(1,"loadfile update:%s",buf);
						loss++;
					}
				} else {//有-P选项，加载失败写入指定文件
					DAU_pack(&_DAU,buf);
					fprintf(ofd,"%s\n",buf);
					ret=dummy_update(&_DAU,buf);
					loss++;
				}
			} else {
				DAU_pack(&_DAU,buf);
				fprintf(ofd,"%s\n",buf);
				ShowLog(1,"loadfile:%s",buf);
				loss++;
			}
			rows--;
			continue;
		}
	}
	gettimeofday(&endtime,0);
now=now_usec();
	DAU_free(&_DAU);
	
	ShowLog(2,"loadfile:rows=%d,upd=%d,loss=%ld,TIMEVAL=%ld DAU_Free TIME=%lld",rows,upd,loss,interval(&begtime,&endtime),now_usec()-now);
	return rows;
}

static char my_showid[200];
main(int argc,char *argv[])
{
int ret,i;
T_SQL_Connect SQL_Connect;
int Pflg=0;
char *tabname=0;
FILE *ifd,*ofd;
INT64 now;
char buf[4096];//最大数据长度有限
char *myname;
struct timeval begtime,endtime;
pthread_t trans_tid=0;

	tzset();
	col_to_lower=1;
	myname=sc_basename(argv[0]);
	sprintf(my_showid,"%s:%d",
		myname,getpid());
	Showid=my_showid;
	ifd=0;
	ofd=0;
	*buf=0;

/*******************************************************************
 *  get Opt
 *******************************************************************/
	for(i=1;i<argc;i++) {
		if(*argv[i]=='-') {
			switch(argv[i][1]) {
			case 'f':
				if(argv[i][2]) ret=envcfg(argv[i]+2);
				else {
					i++;
					ret=envcfg(argv[i]);
				}
				continue;
			case 'P':  //输出 不能加载的记录
				Pflg=1;
				if(argv[i][2]) {
					ofd=fopen(argv[i]+2,"w");
				} else {
					i++;
					if(i<argc) ofd=fopen(argv[i],"w");
				}
				continue;
			case 'W': // WHERE子句
				if(argv[i][2]) {
					strcpy(buf,argv[i]+2);
				} else {
					i++;
					if(i<argc) strcpy(buf,argv[i]);
				}
				continue;
					
			default:
				fprintf(stderr,"no know option:%s",argv[i]);
 				fprintf(stderr,"Usage:%s -f 配置文件 [-P] 输出文件名 [-W]WHERE 子句 [-D]执行日期 [-d]作业日期\n",
					argv[0]);
				continue;
			}
		}
		tabname=argv[i];
	}
	
ShowLog(5,"tabname=%s",tabname);
	ret=db_open(&SQL_Connect);
	if(ret) {
		ShowLog(1,"Open Database err=%d.%s",
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		return 1;
	}
	SQL_Connect.tid=&trans_tid;

ShowLog(5,"DB=%s",SQL_Connect.DBOWN);
	if(!ofd) ofd=stdout;
	ifd=stdin;
	
//system("ps -efl |grep loadsth");
	now=now_usec();
	if(strncmp(myname,"un",2)) {
int cc;
		ret=loadfile(&SQL_Connect,tabname,ifd,ofd,Pflg,buf,sizeof(buf));
		ShowLog(2,"loadasc:load %d rec's TIMEVAL=%d,buf=%s",ret,(int)(now_usec()-now),buf);
now=now_usec();
		cc=trans_commit(&SQL_Connect);
ShowLog(5,"loadfile:%d commit TIMEVAL=%lld,ret=%d",ret,now_usec()-now,cc);
	} else {
		ret=unloadfile(&SQL_Connect,tabname,ifd,ofd,Pflg,buf,sizeof(buf));
ShowLog(5,"unloadfile: %d rec's  TIMEVAL=%lld",ret,now_usec()-now);
	}

//system("ps -efl |grep loadsth");
	if(ofd && ofd != stdout) fclose(ofd);
	ret=___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
