/***********************************************
 * 加载TRS的车次表  
 ***********************************************/
#include <stdio.h>
#define SERVER
#include <kpapp.h>
#include <libgen.h>

T_PkgType  T_train_tpl[]={
	{CH_CHAR,13,"train_no",0,-1},
	{CH_CHAR,7,"run_train_no"},
	{CH_CHAR,11,"start_station_name"},
	{CH_CHAR,11,"end_station_name"},
	{CH_SHORT,sizeof(short),"a"},
	{CH_SHORT,sizeof(short),"b"},
	{CH_SHORT,sizeof(short),"c"},
	{CH_SHORT,sizeof(short),"d"},
	{CH_DATE,9,"beg_date","YYYYMMDD"},
	{CH_DATE,9,"end_date","YYYYMMDD"},
	{-1,0,0,0}
};

typedef struct {
	char train_no[13];
	char run_train_no[7];
	char start_station_name[11];
	char end_station_name[11];
	short a;
	short b;
	short c;
	short d;
	char beg_date[9];
	char end_date[9];
} T_train_stu;

T_PkgType  T_sta_tpl[]={
	{CH_CHAR,4,"station_code",0,-1},
	{CH_CHAR,11,"station_name"},
	{CH_INT,sizeof(int),"statis_code"},
	{CH_CHAR,4,"spell"},
	{CH_CHAR,4,"sta_code"},
	{CH_CHAR,2,"JM"},
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

int get_station(DAU *DP,char *stmt)
{
int ret;
char *p;
	if(DP->cursor<0) {
		p=DAU_mk_where(DP,"station_name",stmt);
		if(!*stmt) {
			ShowLog(1,"get_station:KEY station_name error");
			return -1;
		}
	}
	ret=DAU_prepare(DP,stmt);
	if(ret) {
		ShowLog(1,"get_station:prepare err=%d,%s,stmt=%s",
				DP->SQL_Connect->Errno,
                                DP->SQL_Connect->ErrMsg, stmt);
		return -2;
	}
	return DAU_next(DP);
}

int loadfile(T_SQL_Connect *SQL_Connect,char *buf,int buflen)
{
char *p,tabn[512];
DAU _DAU,station_DAU;
int rows,ret;
int upd,loss;
char *tabname;
int num;
FILE *ifd;
SRM srm;
STATION_stu station;
T_sta_stu sta;
T_train_stu tra;
TRAIN_stu train;

	ifd=fopen("TC_TrainDir.bcp","r");	//车次表   
	if(!ifd) {
		perror("TC_TrainDir.bcp");
		return -1;
	}
ShowLog(5,"loadfile:entry,TC_TrainDir.bcp opened");
	num=0;
	upd=loss=0;
	SRM_init(&srm,&tra,T_train_tpl);
	DAU_init(&_DAU,SQL_Connect,0,&train,TRAIN_tpl);
	DAU_init(&station_DAU,SQL_Connect,0,&station,STATION_tpl);
	data_init(&train,TRAIN_tpl);
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

		strcpy(train.train_no,tra.train_no);
		train.beg_date=rstrfmttojul(tra.beg_date,"YYYYMMDD");
		train.end_date=rstrfmttojul(tra.end_date,"YYYYMMDD");

/*
		strcpy(station.station_name,tra.start_station_name);
		ret=get_station(&station_DAU,buf);
		if(!ret) strcpy(train.start_station,station.station_code);

		strcpy(station.station_name,tra.end_station_name);
		ret=get_station(&station_DAU,buf);
		if(!ret) strcpy(train.end_station,station.station_code);
*/
		stptok(tra.start_station_name,train.start_station,sizeof(train.start_station),0);
		stptok(tra.end_station_name,train.end_station,sizeof(train.end_station),0);

		train.start_time=0;
		train.station_num=0;
		strcpy(train.cycle,"1");
		train.iterms=1;
		
		ret=DAU_insert(&_DAU,buf);
		if(ret) {
			if(SQL_Connect->Errno==DUPKEY) {
				*buf=0;
				ret=update_by_PK(&_DAU,buf);
				if(ret==1) upd++;
				else {
					ShowLog(1,"update err=%s",buf);
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
	DAU_free(&station_DAU);
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
	Showid=my_showid;
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
