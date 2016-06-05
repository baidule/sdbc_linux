#include "sqlcli.h"
#include <unistd.h>

T_PkgType STOP_STATION_tpl[]={
        {CH_CHAR,13,"train_no",0,-1},
        {CH_CHAR,7,"run_train_no"},
        {CH_SHORT,sizeof(short),"sequence"},
        {CH_JUL,sizeof(INT4),"beg_date",YEAR_TO_DAY},
        {CH_JUL,sizeof(INT4),"end_date",YEAR_TO_DAY},
        {CH_CHAR,5,"station_code"},
        {CH_INT,sizeof(int),"distance"},
        {CH_INT,sizeof(int),"running_time"},
        {CH_SHORT,sizeof(short),"stop_time"},
        {CH_SHORT,sizeof(short),"over"},
        {CH_TINY,1,"flag"},
        {CH_SHORT,sizeof(short),"line"},
        {CH_TINY,1,"area_flg"},
        {-1,0,"STOP_STATION","train_no|sequence|end_date|"}
};

extern T_PkgType STOP_STATION_tpl[];
typedef struct {
        char train_no[13];
        char run_train_no[7];
        short sequence;
        INT4 beg_date;
        INT4 end_date;
        char station_code[5];
        int distance;
        int running_time;
        short stop_time;
        short over;
        char flag;
        short line;
        char area_flg;
} STOP_STATION_stu;

int cli_prepare(T_Connect *conn,T_NetHead *head)
{
int ret,i;
char stmt[4096],*p,*rec;
T_CLI_Var *clip=(T_CLI_Var *)conn->Var;
STOP_STATION_stu stop;
T_SqlDa sqlda;
int cursor,num;
INT64 now;

	p=stmt;
	p=stpcpy(p,"select ");
	p=mkfield(p,STOP_STATION_tpl,NULL);
	p+=sprintf(p," from %s.stop_station where rownum<=20",clip->DBOWN);
	now=now_usec();
	ret=N_SQL_Prepare(conn,stmt,&sqlda);
	if(ret<0) {
		ShowLog(1,"%s:%s,err=%d,%s",__FUNCTION__,
			stmt, clip->Errno,clip->ErrMsg);
		return -1;
	}
if(isatty(0)) printf("%s:stmt=%s,ret=%d,cursor=%d\n",__FUNCTION__,stmt,ret,sqlda.cursor_no);
	num=5;
	while(0==(ret=N_SQL_Fetch(conn,sqlda.cursor_no,&rec,&num))) {
		if(isatty(0)) {
			p=rec;
			for(i=0;i<num;i++) {
				p+=net_dispack(&stop,p,STOP_STATION_tpl);
				net_pack(stmt,&stop,STOP_STATION_tpl);
				printf("rec=%s\n",stmt);
			}
		}
		num=5;
	}
	N_SQL_Close(conn,&sqlda);
	ShowLog(2,"%s:TIMEVAL=%d",__FUNCTION__,(int)(now_usec() - now));
	return 0;
}
