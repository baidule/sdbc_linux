#include <sqli.h>
#include <malloc.h>

int
insert_db(T_SQL_Connect *SQL_Connect,char *tabname,void *data,T_PkgType *type,char *tmp1)
{
int ret;
char *p;
	if(!tmp1 || !type ||!data ||!type) return -1;
	p=stpcpy(tmp1,"INSERT INTO ");
	if(*SQL_Connect->DBOWN) {
		p=stpcpy(p,SQL_Connect->DBOWN);
		*p++='.';
	}
	p=mk_values(stpcpy(mkset(stpcpy(p,tabname),type),") VALUES ("),data,type);
	*p++=')';
	*p=0;
	ret=___SQL_Exec(SQL_Connect,tmp1);
        if(ret!=1) {
/*
ShowLog(1,"insert_db:ret=%d,err=%d,%s",ret,
                SQL_Connect->Errno,SQL_Connect->ErrMsg);
*/
                if(!SQL_Connect->Errno) SQL_Connect->NativeError=0;
		p+=sprintf(p,":err=%d,%s",
			SQL_Connect->Errno,
			SQL_Connect->ErrMsg);
                return -1;
        }
        SQL_Connect->NativeError=ret;
	return 0;
}

