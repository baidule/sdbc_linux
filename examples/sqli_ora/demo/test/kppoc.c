//#include <stdio.h>
#include "atmi.h"		/* TUXEDO  Header File */
#include <string.h>
#include <kpapp.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/time.h>

long interval(struct timeval *begtime,struct timeval *endtime)
{
long ret;
        ret=endtime->tv_sec-begtime->tv_sec;
        ret*=1000000;
        ret += endtime->tv_usec - begtime->tv_usec;
        return ret;
}




int poc(int jflg);

#if defined(__STDC__) || defined(__cplusplus)
main(int argc, char *argv[])
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
int ret,i;
int jflg=0;;
char myShowid[200];
struct timeval begtime,endtime;

	if(argc > 1) {
		for(i=1;i<argc;i++) {
			if(argv[i][0]=='-') {
				switch(argv[i][1]) {
				case 'j': //交班
					jflg=1;
					break;
				default:
					break;
				}
			} else {
			ret=envcfg(argv[1]);
			//fprintf(stderr,"envcfg %s ret=%d\n",argv[1],ret);
			}
		}
	}
	sprintf(myShowid,"%s:%d",argv[0],getpid());
	Showid=myShowid;
//POC begin
	ret=poc(jflg);
	return(0);
}

T_PkgType log_type[]={
	{CH_CHAR,33,"devid",0,-1},
	{CH_CHAR,33,"devname"},
	{CH_CHAR,33,"userid"},
	{CH_CHAR,33,"username"},
	{CH_CHAR,81,"DBOWN"},
	{CH_CHAR,33,"bc"},
	{CH_CHAR,9,"bill_num"},
	{CH_INT,sizeof(int),"timezone"},
	{CH_DATE,YEAR_TO_SEC_LEN,"logtime",YEAR_TO_SEC},
	{-1,0,0}
};
typedef struct {
	char devid[33];
	char devname[33];
	char userid[33];
	char username[33];
	char DBOWN[81];
	char bc[33];
	char bill_num[9];	//当前票号
	int timezone;
	char logtime[YEAR_TO_SEC_LEN];
} log_s;


int poc(int jflg)
{
int ret,i;
char *sendbuf, *rcvbuf;
long sendlen, rcvlen;
char buf[100];
char tmp[4096];
log_s logrec;
TRAIN_stu train;
STOP_STATION_stu fz,dz;
SEAT_stu seat;
JSON_OBJECT json;
char *p;
STATION_stu starec;
INT64 now;
INT4 today;
int day;
TUXDEV_stu tuxdev;
TUXCONTEX_stu tuxcontex;
struct utsname ubuf;
char myShowid[200];
struct timeval begtime,endtime;

	/* Attach to System/T as a Client Process */
	if (tpinit((TPINIT *) NULL) == -1) {
		ShowLog(1, "Tpinit failed\n");
		return -1;
	}
	today=rtoday();
	
	/* Allocate CARRAY buffers for the request and the reply */
	if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		ShowLog(1,"Error allocating send buffer\n");
		tpterm();
		return -1;
	}
	if((sendbuf = (char *) tpalloc("CARRAY", NULL, 20000)) == NULL) {
		ShowLog(1,"Error allocating send buffer\n");
		tpterm();
		return -1;
	}
        ret=uname(&ubuf);
	*buf=0;
        getlogin_r(buf,sizeof(buf));
	sprintf(sendbuf,"%s||%s,tuxticket|ylh|12345|",getenv("DEVID"),ubuf.nodename); //AAAA
	ShowLog(3,"login:ret=%d,buf=%s",ret,sendbuf);
	ret = tpcall("login", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	
	ShowLog(3,"clilogin:ret=%d,len=%d,buf=%s",ret,rcvlen,rcvbuf);
	if(ret<0) {
		tpfree(rcvbuf);
		tpfree(sendbuf);
		tpterm();
		return -1;
	}
	json = json_tokener_parse(rcvbuf);
	json_to_struct(&logrec,json,log_type);
	json_object_put(json);

	
	/* Request the service seatapply, waiting for a reply */
	
//timer begin
	rcvlen=0;
	day=cvtdate("2009.09.18",today);
	sprintf(sendbuf, "%s|H6|BEP|SJP|10|0|5|",rjultostrfmt(tmp,day+getpid()%10,YEAR_TO_DAY));
	ShowLog(5,"%s",sendbuf);
	sendlen = strlen(sendbuf)+1;
	gettimeofday(&begtime,0);
	ret = tpcall("seat_query", (char *)sendbuf, sendlen, (char **)&rcvbuf, &rcvlen, (long)0);
	gettimeofday(&endtime,0);
    ShowLog(5,"TIMEVAL=#%ld",interval(&begtime,&endtime));

	if(ret == -1) {
  		ShowLog(1,"rcvlen=%ld,receive: %s\n",rcvlen,rcvbuf);
		tpfree(sendbuf);
		tpfree(rcvbuf);
		tpterm();
		return(1);
	} else {
//		ShowLog(5,"ret=%d,rcvlen=%ld,receive: %s\n",ret,rcvlen,rcvbuf);
/* 清空数据记录 */
		data_init(&seat,SEAT_tpl);
		data_init(&train,TRAIN_tpl);
		data_init(&fz,STOP_STATION_tpl);
		data_init(&dz,STOP_STATION_tpl);
/* 返回数据解析成JSON对象 */
		json = json_tokener_parse(rcvbuf);
		if(json) {
		JSON_OBJECT jo1;
/* 子对象拆包到相应的结构 */
		    json_to_struct(&train,json_object_object_get(json,"train"),TRAIN_tpl);
		    json_to_struct(&fz,json_object_object_get(json,"start_station"),STOP_STATION_tpl);
		    json_to_struct(&dz,json_object_object_get(json,"arrive_station"),STOP_STATION_tpl);
/* 组装席位记录 */
		    jo1=json_object_object_get(json,"seat_data");
		    if(jo1) for(i=0;i<json_object_array_length(jo1);i++) {
		        json_to_struct(&seat,json_object_array_get_idx(jo1, i),SEAT_tpl);
			net_pack(tmp,&seat,SEAT_tpl);
			ShowLog(2,"%d#SEAT[%d]=%s",getpid(),i,tmp);
		    } 	
		    else ShowLog(1,"JSON:Can not find seat_data (jo1)!\n");
		    json_object_put(json);
		} else ShowLog(1,"JSON parse ERROR!\n");
	}
	if(jflg) {
		ShowLog(2,"现在交班\n");
		ret = tpcall("shift_out", 0, 0, &rcvbuf, &rcvlen, (long)0);
		ShowLog(3,"shift_out ret=%d\n",ret);
		if(ret) {
			ret = tpcall("logout", 0, 0, &rcvbuf, &rcvlen, (long)0);
			tpfree(sendbuf);
			tpfree(rcvbuf);
			tpterm();
			return(0);
		}
	}
	/* Free Buffers & Detach from System/T */
//	ret = tpcall("logout", 0, 0, &rcvbuf, &rcvlen, (long)0);
//	ShowLog(3,"logout ret=%d\n",ret);
	tpfree(sendbuf);
	tpfree(rcvbuf);
	tpterm();
	return ret;
}
