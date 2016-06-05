#include <arpa/inet.h>
#include <DAU.h>
#include <DAU_json.h>
#include <sdbc.h>

#define trans_begin(SQL_Connect)   ___SQL_Transaction__(SQL_Connect,TRANBEGIN)
#define trans_rollback(SQL_Connect) ___SQL_Transaction__(SQL_Connect,TRANROLLBACK)
#define trans_commit(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANCOMMIT)


#ifdef __cplusplus
extern "C" {
#endif

int dui_svc(T_Connect *conn,T_NetHead *nethead);
int pageselect_svc(T_Connect *conn,T_NetHead *nethead);
int net_showlog(T_Connect *conn,T_NetHead *NetHead);
int serverToCli(T_Connect *connect,T_NetHead *NetHead);
int cliToServer(T_Connect *connect,T_NetHead *NetHead);

#ifdef __cplusplus
}
#endif
