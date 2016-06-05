#include "sqlcli.h"
#include <unistd.h>

#include "patt.tpl"
#include "patt.stu"

#define BATCH 20
int cli_prepare(T_Connect *conn)
{
int ret,i;
char stmt[4096],*p,*rec;
T_CLI_Var *clip=(T_CLI_Var *)conn->Var;
PATTERN_COL_stu patt;
T_SqlDa sqlda;
int num,recnum;
INT64 now;

	p=stmt;
	p=stpcpy(p,"select ");
	p=mkfield(p,PATTERN_COL_tpl,NULL);
	p+=sprintf(p," from %s.PATTERN_COL ",clip->DBOWN);
printf("stmt=%s\n",stmt);
	now=now_usec();
	ret=N_SQL_Prepare(conn,stmt,&sqlda);
	if(ret<0) {
		ShowLog(1,"%s:%s,err=%d,%s",__FUNCTION__,
			stmt, clip->Errno,clip->ErrMsg);
		return -1;
	}
if(isatty(0)) printf("%s:stmt=%s,ret=%d,cursor=%d,INTERVAL=%d\n",__FUNCTION__,stmt,ret,sqlda.cursor_no,INTERVAL(now));
	recnum=0;
	while(0<(num=N_SQL_Fetch(conn,sqlda.cursor_no,&rec,BATCH))) {
		if(isatty(0)) {
			p=rec;
			for(i=0;i<num;i++) {
				p+=net_dispack(&patt,p,PATTERN_COL_tpl);
				net_pack(stmt,&patt,PATTERN_COL_tpl);
				printf("rec=%s\n",stmt);
			}
		}
		recnum += num;
		if(num < BATCH) break;
	}
	printf("cols=%d\n",clip->NativeError);
	N_SQL_Close(conn,&sqlda);
	ShowLog(2,"%s:%d Rec's TIMEVAL=%d",__FUNCTION__,recnum,INTERVAL(now));
	return 0;
}
