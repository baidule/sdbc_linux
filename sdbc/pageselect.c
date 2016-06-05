//�������� ��ҳ��ѯ��ͨ��JSONЭ�� 
#include <arpa/inet.h>
#include <sdbc.h>
#include <DAU.h>
#include <DAU_json.h>

//DAO�� 
int mk_select(DAU *DP,char *where,int page_idx,int page_size)
{
int ret,rown=-1;
char *p,*rowid=0,tmprowid[10];
int min,max;

	min=(page_idx-1)*page_size;
	max=min+page_size;
	rown=index_col(DP->srm.colidx,abs(DP->srm.Aflg),"ROWID",DP->srm.tp);
	if(rown >=0) { //��ROWID���޳� 
		rowid=(char *)DP->srm.tp[rown].name;
		sprintf(tmprowid,"ROWNUM");
		DP->srm.tp[rown].name=tmprowid;
	}
	if(0!=(ret=SRM_mk_select(&DP->srm,DP->SQL_Connect?DP->SQL_Connect->DBOWN:NULL,where))) {
		if(rowid)  DP->srm.tp[rown].name=rowid;
		DP->srm.hint=0;
ShowLog(5,"%s: aft mk_select where=%s,ret=%d",__FUNCTION__,where,ret);
		return ret;
	}
	if(rowid)  DP->srm.tp[rown].name=rowid;
	p=strdup(where);
	sprintf(where,"select * FROM (select A.*,ROWNUM RN FROM (%s) A WHERE ROWNUM <= %d) WHERE RN > %d",
		p,max,min);
        free(p);
ShowLog(5,"%s:stmt=%s",__FUNCTION__,where);
	return	DAU_select(DP,where,0);
}

#define JSON_NEW(json) { \
                if(!((json)=json_object_new_object())){ \
                        sprintf(msg,"getpage:new json error!"); \
                        break; \
                }}

/*����ṹ��{tablename:����,where:where���,page_idx:ҳ��,page_size:ҳ��,values:{����:ֵ,,,}//��where���bind��ֵ */

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

//app�� 
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
//ShowLog(5,"getpage:%s",rqst);

	//�Դ����JSON������в��
	json=json_tokener_parse(rqst);
	if(!json) {
		sprintf(msg,"getpage:#json ��ʽ����!#");
		return -1;
	}
	val=json_object_object_get(json,"values");
	ret=json_to_struct(&app,json,msg_stu_tpl);
	if(!ret){
		json_object_put(json);
		sprintf(msg,"getpage:transfer json error!");
		return -1;
	}
//ShowLog(5,"getpage:tid=%lX,obj_num of app=%d,tabname=%s",ctx->tid,ret,app.tablename);
	ret=DAU_mk(&table_DAU,ctx->SQL_Connect,app.tablename);
	if(ret) {
		sprintf(msg,"#����%s�����ڣ�#",app.tablename);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
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
			sprintf(msg,"getpage:#table %s:û������!",table_DAU.srm.tabname);
			DAU_free(&table_DAU);
			return -2;
		}
	}

	/*��ҳ����
	  	*���page_idx=0&&page_size=0,�򲻷�ҳ,ȡȫ�������;
	  	*���page_idx=0&&page_size=n,�򲻷�ҳ,ȡn�������;
	   	*���page_idx=n&&page_size=0,���ʾ��nҳ,ȡ1�������;
	   	*���page_idx=n&&page_size=n,���ʾ��nҳ,ȡn�������;
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
			sprintf(msg,"getpage:page_idx=%d select page_size=%d msg=%s!,ret=%d",
				app.page_idx,app.page_size,
				app.where,ret);
			DAU_free(&table_DAU);
			return ret==0?-1:ret;
		}
	}	
//	ShowLog(5,"%s:ret=%d,stmt=%s,err=%d",__FUNCTION__,ret,app.where,table_DAU.SQL_Connect->Errno);
	while(!(ret=DAU_next(&table_DAU))){
	int rn;
		//��������JSON��	
		JSON_NEW(json);
		DAU_toJSON(&table_DAU,json,"");
		if(app.page_idx) {
			table_DAU.srm.rp+=net_dispack(&rn,table_DAU.srm.rp,IntType);
		}
		json_object_array_add(result,json);
	}
	DAU_free(&table_DAU);
	
	return 0;	
}

int page_select(T_Connect *conn,T_NetHead *nethead)
{
T_SRV_Var *sp;
JSON_OBJECT result;
char *p,msg[SDBC_BLKSZ];
int ret,event;

	event=nethead->PROTO_NUM&65535;
        sp=(T_SRV_Var *)conn->Var;
	if(!sp->SQL_Connect) {
		sprintf(msg,"ȡ���ӳ�ʧ�ܣ�,pool[%d]",sp->poolno);
		nethead->ERRNO1=-200;
                nethead->ERRNO2=PACK_NOANSER;
                return_error(conn,nethead,msg);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		return 0;
	}
	ShowLog(5,"%s:%s",__FUNCTION__,nethead->data);
/* ���÷�ҳslelct����  */ 
        result=json_object_new_array();
        ret=getpage(sp,nethead->data,result,msg); 
	if(ret<0) {
		p=msg;
	} else { 
		p=(char *)json_object_to_json_string(result);
	}

	nethead->PKG_REC_NUM=0;//json_object_array_length(result);
        nethead->data=p;
        nethead->PKG_LEN=strlen(nethead->data);
        nethead->PROTO_NUM=PutEvent(conn,event);
        nethead->PROTO_NUM=0;
        nethead->ERRNO1=sp->SQL_Connect->Errno;
        nethead->ERRNO2=ret;
        nethead->O_NODE=ntohl(LocalAddr(conn->Socket,msg+sizeof(msg)-20));
        ret=SendPack(conn,nethead);

        json_object_put(result);
        return 0;
}

