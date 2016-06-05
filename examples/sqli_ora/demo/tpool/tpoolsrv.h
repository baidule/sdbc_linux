#include "../tpc/server.h"
#include <ctxlib.h>

typedef struct myvar {
    char ShowID[40];
    char devid[17];  //这个是直连的DEVID
    char operid[17];
    char ca[256];
    INT64   logtime;
    CTX_stu ctx;	//ctx->DEVID是终端的DEVID
} GDA;

#ifdef __cplusplus
extern "C" {
#endif

int ctx_login(T_SRV_Var *sp);
int ctx_login_app(T_Connect *conn,T_NetHead *head);
int login(T_Connect *conn,T_NetHead *head);
//tpool_svc.c
int select_svc(T_Connect *conn,T_NetHead *nethead);
int prepare_svc(T_Connect *conn,T_NetHead *nethead);
int close_svc(T_Connect *conn,T_NetHead *nethead);
int exec_svc(T_Connect *conn,T_NetHead *nethead);
int transaction_svc(T_Connect *conn,T_NetHead *nethead);
int rpc_svc(T_Connect *conn,T_NetHead *nethead);
int ctx_login_svc(T_Connect *conn,T_NetHead *nethead);
int ctx_logout_svc(T_Connect *conn,T_NetHead *nethead);

#ifdef __cplusplus
}
#endif
