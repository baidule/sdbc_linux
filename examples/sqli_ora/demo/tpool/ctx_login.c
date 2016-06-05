/***********************************************************
 * 远程认证包
 ***********************************************************/
#include "tpoolsrv.h"
#include <bignum.h>

#include <logs.stu>

int ctx_login(T_SRV_Var *sp)
{
int ret;
GDA *gp=(GDA *)sp->var;
	ret=set_ctx(&gp->ctx);
	if(ret>0) sprintf(gp->ShowID,"%s:%d",gp->ctx.DEVID,sp->TCB_no);
	return ret;
}

int ctx_login_app(T_Connect *conn,T_NetHead *head)
{
    T_SRV_Var *srvp=(T_SRV_Var *)conn->Var;
    GDA *gp=(GDA *)srvp->var;
    int ret,event;
    char tmp[30],stmt[10240];
    login_stu logrec;
    log_stu logret;

    event=head->PROTO_NUM&65535;
    unset_callback(srvp->TCB_no);
    if(!srvp->SQL_Connect) {
            sprintf(stmt,"取数据库连接池失败！,pool[%d]",srvp->poolno);
            head->ERRNO1=-200;
            head->ERRNO2=PACK_NOANSER;;
	    return_error(conn,head,stmt);
            ShowLog(1,"%s:%s",__FUNCTION__,stmt);
            return 0;
    }
    net_dispack(&logrec,head->data,login_tpl);
    strcpy(gp->ctx.DEVID,logrec.DEVID);
    strcpy(gp->operid,logrec.UID);
    strcpy(gp->ca,logrec.CA);
	if(head->PKG_REC_NUM) {
	CTX_stu *ctxp=get_ctx(head->PKG_REC_NUM);
		if(!ctxp || strcmp(ctxp->DEVID,gp->ctx.DEVID)) {
			release_DB_connect(&srvp->SQL_Connect,srvp->poolno);
			sprintf(stmt,"%s ctx_id %d 失效",gp->ctx.DEVID,head->PKG_REC_NUM);
			head->ERRNO1=-197;
			head->ERRNO2=-1;
			return_error(conn,head,stmt);
			ShowLog(1,"%s:%s",__FUNCTION__,stmt);
			return 0;
		}
	} else {
      		strcpy(gp->ctx.DEVID,logrec.DEVID);
      		ret=ctx_login(srvp);
	}

	strcpy(logret.DBUSER,srvp->SQL_Connect->UID);
	strcpy(logret.DBOWN,srvp->SQL_Connect->DBOWN);
	sprintf(gp->ShowID,"%s:%d",gp->devid,srvp->TCB_no);
	release_DB_connect(&srvp->SQL_Connect,srvp->poolno);

	strcpy(logret.DEVID,gp->ctx.DEVID);
	strcpy(logret.UID,gp->operid);//gp->ctx.OPERID?
	rsecstrfmt(logret.Logtime,now_sec(),YEAR_TO_SEC);
	str_n64a(1,(u_int *)&gp->ctx.ctx_id,logret.DBLABEL);
	net_pack(stmt,&logret,log_tpl);

	StrAddr(head->O_NODE,tmp);
	head->PROTO_NUM=PutEvent(conn,event);
	head->ERRNO1=0;
	head->ERRNO2=0;
	head->data=stmt;
	head->PKG_LEN=strlen(head->data);
	head->ERRNO1=SendPack(conn,head);
	ShowLog(2,"%s/%s:%s success ctx_id=%u,ret=%d",__FUNCTION__,tmp,stmt,gp->ctx.ctx_id,ret);
	return 0;
}
/*
int ctx_logout_app(T_Connect *conn,T_NetHead *head)
{
T_SRV_Var *srvp=(T_SRV_Var *)conn->Var;
GDA *gp=(GDA *)srvp->var;
//暂时不使用数据库
	return 0;
}
*/
