#include "sqlsrv.h"
#include <dw.h>
#include <unistd.h>
#include <scry.h>
#include <enigma.h>

#include <logs.tpl>

extern srvfunc Function[];

static ENIGMA2 dbegm;
char *decryptpass(char *mstr)
{
int ret;
char tmp[41];
        ret=a64_byte(tmp,mstr);
	enigma2_decrypt(&dbegm,tmp,ret);
        tmp[ret]=0;
        strcpy(mstr,tmp);
        return mstr;
}

int login(T_Connect *conn,T_NetHead *NetHead)
{
time_t tim;
struct tm *timp;
int ret,crc;
char tmp[200];
char *cp,*key;
char tmp1[1024],cliaddr[20];
DWS dw;
login_stu logrec;
log_stu logret;
ENIGMA2 egm;
FILE *fd;
T_SRV_Var *srvp;
GDA *gp;
//u_int e[RSALEN],m[RSALEN];

	srvp=(T_SRV_Var *)conn->Var;
	gp=(GDA *)srvp->var;
	StrAddr(NetHead->O_NODE,cliaddr);
ShowLog(5,"%s:SDBC5.0 Client IP Addr=%s,Net_login %s",__FUNCTION__,cliaddr,NetHead->data);
	net_dispack(&logrec,NetHead->data,login_tpl);
	strcpy(gp->devid,logrec.DEVID);
	sprintf(gp->ShowID,"%s:%s:%d",logrec.DEVID,cliaddr,getpid());
	Showid=gp->ShowID;
	if((int)NetHead->ERRNO1>0) conn->MTU=NetHead->ERRNO1;
	if((int)NetHead->ERRNO2>0) conn->timeout=NetHead->ERRNO2;
	srvp->o_timeout=conn->timeout;

	cp=getenv("KEYFILE");
	if(!cp||!*cp) {
		strcpy(tmp1,"缺少环境变量 KEYFILE");
errret:
		ShowLog(1,"Login Error %s:%s",__FUNCTION__,tmp1);
		NetHead->ERRNO1=-1;
		NetHead->ERRNO2=-1;
		NetHead->O_NODE=LocalAddr(conn->Socket,tmp);
		NetHead->PKG_REC_NUM=0;
		NetHead->data=tmp1;
		NetHead->PKG_LEN=strlen(NetHead->data);
    		SendPack(conn,NetHead);
		return 0; // fail
	}
/* read key */
	ret=initdw(cp,&dw);
	if(ret) {
		sprintf(tmp1,"Init dw KEYFILE=%s error %d",cp,ret);
		goto errret;
	}
	crc=ssh_crc32(logrec.DEVID,strlen(logrec.DEVID));
	key=getdw(crc,&dw);
	if(!key) {
		freedw(&dw);
                sprintf(tmp1,"无效的 DEVID");
                goto errret;
        }

//ShowLog(5,"getdw key=%s",key);
	enigma2_init(&egm,key,0);
/* check CA */
	memset(gp->operid,0,sizeof(gp->operid));
	cp=getenv("CADIR");
	if(!cp||!*cp) cp=".";
    if(strcmp(gp->devid,"REGISTER")) {
	strncpy(gp->operid,logrec.UID,sizeof(gp->operid)-1);
	sprintf(tmp,"%s/%s.CA",cp,logrec.DEVID);
ShowLog(5,"CAfile=%s,key=%s",tmp,key);
	fd=fopen(tmp,"r");
	if(!fd) {
		if(errno==2) {
		    crc=strlen(logrec.CA);
		    enigma2_encrypt(&egm,logrec.CA,crc);
		    byte_a64(tmp1,logrec.CA,crc);
//ShowLog(5,"CA=%s",tmp1);
		    fd=fopen(tmp,"w");
		    if(!fd) {
			sprintf(tmp1,"write %s err=%d",tmp,errno);
err1:
			freedw(&dw);
			goto errret;
		    }
		    fprintf(fd,"%s\n",tmp1);
		    fclose(fd);
		} else {
			sprintf(tmp1,"open CAfile %s err=%d",tmp,errno);
			goto err1;
		}
	} else {
		fgets(tmp1,sizeof(logrec.CA),fd);
		fclose(fd);
		TRIM(tmp1);
		ret=a64_byte(tmp,tmp1);
		enigma2_decrypt(&egm,tmp,ret);
		tmp[ret]=0;
		if(strcmp(tmp,logrec.CA)) {
			sprintf(tmp1,"CA 错误");
//ShowLog(5,"%s CA=%s log=%s len=%d",tmp1,tmp,logrec.CA,ret);
			goto err1;
		}
	}
    } else {   //未注册客户端注册
	char *p;
	char *keyD;
/* REGISTER label|CA|devfile|CHK_Code| */
/*
//???这有问题，5,0版这些函数未必在这 
	Function[8].funcaddr=Echo; 		// SQL_Exec
	Function[8].srvname=Function[1].srvname;
	Function[11].funcaddr=Echo; 		// Rexec
	Function[11].srvname=Function[1].srvname;
	Function[13].funcaddr=Echo;		// Putfile
	Function[13].srvname=Function[1].srvname;
*/

ShowLog(2,"REGISTER %s",logrec.UID);
	if(!*logrec.UID) {
		sprintf(tmp1,"REGSTER is empty!");
		goto err1;
	}
//uid=devfile
	crc=0xFFFF&gencrc(logrec.UID,strlen(logrec.UID));
//pwd=CHK_Code
	sscanf(logrec.PWD,"%04X",&ret);
	ret &= 0xFFFF;
	if(ret != crc) {
		sprintf(tmp1,"REGISTER:devfile CHK Code error! ",
				crc,ret);
		goto err1;
	}
	p=stptok(logrec.UID,logrec.DEVID,sizeof(logrec.DEVID),".");//logrec.DEVID=准备注册的DEVID
	crc=ssh_crc32(logrec.DEVID,strlen(logrec.DEVID));
	keyD=getdw(crc,&dw);
	if(!keyD) {
		sprintf(tmp1,"注册失败,%s:没有这个设备！",
				logrec.DEVID);
		goto err1;
	}
	enigma2_init(&egm,keyD,0);
	sprintf(tmp,"%s/%s.CA",cp,logrec.DEVID);
ShowLog(5,"REGISTER:%s",tmp);
	if(fd=fopen(tmp,"r")) {
		fgets(tmp1,81,fd);
		fclose(fd);
		TRIM(tmp1);
		ret=a64_byte(tmp,tmp1);
		enigma2_decrypt(&egm,tmp,ret);
		tmp[ret]=0;
		if(strcmp(tmp,logrec.CA)) {
			sprintf(tmp1,"注册失败,%s 已被注册,使用中。",
					logrec.DEVID);
			goto err1;
		}
	} else if(errno != 2) {
		sprintf(tmp1,"CA 错误");
		goto err1;
	}
/*把设备特征码写入文件*/
	fd=fopen(tmp,"w");
	if(fd) {
	int len=strlen(logrec.CA);
		enigma2_decrypt(&egm,logrec.CA,len);
		byte_a64(tmp1,logrec.CA,len);
		fprintf(fd,"%s\n",tmp1);
		fclose(fd);
	}
	else ShowLog(1,"net_login:REGISTER open %s for write,err=%d,%s",
		tmp,errno,strerror(errno));

	freedw(&dw);
	sprintf(tmp,"%s/%s",cp,logrec.UID);
	fd=fopen(tmp,"r");
	if(!fd) {
		sprintf(tmp1,"REGISTER 打不开文件 %s err=%d,%s",
					logrec.CA,errno,strerror(errno));
		goto errret;
	}
	fgets(logrec.UID,sizeof(logrec.UID),fd);
	TRIM(logrec.UID);
	ShowLog(2,"REGISTER open %s",tmp);
	fclose(fd);
	cp=tmp1;
	cp+=sprintf(cp,"%s|%s|", logrec.DEVID,logrec.UID);
	cp+=sprintf(cp,"%s|",rsecstrfmt(tmp,now_sec(),YEAR_TO_SEC));
	NetHead->data=tmp1;
	NetHead->PKG_LEN=strlen(NetHead->data);
	NetHead->ERRNO1=0;
	NetHead->ERRNO2=0;
	NetHead->O_NODE=LocalAddr(conn->Socket,tmp);
	NetHead->PKG_REC_NUM=0;
//showhex(5,tmp,(char *)&NetHead->O_NODE,4);
    	SendPack(conn,NetHead);
	return -1;
    } //未注册客户端注册完成

/* decode label */
	ret=a64_byte(tmp1,logrec.LABEL);
	enigma2_decrypt(&egm,tmp1,ret);
	tmp1[ret]=0;
	strcpy(logrec.LABEL,tmp1);

ShowLog(5,"LABEL=%s,tmp1=%s,key=%s",logrec.LABEL,tmp1,key);
/* decode DBLABEL's KEY */
	crc=ssh_crc32(tmp1,strlen(tmp1));
	key=getdw(crc,&dw);
	if(!key) {
                sprintf(tmp1,"无效的 DBLABEL's KEY for %s",logrec.LABEL);
		goto err1;
        } else {
//ShowLog(5,"net_login DBLABEL=%s,key=%s",tmp1,key);
		enigma2_init(&dbegm,key,0);
        	encryptproc=decryptpass;
	}
	freedw(&dw);


	cp=getenv("DATABASEAUTHFILE");
	if(!cp||!*cp) {
		strcpy(tmp1,"缺少环境变量 DATABASEAUTHFILE");
		goto errret;
	}
	
/*
ShowLog(5,"AUTH=%s,label=%s,len=%d",cp,logrec.LABEL,ret);
*/
	ret=SQL_AUTH(cp,logrec.LABEL,
		srvp->SQL_Connect->DSN,
		srvp->SQL_Connect->UID,
		srvp->SQL_Connect->PWD,
		srvp->SQL_Connect->DBOWN);
	if(ret) {
		sprintf(tmp1,"SQL_AUTH err %d",ret);
		goto errret;
	}
/*
ShowLog(5,"SID=%s,UID=%s,PWD=%s,dbown=%s",
		&srvp->SQL_Connect->DSN,
		&srvp->SQL_Connect->UID,
		&srvp->SQL_Connect->PWD,
		&srvp->SQL_Connect->DBOWN);
*/
	ret=___SQL_OpenDatabase__(srvp->SQL_Connect);
	if(ret) {
		sprintf(tmp1,"OpenDatabase err:%d,%s,Nat=%d,%s",
			srvp->SQL_Connect->Errno,
			srvp->SQL_Connect->ErrMsg,
			srvp->SQL_Connect->NativeError,
			srvp->SQL_Connect->SqlState);
		goto errret;
	}
	ShowLog(2,"DATABASE %s Opened!",srvp->SQL_Connect->DBOWN);
	gp->trans_tid=0;
/* 数据库已经打开，应该验证OPERID，并完成授权设置 */

        strcpy(logret.DEVID,gp->devid);
        strcpy(logret.UID,gp->operid);
        strcpy(logret.DBUSER,srvp->SQL_Connect->UID);
        strcpy(logret.DBOWN,srvp->SQL_Connect->DBOWN);
	if(NetHead->PKG_REC_NUM == -1) //不谋求ctx_id
        	strcpy(logret.DBLABEL,logrec.LABEL);
	else *logret.DBLABEL=0; //暂不支持ctx_login
        rsecstrfmt(logret.Logtime,now_sec(),YEAR_TO_SEC);
        net_pack(tmp1,&logret,log_tpl);

	NetHead->data=tmp1;
	NetHead->PKG_LEN=strlen(NetHead->data);
	NetHead->ERRNO1=0;
	NetHead->ERRNO2=0;
	NetHead->O_NODE=LocalAddr(conn->Socket,tmp);
	NetHead->PKG_REC_NUM=0;
//showhex(5,tmp,(char *)&NetHead->O_NODE,4);
    	NetHead->ERRNO1=SendPack(conn,NetHead);
	ShowLog(2,"%s:%s success",__FUNCTION__,tmp1);
	return 1;
}

