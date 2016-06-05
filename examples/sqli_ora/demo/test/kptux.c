//#include <stdio.h>
#include "atmi.h"		/* TUXEDO  Header File */
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
char tmp[4096];
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

	
	/* Request the service seatapply, waiting for a reply */
	

//	rjultostrfmt(today,rtoday(),YEAR_TO_DAY);
	set_offset(STATION_tpl); // 模板要处理一下，否则mkcond不能正常工作
	starec.beg_date=starec.end_date=rtoday();

//	sprintf(sendbuf,"0|select /*+rule*/ %s from %s.station where %s and %s and flag=0 ",
//		mkfield(tmp,STATION_tpl,0),
//		logrec.DBOWN,
//		mkcond(tmp+1000,&starec,STATION_tpl,"beg_date","<="),
//		mkcond(tmp+1200,&starec,STATION_tpl,"end_date",">="));
/*

	printf("stmt=%s\n",sendbuf);
	ret = tpcall("tux_select", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	printf("select ret=%d rcvlen=%d,result=\n%s\n",ret,rcvlen,rcvbuf);
	if(!ret) {
		p=rcvbuf;
		while(*p) {
			p+=net_dispack(&starec,p,STATION_tpl);	//解析到数据记录
			json = json_object_new_object();
			struct_to_json(json,&starec,STATION_tpl,0); //转JSON
			printf("station:%s\n",json_object_get_string(json));
			json_object_put(json);		//释放JSON

		}
	}
*/

