/******************************************
 * 加载TRS的停靠站表 
 ******************************************/

#include <stdio.h>
#define SERVER
#include <kpapp.h>
#include <libgen.h>
// TRS的停靠站模板
T_PkgType  T_stop_sta_tpl[]={
	{CH_CHAR,13,"train_no",0,-1},
	{CH_SHORT,sizeof(short),"sequence"},
	{CH_CHAR,4,"station_code"},
	{CH_CHAR,11,"station_name"},
	{CH_CHAR,7,"run_train_no"},
	{CH_SHORT,sizeof(short),"days"},
	{CH_SHORT,sizeof(short),"arrive_time"},
	{CH_SHORT,sizeof(short),"departure_time"},
	{CH_INT,sizeof(int),"distance"},
	{CH_DATE,9,"beg_date","YYYYMMDD"},
	{CH_DATE,9,"end_date","YYYYMMDD"},
	{-1,0,0,0}
};
typedef struct {
	char train_no[13];
	short sequence;
	char station_code[4];
	char station_name[11];
	char run_train_no[7];
	short days;
	short arrive_time;
	short departure_time;
	int distance;
	char beg_date[9];
	char end_date[9];
} T_stop_tra_stu;

// DAO
int get_train(DAU *DP,char *stmt)
{
int ret;
char *p;

	if(DP->cursor<0) {
		p=DAU_mk_where(DP,"train_no",stmt);
		if(*stmt==0) {
			ShowLog(1,"get_train:KEY train_no not exist!");
			return -1;
		}
		p+=sprintf(p," AND BEG_DATE <= :end_date AND END_DATE >= :end_date ");
	}
	ret=DAU_prepare(DP,stmt);
	if(ret) {
		ShowLog(1,"get_train:prepare err=%d,%s,stmt=%s",
			DP->SQL_Connect->Errno,
			DP->SQL_Connect->ErrMsg,
			stmt);
		return -2;
	}
	return DAU_next(DP);
}

int cvt_min(int itm)
{
int i;
	if(itm<1) return 0;
	i=itm/100;
	itm %= 100;
	return i*60+itm;
}

int loadfile(T_SQL_Connect *SQL_Connect,char *buf,int buflen)
{
char *p,tabn[512];
DAU _DAU,train_DAU;
int rows,ret,minut;
int upd,loss;
char *tabname;
int num;
FILE *ifd;
SRM srm;
STATION_stu station;
TRAIN_stu train;
T_stop_tra_stu stop;
STOP_STATION_stu cz;


	ifd=fopen("TC_StopTime.bcp","r");	//停靠站表   
	if(!ifd) {
		perror("TC_StopTime.bcp");
		return -1;
	}
ShowLog(5,"loadfile:entry,TC_StopTime.bcp opened");
	num=0;
	upd=loss=0;
	SRM_init(&srm,&stop,T_stop_sta_tpl);
	DAU_init(&train_DAU,SQL_Connect,0,&train,TRAIN_tpl);
	DAU_init(&_DAU,SQL_Connect,0,&cz,STOP_STATION_tpl);
	data_init(&train,TRAIN_tpl);
	data_init(&cz,STOP_STATION_tpl);
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

		SRM_copy(&_DAU.srm,&srm,0);
		cz.beg_date=rstrfmttojul(stop.beg_date,"YYYYMMDD");
		cz.end_date=rstrfmttojul(stop.end_date,"YYYYMMDD");
		cz.sequence--;
		TRIM(cz.run_train_no);
		if(!*train.train_no || strcmp(train.train_no,cz.train_no)) {
			if(*train.train_no) {
				*buf=0;
				ret=update_by_PK(&train_DAU,buf);
				if(ret<1) {
char b[30],e[30];
				//	ShowLog(1,"loadfile:update train ret=%d,err=%d,%s",ret,
				//	SQL_Connect->Errno,
				//	SQL_Connect->ErrMsg);
					ShowLog(1,"loadfile:update train %s beg_date=%s,end_date=%s",
						stop.train_no,
						stop.beg_date,stop.end_date);
					rjultostrfmt(b,train.beg_date,YEAR_TO_DAY);
					rjultostrfmt(e,train.end_date,YEAR_TO_DAY);
					ShowLog(1,"loadfile:train_no=%s,train.beg_date=%s,end_date=%s",
						train.train_no,b,e);
				}
			}
			train.beg_date=cz.beg_date;
			train.end_date=cz.end_date;
			strcpy(train.train_no,cz.train_no);
			ret=get_train(&train_DAU,buf);
		}
//修理 stop_station的 running_time,stop_time
		if(cz.sequence==0) {
			train.start_time=cvt_min(stop.departure_time);
			cz.running_time=0;
			cz.stop_time=0;
		} else {
			minut=cvt_min(stop.arrive_time);
			cz.running_time=minut+stop.days*1440-train.start_time;
			cz.stop_time=cvt_min(stop.departure_time)-minut;
			if(cz.stop_time<0) cz.stop_time += 1440;
		}

//修理 train 的 station_num,total_distance
		if(train.station_num < stop.sequence) {
			train.station_num=cz.sequence+1;
			train.total_distance=cz.distance;
		}
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
	*buf=0;
	ret=update_by_PK(&train_DAU,buf); //最后的车次记录 
	trans_commit(SQL_Connect);
	DAU_free(&_DAU);
	DAU_free(&train_DAU);
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
