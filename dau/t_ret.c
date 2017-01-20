/**********************************************************
 * test RETURNING
 * usage:
 * $./t_ret sql.cfg
 **********************************************************/

#include <DAU.h>

/*****************************************
create table ttype (
        "id" number(2),
        "i64" number(20),
        "i2" number(4),
        "d8" number(12,2),
        "d4" number(4,2),
        "t1" timestamp default systimestamp,
        "r5" raw(128),
        "date1" date default sysdate,
        primary key ("t1")
);

以下是模板，由mkpatt程序生成
*****************************************/
T_PkgType TTYPE_tpl[]={
        {CH_TINY,1,"\"id\" id",0,-1},
        {CH_CNUM,22,"\"i64\" i64"},
        {CH_SHORT,sizeof(short),"\"i2\" integer_para"},
        {CH_DOUBLE,sizeof(double),"\"d8\" d8","%14.2lf"},
        {CH_FLOAT,sizeof(float),"\"d4\" d4","%6.2lf"},
        {CH_USEC,sizeof(INT64),"\"t1\" t1","YYYY-MM-DD HH24:MI:SS.FF6"},
        {CH_BYTE,128,"\"r5\" r5"},
        {CH_DATE,YEAR_TO_SEC_LEN,"\"date1\" date1",YEAR_TO_SEC},
        {-1,0,"TTYPE","t1|"}
};

typedef struct {
        char id;
        char i64[22];
        short integer_para;
        double d8;
        float d4;
        INT64 t1;
        char r5[128];
        char date1[YEAR_TO_SEC_LEN];
} TTYPE_stu;
int ins_ttype_dao(DAU *DP,char *stmt);//DAO函数


int main(int argc,char *argv[])
{
DAU dau;
char stmt[1024];
TTYPE_stu tt;
int ret;
T_SQL_Connect SQL_Connect;

	if(argc>1) ret=envcfg(argv[1]);
	ret=db_open(&SQL_Connect);
	if(ret) {
		printf("open database error!\n");
		return 1;
	}
	___SQL_Transaction__(&SQL_Connect,TRANBEGIN);
	DAU_init(&dau,&SQL_Connect,0,&tt,TTYPE_tpl);
//对记录赋值
	for(ret=0;ret<sizeof(tt.r5);ret++)
		tt.r5[ret]=127-ret;
        tt.id=13;
        tt.integer_para=0;
        tt.d8=123456.3;
        tt.d4=1.25;
	tt.t1=INT64NULL;
	*tt.i64=0;

	ret=ins_ttype_dao(&dau,stmt);
	printf("%s,ret=%d\n",stmt,ret);
	printf("t1=%s,date1=%s\n",rusecstrfmt(stmt,tt.t1,YEAR_TO_USEC),tt.date1);

	tt.id=15;
	ret=ins_ttype_dao(&dau,stmt);

	tt.id=0;
	*tt.date1=0;
	*stmt=0;
	ret=prepare_by_PK(&dau,stmt);//公用DAO函数 ，按主键取数据
	printf("prepare %s,ret=%d\n",stmt,ret);
	printf("id=%d,t1=%s,date1=%s,d4=%.2f\n", tt.id,
		rusecstrfmt(stmt,tt.t1,YEAR_TO_USEC),tt.date1,tt.d4);

	tt.d4=1.5;
	tt.d8=1.0;
	sprintf(stmt,"WHERE $d4 between :d8 and :d4");
	ret=DAU_select(&dau,stmt,0);
printf("select float stmt=%s,ret=%d\n",stmt,ret);
	while(!DAU_next(&dau)) {
		DAU_pack(&dau,stmt);
		printf("%s\n",stmt);
	}
	___SQL_Transaction__(&SQL_Connect,TRANROLLBACK);
	DAU_free(&dau);
        ___SQL_CloseDatabase__(&SQL_Connect);

	return 0;
}

int ins_ttype_dao(DAU *DP,char *stmt)
{
int ret;

    do {//以timestamp为主键，大量用户同时插入可能会重码，。
	if(DP->ins_sth < 0)
		DAU_mk_returning(DP,"t1,date1",stmt); //生成RETURNING子句 t1,date1作为返回列
	ret=DAU_ins_returning(DP,stmt); //插入数据库
   } while (DP->SQL_Connect->Errno == DUPKEY);//如果重码再次插入，总会有不重的时候。
   return ret;
}
