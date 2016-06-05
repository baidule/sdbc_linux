#include <tjs.h>

int json_svc(T_Connect *conn,T_NetHead *nethead,int (*app)(CTX * ctxp,JSON_OBJECT cmd,JSON_OBJECT result))
{
T_SRV_Var *sp;
JSON_OBJECT cmd,result;
char *p,msg[SDBC_BLKSZ];
int ret,event,flg;
CTX *ctxp;
//INT64 now;

	//now=now_usec();
	event=nethead->PROTO_NUM&65535;
	flg=nethead->ERRNO2;
        sp=(T_SRV_Var *)conn->Var;
	ctxp=(CTX *)sp->var;

	ShowLog(3,"%s:%s,flg=%d",__FUNCTION__,nethead->data,flg);
	cmd=json_tokener_parse(nethead->data);
	if(!cmd) {
		sprintf(msg,"is not a JSON format!");
			ShowLog(1,"%s:%s",__FUNCTION__,msg);
			nethead->data=msg;
			nethead->PKG_REC_NUM=0;
        		nethead->PKG_LEN=strlen(nethead->data);
		        nethead->PROTO_NUM=PutEvent(conn,event);
		        nethead->ERRNO1=-1;
		        nethead->ERRNO2=-1;
		        ret=SendPack(conn,nethead);
			return 0;
	}
        result=json_object_new_object();
        ret=app(ctxp,cmd,result); 
	if(flg==PACK_NOANSER) {
        	json_object_put(result);
        	json_object_put(cmd);
		return 0;
	}

	p=(char *)json_object_to_json_string(result);

	nethead->PKG_REC_NUM=0;//json_object_array_length(result);
        nethead->data=p;
        nethead->PKG_LEN=strlen(nethead->data);
        nethead->PROTO_NUM=PutEvent(conn,event);
        nethead->ERRNO1=0;
        nethead->ERRNO2=ret;
        ret=SendPack(conn,nethead);
	//ShowLog(3,"%s:now=%lu,INTERVAL=%d",__FUNCTION__,now,INTERVAL);
//	ShowLog(5,"%s:%s", __FUNCTION__, p);
        json_object_put(result);
        json_object_put(cmd);
        return 0;
}

