/***************************************************************
 * 安全数据库连接的service层   
 ***************************************************************/

#include <time.h>
#include <sdbc.h>
#include <regex.h>

/**********************************************************
 * 对客户端传来的SQL语句，其是否合法的检测
 *********************************************************/
char *SQL_stmt[]={
	"^[ 	]*SELECT",
	"[      ]*ORDER[        ]BY[    ]", //1 Used by Prepare
        "[      ]*GROUP[        ]BY[    ]", //2 Used by Prepare
        "[      ]*HAVING[       ]",         //3 Used by Prepare
	"^[ 	]*UPDATE",
	"^[ 	]*INSERT",
	"^[ 	]*DELETE",
	"^[ 	]*CLOSE",
	"^[ 	]*SET",
};
static regex_t SQL_preg[sizeof(SQL_stmt)/sizeof(char *)];
static int regflg=0;

int SQL_Check_Stmt(char *cmd)
{
int i,ret;
regmatch_t rt[20];
    if(!regflg) {
        for(i=0;i<sizeof(SQL_stmt)/sizeof(char *);i++)
        {
	    ret=regcomp(&SQL_preg[i],SQL_stmt[i],REG_EXTENDED|REG_ICASE);
        }
        regflg=1;
    }

    for(i=0;i<sizeof(SQL_stmt)/sizeof(char *);i++)
    {
	ret=regexec(&SQL_preg[i],cmd,19,rt,0);
	if(ret==0) return i;
    }
    return -1;
}

int ___SQL_SendError(T_Connect *connect,T_NetHead *NetHead)
{
T_SRV_Var *srvp;
T_SQL_Connect *SQL_Connect;
int Evtno=NetHead->PROTO_NUM;
	srvp=(T_SRV_Var *)connect->Var;
	SQL_Connect=srvp->SQL_Connect;
	NetHead->ERRNO2=connect->status?PACK_NOANSER:-1;//结束状态
        NetHead->ERRNO1= SQL_Connect->Errno;
	NetHead->data= SQL_Connect->ErrMsg;
     	NetHead->PKG_LEN=strlen(NetHead->data);
     	NetHead->PKG_REC_NUM=0;
     	NetHead->PROTO_NUM=PutEvent(connect,Evtno);
	NetHead->O_NODE=LocalAddr(connect->Socket,0);
	NetHead->ERRNO1=SendPack(connect,NetHead);
	return 0;
}

int SQL_Prepare(T_Connect *connect,T_NetHead *NetHead)
{
register int i;
sqlo_stmt_handle_t sth;
T_SQL_Connect *SQL_Connect;
T_SRV_Var *srvp;
char msg[200];

        if(NetHead->ERRNO2 != PACK_STATUS) {
                sprintf(msg,"缺少状态标志");
                NetHead->ERRNO1=-199;
                goto errret;
        }

     ShowLog(5,"SQL Prepare: %s ",NetHead->data);
	srvp=(T_SRV_Var *)connect->Var;
	SQL_Connect=srvp->SQL_Connect;
	if(!SQL_Connect) {
		sprintf(msg,"未取得数据库连接");
		NetHead->ERRNO1=-200;
		goto errret;
	}
	sth=-1;
     i=SQL_Check_Stmt(NetHead->data);
     if(i!=0){
	sprintf(msg,"%s:invalid statment",NetHead->data);
	NetHead->ERRNO1=-201;
errret:
        NetHead->ERRNO2=connect->status?PACK_NOANSER:-1;//结束状态
     	NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
	NetHead->O_NODE=LocalAddr(connect->Socket,0);
     	NetHead->PKG_REC_NUM=0;
	NetHead->data=msg;
     	NetHead->PKG_LEN=strlen(NetHead->data);
	NetHead->ERRNO1=SendPack(connect,NetHead);
	ShowLog(1,"%s:%s",__FUNCTION__,msg);
	return 0;
     }
     sth=___SQL_Prepare__(SQL_Connect,NetHead->data);
     if (sth<0) {
	___SQL_SendError(connect,NetHead);
	ShowLog(1,"%s:stmt=%s,err=%d,%s",__FUNCTION__,NetHead->data,
                        SQL_Connect->Errno,SQL_Connect->ErrMsg);
	return 0;
     }
     {
	T_SqlVar SqlVar;
	T_SqlDa SqlDa;
    	int	nCol;
	long PackLength=0;
	char SendBuffer[8192];
  	CONST char ** n;/* column names */
  	CONST unsigned short *vn;/* value lengths */
  	int nc;              /* number of columns */

	nc=0;
  	n = sqlo_ocol_names(sth, &nc);
	vn=sqlo_value_lens(sth,0);
	SqlDa.cols=nc;
	SqlDa.cursor_no=sth;
	PackLength=0;
    	for( nCol = 0; nCol < nc; nCol++ ) {
		if(n[nCol]!=0){
			TRIM((char *)n[nCol]);
			strcpy(SqlVar.sqlname,n[nCol]);
		} else strcpy(SqlVar.sqlname,"");
		SqlVar.sqltype=sqlo_get_ocol_dtype(sth,nCol);
		SqlVar.sqllen = vn[nCol];
		*SqlVar.sqlformat = 0;
		PackLength+=net_pack(SendBuffer+PackLength,&SqlVar,SqlVarType);
	}
  	NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
	NetHead->ERRNO1=sth;
	NetHead->ERRNO2=0;
   	NetHead->PKG_REC_NUM=nc;
	NetHead->data=SendBuffer;
     	NetHead->PKG_LEN=strlen(NetHead->data);
	NetHead->ERRNO1=SendPack(connect,NetHead);
    	ShowLog(5,"%s:Success Cursor No:%d cols=%d",__FUNCTION__,
		sth,nc);
     }
     return 0;
}

