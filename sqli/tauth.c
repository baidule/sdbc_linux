/********************************************************************
 * 本程序demo使用LDAP读取数据库登录参数 。 
 *
 ********************************************************************/
#include <sqli.h>
#include <ldap_auth.h>
#include <ldap_dbopen.h>

#define LDAP_AUTH_ENV "LDAP_AUTH"
//usage: tauth ld.ini
main(int argc,char *argv[])
{
int ret;
char *p;
T_SQL_Connect SQL_Connect,SQL1,SQL2;
char buf[256];
ldap_auth_stu auth;
LDAP *ld;
	if(argc>1) envcfg(argv[1]);
	p=getenv(LDAP_AUTH_ENV);
	if(!p || !*p) {
			ShowLog(1,"ldap_auth:缺少环境变量 %s！",LDAP_AUTH_ENV);
			return -1;
	}
	strcpy(buf,p);
	net_dispack(&auth,buf,ldap_auth_tpl);
	if(auth.port <= 0) auth.port=LDAP_PORT;
	ld=ldap_connect(auth.host,auth.port,auth.user_dn,auth.pwd);
	if(!ld) {
		ShowLog(1,"LDAP_Connect fail!");
		exit(1);
	}

	p=getenv("DBLABEL");
	if(!p || !*p) {
		ShowLog(1,":缺少环境变量 DBLABEL!");
		return 1;
	}
	ret=ldap_dbopen(&SQL_Connect,ld,p);
	if(ret) {
			ShowLog(1,"ldap_dbopen error!");
			return 2;
	}
	printf("open database:%s,%s\n",SQL_Connect.DBOWN,SQL_Connect.UID);
// 以下是分别打开3个数据库的例子 
	p=getenv("DBLABEL1");
	if(!p || !*p) {
			ShowLog(1,":缺少环境变量 DBLABEL1!");
			___SQL_CloseDatabase__(&SQL_Connect);
			return 1;
	}
	ret=ldap_dbopen(&SQL1,ld,p);
	if(ret) {
			ShowLog(1,"ldap_dbopen SQL1 error!");
			___SQL_CloseDatabase__(&SQL_Connect);
			return 2;
	}
	printf("open database SQL1:%s,%s\n",SQL1.DBOWN,SQL1.UID);

	p=getenv("DBLABEL2");
	if(!p || !*p) {
			ShowLog(1,":缺少环境变量 DBLABEL2!");
			___SQL_CloseDatabase__(&SQL_Connect);
			___SQL_CloseDatabase__(&SQL1);
			return 1;
	}
	ret=ldap_dbopen(&SQL2,ld,p);
	if(ret) {
			ShowLog(1,"ldap_dbopen SQL2 error!");
			___SQL_CloseDatabase__(&SQL_Connect);
			___SQL_CloseDatabase__(&SQL1);
			return 2;
	}
	printf("open database SQL2:%s,%s\n",SQL2.DBOWN,SQL2.UID);
			
    ldap_unbind(ld);
	___SQL_CloseDatabase__(&SQL_Connect);
	___SQL_CloseDatabase__(&SQL1);
	___SQL_CloseDatabase__(&SQL2);
	return 0;
}
