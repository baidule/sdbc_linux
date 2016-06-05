#include <kpapp.h>
#include <sys/time.h>

int unldseat(T_SQL_Connect *SQL_Connect,char *msg)
{
int ret;
DAU seat_DAU;
SEAT_stu seat;
struct timeval begtime,endtime;
T_PkgType minitpl[6];

	patt_copy(minitpl,SEAT_tpl,"start_date,beg_station,train_no");
	ret=DAU_init(&seat_DAU,SQL_Connect,0,&seat,minitpl);
	strcpy(seat.beg_station,"VNP");
	strcpy(seat.start_date,"2009-05-01");
	DAU_mk_where(&seat_DAU,"start_date,beg_station",msg);
    gettimeofday(&begtime,0);
	ret=DAU_select(&seat_DAU,msg,0);
    gettimeofday(&endtime,0);
	ShowLog(5,"unldseat:select %d rec's,TIMEVAL=%ld",ret,interval(&begtime,&endtime));

	if(ret <= 0) {
printf("unldseat:aft getm ret=%d,stmt=%s\n",ret,msg);
			return ret;
	}
	while(!DAU_next(&seat_DAU)) {
	}
	DAU_free(&seat_DAU);
	return 0;
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
			default:
				fprintf(stderr,"no know option:%s",argv[i]);
 				fprintf(stderr,"Usage:%s -f 配置文件 [-P]  输出文件名 ",
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
	
	ret=unldseat(&SQL_Connect,buf);

	ret=___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
