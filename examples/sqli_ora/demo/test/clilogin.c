#include <string.h>
#include <atmi.h>
#define CLIENT
#include <sdbc.h>
#include <stdio.h>
#include <unistd.h>
//extern char *getenv(char *);
#include <sys/utsname.h>


clilogin(char **bufp)
{

int cc;
char *p,*p1,*sendbuf,*rcvbuf;
char pack[240],tmp[200]; // ca leng?
struct utsname ubuf;
int regflg=0;
long sendlen,rcvlen;
	*bufp=0;
	rcvlen=0;
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
        if((sendbuf = (char *) tpalloc("CARRAY", NULL, 100)) == NULL) {
                ShowLog(1,"Error allocating send buffer\n");
		return -1;
        }
        if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 100)) == NULL) {
                ShowLog(1,"Error allocating send buffer\n");
		return -1;
        }
	p=getenv("DEVID");
	if(p && *p) {
	char uname[17];
		sprintf(sendbuf,"%s|",p);
		p=getenv("LABEL");
		sprintf(sendbuf+strlen(sendbuf),"%s|%s,%s|",
				p,ubuf.nodename,p1);
		printf("Input operator name:\n");
		fgets(uname,sizeof(uname),stdin);
		TRIM(uname);
		printf("Input passwd:\n");
		p=getpass("");
		sprintf(sendbuf+strlen(sendbuf),"%s|%s|",uname,p);
	} else {
		strcpy(sendbuf,"REGISTER|");
		printf("输入设备文件名:\n");
		scanf("%s",tmp);
		printf("输入校验码:\n");
		scanf("%s",tmp+100);
		sprintf(sendbuf+strlen(sendbuf),"5n9Y7FDGPnEQeW@@|%s,%s,%s|%s|",
				ubuf.nodename,p1,
				tmp,tmp+100);
		regflg=1;
	}
printf("LogString=%s\n",sendbuf);
	sendlen=strlen(sendbuf)+1;

        /* Request the service occupy, waiting for a reply */
        cc = tpcall("login", sendbuf, sendlen, &rcvbuf, &rcvlen, (long)0);
	
ShowLog(2,"Login:%s,cc=%d",rcvbuf,cc);
	*bufp=rcvbuf;
	tpfree(sendbuf);

	if(cc) return cc;
	if(regflg) return regflg;
	return rcvlen;
}

int chpwd()
{
char * sendbuf,*recvbuf,*p;
char buf[100];
int cc;
long recvlen;

	if((sendbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		fprintf(stderr,"Error allocating send buffer\n");
		return -1;
	}
	if((recvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		fprintf(stderr,"Error allocating recv buffer\n");
		return -1;
	}
	strcpy(sendbuf,getpass("改变用户口令，请输入老口令:"));
	strcat(sendbuf,"|");
	strcpy(buf,getpass("请输入新口令:"));
	p=getpass("请再输入一次新口令:");
	if(strcmp(buf,p)) {
		fprintf(stderr,"两次口令不同，修改口令失败!\n");
		tpfree(sendbuf);
		tpfree(recvbuf);
		return -2;
	}
	strcat(sendbuf,buf);
	strcat(sendbuf,"|");
        cc = tpcall("change_pwd", sendbuf, strlen(sendbuf), &recvbuf, &recvlen, (long)0);
	if(cc) printf("%s\n",recvbuf);
	return cc;
}