int SQL_Select(T_Connect *connect,T_NetHead *NetHead)
{
T_SRV_Var *srvp=(T_SRV_Var *)connect->Var;
T_SQL_Connect *SQL_Connect=srvp->SQL_Connect;
int i;
char *rec=0;
char msg[2048];

	if(!SQL_Connect) {
		sprintf(msg,"未取得数据库连接");
		NetHead->ERRNO1=-200;
		goto errret;
	}
     i=SQL_Check_Stmt(NetHead->data);
     if(i!=0){
	sprintf(msg,"SQL_Select fail:%s,invalid statment",NetHead->data);
	NetHead->ERRNO1=-201;
errret:
        NetHead->ERRNO2=connect->status?PACK_NOANSER:-1;//结束状态
     	NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
	NetHead->O_NODE=LocalAddr(connect->Socket,0);
	NetHead->PKG_REC_NUM=0;
	NetHead->data=msg;
	NetHead->PKG_LEN=strlen(NetHead->data);
	NetHead->ERRNO1=SendPack(connect,NetHead);
	ShowLog(1,"%s:%s",__FUNCTION__,msg);
	return 0;
     }
     i=___SQL_Select__(SQL_Connect,NetHead->data,&rec,NetHead->PKG_REC_NUM);
     if(i<=0) {
     	ShowLog(1,"SQL Select error: %d,%s",
		SQL_Connect->Errno,
		SQL_Connect->ErrMsg);
	___SQL_SendError(connect,NetHead);
	if(rec) free(rec);
	return 0;
     }
     ShowLog(5,"SQL Select Success nROWS=%d",i);
     	NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
     	NetHead->O_NODE=0;
     	NetHead->D_NODE=0;
     	NetHead->ERRNO2=SQL_Connect->NativeError;
	NetHead->ERRNO1=0;
     	NetHead->PKG_REC_NUM=i;
	NetHead->data=rec;
     	NetHead->PKG_LEN=strlen(NetHead->data);
	NetHead->ERRNO1=SendPack(connect,NetHead);
	if(rec) free(rec);
	return 0;
}

int SQL_Exec(T_Connect *connect ,T_NetHead *NetHead)
{
int ret;
T_SQL_Connect *SQL_Connect;
T_SRV_Var *srvp;

    srvp=(T_SRV_Var *)connect->Var;
    SQL_Connect=srvp->SQL_Connect;
	if(!SQL_Connect) {
		ShowLog(1,"%s:未取得数据库连接",__FUNCTION__);
     		NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
		NetHead->ERRNO2=connect->status?PACK_NOANSER:-1;//结束状态
		NetHead->ERRNO1=-200;
		NetHead->data=NULL;
     		NetHead->PKG_LEN=0;
		NetHead->PKG_REC_NUM=0;
		NetHead->O_NODE=LocalAddr(connect->Socket,0);
		NetHead->D_NODE=0;
		NetHead->ERRNO1=SendPack(connect,NetHead);
		return 0;
	}

    ShowLog(5,"SQL Execute:%s",NetHead->data);
    ret=SQL_Check_Stmt(NetHead->data);
    if(ret<=3)
    {
	sprintf(SQL_Connect->ErrMsg,"%s:invalid statment",NetHead->data);
        ShowLog(1,"SQL_Exec fail:%s",SQL_Connect->ErrMsg);
     	NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
        NetHead->ERRNO1=-201;
        NetHead->ERRNO2=0;
        NetHead->O_NODE=LocalAddr(connect->Socket,0);
        NetHead->PKG_REC_NUM=0;
        NetHead->data=SQL_Connect->ErrMsg;
        NetHead->PKG_LEN=strlen(NetHead->data);
        NetHead->ERRNO1=SendPack(connect,NetHead);
	return 0;
    }
    ret=___SQL_Exec(SQL_Connect ,NetHead->data);
    if(ret<0){
	___SQL_SendError(connect,NetHead);
	return 0;
    }
    NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
    NetHead->PKG_REC_NUM=ret;
    NetHead->ERRNO1=0;
    NetHead->ERRNO2=0;
    NetHead->PKG_LEN=0;
    NetHead->O_NODE=0;
    NetHead->ERRNO1=SendPack(connect,NetHead);
    ShowLog(5,"SQL Execute Success row:%d  ",NetHead->PKG_REC_NUM);
    return 0;
}

