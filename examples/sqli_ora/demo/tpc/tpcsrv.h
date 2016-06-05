#include "server.h"
#include <ctxlib.h>

typedef struct myvar {
        char ShowID[100];
        char devid[21];
        char operid[17];
	char ca[256];
        INT64   logtime;
	CTX_stu ctx;
} GDA;

#ifdef __cplusplus
extern "C" {
#endif

int ctx_login(T_SRV_Var *sp);
int ctx_login_app(T_Connect *conn,T_NetHead *head);
int login(T_Connect *conn,T_NetHead *head);

int transaction_svc(T_Connect *conn,T_NetHead *head);
int select_svc(T_Connect *conn,T_NetHead *head);
int prepare_svc(T_Connect *conn,T_NetHead *head);
int close_svc(T_Connect *conn,T_NetHead *head);
int exec_svc(T_Connect *conn,T_NetHead *head);
int rpc_svc(T_Connect *conn,T_NetHead *head);
int pageselect_svc(T_Connect *conn,T_NetHead *head);
int dui_svc(T_Connect *conn,T_NetHead *nethead);

int ctx_login_svc(T_Connect *conn,T_NetHead *nethead);
int ctx_logout_svc(T_Connect *conn,T_NetHead *nethead);

#ifdef __cplusplus
}
#endif
