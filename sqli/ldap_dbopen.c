#include <sqli.h>
#include <dw.h>
#include <scry.h>
#include <ldap_dbopen.h>
#include <crc32.h>

#ifdef TEST_TMVAL
#include <sys/time.h>
static long interval(struct timeval *begtime,struct timeval *endtime)
{
		long ret;
        ret=endtime->tv_sec-begtime->tv_sec;
        ret*=1000000;
        ret += endtime->tv_usec - begtime->tv_usec;
        return ret;
}
#endif

T_PkgType ldap_auth_tpl[]={
        {CH_CHAR,256,"host",0,-1}, //LDAP的主机 地址
        {CH_SHORT,sizeof(short),"port"}, //LDAP的端口
        {CH_CHAR,MAX_DN,"user_dn"},
        {CH_CHAR,16,"pwd"},
        {-1,0,0,0}
};


extern char *(*encryptproc)(char *mstr);
extern int ldap_auth(LDAP *ld,char *auth_dn,char *sid,char *uid,char *pwd,char *dbown);

static char keyid[20]="";

static char *encryptpass(char *mstr)
{
static struct crypt_s cryptk={"","","","",""};
int ret;
char tmp[41];
        if(!*keyid) return mstr;
        ret=a64_byte(tmp,mstr);
        crypt_password(keyid+2,keyid,tmp,ret,&cryptk);
        tmp[ret]=0;
        strcpy(mstr,tmp);
        return mstr;
}

static char *encodeprepare(char *dblabel)
{
char *p;
DWS dw;
int ret;
/********************************************************************
 * 用户口令解密准备
 ********************************************************************/
	p=getenv("KEYFILE");
	if(!p||!*p) {
		ShowLog(1,"缺少环境变量 KEYFILE");
		ret=-1;
	} else {
		ret=initdw(p,&dw);
		if(ret) {
			ShowLog(1,"Init dw %s error %d",p,ret);
		}
	}
	if(!ret) {
int crc;
char *cp;
		crc=ssh_crc32((unsigned char *)dblabel,strlen(dblabel));
		cp=getdw(crc,&dw);
		if(!cp) {
			ShowLog(1,"无效的 DBLABEL %s",dblabel);
		} else {
			strcpy(keyid,cp);
			encryptproc=encryptpass;
		}
		freedw(&dw);
	}
	return dblabel;
}

int ldap_dbopen(T_SQL_Connect *SQL_Connect,LDAP *ld,char *dblabel)
{
char *p;
int ret;
char auth_dn[256],buf[256];
#ifdef TEST_TMVAL
struct timeval begtime,endtime;
#endif

	p=getenv("AUTH_DN");
	if(!p || !*p) {
			ShowLog(1,"缺少环境变量 AUTH_DN!");
			return -1;
	}
	strcpy(auth_dn,p);
	sprintf(buf,"DBLABEL=%s,",dblabel);
	strsubst(auth_dn,0,buf);

	encodeprepare(dblabel);

#ifdef TEST_TMVAL
    gettimeofday(&begtime,0);
#endif

   	ret=ldap_auth(ld,auth_dn,
               SQL_Connect->DSN,
               SQL_Connect->UID,
               SQL_Connect->PWD,
               SQL_Connect->DBOWN);
	
#ifdef TEST_TMVAL
	gettimeofday(&endtime,0);
	ShowLog(5,"ldap_dbopen: aft ldap_auth TIMEVAL=%ld",interval(&begtime,&endtime));
#endif

	if(ret) {
			SQL_Connect->Errno=ret;
			strcpy(SQL_Connect->ErrMsg,"LDAP_AUTH Error");
			return -2;
    }

    ret=___SQL_OpenDatabase__(SQL_Connect);
    if(ret) {
    	ShowLog(1,"ldap_dbopen Open Database error %d,%s",
						ret,
						SQL_Connect->Errno,
						SQL_Connect->ErrMsg);
    }
    return ret;
}

