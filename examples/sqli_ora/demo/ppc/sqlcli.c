#include "sqlcli.h"

#include "../srvcomm/logs.tpl" 

T_PkgType reg_type[]={
	{CH_CHAR,41,"DEVID",0,-1},
	{CH_CHAR,256,"LABEL"},
	{-1,0,0}
};

struct reg_s {
	char DEVID[41];
	char LABEL[256];
};

static char my_showid[100];
main(int argc,char *argv[])
{
char *cp;
T_Connect conn;
svc_table svct;
T_NetHead NetHead;
int cc,n;
char buf[1024],tmp[1024],tmp1[1024];
char *p,newpass[20];
char values[512];
FILE *fd;
struct reg_s reg_rec;
log_stu log;
T_CLI_Var clis;
INT64 now;
int loop=1;


	if(argc < 2) {
		fprintf(stderr,"Usage: %s cfgfile\n",argv[0]);
		return 1;
	}
	if(envcfg(argv[1])) {
		perror(argv[1]);
		return 1;
	}
	tzset();
	if(argc>2) {
		loop=atoi(argv[2]);
		if(loop<1) loop=1;
	}
		
	sprintf(my_showid,"%s:%d",argv[0],getpid());
	Showid=my_showid;

	cp=getenv("HOST");
	if(!cp||!*cp) cp="localhost";
	strcpy(conn.Host,cp);
	cp=getenv("SERVER");
	if(!cp||!*cp) cp="sqlsrv";
	strcpy(conn.Service,cp);
	Init_CLI_Var(&clis);
	clis.var=&log;
	clis.svc_tbl=&svct;
/* 测试连接 
	n=tcpopen(conn.Host,conn.Service);
	if(n>=0) {
		printf("tcpopen return %d\n",n);
		close(n);
	} else {
		printf("tcpopen err=%d,%s\n",errno,strerror(errno));
		return 2;
	}
*/
	conn.timeout=0;
//	sleep(1);//集结，先后启动的进程在同一秒点发起连接。
	cc=Net_Connect(&conn,&clis,0);
printf("host=%s,server=%s,cc=%d,timeout=%d\n",conn.Host,conn.Service,cc,conn.timeout);
	if(cc<0) {
		perror("connect");
		printf("Connect  %s/%s err %d %d,%s\n",
			conn.Host,conn.Service,cc,errno,strerror(errno));
		freeconnect(&conn);
		return 2;
	}
	cp=getenv("SENDSIZE");
	if(cp && isdigit(*cp)) {
		conn.MTU=atoi(cp);
	}
	conn.timeout=120;
//登录服务器
	cc=clilogin(&conn,&NetHead);
	if(cc<0||NetHead.ERRNO2) {
		StrAddr(htonl(NetHead.O_NODE),tmp);
		if(NetHead.PKG_LEN) {
			ShowLog(1,"Login cc=%d,err %d :%s,e_node=%s\n",
				cc,NetHead.ERRNO1,NetHead.data,tmp);
		} else {
			ShowLog(1,"Login cc=%d errno2=%d,e_node=%s\n",
			cc,NetHead.ERRNO2,tmp);
		}
		disconnect(&conn);
		return 5;
	}
	if(NetHead.PKG_LEN) ShowLog(3,"Login success %s,timeout=%d\n",NetHead.data,conn.timeout);
	else  ShowLog(3,"Login success \n");
//自动设置标签 
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

	net_dispack(&log,NetHead.data,log_tpl);
	stptok(log.DBOWN,clis.DBOWN,sizeof(clis.DBOWN),0);
//取服务名 
        cc=init_svc_no(&conn);
        if(cc) {
                ShowLog(1,"init_svc_no Error!");
		disconnect(&conn);
		return 1;
        }
	now=now_sec();
	for(n=0;n<loop;n++) {
//		cc=cli_prepare(&conn,&NetHead);
//测试select语句
//		cc=testselect(&conn,&NetHead);
//分页查询
		cc=testpage(&conn,&NetHead);
//		if((now_sec()-now) > 150) {
//			ShowLog(5,"%s:n=%d,TCB=%d,TIM=%d",__FUNCTION__,n,log.TCB,
//				(int)(now_sec() - now));
//			sleep(1);
//			now=now_sec();
//		}
//		cc=price_cli(&conn,&NetHead);
		if(cc) break;
	}
	disconnect(&conn);
	ShowLog(2,"%s:term!",log.DBUSER);
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
