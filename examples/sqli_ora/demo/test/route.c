//#include <stdio.h>
#include <atmi.h>		/* TUXEDO  Header File */
#include <string.h>
#include <kpapp.h>
#include <sys/time.h>

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

/*
long interval(struct timeval *begtime,struct timeval *endtime)
{
long ret;
        ret=endtime->tv_sec-begtime->tv_sec;
        ret*=1000000;
        ret += endtime->tv_usec - begtime->tv_usec;
        return ret;
}
*/

extern JSON_OBJECT train_transfer(char * start ,char * end,char *msg);

#if defined(__STDC__) || defined(__cplusplus)
main(int argc, char *argv[])
#else
int main(argc, argv)
int argc;
char *argv[];
#endif
{
char *sendbuf, *rcvbuf;
long sendlen, rcvlen;
int ret,i;
char *buf;
char tmp[4096],msg[1024];
log_s logrec;
TRAIN_stu train;
STOP_STATION_stu fz,dz;
SEAT_stu seat;
JSON_OBJECT json,backjson;
char *p;
STATION_stu starec;
char today[30];
INT64 now;
int day,jflg=0;;
TUXDEV_stu tuxdev;
TUXCONTEX_stu tuxcontex;
FDZquery_stu fdz;
struct timeval begtime,endtime;
	buf=0;
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
			printf("envcfg %s ret=%d\n",argv[1],ret);
			}
		}
	}
	day=rtoday();
	/* Attach to System/T as a Client Process */
	if (tpinit((TPINIT *) NULL) == -1) {
		(void) fprintf(stderr, "Tpinit failed\n");
		exit(1);
	}
	
	/* Allocate CARRAY buffers for the request and the reply */
	if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		(void) fprintf(stderr,"Error allocating send buffer\n");
		tpterm();
		exit(1);
	}
	if((sendbuf = (char *) tpalloc("CARRAY", NULL, 20000)) == NULL) {
		(void) fprintf(stderr,"Error allocating send buffer\n");
		tpterm();
		exit(1);
	}
	ret=clilogin(&buf);
	printf("clilogin:ret=%d,buf=%s\n",ret,buf);
	if(ret<0) {
		ShowLog(1,"clilogin fail %d",ret);
		tpfree(buf),buf=0;
		tpfree(rcvbuf);
		tpfree(sendbuf);
		tpterm();
		exit(1);
	}
	json = json_tokener_parse(buf);
	tpfree(buf),buf=0;
	json_to_struct(&logrec,json,log_type);
	json_object_put(json);
/**************************************/
	json=train_transfer("TJP","VNP",msg);  // transfer zero 
//	json=train_transfer("TXP","SJP",msg);    // transfer once 
//	json=train_transfer("COP","ZJZ",msg);  // transfer twice
	ret=json_object_get_int(json_object_object_get(json,"transfer_count"));
	switch(ret)
	{
		case 0:
			transfer_parse_zero(json);			
			break;
		case 1:
			transfer_parse_one(json);			
			break;
		case 2:
			//break;
		case 3:
			//break;
		case 4:
			//break;
		default:
			ShowLog(5,"%s",json_object_to_json_string(json));
			break;
	
	}
     json_object_put(json);

/*************************************/
	if(jflg) {
		printf("现在交班\n");
		ret = tpcall("shift_out", 0, 0, &rcvbuf, &rcvlen, (long)0);
printf("shift_out ret=%d\n",ret);
		if(ret)
			ret = tpcall("logout", 0, 0, &rcvbuf, &rcvlen, (long)0);
		tpfree(sendbuf);
		tpfree(rcvbuf);
		tpterm();
		return(0);
	}
	/* Free Buffers & Detach from System/T */
	ret = tpcall("logout", 0, 0, &rcvbuf, &rcvlen, (long)0);
printf("logout ret=%d\n",ret);
	tpfree(sendbuf);
	tpfree(rcvbuf);
	tpterm();
	return(0);
}

