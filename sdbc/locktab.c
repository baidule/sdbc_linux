#include <sdbc.h>
#include <time.h>

extern int ___SQL_SendError(T_Connect *connect,int Evtno);
/*   PROTO_NUM=3 */

int locktab(T_Connect *conn,T_NetHead *NetHead)
{
T_SRV_Var *up;
char stmt[512],tmp1[812];
int ret;

	up=(T_SRV_Var *)conn->Var;
	 if(up->tid) {
T_SQL_Connect *SQL_Connect=up->SQL_Connect;
                sprintf(SQL_Connect->ErrMsg,"多线程连接池服务器不支持此类有状态服务！");
                NetHead->ERRNO1=200;
                NetHead->ERRNO2=-1;
                NetHead->data=SQL_Connect->ErrMsg;
                NetHead->PKG_LEN=strlen(NetHead->data);
                NetHead->PKG_REC_NUM=0;
                NetHead->O_NODE=LocalAddr(conn->Socket,0);
                NetHead->D_NODE=SQL_Connect->NativeError;
                SendPack(conn,NetHead);
                return 0;
        }

	if(!NetHead->PKG_LEN) {
		sprintf(tmp1,"locktab which table?");
		ShowLog(1,"locktab:%s",tmp1);
		NetHead->PROTO_NUM=PutEvent(conn,NetHead->PROTO_NUM);
		NetHead->ERRNO1=0;
		NetHead->ERRNO2=5;
		NetHead->PKG_REC_NUM=0;
        	NetHead->O_NODE=LocalAddr(conn->Socket,0);
        	NetHead->D_NODE=0;

		NetHead->data=tmp1;
		NetHead->PKG_LEN=strlen(NetHead->data);
		ret=SendPack(conn,NetHead);
		return 0;
	}
	ShowLog(5,"locktab data=%s",NetHead->data);
	sprintf(stmt,"lock table %s",NetHead->data);
	ret=___SQL_Exec(up->SQL_Connect,stmt);
	if(ret<0) {
		___SQL_SendError(conn,NetHead->PROTO_NUM);
		return 0;
	}
	sprintf(tmp1,"%s succeed",stmt);
	NetHead->PROTO_NUM=PutEvent(conn,NetHead->PROTO_NUM);
	NetHead->ERRNO1=0;
	NetHead->ERRNO2=0;
	NetHead->PKG_REC_NUM=0;
	NetHead->data=tmp1;
	NetHead->PKG_LEN=strlen(NetHead->data);
	ret=SendPack(conn,NetHead);
	return 0;
}
