#include <DAU.h>

T_PkgType test_tpl[]={
	{CH_DATE,11,"to_date(a.beg_date+3,'YYYY-MM-DD') beg_date","YYYY-MM-DD",-1},
	{CH_DATE,11,"IA0067 end_date","YYYY-MM-DD"},
	{CH_CHAR,11,"b.first_name first_name"},
	{CH_CHAR,20,"(select a from $DB.tab3 d  where d.b=:last) last"},
	{-1,0,"$DB.tab1 a,  $DB.tab2 b",0}
};

typedef struct {
	char beg_date[11];
	char end_date[11];
	char first_name[11];
	char last[20];
} test_stu;

main(int argc,char *argv[])
{
test_stu t;
DAU t_DAU;
T_SQL_Connect sql;
char stmt[1024];
int ret;
char *p;

	if(argc>1) envcfg(argv[1]);
	data_init(&t,test_tpl);
	ret=db_open(&sql);
	if(ret) {printf("open db err!\n");exit(1);}
	DAU_init(&t_DAU,&sql,0,&t,test_tpl);
	p=DAU_mk_where(&t_DAU,"end_date",stmt);
	p+=sprintf(p," and a.first_name=b.first_name and a.last_name=(select aa from $DB.tab3 c where c.bb=:first_name) ");
	ret=DAU_select(&t_DAU,stmt,0);
/* 生成了如下语句： 
SELECT  to_char(to_date(a.beg_date+3,'YYYY-MM-DD'),'YYYY-MM-DD') beg_date,b.first_name first_name,(select a from TICKET.tab3 d  where d.b=:1) last FROM TICKET.tab1 a,  TICKET.tab2 b where a.first_name=b.first_name and beg_date>to_date(:2,'YYYY-MM-DD') and a.last_name=(select aa from TICKET.tab3 c where c.bb=:3)
*/
	printf("ret=%d,stmt=%s\n",ret,stmt);
	DAU_free(&t_DAU);
	___SQL_CloseDatabase__(&sql);
	return 0;

}
