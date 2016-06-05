#include <server.h>
#include <ctxlib.h>

#include <logs.stu>

typedef struct myvar {
	char ShowID[100];
	char devid[17];
	char operid[17];
	INT64	logtime;
	pthread_t trans_tid;
	CTX_stu ctx;
} GDA;

#ifdef __cplusplus
extern "C" {
#endif

//·þÎñº¯Êý 
extern int login(T_Connect *conn,T_NetHead *NetHead);
extern int dui(T_Connect *conn,T_NetHead *nethead);
extern int page_select(T_Connect *conn,T_NetHead *NetHead);
extern int price_svc(T_Connect *conn,T_NetHead *NetHead);

#ifdef __cplusplus
}
#endif
