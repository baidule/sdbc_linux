#include <sccli.h>
typedef struct {
	char DEVID[41];
	char Opername[41];
	char DBUSER[41];
	char DBOWN[41];
	char Logtime[YEAR_TO_SEC_LEN];
	short TCB;
}log_s ;

int price_cli(T_Connect *conn,T_NetHead *head)
{
int i,ret;
char buf[256];
INT64 now;
T_CLI_Var *clip=(T_CLI_Var *)conn->Var;
log_s *log=(log_s *)clip->var;

	head->ERRNO1=1461;//公里
	head->ERRNO2=5;//新空调
	head->PKG_LEN=0;
	head->PKG_REC_NUM=0;
	head->O_NODE=LocalAddr(conn->Socket,NULL);
	head->D_NODE=0;
	head->PROTO_NUM=get_srv_no(conn->Var,"price_svc");
	if(head->PROTO_NUM==1) {
		ShowLog(1,"%s:price_svc不存在",__FUNCTION__);
		return -1;
	}
	now=now_usec();
	SendPack(conn,head);
	for(i=0;i<9;i++) {
		head->ERRNO1=1461;//公里
		head->ERRNO2=5;//新空调
		head->PKG_LEN=0;
		head->PKG_REC_NUM=0;
		head->O_NODE=LocalAddr(conn->Socket,NULL);
		head->D_NODE=0;
		head->PROTO_NUM=get_srv_no(conn->Var,"price_svc");
		SendPack(conn,head);
		ret=RecvPack(conn,head);
		if(ret) break;
	}
	if(!ret) ret=RecvPack(conn,head);
	if(ret) {
		ShowLog(1,"Net error=%d,%s,TCB:%d",errno,strerror(errno),log->TCB);
		return -1;
	}
	sprintf(buf,"jflc=%d,price=%s,TIMEVAL=%d",head->ERRNO2,head->data,(int)(now_usec()-now));
	ShowLog(2,"%s:%s",__FUNCTION__,buf);
//	printf("%s\n",buf);
	return 0;
}

