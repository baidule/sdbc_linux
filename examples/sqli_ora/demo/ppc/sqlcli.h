#include <sccli.h> 
#include <ctype.h>
#include <json_pack.h>

#include "../srvcomm/logs.stu"


extern void phex(char *title,char *str,int i);

extern int clilogin(T_Connect *conn,T_NetHead *nethead);
extern int testpage(T_Connect *conn,T_NetHead *nethead);
extern int testselect(T_Connect *conn,T_NetHead *nethead);
extern int price_cli(T_Connect *conn,T_NetHead *nethead);
extern int cli_prepare(T_Connect *conn,T_NetHead *head);
typedef struct {
	char DEVID[41];
	char Opername[41];
	char DBUSER[41];
	char DBOWN[41];
	char Logtime[YEAR_TO_SEC_LEN];
	short TCB;
}log_s ;

