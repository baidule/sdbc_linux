#include <sccli.h>
#include <unistd.h>
//extern char *getenv(char *);
#include <sys/utsname.h>



extern char netconnect_o_node[16];
clilogin(T_Connect *conn,T_NetHead *NetHead)
{

int cc;
char *p,*p1;
char buf[256],pack[240],tmp[200]; // ca leng?
struct utsname ubuf;
int regflg=0;
	*buf=0;
	p1=getlogin();
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
	if(p && *p) {
	char uname[17];
		sprintf(buf,"%s|",p);
		p=getenv("LABEL");
		if(p) sprintf(buf+strlen(buf),"%s|%s,%s,%s|",
				p,ubuf.nodename,p1,pack);
		printf("Input operator name:\n");
		fgets(uname,sizeof(uname),stdin);
		TRIM(uname);
		printf("Input passwd:\n");
		p=getpass("");
		sprintf(buf+strlen(buf),"%s|%s|",uname,p);
	} else {
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
	NetHead->PROTO_NUM=0;
	NetHead->data=buf;
	NetHead->PKG_LEN=strlen(NetHead->data)+1;
	NetHead->ERRNO1=conn->MTU;
	NetHead->ERRNO2=0;
	NetHead->D_NODE=0;
	NetHead->O_NODE=htonl(LocalAddr(conn->Socket,0));
	p=getenv("DNODE");
	if(p&&*p) NetHead->D_NODE=atoi(p);
printf("d_node=%d\n",NetHead->D_NODE);
	cc=SendPack(conn,NetHead);
	if(cc) {
		ShowLog(0,"send %d\n",cc);
		return cc;
	}
	cc=RecvPack(conn,NetHead);
/*  get gw */
	StrAddr(ntohl(NetHead->O_NODE),netconnect_o_node);
	
	if(regflg) return regflg;
	return cc;
}
