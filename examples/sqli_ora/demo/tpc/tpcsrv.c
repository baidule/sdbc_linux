#include <unistd.h>
#include <ctype.h>
#include "tpcsrv.h"
#include <sys/resource.h>

#include "userfunc.c"

void quit(int n)
{
	DB_pool_free();
	ShowLog(0,"quit by %d",n);
	exit(n);
}

void srv_check()
{
//每30秒被执行一次，可以把低精度定时任务排列在此
        dbpool_check();
        ctx_check();
}

void sc_init(T_Connect *conn,T_NetHead *head)
{
char *env;
char cp[21];
T_SRV_Var *srvp=(T_SRV_Var *)conn->Var;
GDA *gp=(GDA *)srvp->var;
        env=getenv("SENDSIZE");
        if(env && *env) {
                conn->MTU=atoi(env);
        }
	env=getenv("TIMEOUT");
        if(env && isdigit(*env)) {
                srvp->o_timeout=conn->timeout=60*atoi(env);
        } else srvp->o_timeout=conn->timeout=0;
	gp->ctx.ctx_id=0;
	*gp->ctx.DEVID=0;
        peeraddr(conn->Socket,cp);;
        ShowLog(0,"tid %lX 连接:%s ",pthread_self(),cp);
/* 在此设置事件处理 */
//      connect->Event_proc=事件设置函数;

}

void FreeVar(void *data)
{
T_SRV_Var *srvp=(T_SRV_Var *)data;
//GDA *gp=(GDA *)srvp->var;
	if(srvp->SQL_Connect) {
		release_DB_connect(&srvp->SQL_Connect,srvp->poolno);
	}
}

static char my_showid[200];
int main(int argc,char *argv[])
{
int ret;
struct rlimit sLimit;
char *p;

//设置可以core dumpped 
	sLimit.rlim_cur = -1; 
        sLimit.rlim_max = -1; 
        ret=setrlimit(RLIMIT_CORE,(const struct rlimit *)&sLimit);

	if(argc>1) ret=envcfg(argv[1]);
//生成模板时，列名变小写
	p=getenv("COL_TO_LOWER");
        if(p && isdigit(*p)) col_to_lower=atoi(p);

	sprintf(my_showid,"%s:%d",sc_basename(argv[0]),getpid());
        Showid=my_showid;

/*
	ret=getrlimit(RLIMIT_STACK,&sLimit);
ShowLog(5,"STACK_cur=%d,max=%d",sLimit.rlim_cur,sLimit.rlim_max);
	sLimit.rlim_cur=300000;
        ret=setrlimit(RLIMIT_STACK,(const struct rlimit *)&sLimit);
ShowLog(5,"set STACK ret=%d",ret);
*/
	ret=getrlimit(RLIMIT_NOFILE,&sLimit);
	ShowLog(5,"%s:%d,get NOFILE ret=%d,cur=%d,max=%d",argv[0],getpid(),
		sLimit.rlim_cur,sLimit.rlim_max);
/*****************************************
需要修改：/etc/security/limits.conf：
sdbc    soft    nofile  1024
sdbc    hard    nofile  16384
	p=getenv("MAXFILNUM");
	if(p && isdigit(*p)) {
		ret=atoi(p);
		sLimit.rlim_cur=ret;
		ret=setrlimit(RLIMIT_NOFILE,&sLimit);
		if(ret) ShowLog(1,"set FILENUM ret=%d,err=%d,%s",ret,errno,strerror(errno));
		ret=getrlimit(RLIMIT_NOFILE,&sLimit);
ShowLog(5,"FILENUM ret=%d,num=%d",ret,sLimit.rlim_cur);
	}
*****************************************/

	ret=DB_pool_init();
	if(ret) {
		ShowLog(1,"DB_pool_init:fail");
		return 1;
	}
	TPC_srv(sc_init,quit,srv_check,sizeof(GDA));//GDA,告它一个尺寸，内部自己分配 
	return 0;
}
