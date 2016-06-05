/*********************************************************
 * 自动生成母表 
 *********************************************************/
#include <stdio.h>
#define SERVER
#include <kpapp.h>
#include <libgen.h>


int mkpre(T_SQL_Connect *SQL_Connect,INT4 today,char *buf,int buflen)
{
char *p,tabn[512];
DAU train_DAU,pre_index_DAU,pre_seat_DAU;
int rows,ret;
TRAIN_stu train;
PRE_SEAT_INDEX_stu pre_index;
PRE_SEAT_stu pre_seat;

	rows=0;
	DAU_init(&train_DAU,SQL_Connect,0,&train,TRAIN_tpl);
	data_init(&train,TRAIN_tpl);
	train.end_date=today;
	strcpy(buf,"WHERE flag=0 AND end_date >= :end_date AND beg_date<=:end_date and station_num > 0");
	ret=DAU_select(&train_DAU,buf,0);
	if(ret<=0) {
		ShowLog(1,"mkpre:stmt=%s,err=%d,%s",buf,
			SQL_Connect->Errno,
			SQL_Connect->ErrMsg);
		DAU_free(&train_DAU);
		return -1;
	}
	DAU_init(&pre_index_DAU,SQL_Connect,0,&pre_index,PRE_SEAT_INDEX_tpl);
	DAU_init(&pre_seat_DAU,SQL_Connect,0,&pre_seat,PRE_SEAT_tpl);
	data_init(&pre_index,PRE_SEAT_INDEX_tpl);
	pre_seat.carno=1;
	pre_seat.seat_type=1;
	pre_seat.beg_seatno=1;
	pre_seat.end_seatno=120;
	pre_seat.shortest_station=0;
	pre_seat.purpose=0;
	pre_seat.beg_station=0;
	while(!DAU_next(&train_DAU)) {
		if(strlen(train.train_no) < 10) continue;
		pre_index.beg_date=train.beg_date;
		pre_index.end_date=train.end_date;
		strcpy(pre_index.train_no,train.train_no);
		*pre_index.item='1';
		pre_index.flag=0;
		ret=DAU_insert(&pre_index_DAU,buf);
		if(ret) {
			ShowLog(1,"mkpre:%s,err=%s,%d",buf,
				SQL_Connect->Errno,
				SQL_Connect->ErrMsg);
			continue;
		}
		sprintf(pre_seat.pre_id,"%s:%c:%s",
			pre_index.train_no,
			*pre_index.item,
			rjultostrfmt(buf,pre_index.beg_date,"YYYYMMDD"));
		strcpy(pre_seat.gride,train.gride);
		pre_seat.end_station=train.station_num-1;
		ret=DAU_insert(&pre_seat_DAU,buf);
		if(ret) {
			ShowLog(1,"mkpre:%s,err=%s,%d",buf,
				SQL_Connect->Errno,
				SQL_Connect->ErrMsg);
			trans_rollback(SQL_Connect);
			continue;
		}
		trans_commit(SQL_Connect);
		rows++;
	}
	trans_commit(SQL_Connect);
	DAU_free(&train_DAU);
	DAU_free(&pre_seat_DAU);
	DAU_free(&pre_index_DAU);
	ShowLog(5,"mkpre:rows=%d",rows);
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
INT4 today;

	tzset();
	today=rtoday();
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
	
	ret=mkpre(&SQL_Connect,today,buf,sizeof(buf));

	ret=___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
