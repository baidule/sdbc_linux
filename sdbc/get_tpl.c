/**************************************************************
 * get_tpl.c 获取表模板
 * data="tabname1,tabname2,...."
 * ************************************************************/

#include <arpa/inet.h>
#include <sdbc.h>
#include <DAU.h>
#include <DAU_json.h>

int get_tpl(T_Connect *conn,T_NetHead *head)
{
T_SRV_Var *srvp=(T_SRV_Var *)conn->Var;
//GDA *gp=(GDA *)srvp->var;
int i,ret;
DAU dau;
char msg[2048],*cp,*save;
JSON_OBJECT json,result,err_json,view;
INT64 now=now_usec();

	if(head->ERRNO2 == PACK_STATUS) conn->status=1;
	else conn->status=0;

	result=json_object_new_array();
	view=json_object_new_array();
	err_json=json_object_new_array();
	cp=head->PKG_LEN>0?strtok_r(head->data,",",&save):NULL;
	dau.pos=srvp->poolno;
	for(i=0;cp;i++,cp=strtok_r(NULL,",",&save)) {//每个表名
		ret=DAU_mk(&dau,srvp->SQL_Connect,cp);
		if(ret) {
			sprintf(msg,"get table %s fault!",cp);
			json_object_array_add(err_json,jerr(ret,msg));
			continue;
		}
		json=json_object_new_object();
		ret=tpl_to_JSON(dau.srm.tp,view);
		DAU_free(&dau);
		json_object_object_add(json,cp,view);
		json_object_array_add(result,json);
		ShowLog(5,"%s:table %s TIMEVAL=%d",__FUNCTION__,cp,INTERVAL(now));
		DAU_free(&dau);
	}
	json=json_object_new_object();
	json_object_object_add(json,"templates",result);
	json_object_object_add(json,"status",err_json);

	head->data=(char *)json_object_to_json_string(json);
	head->PKG_LEN=strlen(head->data);
	head->ERRNO1=0;
	head->ERRNO2=conn->status?PACK_STATUS:0;
	head->O_NODE=ntohl(LocalAddr(conn->Socket,NULL));
	head->PROTO_NUM=PutEvent(conn,65535 & head->PROTO_NUM);
	head->ERRNO1=SendPack(conn,head);
	json_object_put(json);
	
	return 0;
}

//清空服务器中的模板库

int tpl_cancel(T_Connect *conn,T_NetHead *head)
{
	if(head->ERRNO2 == PACK_STATUS) conn->status=1;
	else conn->status=0;

	tpl_lib_cancel();

	head->PKG_LEN=0;
	head->ERRNO1=0;
	head->ERRNO2=0;
	head->O_NODE=ntohl(LocalAddr(conn->Socket,NULL));
	head->PROTO_NUM=PutEvent(conn,65535 & head->PROTO_NUM);
	head->ERRNO1=SendPack(conn,head);
	return 0;
}
