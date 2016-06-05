//服务器用 分页查询，通过JSON协议 
#include "tpoolsrv.h"
#include <DAU_json.h>

//扩展模板，绑定max min
static int extent_rn(DAU *DP,int max,int min)
{
T_PkgType *etp;
int i,ret;

        i=abs(DP->srm.Aflg);
        etp=(T_PkgType *)realloc(DP->srm.tp,(i+3)*sizeof(T_PkgType));
        if(!etp) return -1;
        etp[i+2]=etp[i];
        etp[i].type=CH_INT;
        etp[i].len=sizeof(int);
        etp[i].name=strdup("rn_max");
        etp[i].format=NULL;
        etp[i+1].type=CH_INT;
        etp[i+1].len=sizeof(int);
        etp[i+1].name=strdup("rn_min");
        etp[i+1].format=NULL;
        etp->offset=-1;
        ret=set_offset(etp);
        DP->srm.tp=etp;
        DP->srm.rec=realloc(DP->srm.rec,etp[ret].offset);
        *(int *)((char *)DP->srm.rec+etp[i].offset)=max;
        etp[i].bindtype|=NOSELECT|NOINS;
        *(int *)((char *)DP->srm.rec+etp[i+1].offset)=min;
        etp[i+1].bindtype|=NOSELECT|NOINS;
        DP->srm.Aflg=ret;
        if(DP->srm.colidx) free(DP->srm.colidx);
        DP->srm.colidx=mk_col_idx(etp);
        return 0;
}

//DAO层 
int mk_select(DAU *DP,char *where,int page_idx,int page_size)
{
int ret,rown=-1;
char *p;
//char tmprowid[10];
const char *rowid=NULL;
int min,max;

	min=(page_idx-1)*page_size;
	max=min+page_size;
	rown=index_col(DP->srm.colidx,abs(DP->srm.Aflg),(char *)"ROWID",DP->srm.tp);
	if(rown >=0) { //把ROWID列剔除 
		rowid=DP->srm.tp[rown].name;
//		sprintf(tmprowid,"ROWNUM");
		DP->srm.tp[rown].name="ROWNUM";
	}

        if(extent_rn(DP,max,min)) {
                ShowLog(1,"%s:extent_rn fail",__FUNCTION__);
                return -1;
        }

	if(0!=(ret=SRM_mk_select(&DP->srm,DP->SQL_Connect->DBOWN,where))) {
		if(rowid)  DP->srm.tp[rown].name=rowid;
		DP->srm.hint=0;
		return ret;
	}
	if(rowid)  DP->srm.tp[rown].name=rowid;
	p=strdup(where);
//	sprintf(where,"select * FROM (select A.*,ROWNUM RN FROM (%s) A WHERE ROWNUM <= %d) WHERE RN > %d",
//		p,max,min);
	 sprintf(where,"select * FROM (select A.*,ROWNUM RN FROM (%s) A WHERE ROWNUM <= :rn_max) WHERE RN > :rn_min", p);
        free(p);
	return	DAU_select(DP,where,0);
}

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
int getpage(T_SRV_Var *ctx,char *rqst,JSON_OBJECT result,char *msg)
{
int ret;
INT64 now;
struct msg_stu app;	
DAU table_DAU;
JSON_OBJECT json,val;

	if(!msg) {
		return -1 ;	
	}
	now=now_sec();
	*msg=0;
	data_init(&app, msg_stu_tpl);
ShowLog(5,"getpage:%s",rqst);

	//对传入的JSON对象进行拆包
	json=json_tokener_parse(rqst);
	if(!json) {
		sprintf(msg,"getpage:#json 格式错误!#");
		return -1;
	}
	val=json_object_object_get(json,"values");
	ret=json_to_struct(&app,json,msg_stu_tpl);
	if(!ret){
		json_object_put(json);
		sprintf(msg,"getpage:transfer json error!");
		return -1;
	}
//ShowLog(5,"getpage:tid=%lu,obj_num of app=%d,tabname=%s",ctx->tid,ret,app.tablename);
	ret=DAU_init(&table_DAU,ctx->SQL_Connect,app.tablename,0,0);
	if(ret) {
		sprintf(msg,"getpage:#表名%s不存在！#",app.tablename);
		json_object_put(json);
		return -1;
	}
	if(val) {
		DAU_fromJSON(&table_DAU,val);
	}
	json_object_put(json);
	if(!strcmp(app.where,"PRIMARY_KEY")) {
	char *p;
		p=mk_where(table_DAU.srm.pks,app.where);
		if(!*app.where) {
			sprintf(msg,"getpage:#table %s:没有主键!",table_DAU.srm.tabname);
			DAU_free(&table_DAU);
			return -2;
		}
	}

	/*分页操作
	  	*如果page_idx=0&&page_size=0,则不分页,取全部结果集;
	  	*如果page_idx=0&&page_size=n,则不分页,取n条结果集;
	   	*如果page_idx=n&&page_size=0,则表示分n页,取1条结果集;
	   	*如果page_idx=n&&page_size=n,则表示分n页,取n条结果集;
	*/

	if(app.page_idx==0) {
		if(0>=(ret=DAU_select(&table_DAU,app.where,app.page_size))){
			sprintf(msg,"getpage:nopage select error:%s",app.where);
			DAU_free(&table_DAU);
			return -3;
		}
	} else {
		if(app.page_size==0)  app.page_size=1;
		if(0>=(ret=mk_select(&table_DAU,app.where,app.page_idx,app.page_size))){
			sprintf(msg,"getpage:error page_idx=%d select page_size=%d msg=%s!err=%d,%s",
				app.page_idx,app.page_size,
				app.where,ctx->SQL_Connect->Errno,ctx->SQL_Connect->ErrMsg);
			DAU_free(&table_DAU);
			return -4;
		}
	}	
	ShowLog(5,"getpage:ret=%d,stmt=%s",ret,app.where);
	while(!(ret=DAU_next(&table_DAU))){
	int rn;
		//结果集打成JSON包	
		JSON_NEW(json);
		DAU_toJSON(&table_DAU,json,0);
		if(app.page_idx) {
			table_DAU.srm.rp+=net_dispack(&rn,table_DAU.srm.rp,IntType);
		}
		json_object_array_add(result,json);
	}
	DAU_free(&table_DAU);
	
	return 0;	
}
//service层  
int page_select(T_Connect *conn,T_NetHead *nethead)
{
T_SRV_Var *sp;
JSON_OBJECT result;
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

        result=json_object_new_array();
        ret=getpage(sp,nethead->data,result,msg); 
        nethead->ERRNO1=sp->SQL_Connect->Errno;
        if(conn->status) {
                release_DB_connect(&sp->SQL_Connect,sp->poolno);
        }

	if(ret<0) {
		p=msg;
	} else { 
		p=(char *)json_object_to_json_string(result);
	}

	nethead->PKG_REC_NUM=0;//json_object_array_length(result);
        nethead->data=p;
        nethead->PKG_LEN=strlen(nethead->data);
        nethead->PROTO_NUM=PutEvent(conn,event);
        nethead->ERRNO2=ret;
        nethead->O_NODE=LocalAddr(conn->Socket,msg+sizeof(msg)-20);
        ret=SendPack(conn,nethead);

        json_object_put(result);
        return 0;
}
