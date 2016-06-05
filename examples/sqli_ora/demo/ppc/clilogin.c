#include "sqlcli.h"
#include <unistd.h>
//extern char *getenv(char *);
#include <sys/utsname.h>

clilogin(T_Connect *conn,T_NetHead *nethead)
{

int cc;
char *p,*p1;
char buf[256],pack[240],tmp[200]; // ca leng?
struct utsname ubuf;
int regflg=0;
	*buf=0;
	p1=getlogin();
	if(!p1) p1="sdbc";
	cc=uname(&ubuf);
/*
printf("sysname=%s,nodename=%s,release=%s,version=%s.machine=%s\n",
		 ubuf.sysname,
                 ubuf.nodename,
                 ubuf.release,
                 ubuf.version,
                 ubuf.machine);
*/
	LocalAddr(conn->Socket,pack);
	p=getenv("DEVID");
	if(p && *p) {//正常的登录 
	char uname[17];
		sprintf(buf,"%s|",p);
		p=getenv("LABEL");
		if(p) sprintf(buf+strlen(buf),"%s|%s,%s,%s|||",
				p,ubuf.nodename,p1,pack);
	} else {//还没有进行配置，进行初始配置 
		strcpy(buf,"REGISTER|");
		printf("输入设备文件名:\n");
		scanf("%s",tmp);
		printf("输入校验码:\n");
		scanf("%s",tmp+100);
		sprintf(buf+strlen(buf),"5n9Y7FDGPnEQeW@@|%s,%s,%s|%s|%s|",
				ubuf.nodename,p1,pack,
				tmp,tmp+100);
		regflg=1;
	}
printf("LogString=%s\n",buf);
	nethead->PROTO_NUM=0;
	nethead->data=buf;
	nethead->PKG_LEN=strlen(nethead->data);
	nethead->ERRNO1=conn->MTU;
	p=getenv("TCPTIMEOUT");
	if(p && isdigit(*p)) nethead->ERRNO2=atoi(p);
	else nethead->ERRNO2=0;
	nethead->D_NODE=0;
	nethead->O_NODE=LocalAddr(conn->Socket,0);
	p=getenv("DNODE");
	if(p&&*p) nethead->D_NODE=atoi(p);
//printf("d_node=%d\n",nethead->D_NODE);
	ShowLog(2,"LOGIN");
	cc=SendPack(conn,nethead);
	if(cc) {
		ShowLog(1,"send %d\n",cc);
		return cc;
	}
	cc=RecvPack(conn,nethead);
	ShowLog(2,"%s:cc=%d,errno2=%d,data=%s",__FUNCTION__,cc,nethead->ERRNO2,nethead->data);
if(isatty(0)) printf("login data=%s,timeout=%d\n",nethead->data,conn->timeout);
	
	if(regflg) return regflg;
	return cc;
}
