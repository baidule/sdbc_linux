/* 客户端，测试select语句 */
#include "sqlcli.h"

int testselect(T_Connect *conn,T_NetHead *nethead)
{
int ret,num,count;
T_CLI_Var *clip=(T_CLI_Var *)conn->Var;
char *rec,stmt[1024];
INT64 now;

	rec=stmt;
	rec += sprintf(rec,"select count(*) from ");
	if(*clip->DBOWN) rec+=sprintf(rec,"%s.",clip->DBOWN);
	rec += sprintf(rec,"seat where start_date=to_date('2009-09-25','YYYY-MM-DD') and beg_station='VNP'");
	num=0;
	now=now_usec();
	ret=N_SQL_Select(conn,stmt,&rec,&num);
	if(ret) {
		ShowLog(1,"%s:N_SQL_Select err=%d,%s",__FUNCTION__,
			clip->Errno,clip->ErrMsg);
		ret -1;
	}
//使用预置模板解析结果集 
	net_dispack(&count,rec,IntType);
//客户端，rec不能释放！ 
	ShowLog(2,"测试select语句:");
	ShowLog(2,"%s:stmt=%s,ret=%d,num=%d,count=%d,TIMEVAL=%d",__FUNCTION__,
		stmt,ret,num,count,(int)(now_usec()-now));
	return 0;
}
