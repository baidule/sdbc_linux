#include "tpoolsrv.h"

static int to_get_ctx(T_Connect *conn,T_NetHead *nethead)
{
T_SRV_Var *srvp=(T_SRV_Var *)conn->Var;;
GDA *gp=(GDA *)srvp->var;
char msg[200];
sdbcfunc app;
//取ctx_id,在nethead->O_NODE里
CTX_stu *ctxp;
int ret;

        unset_callback(srvp->TCB_no);
        app=conn->only_do;
        conn->only_do=NULL;
        if(!srvp->SQL_Connect) {
                sprintf(msg,"取数据库连接池失败！,pool[%d]",srvp->poolno);
		nethead->ERRNO1=-200;
		nethead->ERRNO2=PACK_NOANSER;
		return_error(conn,nethead,msg);
                ShowLog(1,"%s:%s",__FUNCTION__,msg);
                return 0;
        }
        ctxp=get_ctx(nethead->O_NODE);// 暂时不用数据库
        if(!ctxp&&nethead->ERRNO2!=PACK_NOANSER) {
                release_DB_connect(&srvp->SQL_Connect,srvp->poolno);
		conn->timeout=srvp->o_timeout;
		sprintf(msg,"取CTX失败！PROTO=%d,ctx_id=%u",nethead->PROTO_NUM,nethead->O_NODE);
		nethead->ERRNO1=-198;
		nethead->ERRNO2=PACK_NOANSER;
                return_error(conn,nethead,msg);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		return 0;
        }
	if(conn->status) {
		conn->timeout=75;
	}
	gp->ctx=*ctxp;
	sprintf(gp->ShowID,"%s:%d",gp->ctx.DEVID,srvp->TCB_no);

        ret=app(conn,nethead);
        if(!conn->status || nethead->ERRNO2==PACK_NOANSER) {
//无状态或状态夭折
                release_DB_connect(&srvp->SQL_Connect,srvp->poolno);
		sprintf(gp->ShowID,"%s:%d",gp->devid,srvp->TCB_no);
		conn->status=0;
		conn->timeout=srvp->o_timeout;
		gp->ctx.ctx_id=0;
		*gp->ctx.DEVID=0;
        }
        return ret;
}

//service层  
//帮助应用处理数据库连接池
//状态服务的例子
int tpool_svc(T_Connect *conn,T_NetHead *nethead,sdbcfunc app)
{
T_SRV_Var *sp;
char msg[200];
int ret;

        sp=(T_SRV_Var *)conn->Var;
	conn->status=0;
//	ShowLog(2,"%s:%s",__FUNCTION__,nethead->data);
	conn->status = nethead->ERRNO2==PACK_STATUS; 
	if(!sp->SQL_Connect) {
		conn->only_do=app;
        	ret=DB_connect_MGR(sp->TCB_no,sp->poolno,&sp->SQL_Connect,to_get_ctx);
       		if(ret==0) return to_get_ctx(conn,nethead);
       		if(ret==1) return THREAD_ESCAPE;//DB_connect_MGR设置了回调函数

		sprintf(msg,"取数据库连接池失败!,pool[%d],ret=%d",sp->poolno,ret);
        	nethead->ERRNO1=-200;
        	nethead->ERRNO2=PACK_NOANSER;
		conn->status=0;
		conn->only_do=0;
		return_error(conn,nethead,msg);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		return 0;
	}
//如果数据库已存在，说明在状态中，不必取ctx了，直接执行app
	ret=app(conn,nethead);
	if(!conn->status||nethead->ERRNO2==PACK_NOANSER) {
	GDA *gp=(GDA *)sp->var;
//执行失败，要求夭折事务
		release_DB_connect(&sp->SQL_Connect,sp->poolno);
		conn->status=0;
		conn->timeout=sp->o_timeout;
		sprintf(gp->ShowID,"%s:%d",gp->devid,sp->TCB_no);
		gp->ctx.ctx_id=0;
		*gp->ctx.DEVID=0;
	}
	return ret;
}

int pageselect_svc(T_Connect *conn,T_NetHead *nethead)
{
	return tpool_svc(conn,nethead,page_select);
}

int dui_svc(T_Connect *conn,T_NetHead *nethead)
{
	return tpool_svc(conn,nethead,dui);
}

int select_svc(T_Connect *conn,T_NetHead *nethead)
{
	return tpool_svc(conn,nethead,SQL_Select);
}

//建立有状态服务
int prepare_svc(T_Connect *conn,T_NetHead *nethead)
{
	if(nethead->ERRNO2 != PACK_STATUS) {
	char msg[200];
	T_SRV_Var *sp=(T_SRV_Var *)conn->Var;
		release_DB_connect(&sp->SQL_Connect,sp->poolno);
		conn->timeout=sp->o_timeout;
                sprintf(msg,"缺少状态标志");
                nethead->ERRNO1=-199;
		nethead->ERRNO2=conn->status?PACK_NOANSER:-1;//结束状态
        	nethead->PROTO_NUM=PutEvent(conn,65535 & nethead->PROTO_NUM);
		nethead->O_NODE=LocalAddr(conn->Socket,0);
		nethead->PKG_REC_NUM=0;
		nethead->data=msg;
		nethead->PKG_LEN=strlen(nethead->data);
		nethead->ERRNO1=SendPack(conn,nethead);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		conn->status=0;
		return 0;
	}
	return tpool_svc(conn,nethead,SQL_Prepare);
}

