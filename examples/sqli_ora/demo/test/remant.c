#define SERVER
#include <kpapp.h>
#include <sys/time.h>
long interval(struct timeval *begtime,struct timeval *endtime)
{
long ret;
        ret=endtime->tv_sec-begtime->tv_sec;
        ret*=1000000;
        ret += endtime->tv_usec - begtime->tv_usec;
        return ret;
}


/*
SELECT	
	to_char(on_date,'YYYY-MM-DD') on_date,
	train_no,
	carno,
	beg_station beg_station,
	b.station_code end_station,
	purpose
	,count(*) quantity
FROM seat a,stop_station b
WHERE 	FLAG=0 
	AND to_char(on_date,'YYYY-MM-DD') = '2009-05-06'
	AND beg_station = 'BEP'
	AND train_no = 'H6'
	AND end_station >= 2
	AND seat_type > 20 AND seat_type <= 29 
	AND purpose=0 
	AND b.train_no = train_no
	AND b.sequence = end_station
	AND b.end_date >= to_date('2009-05-06','YYYY-MM-DD')
group by on_date,train_no,carno,beg_station,b.station_code,purpose;

"2009-05-06|H6B|BEP|6|0|20|
*/

T_PkgType remant_tpl[]={
	{CH_DATE,11,"start_date",YEAR_TO_DAY,-1},
	{CH_CHAR,13,"train_no"},
	{CH_CHAR,5,"beg_station"},
	{CH_SHORT,sizeof(short),"end_station"},
	{CH_INT,sizeof(int),"purpose"},
	{CH_SHORT,sizeof(short),"decode(seat_type, 21,20, 22,20, 23,20, 31,30, 32,30, seat_type) seat_type"},
	{CH_SHORT,sizeof(short),"carno"},
	{CH_SHORT,sizeof(short),"shortest_station"},
	{CH_INT,sizeof(int),"count(*) quantity"},
	{-1,0,"seat",0}
};

typedef struct {
	char start_date[11];
	char train_no[13];
	char beg_station[5];
	short end_station;
	int purpose;
	short seat_type;
	short carno; 
	short shortest_station;
	int quantity;
} remant_stu;
/*
req:
始发日期|全车次|上车站代码|下车站顺号|用途|席别|
*/

int query_remant(T_SQL_Connect *SQL_Connect,char *req)
{
remant_stu remant;
DAU rem_DAU;
char stmt[4096],*p;
int ret;
struct timeval begtime,endtime;
	
	DAU_init(&rem_DAU,SQL_Connect,"SEAT",&remant,remant_tpl);
	DAU_dispack(&rem_DAU,req);
	p=stmt;
	p+=sprintf(p,"WHERE flag=0  AND start_date = :start_date"
		     " AND beg_station = :beg_station AND train_no = :train_no AND end_station >= :end_station "
		     "AND purpose=:purpose AND ");
	if(remant.seat_type % 10) 	//席别
		p+=sprintf(p,"seat_type = :seat_type ");
	else p+=sprintf(p,"seat_type >= :seat_type AND seat_type <= :seat_type+9 ");
	p+=sprintf(p, "GROUP BY  start_date,train_no,beg_station,end_station"
		      ",purpose,decode(seat_type, 21,20, 22,20, 23,20, 31,30, 32,30, seat_type),carno,shortest_station "
		      "ORDER BY end_station ");
                gettimeofday(&begtime,0);
	ret=DAU_select(&rem_DAU,stmt,0);
                gettimeofday(&endtime,0);
printf("query_remant from seat:select ret=%d,stmt=%s\nTIMEVAL=%ld\n",ret,stmt,interval(&begtime,&endtime));

	ShowLog(5,"query_remant:ret=%d,stmt=%s",ret,stmt);
	if(ret>0) {
		while(!DAU_next(&rem_DAU)) {
			DAU_pack(&rem_DAU,stmt);
			printf("%s\n",stmt);
		}
	}
	DAU_free(&rem_DAU);
	return ret;
}
	
int q_remant(T_SQL_Connect *SQL_Connect,char *req)
{
SEAT_REMANT_stu remant;
DAU rem_DAU;
char stmt[4096],*p;
int ret;
struct timeval begtime,endtime;
remant_stu q_remant;

	net_dispack(&q_remant,req,remant_tpl);
	strcpy(remant.on_date,q_remant.start_date);
	strcpy(remant.run_train,q_remant.train_no);
	strcpy(remant.beg_station,q_remant.beg_station);
	remant.end_station=q_remant.end_station;
	remant.purpose=q_remant.purpose;
	switch(q_remant.seat_type) {
	case 21:
	case 22:
	case 23:
		remant.seat_type=20;
		break;
	case 31:
	case 32:
		remant.seat_type=30;
		break;
	default:
		remant.seat_type=q_remant.seat_type;
		break;
	}

	DAU_init(&rem_DAU,SQL_Connect,0,&remant,SEAT_REMANT_tpl);
	p=DAU_mk_where(&rem_DAU,"on_date,run_train,beg_station,purpose",stmt);
	p+=sprintf(p," AND end_station >= :end_station AND ");
	if(remant.seat_type != 10)
			p+=sprintf(p,"seat_type=:seat_type ");
	else	p+=sprintf(p,"seat_type >= :seat_type AND seat_type <= :seat_type + 9");
                gettimeofday(&begtime,0);
	ret=DAU_select(&rem_DAU,stmt,0);
                gettimeofday(&endtime,0);
printf("q_remant:select ret=%d,stmt=%s\nTIMEVAL=%ld\n",ret,stmt,interval(&begtime,&endtime));

	if(ret>0) {
		while(!DAU_next(&rem_DAU)) {
			DAU_pack(&rem_DAU,stmt);
			printf("%s\n",stmt);
		}
	}
	DAU_free(&rem_DAU);
	return ret;
}

/**************************************
 * usage:remant -f config.ini
 **************************************/

int main(int argc,char *argv[])
{
int ret;
T_SQL_Connect SQL_Connect;
char buf[200];

	if(argc > 1) ret=envcfg(argv[1]);

	ret=db_open(&SQL_Connect);
	if(ret) return 1;
/*
req:
始发日期|全车次|上车站代码|下车站顺号|用途|席别|
*/
//	sprintf(buf,"%s|H6B|BEP|5|0|10|",rjultostrfmt(buf,2+rtoday(),"YYYY-MM-DD"));
	sprintf(buf,"2009-09-17|H6B|BEP|5|0|10|");
	ret=query_remant(&SQL_Connect,buf);
//	sprintf(buf,"%s|H6|BEP|5|0|10|",rjultostrfmt(buf,3+rtoday(),"YYYY-MM-DD"));
	sprintf(buf,"2009-09-18|H6|BEP|5|0|10|");
	ret=q_remant(&SQL_Connect,buf);
	___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}

