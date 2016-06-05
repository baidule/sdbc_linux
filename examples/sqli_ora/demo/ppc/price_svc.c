#include "sqlcli.h"

int price_svc(T_Connect *conn,T_NetHead *nethead)
{
char msg[2048];
int event;

	event=nethead->PROTO_NUM&65535;
	*msg=0;
        nethead->ERRNO2=price_app(nethead->ERRNO1,nethead->ERRNO2,msg);
	nethead->PKG_REC_NUM=0;
ShowLog(1,"%s:ERRNO1=%d,msg=%s",__FUNCTION__,nethead->ERRNO1,msg);
        nethead->data=msg;
        nethead->PKG_LEN=strlen(nethead->data);
        nethead->PROTO_NUM=PutEvent(conn,event);
        nethead->PKG_REC_NUM=0;
        nethead->ERRNO1=0;
        nethead->O_NODE=LocalAddr(conn->Socket,NULL);
        SendPack(conn,nethead);
        return 0;
}

