#include "tpoolsrv.h"
#include <DAU_json.h>

extern

#ifdef __cplusplus
 "C"
#endif

JSON_OBJECT jerr(int jerrno,const char *errmsg);

//插入一个表，返回行数
static int tab_oper(DAU *DP,int flg,JSON_OBJECT data,char *stmt,JSON_OBJECT errjson)
{
int i,j,ret,rows=0;
JSON_OBJECT json,ejson;
char *p;
char msg[4096];

	*msg=0;
	if(!strcmp(stmt,"PRIMARY_KEY")) {
		p=mk_where(DP->srm.pks,stmt);
		if(!*stmt) {
			sprintf(stmt,"tab_oper:%s,#该表无主键#",DP->srm.tabname);
			ejson=jerr(105,(const char *)stmt);
			json_object_array_add(errjson,ejson);
			return FORMATERR;
		}
	} else  p=stmt+strlen(stmt);
	j=-1;
	if(!data || (j=json_object_array_length(data))==0) {
		sprintf(stmt,"tab_oper:#没有数据#,array length=%d",j);
		ejson=jerr(106,stmt);
		json_object_array_add(errjson,ejson);
		return 0;
	}
//读出每一行
	for(i=0;i<j;i++) {
		json=json_object_array_get_idx(data, i);
		ret=DAU_fromJSON(DP,json);
		if(ret<1) continue;
		if(flg &4) { //删除
			ret=DAU_delete(DP,stmt);
			if(ret!=1) {
				ejson=jerr(107,stmt);
				json_object_array_add(errjson,ejson);
				ShowLog(1,"tab_oper:delete %s",stmt);
				if(flg &1) return -1;
				continue;
			} else rows++;
			continue;
		}
		ret=DAU_insert(DP,msg);
		if(ret != 0) {
			if(DP->SQL_Connect->Errno==DUPKEY && (flg&2)) {
				ret=DAU_update(DP,stmt);
//ShowLog(5,"tab_oper:stmt=%s,ret=%d",stmt,ret);
				if(ret>0) rows+=ret;
				else {
					ejson=jerr(108,stmt);
					json_object_array_add(errjson,ejson);
					ShowLog(1,"tab_oper:update msg=%s",msg);
					if(flg&1) return -1;
				}
				continue;
			} else {
				if(DP->SQL_Connect->Errno==DUPKEY)
					dummy_update(DP,stmt); 
				else ShowLog(1,"tab_oper:msg=%s",msg);
				ejson=jerr(107,msg);
				json_object_array_add(errjson,ejson);
				if(flg&1) return -1;
			}
		} else rows++;
	}
	strcpy(stmt,msg);
	return rows;
}

static int do_tab(T_SRV_Var *ctx,JSON_OBJECT tabjson,char *msg,JSON_OBJECT errjson)
{
JSON_OBJECT json,ejson;
char *tabname=0;
int ret,flag=0;
char *where=0;
DAU t_DAU;
T_SQL_Connect *SQL_Connect;

	
	if(!tabjson) {
		ejson=jerr(102,"du_tab:#请求的JSON 格式错,空项#");
		json_object_array_add(errjson,ejson);
		return FORMATERR;
	}
	json=json_object_object_get(tabjson,"tablename");
	if(!json) {
		ejson=jerr(103,"do_tab:#没有表名#");
		json_object_array_add(errjson,ejson);
		return FORMATERR;
	}
	SQL_Connect=ctx->SQL_Connect;
	tabname=(char *)json_object_get_string(json);
	json=json_object_object_get(tabjson,"flag");
	if(json) flag=atoi(json_object_get_string(json));
	json=json_object_object_get(tabjson,"where");
	if(json) where=(char *)json_object_get_string(json);
	json=json_object_object_get(tabjson,"values");
	ret=DAU_init(&t_DAU,SQL_Connect,tabname,0,0);
	if(ret) {
		sprintf(msg,"do_tab:#表%s不存在#,err=%d,%s",tabname,
			SQL_Connect->Errno,
			SQL_Connect->ErrMsg);
		ejson=jerr(104,msg);
		json_object_array_add(errjson,ejson);
		return SYSERR;
	}
	if(where) strcpy(msg,where);
	else *msg=0;

	ret=tab_oper(&t_DAU,flag,json,msg,errjson);
	sprintf(msg,"成功处理 %s,rows=%d",t_DAU.srm.tabname,ret);
	if(ret>0) json_object_array_add(errjson, jerr(0,msg));
	ShowLog(2,"do_tab:%s",msg);
	DAU_free(&t_DAU);
	return ret;//返回行数
}

int dui_app(T_SRV_Var *ctx,char *data,char *msg,JSON_OBJECT errjson)
{
JSON_OBJECT rqjson,ejson;
int i,j,ret,finished=0;;

	rqjson = json_tokener_parse(data);
	if(!rqjson) {
		ejson=jerr(100,"dui_app:#请求的JSON 格式错#");
		json_object_array_add(errjson,ejson);
		return FORMATERR;
	}
	j=json_object_array_length(rqjson);
	if(j<1) {

		ejson=jerr(101,"dui_app:#请求的JSON 格式错,不是数组#");
		json_object_array_add(errjson,ejson);
		json_object_put(rqjson);
		return LENGERR;
	}
	ret=0;

        trans_begin(ctx->SQL_Connect);
	for(i=0;i<j;i++) {
		ret=do_tab(ctx,json_object_array_get_idx(rqjson, i),msg,errjson);
		if(ret==-1) {
        		trans_rollback(ctx->SQL_Connect);
			json_object_put(rqjson);
			return -1; //-1:rollback
		}
		if(ret>0) finished++;
	}
        trans_commit(ctx->SQL_Connect);
	json_object_put(rqjson);
	return finished; //返回表数
}
int dui(T_Connect *conn,T_NetHead *nethead)
{
T_SRV_Var *sp;
JSON_OBJECT errjson;
char *p,msg[SDBC_BLKSZ];
int ret,event;

	event=nethead->PROTO_NUM&65535;
	sp=(T_SRV_Var *)conn->Var;
	unset_callback(sp->TCB_no);
	if(!sp->SQL_Connect) {
		sprintf(msg,"TCB:%d,DB link pool error!",sp->TCB_no);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		nethead->data=msg;
		nethead->PKG_LEN=strlen(nethead->data);
		nethead->ERRNO1=-1;
		nethead->ERRNO2=PACK_NOANSER;
		nethead->PKG_REC_NUM=0;
		nethead->PROTO_NUM=PutEvent(conn,event);
		nethead->O_NODE=LocalAddr(conn->Socket,msg+sizeof(msg)-20);
		ret=SendPack(conn,nethead);
		return 0;
	}
/* 调用删、改、插服务 */
        errjson=json_object_new_array();
        ret=dui_app(sp,nethead->data,msg,errjson); //返回插入的表数
        p=(char *)json_object_to_json_string(errjson);
	nethead->ERRNO1=sp->SQL_Connect->Errno;
        if(conn->status) {
//无状态
                release_DB_connect(&sp->SQL_Connect,sp->poolno);
        }

	nethead->data=p;
	nethead->PKG_LEN=strlen(nethead->data);
	nethead->ERRNO2=ret<0?ret:0;
	nethead->PKG_REC_NUM=ret>0?ret:0;
	nethead->PROTO_NUM=PutEvent(conn,event);
	nethead->O_NODE=LocalAddr(conn->Socket,msg+sizeof(msg)-20);
	ret=SendPack(conn,nethead);

        ShowLog(2,"%s:SendPack=%d,%s",__FUNCTION__,ret,p);
        json_object_put(errjson);
	return 0;
}