int SQL_Fetch(T_Connect *connect,T_NetHead *NetHead)
{
register int curno;
int recnum;
char *p;
T_SRV_Var *srvp;
T_SQL_Connect *SQL_Connect;
char msg[200];

        if(NetHead->ERRNO2 != PACK_STATUS) {
                sprintf(msg,"缺少状态标志");
                NetHead->ERRNO1=-199;
                goto errret;
	}
	srvp=(T_SRV_Var *)connect->Var;
	SQL_Connect=srvp->SQL_Connect;
	if(!SQL_Connect) {
		sprintf(msg,"未取得数据库连接");
		NetHead->ERRNO1=-200;
errret:
     		NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
		NetHead->ERRNO2=PACK_NOANSER;
		NetHead->data=msg;
     		NetHead->PKG_LEN=strlen(NetHead->data);
		NetHead->PKG_REC_NUM=0;
		NetHead->O_NODE=LocalAddr(connect->Socket,0);
		NetHead->D_NODE=0;
		NetHead->ERRNO1=SendPack(connect,NetHead);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		return 0;
	}

     curno=NetHead->ERRNO1;
     recnum=NetHead->PKG_REC_NUM;
     ShowLog(5,"%s:cur=%d,recnum=%d",__FUNCTION__,curno,recnum);

	p=___SQL_Fetch(SQL_Connect,curno,&recnum);
	if (!p) {
		NetHead->ERRNO1=SQLNOTFOUND;
		NetHead->data="Not Found|";
		SQL_Connect->NativeError=0;
	}
	else {
		NetHead->data=p;
		NetHead->ERRNO1=0;
	}
     	NetHead->PKG_LEN=strlen(NetHead->data);
     	NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
	NetHead->ERRNO2=SQL_Connect->NativeError;
	NetHead->PKG_REC_NUM=recnum;
	NetHead->O_NODE=LocalAddr(connect->Socket,0);
	NetHead->D_NODE=0;
	NetHead->ERRNO1=SendPack(connect,NetHead);
	if(p) free(p);
    ShowLog(5,"SQL_Fetch %d rec's Fetched coln=%d",recnum,SQL_Connect->NativeError);
    return 0;
}

int SQL_Close(T_Connect *connect,T_NetHead *NetHead)
{
int CursorNo;
int i=0;
T_SQL_Connect *SQL_Connect;
T_SRV_Var *srvp;
char msg[200];
	srvp=(T_SRV_Var *)connect->Var;
	SQL_Connect=srvp->SQL_Connect;
	if(!SQL_Connect) {
		sprintf(msg,"数据库连接丢失");
     		NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
		NetHead->ERRNO2=PACK_NOANSER;
		NetHead->ERRNO1=-200;
		NetHead->data=msg;
     		NetHead->PKG_LEN=strlen(NetHead->data);
		NetHead->PKG_REC_NUM=0;
		NetHead->O_NODE=LocalAddr(connect->Socket,0);
		NetHead->D_NODE=0;
		NetHead->ERRNO1=SendPack(connect,NetHead);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		return 0;
	}
     	CursorNo=NetHead->ERRNO1;
	if(CursorNo>=0) {
	    ShowLog(5,"%s:Close dbh=%d,CurrsorNo:%d",__FUNCTION__,SQL_Connect->dbh,CursorNo);
	    i=___SQL_Close__(SQL_Connect,CursorNo);
	    if(i!=SQLO_SUCCESS){
		ShowLog(1,"SQL_Close:Return:%d\n",i);
		___SQL_SendError(connect,NetHead);
		return 0;
	    }
	}
     	NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
     	NetHead->PKG_LEN=0;
     	NetHead->PKG_REC_NUM=0;
     	NetHead->ERRNO2=0;
     	NetHead->O_NODE=0;
     	NetHead->D_NODE=0;
     	NetHead->ERRNO1=i;
     	NetHead->ERRNO1=SendPack(connect,NetHead);
	ShowLog(5,"Close CurrsorNo:%d Success",CursorNo);
	return 0;
}

