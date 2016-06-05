#include "xcomd.h"
#include <bignum.h>
#include <enigma.h>
#include <unistd.h>
#include <libgen.h>
#include <crc32.h>
#include <sys/resource.h>

int net_login(T_Connect *,T_NetHead *);
extern int substitute_env(char *line);

void showhex(int level,char *title,char *str,int i);

/*********************************************************
 * Can add other functions into Function[] by user       *
 *********************************************************/

//int (*Function[])(T_Connect *,T_NetHead *)={
srvfunc Function[]={
	{net_login,"NetLogin"},		/*0*/
	{Echo,"Echo"},			/*1*/
	{Rexec,"Rexec"},
	{GetFile,"GetFile"},
	{PutFile,"PutFile"},
	{Pwd,"Pwd"},
	{ChDir,"ChDir"},
	{filels,"filels"},
	{PutEnv,"PutEnv"},	
	{busToServer,"busToServer"},
	{serverToBus,"serverToBus"},
	{0,0}
};
void quit(int);
void netinit(T_Connect *connect ,T_NetHead *NetHead);
extern FILE *logfile;
long today;

static char myshowid[100];
int main(int ac,char *av[])
{
char *cp;
int ret;
struct rlimit sLimit;

//设置可以core dumpped
        sLimit.rlim_cur = -1;
        sLimit.rlim_max = -1;
        ret=setrlimit(RLIMIT_CORE,(const struct rlimit *)&sLimit);

	if(ac>1){
		ret=envcfg(av[1]);
	}

	tzset();
	cp=getenv("GBK_FLAG"); //汉字字符集
	if(cp && *cp=='T') GBK_flag=1;

	cp=getenv("WDIR"); //取工作目录 
	if(!cp || !*cp) cp=getenv("HOME");
	if(cp && *cp) chdir(cp);
	sprintf(myshowid,"%s:%d",sc_basename(av[0]),getpid());
	Showid=myshowid;
ShowLog(2,"start!");
	ret=TPOOL_srv(netinit,quit,NULL,sizeof(GDA));

	quit(ret);
	return 0;
}
void quit(int n)
{
	ShowLog(0,"quit %d",n);
	ShowLog(-1,0);
	tpool_free();
	exit(n);
}

void FreeVar(void *P)
{
}

void netinit(T_Connect *connect ,T_NetHead *NetHead)
{
char addr[16],*cp;
int ret;
//T_SRV_Var *srvp=(T_SRV_Var *)connect->Var;
//GDA *ctx=(GDA *)srvp->var;

	connect->freevar=FreeVar;
	connect->Event_proc=event_put;//事件处理器 
	cp=getenv("SENDSIZE");
	if(cp && *cp) {
		ret=atoi(cp);
		connect->MTU=ret;
	}
	peeraddr(connect->Socket,addr);
	ShowLog(0,"连接 %s",addr);
}

#include <dw.h>
struct log_s {
	char devid[17];
	char label[51];
	char CA[180];
};
T_PkgType login_type[]={
	{CH_CHAR,17,"devid",0,-1},
	{CH_CHAR,51,"label"},
	{CH_CHAR,180,"CA"},
	{-1,0,0}
};
#include <arpa/inet.h>
int net_login(T_Connect *conn,T_NetHead *NetHead)

{
T_SRV_Var *srvp=(T_SRV_Var *)conn->Var;
GDA *gp=(GDA *)srvp->var;
int ret;
char err[100],tmp[256],tmp1[1024];
char *p;
struct log_s logrec;
INT64 now=now_sec();

	ret=1;
	if(!NetHead->PKG_LEN) {
		ShowLog(1,"log is empty!");
		return -1;
	}
	if(NetHead->ERRNO2>0) conn->MTU=NetHead->ERRNO2;
	if(NetHead->ERRNO1>0) conn->timeout=NetHead->ERRNO1;
	
	p=NetHead->data;	
	p+=net_dispack(&logrec,p,login_type);
	peeraddr(conn->Socket,err);
	strcpy(gp->devid,logrec.devid);
	sprintf(gp->Showid,"%s:%s:%d",logrec.devid,err,srvp->TCB_no);
	mthr_showid_add(srvp->tid,gp->Showid);
	srvp->poolno=0;
	rsecstrfmt(tmp,now,YEAR_TO_SEC);
	ShowLog(3,"login:%s,len=%d,t_len=%d,%s",StrAddr(htonl(NetHead->O_NODE),err),
			NetHead->PKG_LEN,NetHead->T_LEN,NetHead->data);

	ret=1;
	sprintf(tmp1,"%s|%s|",logrec.devid,tmp);
	NetHead->data=tmp1;
	NetHead->PKG_LEN=strlen(NetHead->data);
	NetHead->ERRNO1=0;
	NetHead->ERRNO2=0;
	NetHead->PKG_REC_NUM=0;
	NetHead->O_NODE=LocalAddr(conn->Socket,err);
//showhex(5,err,(char *)&NetHead->O_NODE,4);
    	SendPack(conn,NetHead);
ShowLog(3,"%s:succeed,O_NODE=%s %s",__FUNCTION__,err,NetHead->data);
	return ret;
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

void set_showid(void *ctx)
{
GDA *gp=(GDA *)ctx;
pthread_t tid=pthread_self();
        if(!ctx) return;
        mthr_showid_add(tid,gp->Showid);
}

