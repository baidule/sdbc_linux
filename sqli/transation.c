#include <unistd.h>
#include <sqli.h>

int ___SQL_Transaction__(T_SQL_Connect *SQL_Connect,int flag)
{
int ret=0;

	if(!SQL_Connect || SQL_Connect->dbh <0 ) return -1;
	switch(flag) { //如果不能产生新线程 
		case TRANCOMMIT:
			ret=sqlo_commit(SQL_Connect->dbh);
			break;
		case TRANBEGIN:
		case TRANROLLBACK:
			ret=sqlo_rollback(SQL_Connect->dbh); 
			break;
		default: 
			ret=-1;
			break;
	}
	if(ret) ___SQL_GetError(SQL_Connect);
	else {
		SQL_Connect->Errno=0; 
		*SQL_Connect->ErrMsg=0;
	}
	return ret;
}
