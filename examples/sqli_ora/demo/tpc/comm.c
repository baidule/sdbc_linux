#include "server.h"

JSON_OBJECT jerr(int jerrno,const char *errmsg)
{
JSON_OBJECT ejson,ijson,sjson;

        ejson=json_object_new_object();
        ijson=json_object_new_int(jerrno);
        json_object_object_add(ejson,"errno",ijson);
        sjson=json_object_new_string(errmsg);
        json_object_object_add(ejson,"msg",sjson);
        return ejson;
}

int return_error(T_Connect *conn,T_NetHead *nethead,char *msg)
{
	nethead->data=msg;
	nethead->PKG_REC_NUM=0;
	nethead->PKG_LEN=strlen(nethead->data);
	nethead->PROTO_NUM=PutEvent(conn,65535 & nethead->PROTO_NUM);
	nethead->O_NODE=LocalAddr(conn->Socket,NULL);
	nethead->ERRNO1=SendPack(conn,nethead);
	return 0;
}

