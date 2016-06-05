/**********************************************************
 * test BYTE type with ORACLE's RAW type 
 **********************************************************/

#include <kpapp.h>
#include <sys/time.h>

#define M_ST_OFF(struct_type, member)    \
    ((void*) &((struct_type*) 0)->member)
T_PkgType TTYPE_tpl[]={
        {CH_TINY,1,"\"id\" id",0,-1},
        {CH_CNUM,22,"\"i64\" i64"},
        {CH_SHORT,sizeof(short),"\"i2\" integer_para"},
        {CH_DOUBLE,sizeof(double),"\"d8\" d8","%14.2lf"},
        {CH_DOUBLE,sizeof(double),"\"d4\" d4","%6.2lf"},
        {CH_USEC,sizeof(INT64),"\"t1\" t1","YYYY-MM-DD HH24:MI:SS.FF6"},
        {CH_BYTE,128,"\"r5\" r5"},
        {CH_DATE,YEAR_TO_SEC_LEN,"\"date1\" date1",YEAR_TO_SEC},
        {-1,0,"TTYPE","id|"}
};

extern T_PkgType TTYPE_tpl[];
typedef struct {
        char id;
        char i64[22];
        short integer_para;
        double d8;
        double d4;
        INT64 t1;
        char r5[128];
        char date1[YEAR_TO_SEC_LEN];
} TTYPE_stu;

char * phex(char *buf,char *bin,int len)
{
char *p=buf,*p1=bin;
int i;
	for(i=0;i<len;i++) p+=sprintf(p,"%02X",255&*p1++);
	return buf;
}


main(int argc,char **argv)
{
TTYPE_stu tt;
DAU tt_DAU;
T_SQL_Connect SQL_Connect;
int ret;
char stmt[4096];
JSON_OBJECT json;
struct timeval ts;
struct tm *timp;
INT64 now;

#ifdef __GNUC__
printf("GNUC!\n");
#else
printf("is't GNUC!\n");
#endif
	data_init(&tt,TTYPE_tpl);
	if(argc>1) ret=envcfg(argv[1]);
	gettimeofday(&ts,0);
	timp=localtime(&ts.tv_sec);
	ret=db_open(&SQL_Connect);
	if(ret) {
		now=now_usec();
		rusecstrfmt(stmt,now,"YYYY-MM-DD HH24:MI\'SS.FF5");
		printf("now=%s,usec=%lld,rstr=%lld\n",stmt,now,rstrusecfmt(stmt,"YYYY-MM-DD HH24:MI\'SS.FF5"));
		printf("%s,dbopen error\n",argv[0]);
		now=now_usec();
		ret=except_col(stmt,TTYPE_tpl,"t1");
		printf("except %d,stmt=%stime=%d\n",ret,stmt,(int)(now_usec() - now));
		now=now_usec();
		for(ret=0;ret<100;ret++) sprintf(stmt,"%lld",now); //lltoStr(now,stmt);
		printf("now=%lld,stmt=%s,time=%d\n",now,stmt,(int)(now_usec() - now));
		ret=-12345;
		itoStr(ret,stmt);
		printf("i=%d,itoStr=%s\n",ret,stmt);
		printf("integer_para offset=%d\n",M_ST_OFF(TTYPE_stu,integer_para));
		return 1;
	}
	DAU_init(&tt_DAU,&SQL_Connect,0,&tt,TTYPE_tpl);
	trans_begin(&SQL_Connect);
	*stmt=0;
	tt.t1=now_usec();
	rsecstrfmt(tt.date1,now_sec(),YEAR_TO_SEC);
	ret=DAU_prepare(&tt_DAU,stmt);
	ShowLog(5,"%s:stmt=%s",__FUNCTION__,stmt);
	if(ret!=0) {
			printf("%s,err=%d,%s\n",stmt,
				SQL_Connect.Errno,
				SQL_Connect.ErrMsg);
	} else {
		while(!DAU_next(&tt_DAU)) {
			DAU_pack(&tt_DAU,stmt);
			printf("%s\n",stmt);
			json=json_object_new_object();
			tt.r5[118]++;
			tt.r5[119]=119;
		//	rusecstrfmt(tt.t1,now_usec(),"YYYY-MM-DD HH24:MI\'SS.FF6");
			*stmt=0;
			ret=update_by_PK(&tt_DAU,stmt);
			if(ret<=0) ShowLog(1,"update_by_PK=%d,err=%d,%s",ret,
								SQL_Connect.Errno,
								SQL_Connect.ErrMsg);
			DAU_toJSON(&tt_DAU,json,0);
			printf("update=%d,%s\n",ret,json_object_to_json_string(json));
			json_object_put(json);
		}
		tt.id++;
		tt.r5[6]=tt.id&255;
		strcpy(tt.i64,"-12345678901234567890");
		tt.integer_para=1000;
		ret=DAU_insert(&tt_DAU,stmt);
		if(ret) ShowLog(1,"stmt=%s",stmt);
	}

	tt.id=1;
	strcpy(stmt,"delete from $DB.ttype where $id=:id");
	ret=DAU_exec(&tt_DAU,stmt);
	printf("DAU_exec ret=%d,stmt=%s,err=%d,%s\n",ret,stmt,
		SQL_Connect.Errno,
		SQL_Connect.ErrMsg);

	DAU_free(&tt_DAU);
	trans_commit(&SQL_Connect);
	___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
