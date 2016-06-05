/* 客户端，测试分页查询功能 */

#include "sqlcli.h"

//测试分页查询 
int testpage(T_Connect *conn,T_NetHead *nethead)
{
JSON_OBJECT json,val;
int cc,event;
T_CLI_Var *clip=(T_CLI_Var *)conn->Var;
log_s *log=(log_s *)clip->var;
INT64 now;

	val=json_object_new_object();
	add_string_to_json(val,"trn_no","G7154");

	json=json_object_new_object();
	add_string_to_json(json,"tablename","trn_mmnt");
	add_string_to_json(json,"where","where trn_no=:trn_no order by stn_no");
	add_string_to_json(json,"page_size","10");
	add_string_to_json(json,"page_idx","1");
	json_object_object_add(json,"values",val);
	
	nethead->PROTO_NUM=get_srv_no(clip,"page_select");
	nethead->data=(char *)json_object_to_json_string(json);
	nethead->PKG_LEN=strlen(nethead->data);
	nethead->ERRNO1=nethead->ERRNO2=nethead->PKG_REC_NUM=0;
	nethead->O_NODE=LocalAddr(conn->Socket,0);
now=now_usec();
	cc=SendPack(conn,nethead);
if(isatty(0)) printf("send=%s\n",nethead->data);
	json_object_put(json);

	cc=RecvPack(conn,nethead);
	if(cc<0||nethead->ERRNO1!=0) {
		ShowLog(1,"page_select cc=%d net err %d,%s,TCB=%d",
			cc,errno,strerror(errno),log->TCB);
		return -1;
	} else {
ShowLog(2,"测试分页功能:RecvPack %d,t_len=%d,len=%d,ERRNO2=%d,data=%s,TIMEVAL=%ld",cc,
	nethead->T_LEN,nethead->PKG_LEN,nethead->ERRNO2,nethead->data,now_usec()-now);
		event=nethead->PROTO_NUM&65535; //准备返回事件 
		if(event)  EventCatch(conn,event);
		if(isatty(0)) printf("page_select errno2=%d,data=%s\n",nethead->ERRNO2,nethead->data);
/*
		json=json_tokener_parse(nethead->data);
		if(json) {
			printf("%s\n",json_object_to_json_string(json));
			json_object_put(json);
		} else printf("page_select return=%s\n",nethead->data);
*/
	}
	return 0;
}
