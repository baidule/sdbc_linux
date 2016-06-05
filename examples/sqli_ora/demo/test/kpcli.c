#include <sccli.h>
#include <ctype.h>
#define YEAR_TO_SEC_LEN 21
char YEAR_TO_SEC[]="YYYY-MM-DD HH24:MI:SS";

//char *getenv(char *);
void e_output(char *str);
void phex(char *title,char *str,int i);

T_PkgType reg_type[]={
	{CH_CHAR,41,"DEVID",0,-1},
	{CH_CHAR,256,"LABEL"},
	{-1,0,0}
};

struct reg_s {
	char DEVID[41];
	char LABEL[256];
};
T_PkgType log_type[]={
	{CH_CHAR,41,"DEVID",0,-1},
	{CH_CHAR,41,"Opername"},
	{CH_CHAR,41,"DBUSER"},
	{CH_CHAR,41,"DBOWN"},
	{CH_DATE,YEAR_TO_SEC_LEN,"Logtime",YEAR_TO_SEC},
	{-1,0,0}
};
typedef struct {
	char DEVID[41];
	char Opername[41];
	char DBUSER[41];
	char DBOWN[41];
	char Logtime[YEAR_TO_SEC_LEN];
}log_s ;

main(int argc,char *argv[])
{
char *cp;
T_Connect conn;
T_NetHead NetHead;
int cc,n;
char buf[1024],tmp[1024],tmp1[1024];
char *p,newpass[20];
char values[512];
FILE *fd;
struct reg_s reg_rec;
log_s log;
T_CLI_Var clis;
INT64 btime;

	if(argc < 2) {
		fprintf(stderr,"Usage: %s cfgfile\n",argv[0]);
		return 1;
	}
	if(envcfg(argv[1])) {
		perror(argv[1]);
		return 1;
	}
		
	cp=getenv("HOST");
	if(!cp||!*cp) cp="localhost";
	strcpy(conn.Host,cp);
	cp=getenv("SERVER");
	if(!cp||!*cp) cp="sqlsrv";
	strcpy(conn.Service,cp);
printf("host=%s,server=%s\n",conn.Host,conn.Service);
	*clis.DBOWN=0;
n=0;
//for(n=0;n<1000;n++) {
	cc=Net_Connect(&conn,&clis,0);
	if(cc<0) {
		perror("connect");
		printf("Connect %d %s/%s err %d %d,%s\n",n,
			conn.Host,conn.Service,cc,errno,strerror(errno));
		freeconnect(&conn);
//		sleep(3);
//		continue;
		return 2;
	}
//	ShowLog(2,"connect %d",n);
//	disconnect(&conn);
//	usleep(100000);
//}
//return 0;
	cp=getenv("SENDSIZE");
	if(cp && isdigit(*cp)) {
		conn.MTU=atoi(cp);
	}
	cc=clilogin(&conn,&NetHead);
	if(cc<0||NetHead.ERRNO2) {
		StrAddr(htonl(NetHead.O_NODE),tmp);
		if(NetHead.PKG_LEN) {
			printf("Login cc=%d,err %d :%s,e_node=%s\n",
				cc,NetHead.ERRNO1,NetHead.data,tmp);
		} else {
			printf("Login cc=%d errno2=%d,e_node=%s\n",
			cc,NetHead.ERRNO2,tmp);
		}
		disconnect(&conn);
		return 5;
	}
	if(NetHead.PKG_LEN) ShowLog(3,"Login success %s\n",NetHead.data);
	else  ShowLog(3,"Login success \n");
	if(cc==1) { // REGISTER
		cp=NetHead.data;
		cp+=net_dispack(&reg_rec,cp,reg_type);
		fd=fopen(argv[1],"a");
		if(!fd) {
			perror(argv[1]);
			disconnect(&conn);
			return(0);
		}
		fprintf(fd,"DEVID=%s\nLABEL=%s\n",
			reg_rec.DEVID,reg_rec.LABEL);
		fclose(fd);
		disconnect(&conn);
		return 0;
	}
	net_dispack(&log,NetHead.data,log_type);

	NetHead.PROTO_NUM=30;	//net_showlog
	NetHead.ERRNO1=3;	//LOGLEVEL
	NetHead.ERRNO2=PACK_NOANSER;	//不必回答
	NetHead.PKG_REC_NUM=1;		//设定新的Showid
	NetHead.O_NODE=htonl(LocalAddr(conn.Socket,0)); //报告本端地址

	sprintf(buf,"%s|测试net_log,err=%d,%s","SP0102003",
		943,"my error 943");
	NetHead.data=buf;
	NetHead.PKG_LEN=strlen(buf)+1;
	cc=SendPack(&conn,&NetHead);

	disconnect(&conn);
	return 0;
}
FreeVar(char *p)
{
}
void phex(char *title,char *str,int i)
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
		printf("%s\n",errbuf);
		p=errbuf;
	}
	for(j=0;j<i;j++) p+=sprintf(p,"%02X ",*str++ & 255);
	printf("%s\n",errbuf);
}
