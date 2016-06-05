#include <stdio.h>
#define SERVER
#include <DAU_utility.h>
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

static int mk_select(DAU *DP,char *stmt)
{
int ret;
int bindnum=0;
char *tail;
static int first=0;

	if(!first) {
		DAU_mk_where(DP,DP->srm.pks,stmt);  //生成PK条件
		first=1;
	}
//	ShowLog(5,"mk_select:stmt1=%s",stmt);
/*
	if(DP->srm.rp) {
		free(DP->srm.result);
		DP->srm.rp=0;
	}
	ret=bind_select(DP,stmt,1);
*/
	ret=DAU_prepare(DP,stmt);
	if(!ret) ret=DAU_next(DP);

//ShowLog(5,"mk_select:ret=%d,stmt=%s",ret,stmt);
	return ret;
}

static int updaterec(DAU *DP,char *stmt)
{
static int first=0;
int ret;

	if(stmt && !first) {
		DAU_mk_where(DP,DP->srm.pks,stmt);  //生成PK条件
		first=1;
	}
	ret=DAU_update(DP,stmt);
	return ret;
}
static int deleterec(DAU *DP,char *stmt)
{
static int first=0;
int ret;

	if(stmt && !first) {
		DAU_mk_where(DP,DP->srm.pks,stmt);  //生成PK条件
		first=1;
	}
	ret=DAU_delete(DP,stmt);
	return ret;
}

int unloadfile(T_SQL_Connect *SQL_Connect,char *tablename,FILE *ifd,FILE *ofd,int Pflg,char *where,int buflen)
{
char *p;
DAU _DAU;
int num,ret,cur;

	ShowLog(5,"unloadfile:entry");
	ret=DAU_init(&_DAU,SQL_Connect,tablename,0,0);
	if(ret) {
		ShowLog(1,"unloadfile:DAU_init tabname=%s,ret=%d",tablename,ret);
		return -1;
	}
	ret=DAU_prepare(&_DAU,where);
	if(ret) {
		ShowLog(1,"unloadfile:err=%d,%s",ret,where);
		return ret;
	}
	cur=_DAU.cursor;
	ret=1;
	num=0;
	while(p=___SQL_Fetch(SQL_Connect,cur,&ret)) {
//		JSON_OBJECT json=json_object_new_object();
//		net_dispack(_DAU.srm.rec,p,_DAU.srm.tp);
//		net_pack(where,_DAU.srm.rec,_DAU.srm.tp);
		fprintf(ofd,"%s\n",p);
		free(p);
//		DAU_toJSON(&_DAU,json,0);
//		p=json_object_to_json_string(json);	
//		json_object_put(json);
		num++;
	}
	DAU_free(&_DAU);
	ShowLog(2,"unload %s:rows=%d",tablename,num);
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
	for(rows=0;!ferror(ifd);rows++) {
		fgets(buf,buflen,ifd);
		if(feof(ifd)) break;
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
		if(!(++num %10000)) {
			 gettimeofday(&begtime,0);
			trans_commit(SQL_Connect);
			 gettimeofday(&endtime,0);
ShowLog(5,"loadsth:%d commit TIMEVAL=%ld",num,interval(&begtime,&endtime));
		}
		ret=DAU_dispack(&_DAU,buf);
		p=buf+ret+1;
//ret=mk_select(&_DAU,p);
//ShowLog(5,"loadfile mk_select:ret=%d,%s,msg=%s",ret,p,SQL_Connect->ErrMsg);
//if(ret==0) ShowLog(5,"loadfile:mk_select=%d,rec=%s",ret,_DAU.srm.result);
//ret=deleterec(&_DAU,p);
//ShowLog(5,"loadfile deleterec:ret=%d,%s,msg=%s",ret,p,SQL_Connect->ErrMsg);
//		_DAU.srm.selectExtra="/* +APPEND */";
//		gettimeofday(&begtime,0);
		ret=DAU_insert(&_DAU,buf);
		if(ret) {
			if(SQL_Connect->Errno == DUPKEY) {
				if(!Pflg) {//如果没有-P选项
					ret=updaterec(&_DAU,buf);
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
/*
*/
	}
	DAU_free(&_DAU);
	ShowLog(2,"loadfile:rows=%d,upd=%d,loss=%d",rows,upd,loss);
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

	tzset();
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
ShowLog(5,"DB=%s",SQL_Connect.DBOWN);
	if(!ofd) ofd=stdout;
	ifd=stdin;
	
	now=now_sec();
	if(strncmp(myname,"un",2)) {
		ret=loadfile(&SQL_Connect,tabname,ifd,ofd,Pflg,buf,sizeof(buf));
		ShowLog(2,"loadasc:load %d rec's time=%d,buf=%s",ret,(int)(now_sec()-now),buf);
		gettimeofday(&begtime,0);
		trans_commit(&SQL_Connect);
		gettimeofday(&endtime,0);
ShowLog(5,"loadfile:%d commit TIMEVAL=%ld",ret,interval(&begtime,&endtime));
	} else ret=unloadfile(&SQL_Connect,tabname,ifd,ofd,Pflg,buf,sizeof(buf));

	if(ofd && ofd != stdout) fclose(ofd);
	ret=___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
