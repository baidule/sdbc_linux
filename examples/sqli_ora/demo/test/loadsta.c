/********************************************
 * 加载TRS的车站文件 
 ********************************************/

#include <stdio.h>
#define SERVER
#include <kpapp.h>
#include <libgen.h>


T_PkgType  T_sta_tpl[]={
	{CH_CHAR,4,"station_code",0,-1},
	{CH_CHAR,11,"station_name"},
	{CH_INT,sizeof(int),"statis_code"},
	{CH_CHAR,4,"spell"},
	{CH_CHAR,4,"sta_code"},
	{CH_CHAR,2,"JM"}, // T=停靠，B=不停 
	{CH_DATE,9,"beg_date","YYYYMMDD"},
	{CH_DATE,9,"end_date","YYYYMMDD"},
	{-1,0,0,0}
};

typedef struct {
	char station_code[4];
	char station_name[11];
	int statis_code;
	char spell[4];
	char sta_code[4];
	char JM[2];
	char beg_date[9];
	char end_date[9];
} T_sta_stu;

int loadfile(T_SQL_Connect *SQL_Connect,char *buf,int buflen)
{
char *p,tabn[512];
DAU _DAU;
int rows,ret;
int upd,loss;
char *tabname;
int num;
FILE *ifd;
SRM srm;
STATION_stu station;
T_sta_stu sta;

	ifd=fopen("DF02.bcp","r");	//车站表   
	if(!ifd) {
		perror("DF02.bcp");
		return -1;
	}
ShowLog(5,"loadfile:entry,DF02.bcp opened");
	num=0;
	upd=loss=0;
	SRM_init(&srm,&sta,T_sta_tpl);
	DAU_init(&_DAU,SQL_Connect,0,&station,STATION_tpl);
	data_init(&station,STATION_tpl);
	for(rows=0;!ferror(ifd);rows++) {
		fgets(buf,buflen,ifd);
		if(feof(ifd)) break;
		TRIM(buf);
		if(!*buf) {
			rows--;
			continue;
		}
		if(!(++num %10000)) {
			trans_commit(SQL_Connect);
			ShowLog(5,"loadfile:num=%d,rows=%d",num,rows);
		}
		SRM_pkg_dispack(&srm,buf,'\t');
		strlower(sta.spell);
		SRM_copy(&_DAU.srm,&srm,"station_code,station_name,spell,statis_code");
		station.station_type[2]=0;
		station.beg_date=rstrfmttojul(sta.beg_date,"YYYYMMDD");
		station.end_date=rstrfmttojul(sta.end_date,"YYYYMMDD");
		ret=DAU_insert(&_DAU,buf);
		if(ret) {
			if(SQL_Connect->Errno==DUPKEY) {
				*buf=0;
				ret=update_by_PK(&_DAU,buf);
				if(ret==1) upd++;
				else {
					ShowLog(1,"update err=%d,%s",
						SQL_Connect->Errno,
						SQL_Connect->ErrMsg
					);
					loss++;
				}
			} else {
				ShowLog(1,"insert:err=%d,%s,buf=%s",
					SQL_Connect->Errno,
					SQL_Connect->ErrMsg,
					buf);
				loss++;
			}
			rows--;
		} 
	}
	trans_commit(SQL_Connect);
	DAU_free(&_DAU);
	fclose(ifd);
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
INT64 now;
char buf[4096];//最大数据长度有限
char *myname;

	tzset();
	myname=sc_basename(argv[0]);
	sprintf(my_showid,"%s:%d",
		myname,getpid());
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
					
			default:
				fprintf(stderr,"no know option:%s",argv[i]);
 				fprintf(stderr,"Usage:%s -f 配置文件 [-P] 输出文件名 [-W]WHERE 子句 [-D]执行日期 [-d]作业日期\n",
					argv[0]);
				continue;
			}
		}
		tabname=argv[i];
	}
	Showid=my_showid;
	
	ret=db_open(&SQL_Connect);
	if(ret) {
		ShowLog(1,"Open Database err=%d.%s",
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		return 1;
	}
ShowLog(5,"DB=%s",SQL_Connect.DBOWN);
	
	ret=loadfile(&SQL_Connect,buf,sizeof(buf));

	ret=___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
