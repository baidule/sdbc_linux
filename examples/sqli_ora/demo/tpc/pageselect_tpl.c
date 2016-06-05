//服务器用 分页查询，通过JSON协议 
#include "tjsTPC.h"

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
        *(int *)(DP->srm.rec+etp[i].offset)=max;
        etp[i].bindtype|=NOSELECT|NOINS;
        *(int *)(DP->srm.rec+etp[i+1].offset)=min;
        etp[i+1].bindtype|=NOSELECT|NOINS;
        DP->srm.Aflg=DP->srm.Aflg<0?-ret:ret;
        if(DP->srm.colidx) free(DP->srm.colidx);
        DP->srm.colidx=mk_col_idx(etp);
        return 0;
}

//DAO层 
static int mk_select(DAU *DP,char *where,int page_idx,int page_size)
{
int ret,rown=-1;
char *p,*rowid=0,tmprowid[10];
int min,max;

	min=(page_idx-1)*page_size;
	max=min+page_size;
	rown=index_col(DP->srm.colidx,abs(DP->srm.Aflg),"ROWID",DP->srm.tp);
	if(rown >=0) { //把ROWID列剔除 
		rowid=(char *)DP->srm.tp[rown].name;
		sprintf(tmprowid,"ROWNUM");
		DP->srm.tp[rown].name=tmprowid;
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
int getpage(CTX *ctx,JSON_OBJECT cmd,JSON_OBJECT result)
{
int ret;
INT64 now;
struct msg_stu app;	
DAU table_DAU;
JSON_OBJECT val;
char msg[4096];
T_PkgType *tab_tp=NULL;
char *rec=NULL;

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
	if(NULL!=(tab_tp=get_tpl(app.tablename))&&
	   NULL!=(rec=mk_rec(tab_tp))) {//如果能在模板库找到，就不要让数据库干了。
		DAU_init(&table_DAU,ctx->SQL_Connect,app.tablename,rec,tab_tp);
	} else  {
		ret=DAU_init(&table_DAU,ctx->SQL_Connect,app.tablename,0,0);
		if(ret) {
			sprintf(msg,"getpage:#表名%s不存在！#",app.tablename);
			json_object_object_add(result,"error",jerr(-1,msg));
			return -1;
		}
	}
	if(val) {
		DAU_fromJSON(&table_DAU,val);
	}
	if(!strcmp(app.where,"PRIMARY_KEY")) {
	char *p;
		p=mk_where(table_DAU.srm.pks,app.where);
		if(!*app.where) {
			sprintf(msg,"getpage:#table %s:没有主键!",table_DAU.srm.tabname);
			DAU_free(&table_DAU);
			json_object_object_add(result,"error",jerr(-2,msg));
			if(rec) free(rec);
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
			json_object_object_add(result,"error",jerr(-3,msg));
			if(rec) free(rec);
			return -3;
		}
	} else {
		if(app.page_size==0)  app.page_size=1;
		if(0>=(ret=mk_select(&table_DAU,app.where,app.page_idx,app.page_size))){
			sprintf(msg,"getpage:page_idx=%d select page_size=%d msg=%s!",
				app.page_idx,app.page_size,
				app.where);
			json_object_object_add(result,"error",jerr(-4,msg));
			DAU_free(&table_DAU);
			if(rec) free(rec);
			return -4;
		}
	}	
	ShowLog(5,"getpage:ret=%d,stmt=%s",ret,app.where);
	val=json_object_new_array();
	while(!(ret=DAU_next(&table_DAU))){
	int rn;
	JSON_OBJECT json;
		//结果集打成JSON包	
		JSON_NEW(json);
		DAU_toJSON(&table_DAU,json,0);
		if(app.page_idx) {
			table_DAU.srm.rp+=net_dispack(&rn,table_DAU.srm.rp,IntType);
		}
		json_object_array_add(val,json);
	}
	DAU_free(&table_DAU);
	json_object_object_add(result,app.tablename,val);
	if(rec) free(rec);
	
	return 0;	
}
int getpage_app(CTX *ctx,JSON_OBJECT cmd,JSON_OBJECT result)
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

int pageselect_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead,getpage_app);
}