int SQL_EndTran(T_Connect *connect,T_NetHead *NetHead)
{

int flag;
int ret;
T_SQL_Connect *SQL_Connect;
T_SRV_Var *srvp;
char msg[200];
	srvp=(T_SRV_Var *)connect->Var;
	SQL_Connect=srvp->SQL_Connect;

	if(!SQL_Connect) {
		sprintf(msg,"未取得数据库连接");
     		NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
		NetHead->ERRNO1=-200;
		NetHead->ERRNO2=PACK_NOANSER;
		NetHead->data=msg;
     		NetHead->PKG_LEN=strlen(NetHead->data);
		NetHead->PKG_REC_NUM=0;
		NetHead->O_NODE=LocalAddr(connect->Socket,0);
		NetHead->D_NODE=0;
		NetHead->ERRNO1=SendPack(connect,NetHead);
		ShowLog(1,"%s:%s",__FUNCTION__,msg);
		return 0;
	}
	flag=NetHead->ERRNO1;
	switch(flag){
		case TRANBEGIN:
			ret=sqlo_commit (SQL_Connect->dbh) ;
			break;
		case TRANCOMMIT:
			ret=sqlo_commit (SQL_Connect->dbh) ;
			break;
		case TRANROLLBACK:
			ret=sqlo_rollback (SQL_Connect->dbh); 
			break;
		default:
			ret=-1;
			break;
	}
    	ShowLog(5,"SQL EndTran %d ret=%d",flag,ret);
     	NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
     	NetHead->ERRNO1=0;
     	NetHead->ERRNO2=ret;
     	NetHead->PKG_REC_NUM=0;
     	NetHead->O_NODE=0;
     	NetHead->D_NODE=0;
	NetHead->data=0;
     	NetHead->PKG_LEN=0;
     	NetHead->ERRNO1=0;
	NetHead->ERRNO1=SendPack(connect,NetHead);
	return 0;
}

int SQL_RPC(T_Connect *connect,T_NetHead *NetHead)
{
char *SendBuffer=0;
char *cmd;
int nrets=0;
int event;
T_SQL_Connect *SQL_Connect;
T_SRV_Var *srvp;

	srvp=(T_SRV_Var *)connect->Var;
	SQL_Connect=srvp->SQL_Connect;
	event=NetHead->PROTO_NUM;
	if(!SQL_Connect) {
		sprintf(SQL_Connect->ErrMsg,"未取得数据库连接");
     		NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
		NetHead->ERRNO1=-200;
		NetHead->ERRNO2=connect->status?PACK_NOANSER:-1;
		NetHead->data=SQL_Connect->ErrMsg;
     		NetHead->PKG_LEN=strlen(NetHead->data);
		NetHead->PKG_REC_NUM=0;
		NetHead->O_NODE=LocalAddr(connect->Socket,0);
		NetHead->D_NODE=0;
		NetHead->ERRNO1=SendPack(connect,NetHead);
		return 0;
	}
     	cmd=NetHead->data;
ShowLog(5,"SQL_RPC:%s",cmd);
	nrets=SQL_Rpc(SQL_Connect,cmd,&SendBuffer);
     	if(nrets<0){
		ShowLog(1,"SQL_RPC %d:%d,%s;Native=%d,%s",nrets,
			SQL_Connect->Errno,
			SQL_Connect->ErrMsg,
			SQL_Connect->NativeError,
			SQL_Connect->SqlState);
		if(SendBuffer) free(SendBuffer);
		___SQL_SendError(connect,NetHead);
		NetHead->ERRNO2=PACK_NOANSER;
		return 0;
	}
	NetHead->ERRNO1=0;
   	NetHead->ERRNO2=SQL_Connect->Errno;  //rpcstate
     	NetHead->PROTO_NUM=PutEvent(connect,NetHead->PROTO_NUM);
	NetHead->PKG_REC_NUM=SQL_Connect->NativeError; //ncols
	NetHead->data=SendBuffer;
	NetHead->PKG_LEN=strlen(SendBuffer);
   	NetHead->O_NODE=LocalAddr(connect->Socket,0);
   	NetHead->D_NODE=0; 
	NetHead->ERRNO1=SendPack(connect,NetHead);
	if(SendBuffer)free(SendBuffer);
	ShowLog(3,"%s:Success nrets=%d,%s",__FUNCTION__,
			nrets,
			SQL_Connect->ErrMsg);

    return 0;
}
