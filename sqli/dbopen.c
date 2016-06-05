#include <sqli.h>

int db_open1(T_SQL_Connect *SQL_Connect,char *dblabel)
{
char *p;
int ret;

	p=decodeprepare(dblabel);
        if(!p) {
		SQL_Connect->Errno=-1;
		strcpy(SQL_Connect->ErrMsg,"Envenroment not init");
		return -1;
	}

        ret=SQL_AUTH(getenv("DATABASEAUTHFILE"),p,
                        SQL_Connect->DSN,
                        SQL_Connect->UID,
                        SQL_Connect->PWD,
                        SQL_Connect->DBOWN);
        if(ret) {
		SQL_Connect->Errno=ret;
		strcpy(SQL_Connect->ErrMsg,"SQL_AUTH Error");
		return -2;
        }

        ret=___SQL_OpenDatabase__(SQL_Connect);
        if(ret) {
                ShowLog(1,"dbopen Open Database error %d,DBSERVER=%s,UID=%s,DBOWN=%s",
			ret,
			SQL_Connect->DSN,
			SQL_Connect->UID,
			SQL_Connect->DBOWN);
        }
        return ret;
}