sprintf(sendbuf,"[{\"values\":[{\"unit\":\"1001\",\"username\":\"huang\",\"flag\":\"0\",\"userid\":\"huang\",\"last_log\":\"2009-01-04 15:02:24\",\"password\":\"\"},{\"unit\":\"1001\",\"username\":\"weiwei\",\"flag\":\"0\",\"userid\":\"weiwei\",\"last_log\":\"2009-02-12 17:21:22\",\"password\":\"\"}],\"flag\":\"0\",\"tablename\":\"TUXUSER\",\"where\":\"PRIMARY_KEY\"}]");
printf("tuxdui:%s\n",sendbuf);
ret=tpcall("tuxdui",(char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
printf("tuxdui:ret=%d rcvlen=%d,result=\n%s\n",ret,rcvlen,rcvbuf);
/*
	sprintf(sendbuf,"0|select %s,%s from %s.tuxdev a,%s.tuxcontex b where a.devid='%s' "
	    "and b.devid = a.devid and b.clid=a.clid",
		mkfield(tmp,TUXDEV_tpl,"a"),
		mkfield(tmp+500,TUXCONTEX_tpl,"b"),
		logrec.DBOWN,logrec.DBOWN,
		logrec.devid);
	printf("stmt=%s\n",sendbuf);
	rcvlen=0;
	ret = tpcall("tux_select", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	printf("select rcvlen=%d,result=\n%s\n",rcvlen,rcvbuf);
	if(!ret) {
		p=rcvbuf;
		while(*p) {
		JSON_OBJECT json1;
			p+=net_dispack(&tuxdev,p,TUXDEV_tpl);	//解析到数据记录
			p+=net_dispack(&tuxcontex,p,TUXCONTEX_tpl);	//解析到数据记录
			json = json_object_new_object();
			json1 = json_object_new_object();
			struct_to_json(json,&tuxdev,TUXDEV_tpl,0); //转JSON
			struct_to_json(json1,&tuxcontex,TUXCONTEX_tpl,0); //转JSON
			printf("tuxdev:%s\n",json_object_get_string(json));
			printf("tuxcontex:%s\n",json_object_get_string(json1));
			json_object_put(json);		//释放JSON
			json_object_put(json1);		//释放JSON
		}
	}
	sprintf(sendbuf,"{tablename:\"station\",where:\"where area=(select area from TICKET.station where station_code=:station_code) and flag=0\",values:{station_code:\"VNP\"}}");
*/
	backjson=json_object_new_object();
	add_string_to_json(backjson,"station_code","HDP");
	add_string_to_json(backjson,"end_date","2009-08-06");
	json=json_object_new_object();
	add_string_to_json(json,"tablename","station");
	add_string_to_json(json,"where",
	 "where area=(select area from TICKET.station where station_code=:station_code  and beg_date<=:end_date and end_date >= :end_date and flag=0) and beg_date<=:end_date and end_date >= :end_date and flag=0");
	json_object_object_add(json,"values",backjson);
	strcpy(sendbuf,json_object_to_json_string(json));
	json_object_put(json);
	
	printf("area stmt=%s\n",sendbuf);
	ret=tpcall("tuxselect", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	printf("area,ret=%d,rcvbuf=%s\n",ret,rcvbuf);
	sprintf(sendbuf, "%s||TXP||",rjultostrfmt(today,day+1,YEAR_TO_DAY));
	printf("train_query:%s\n",sendbuf);
	ret = tpcall("fdzquery", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	ShowLog(2,"train_query:recv=%s,ret=%d,rcvlen=%ld,\n",rcvbuf,ret,rcvlen);
////////
	rjultostrfmt(fdz.on_date,day+2,YEAR_TO_DAY);
	strcpy(fdz.begin,"YIJ");
	strcpy(fdz.arrive,"BJP");
	strcpy(fdz.run_train_no,"K44");
	*fdz.run_train_no=0;
	json=json_object_new_object();
	struct_to_json(json,&fdz,FDZquery_tpl,0);
	strcpy(sendbuf,json_object_to_json_string(json));
	json_object_put(json);
////////	
	printf("fdzquery:%s\n",sendbuf);
	sendlen = strlen(sendbuf)+1;
	gettimeofday(&begtime,0);
	ret = tpcall("fdzquery", (char *)sendbuf, sendlen, (char **)&rcvbuf, &rcvlen, (long)0);
	gettimeofday(&endtime,0);
	printf("fdzquery:recv=%s,ret=%d,rcvlen=%ld,\n",rcvbuf,ret,rcvlen);
	printf("fdzquery:%d time=%d \n",rcvlen,interval(&begtime,&endtime));
	ShowLog(2,"fdzquery result = %s ",rcvbuf);
/*
	buf=malloc(rcvlen+1);
	gettimeofday(&begtime,0);
	ret=memzip(buf,rcvbuf,rcvlen);
	gettimeofday(&endtime,0);
	printf("memzip:%d/%d time=%d\n",ret,rcvlen,interval(&begtime,&endtime));

	gettimeofday(&begtime,0);
	ret=memunzip(buf,ret,rcvbuf,rcvlen);
	gettimeofday(&endtime,0);
	printf("memunzip:%d/%d time=%d\n",ret,rcvlen,interval(&begtime,&endtime));

	free(buf);
*/
	rcvlen=0;
	day=rtoday();
	sprintf(sendbuf, "%s|H6|BEP|SJP|20|0|2|",rjultostrfmt(today,day+1,YEAR_TO_DAY));
	printf("%s\n",sendbuf);
	sendlen = strlen(sendbuf)+1;
	gettimeofday(&begtime,0);
	ret = tpcall("seat_query", (char *)sendbuf, sendlen, (char **)&rcvbuf, &rcvlen, (long)0);
	gettimeofday(&endtime,0);

	if(ret == -1) {
  		printf("rcvlen=%ld,receive: %s\n",rcvlen,rcvbuf);
	} else {
		printf("ret=%d,rcvlen=%ld,receive: %s\n",ret,rcvlen,rcvbuf);
/* 清空数据记录 */
		data_init(&seat,SEAT_tpl);
		data_init(&train,TRAIN_tpl);
		data_init(&fz,STOP_STATION_tpl);
		data_init(&dz,STOP_STATION_tpl);
		backjson=json_object_new_array();
/* 返回数据解析成JSON对象 */
		json = json_tokener_parse(rcvbuf);
		if(json) {
		JSON_OBJECT jo1,jo2;
/* 子对象拆包到相应的结构 */
		    json_to_struct(&train,json_object_object_get(json,"train"),TRAIN_tpl);
net_pack(tmp,&train,TRAIN_tpl);
printf("train=%s\n",tmp);
		    json_to_struct(&fz,json_object_object_get(json,"start_station"),STOP_STATION_tpl);
net_pack(tmp,&fz,STOP_STATION_tpl);
printf("fz=%s\n",tmp);
		    json_to_struct(&dz,json_object_object_get(json,"arrive_station"),STOP_STATION_tpl);
net_pack(tmp,&dz,STOP_STATION_tpl);
printf("dz=%s\n",tmp);
/* 组装席位记录 */
		    jo1=json_object_object_get(json,"seat_data");
		    if(jo1) {
			for(i=0;i<json_object_array_length(jo1);i++) {
		        	json_to_struct(&seat,json_object_array_get_idx(jo1, i),SEAT_tpl);
				net_pack(tmp,&seat,SEAT_tpl);
				printf("seat[%d]=%s\n",i,tmp);
				jo2=json_object_new_object();
				struct_to_json(jo2,&seat,SEAT_tpl,"ROWID");
				json_object_array_add(backjson,jo2);
			}
			strcpy(sendbuf,json_object_to_json_string(backjson));
			json_object_put(backjson);
			ret = tpcall("seat_back", (char *)sendbuf, strlen(sendbuf)+1, (char **)&rcvbuf, &rcvlen, (long)0);
			printf("\nSEAT_BACK:ret=%d,rcvlen=%ld,receive: %s\n",ret,rcvlen,rcvbuf);
		    } 	
		    else printf("JSON:Can not find seat_data (jo1)!\n");
		    json_object_put(json);
		} else printf("JSON parse ERROR!\n");
	printf("seat_query: TIMEVAL=%d\n",interval(&begtime,&endtime));
	ShowLog(5,"notrainquery:TIMEVAL=#%ldus\n",interval(&begtime,&endtime));
		
	}
/*
	sprintf(sendbuf,"0|select %s from %s.seat",mkfield(tmp,SEAT_tpl,0),logrec.DBOWN);
	printf("%s\n",sendbuf);
now=now_sec();
	ret = tpcall("tux_select", (char *)sendbuf, strlen(sendbuf)+1, (char **)&rcvbuf, &rcvlen, (long)0);
	printf("TEST BIGDATA,time=%lld,len=%d,ret=%d,data=%.250s\n",now_sec()-now,rcvlen,ret,rcvbuf);
*/  	
//	ret=chpwd();
/*
	printf("交班吗(Y/N)\n",ret);
	fgets(tmp,5,stdin);
	if(*tmp=='Y') {
*/
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

