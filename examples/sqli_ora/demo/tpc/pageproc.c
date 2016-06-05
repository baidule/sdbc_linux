/************************************************************************
 * 调用存储过程的例子
 ***********************************************************************/

//服务器用 分页查询，通过JSON协议 
#include "tjsTPC.h"
#include <json_pack.h>

#define JSON_NEW(json) { \
                if(!((json)=json_object_new_object())){ \
                        sprintf(msg,"getpage:new json error!"); \
                        break; \
                }}

/*输入结构：{tablename:表名,where:where语句,page_idx:页号,page_size:页长,values:{列名:值,,,}//对where里的bind赋值 */

static T_PkgType msg_stu_tpl[]={
        {CH_CHAR,1024,"tablename",0,-1},
        {CH_CHAR,4096,"where"},
        {CH_INT,sizeof(int),"page_idx"},
	{CH_INT,sizeof(int),"page_size"},
        {-1,0,0,0,0}
};

struct msg_stu {
	char tablename[1024];
	char where[4096];
	int page_idx;
	int page_size;
};

//app层 
static int getpage(CTX *ctx,JSON_OBJECT cmd,JSON_OBJECT result)
{
int ret,cursor;
INT64 now;
struct msg_stu app;	
JSON_OBJECT val;
char msg[4096],*p;

	now=now_sec();
	*msg=0;
	data_init(&app, msg_stu_tpl);

	//对传入的JSON对象进行拆包
	val=json_object_object_get(cmd,"values");
	ret=json_to_struct(&app,cmd,msg_stu_tpl);
	if(!ret){
		sprintf(msg,"getpage:transfer json error!");
		json_object_object_add(result,"error",jerr(-1,msg));
		return -1;
	}
	p=msg;
	p+=sprintf(p,"get_data.read(:cursor,'%s','%s',%d,%d,'",
		app.tablename,app.where,app.page_size,
		app.page_idx);
//存储过程的val不是JSON串，是他的一部分，比较麻烦
	if(val)  {
		struct lh_entry *entry ;
		for(entry = json_object_get_object(val)->head;entry;entry = entry->next ) {
			*p++ = '\"';
			p=stpcpy(p,entry->k);				//KEY
			p=stpcpy(p,"\":\"");
			p=stpcpy(p,(char *)json_object_get_string((struct json_object *)entry->v));	//Value
			p=stpcpy(p,"\",");
		}
		*(--p)=0;
	}

	strcpy(p,"')");
	p=NULL;
	ShowLog(5,"%s:RPC stmt=%s",__FUNCTION__,msg);
	ret=ORA_Rpc(ctx->SQL_Connect,msg,&p);
	ShowLog(5,"%s:RPC ret=%d,p=%s",__FUNCTION__,ret,p);
	if(ret<0||!p) {
		ShowLog(1,"%s:RPC fault ret=%d,err=%d,%s",__FUNCTION__,
			ret,
			ctx->SQL_Connect->Errno,
			ctx->SQL_Connect->ErrMsg);
		json_object_object_add(result,"error",
			jerr(ctx->SQL_Connect->Errno,ctx->SQL_Connect->ErrMsg));
		return -1;
	}
	cursor=atoi(p);
	free(p);

/* 如果是裸数据
	int i,nc;
   	char *cp,*cp1;
	CONST char **n = sqlo_ocol_names(cursor, &nc);//取列名
*/

	val=json_object_new_array();
	while(NULL!=(p=___SQL_Fetch(ctx->SQL_Connect,cursor,NULL))){
	JSON_OBJECT json;
		//结果集打成JSON包	
		json=json_tokener_parse(p);
		if(!json) {
			ShowLog(1,"%s:%s:not a JSON",__FUNCTION__,p);
			free(p);
			continue;
		}
/* 如果是裸数据
		JSON_NEW(json);
		cp=strtok_r(p,"|",&cp1);
		for(i=0;i<nc;i++) {
			add_string_to_json(json,n[i],cp);
			cp=strtok_r(NULL,"|",&cp1);
		}
*/
		free(p);
		json_object_array_add(val,json);
	}
	json_object_object_add(result,app.tablename,val);
	___SQL_Close__(ctx->SQL_Connect,cursor);
	
	return 0;	
}
int pageproc_app(CTX *ctx,JSON_OBJECT cmd,JSON_OBJECT result)
{
int ret,flg=0;
char msg[256];

	if(!ctx->SQL_Connect) {
		ret=get_DB_connect(&ctx->SQL_Connect,ctx->poolno);
                if(ret) {
                        sprintf(msg,"取连接池失败！,pool[%d],ret=%d",ctx->poolno,ret);
                        ShowLog(1,"%s:%s",__FUNCTION__,msg);
			json_object_object_add(result,"error",jerr(-1,msg));
                        return 0;
                }
		flg=1;
	}
	ret=getpage(ctx,cmd,result);
	if(flg) release_DB_connect(&ctx->SQL_Connect,ctx->poolno);
	return 0;
}

int pageproc_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead,pageproc_app);
}

