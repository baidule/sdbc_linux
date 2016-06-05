#include <stdio.h>
#define SERVER
//#include <sdbc.h>
#include <DAU_utility.h>
#include <ctype.h>

#define trans_begin(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANBEGIN)
#define trans_rollback(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANROLLBACK)
#define trans_commit(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANCOMMIT)

static int mkwhere(SRM *srmp,char *where)
{
char *pks,*p,*p1;
char kbuf[31];
int i;
T_PkgType Char_Type[2];
        if(!srmp||!where) return 0;
        if(srmp->pkn<=0 || !srmp->pks) {
		return 0;
	}
	*where=0;
        pks=strdup(srmp->pks);
        p=pks;
        Char_Type[0].type=CH_CHAR;
        Char_Type[0].len=-1;
        Char_Type[0].offset=0;
        Char_Type[1].type=-1;
        Char_Type[1].len=0;
        p1=where;
        p1+=sprintf(p1,"WHERE ");
        for(i=0;i<srmp->pkn;i++) {
                if(i>0) p1+=sprintf(p1," AND ");
                p+=net_dispack(kbuf,p,Char_Type);
                p1+=SRM_mkcond(srmp,p1,kbuf,"=");
        }
        free(pks);
//ShowLog(5,"mkwere:%s",where);
        return strlen(where);
}


static int updaterec(DAU *DP,char *buf)
{
int ret;

	ret=mkwhere(&DP->srm,buf);
	if(!ret)  return -1;

	ret=DAU_update(DP,buf);
	if(ret<=0) {
		ShowLog(1,"updaterec:%s",buf);
	}
	return ret;
}

int loadfile(T_SQL_Connect *SQL_Connect,char *tabname,FILE *ifd,FILE *ofd,int Pflg,char *buf,int buflen)
{
char *p,tabn[512];
DAU _DAU;
int rows,ret;
int upd,loss;

	ShowLog(5,"loadfile:entry");
	if(tabname) {
		ret=DAU_init(&_DAU,SQL_Connect,tabname,0,0);
		if(ret) {
			ShowLog(1,"loadfile:DAU_init tabname=%s,ret=%d",tabname,ret);
			return -1;
		}
	}
	upd=loss=0;
	for(rows=0;!ferror(ifd);rows++) {
		fgets(buf,buflen,ifd);
		if(feof(ifd)) break;
		TRIM(buf);
		if(!*buf) {
			rows--;
			continue;
		}
		if(rows==0 && !tabname) {
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
			continue;
		}
		DAU_dispack(&_DAU,buf);
//		ret=mkwhere(&_DAU.srm,buf);
//		ret=DAU_select(&_DAU,buf,1);
		ret=DAU_insert(&_DAU,buf);
		if(ret) {
			if(SQL_Connect->Errno == DUPKEY) {
				if(!Pflg) {//如果没有-P选项
					ret=updaterec(&_DAU,buf);
					if(ret==1) upd+=ret;
					else loss++;
				} 
				else loss++;
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

	tzset();
	sprintf(my_showid,"%s:%d",
		argv[0],getpid());
	Showid=my_showid;
	ifd=0;
	ofd=0;

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
					
			default:
				fprintf(stderr,"no know option:%s",argv[i]);
 				fprintf(stderr,"Usage:%s -f 配置文件 [-P]  输出文件名 ",
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
	ret=loadfile(&SQL_Connect,tabname,ifd,ofd,Pflg,buf,sizeof(buf));
	ShowLog(2,"loadasc:load %d rec's time=%d,buf=%s",ret,(int)(now_sec()-now),buf);
	trans_commit(&SQL_Connect);

	if(ofd && ofd != stdout) fclose(ofd);
	ret=___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
