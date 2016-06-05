#include <signal.h> 
#include <libgen.h>
#include "sqlsrv.h"


/*********************************************************
 * Can add other functions into Function[] by user       *
 * SDBC 5.0 utility                                      *
 *********************************************************/

#include "userfunc.c"

void quit(int);
void netinit(T_Connect *connect ,T_NetHead *NetHead);
extern FILE *logfile;
//extern char *getenv(char *);

static T_Connect *q_conn=0;

main(int ac,char *av[])
{
char buffer[1024],*cp;
int ret;
int cryptflg;
T_SRV_Var srv_ctx;
T_SQL_Connect sql;
GDA gvar;
	if(ac>1){
		ret=envcfg(av[1]);
	}
	tzset();
	Showid=basename(av[0]);
	cp=getenv("COL_TO_LOWER");
	if(cp && isdigit(*cp)) col_to_lower=atoi(cp);

	kill(getppid(),9);
	srv_ctx.SQL_Connect=&sql;
	srv_ctx.poolno=0;
	srv_ctx.tid=0; //标志多进程单线程有状态服务  
	srv_ctx.TCB_no=-1; //标志非线程池
	gvar.logtime=now_sec();
	srv_ctx.var=&gvar;
	setquit(quit);
	ret=PPC_srv(netinit,Function,&srv_ctx);

	ShowLog(-1,0);
	return (ret);
}
void quit(int n)
{
	signal(n,SIG_IGN);
	if(q_conn && q_conn->Socket>=0) {
		freeconnect(q_conn);
	}
	ShowLog(-1,0);
	exit(n);
}

void netinit(T_Connect *connect ,T_NetHead *NetHead)
{
T_SRV_Var *srvp=(T_SRV_Var *)connect->Var;
GDA *gp=(GDA *)srvp->var;
int return_no;
char *env;
char cp[21];
FILE *fd;
int pid;
	env=getenv("SENDSIZE");
	if(env && *env) {
		connect->MTU=atoi(env);
	}
	env=getenv("TIMEOUT");
        if(env && isdigit(*env)) {
                srvp->o_timeout=connect->timeout=60*atoi(env);
        } else srvp->o_timeout=connect->timeout=0;

	peeraddr(connect->Socket,cp);;
	ShowLog(0,"pid %d 连接:%s ",getpid(),cp);
	q_conn=connect;
	gp->ctx.ctx_id=0;
	*gp->ctx.DEVID=0;
/* 在此设置事件处理 */
//	connect->Event_proc=事件设置函数;
}

FreeVar(char *P)
{
T_SRV_Var *up;
int ret;
	up=(T_SRV_Var *)P;
	if(!up->SQL_Connect) return;
ShowLog(0,"Free_Var:dbh=%d,Server exit!",up->SQL_Connect->dbh);
	if(up->SQL_Connect->dbh>=0) {
		ret=___SQL_Transaction__(up->SQL_Connect,TRANROLLBACK);
		ret=___SQL_CloseDatabase__(up->SQL_Connect);
	}
}

void showhex(int level,char *title,char *str,int i)
{
int j;
char errbuf[400];
char *p;
	strcpy(errbuf,title);
	strcat(errbuf," ");
	p=errbuf+strlen(errbuf);
	while(i>64) {
		i-=64;
		for(j=0;j<64;j++) p+=sprintf(p,"%02X ",*str++ & 255);
		ShowLog(level,"%s",errbuf);
		p=errbuf;
	}
	for(j=0;j<i;j++) p+=sprintf(p,"%02X ",*str++ & 255);
	ShowLog(level,"%s",errbuf);
}
