#include <sqli.h>
#include <malloc.h>

int ___SQL_Select__(T_SQL_Connect *SQL_Connect,char *stmt,char **rec,int recnum)
{
char errmsg[2148];
int curno,olderrno;
	
	if(!rec) return FORMATERR;
	*rec=0;
	if(!SQL_Connect ||!stmt) {
		sprintf(errmsg,"SQL_Connect or stmt is empty!");
		*rec=strdup(errmsg);
		return FORMATERR;
	}
	curno=___SQL_Prepare__(SQL_Connect,stmt);
	if(curno<0) {
		if(SQL_Connect->Errno != SQLNOTFOUND) {
			sprintf(errmsg,"err=%d,%s,stmt=%.1824s",
					SQL_Connect->Errno,
					SQL_Connect->ErrMsg, stmt);
	  		ShowLog(1,"___SQL_Select__:%s ", errmsg);
		}
		*rec=strdup(errmsg);
		return curno;
	}
//	if(recnum>0) sqlo_set_prefetch_rows(curno,recnum);
	*rec=___SQL_Fetch(SQL_Connect,curno,&recnum);
	olderrno= SQL_Connect->Errno;
	strcpy(errmsg, SQL_Connect->ErrMsg);
	___SQL_Close__(SQL_Connect,curno);
	SQL_Connect->Errno=olderrno;
	strcpy(SQL_Connect->ErrMsg, errmsg);
	if(olderrno && olderrno!=SQLNOTFOUND) {
		SQL_Connect->NativeError=0;
		sprintf(errmsg,"err=%d,%s,stmt=%.1024s",
			SQL_Connect->Errno,
			SQL_Connect->ErrMsg, stmt);
		if(SQL_Connect->Errno != SQLNOTFOUND) 
			ShowLog(1,"___SQL_Select__:%s", errmsg);
		*rec=strdup(errmsg);
	  	return -1;
	}
	return recnum;
}

