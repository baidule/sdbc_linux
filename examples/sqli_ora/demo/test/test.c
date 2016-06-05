#define SERVER
#include "kpsrv.h"
static char seatdata[]="2007-05-27|BEP|H6B|H6|2007-05-28 00:10|5|21|12|6|0|0|\"G04\"\\G||1|WEB_JAVA|yulihua|2008-09-02 10:50:16|AAAM3iAAFAAAB81AAO|";

int main(int argc,char *argv[])
{
SEAT_stu seat;
char stmt[4096],tmp[4096];
T_SQL_Connect Sql;
int ret;
DAU seat_DAU;
JSON_OBJECT json;
	if(argc>1) {
		ret=envcfg(argv[1]);
		if(ret) fprintf(stderr,"init %s err=%d\n",argv[1],ret);
	}

	ret=db_open(&Sql);
	if(ret!=0) {
		printf("Open database err=%d,%s\n",Sql.Errno,Sql.ErrMsg);
		exit(1);
	}

	DAU_init(&seat_DAU,&Sql,0,&seat,SEAT_tpl);
	printf("\nseatdata=%s\n",seatdata);
	net_dispack(&seat,seatdata,SEAT_tpl);
	seat.flag=2;

	json=json_object_new_object();
	DAU_toJSON(&seat_DAU,json,0);
	printf("json_string=%s\n",json_object_to_json_string(json));
	json_object_put(json);

	ret=DAU_insert(&seat_DAU,stmt);
	printf("DAU_insert %d,stmt=%s\n",ret,stmt);
	if(ret) printf("err=%d,%s\n",Sql.Errno,Sql.ErrMsg);
	ret=DAU_print_bind(&seat_DAU,tmp);
	printf("print_bind=%02X,bind=%s\n",ret,tmp);

	___SQL_Transaction__(&Sql,TRANROLLBACK);
	___SQL_CloseDatabase__(&Sql);
	return 0;	
}

