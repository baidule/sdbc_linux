/* 客户端，测试select语句 */
#include "sqlcli.h"

int testselect(T_Connect *conn)
{
int ret,num,count;
T_CLI_Var *clip=(T_CLI_Var *)conn->Var;
char *rec,stmt[1024],*p;
INT64 now;

	now=now_usec();
	rec=stmt;
	rec += sprintf(rec,"select tab_name,count(*) cnt from ");
	if(*clip->DBOWN) rec+=sprintf(rec,"%s.",clip->DBOWN);
	rec += sprintf(rec,"PATTERN_COL group by tab_name");
	num=N_SQL_Select(conn,stmt,&rec,0);
	if(num<0) {
		ShowLog(1,"%s:N_SQL_Select fault! stmt=%s,rec=%s",__FUNCTION__,stmt,rec?rec:"NULL");
		return -1;
	}
//使用预置模板解析结果集 
//客户端，rec不能释放！ 
	printf("测试select语句:\n");
	printf("%s:stmt=%s,num=%d,cols=%d,TIMEVAL=%d\n",__FUNCTION__,stmt,num,
		clip->NativeError, INTERVAL(now));
	p=rec;
	for(ret=0;ret<num;ret++) {
		p+=net_dispack(stmt,p,CharType);
		p+=net_dispack(&count,p,IntType);
		printf("%s,%d\n",stmt,count);
	}
	return 0;
}