//有状态服务
int close_svc(T_Connect *conn,T_NetHead *nethead)
{
T_SRV_Var *sp=(T_SRV_Var *)conn->Var;
	if(!sp->SQL_Connect) {
	char msg[200];
		nethead->ERRNO1=-200;
		nethead->ERRNO2=PACK_NOANSER;
		sprintf(msg,"数据库连接丢失");
		conn->status=0;
		return_error(conn,nethead,msg);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		return 0;
	}
	return tpool_svc(conn,nethead,SQL_Close);
}

int transaction_svc(T_Connect *conn,T_NetHead *nethead)
{
T_SRV_Var *sp=(T_SRV_Var *)conn->Var;
char msg[200];

	if(nethead->ERRNO1==TRANBEGIN) {
		if(nethead->ERRNO2 != PACK_STATUS) {
			release_DB_connect(&sp->SQL_Connect,sp->poolno);
			conn->timeout=sp->o_timeout;
       		        sprintf(msg,"缺少状态标志");
       	         	nethead->ERRNO1=-199;
			nethead->ERRNO2=PACK_NOANSER;//结束状态
			conn->status=0;
			return_error(conn,nethead,msg);
			ShowLog(1,"%s:%s",__FUNCTION__,msg);
			return 0;
		}
	} else if(!sp->SQL_Connect) {
		nethead->ERRNO1=-200;
		sprintf(msg,"数据库连接丢失");
		nethead->ERRNO2=PACK_NOANSER;//结束状态
		return_error(conn,nethead,msg);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		return 0;
	}
	return tpool_svc(conn,nethead,SQL_EndTran);
}

int exec_svc(T_Connect *conn,T_NetHead *nethead)
{
//要先取 DB 连接池
	return tpool_svc(conn,nethead,SQL_Exec);
}

int rpc_svc(T_Connect *conn,T_NetHead *nethead)
{
	return tpool_svc(conn,nethead,SQL_RPC);
}

int ctx_login_svc(T_Connect *conn,T_NetHead *head)
{
T_SRV_Var *srvp=(T_SRV_Var *)conn->Var;
int ret;
char tmp[204];
    if(head->PKG_REC_NUM==-1) {
    	strcpy(tmp,"无效的ctx_id -1");
    	head->ERRNO1=-198;
   	head->ERRNO2=-1;
    	return_error(conn,head,tmp);
   	ShowLog(1,"%s:%s",__FUNCTION__,tmp);
	return 0;
    }
/*
GDA *gp=(GDA *)srvp->var;
    if(gp->ctx.ctx_id&&gp->ctx.ctx_id!=head->PKG_REC_NUM) {
	sprintf(tmp,"已经登录过，不能再登录");
	if(head->ERRNO2 != PACK_NOANSER) {
            head->ERRNO1=-198;
            head->ERRNO2=PACK_NOANSER;;
	    return_error(conn,head,tmp);
	}
        ShowLog(1,"%s:%s",__FUNCTION__,tmp);
        return 0;
    }
*/
//验证CA？需记录目标DEVID及其密钥

    //取数据库连接池
    ret=DB_connect_MGR(srvp->TCB_no,srvp->poolno,&srvp->SQL_Connect,ctx_login_app);
    if(ret==0) return ctx_login_app(conn,head);
    if(ret==1) return THREAD_ESCAPE;//DB_connect_MGR设置了回调函数

    sprintf(tmp,"数据库连接失败");
    head->ERRNO1=-200;
    head->ERRNO2=-1;
    return_error(conn,head,tmp);
    ShowLog(1,"%s:%s",__FUNCTION__,tmp);
    return ret;
}

int ctx_logout_svc(T_Connect *conn,T_NetHead *head)
{
T_SRV_Var *srvp=(T_SRV_Var *)conn->Var;
int ret;
GDA *gp=(GDA *)srvp->var;
char msg[200];
CTX_stu *ctxp;

        ctxp=get_ctx(head->O_NODE);// 暂时不用数据库
	if(srvp->SQL_Connect) {
		srvp->SQL_Connect->Errno=DBFAULT;
		release_DB_connect(&srvp->SQL_Connect,srvp->poolno);
		conn->timeout=srvp->o_timeout;
		conn->status=0;
	}
        if(!ctxp) {
		sprintf(msg,"取CTX %u 失败！",head->O_NODE);
		if(head->ERRNO2 != PACK_NOANSER) {
			head->ERRNO1=-198;
			head->ERRNO2=PACK_NOANSER;
                	return_error(conn,head,msg);
		}
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		return 0;
        }
	gp->ctx=*ctxp;
	ret=ctx_del(&gp->ctx);
	sprintf(msg,"%s logout flg=%d",gp->ctx.DEVID,ret);
	if(head->ERRNO2 != PACK_NOANSER) {
		head->ERRNO1=0;
		head->ERRNO2=ret;
		return_error(conn,head,msg);
	}
        gp->ctx.ctx_id=0;
        *gp->ctx.DEVID=0;

	ShowLog(2,"%s:tid=%lX,%s",__FUNCTION__,pthread_self(),msg);
	sprintf(gp->ShowID,"%s:%d",gp->devid,srvp->TCB_no);
	return 0;
}

