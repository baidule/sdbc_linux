#include <stdio.h>
#define SERVER
#include <DAU.h>
//#include <ctype.h>

#define trans_begin(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANBEGIN)
#define trans_rollback(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANROLLBACK)
#define trans_commit(SQL_Connect)  ___SQL_Transaction__(SQL_Connect,TRANCOMMIT)

JSON_OBJECT get_shift(T_SQL_Connect *SQL_Connect,char *msg)
{
int ret;
DAU u_DAU[2],tdev_DAU;
JSON_OBJECT result,json;
T_PkgType tdev_tpl[2];	////想选几个字段n+1

	ret=DAU_init(&tdev_DAU,SQL_Connect,"TUXDEV",0,0);
	if(ret) {
			ShowLog(1,"get_shift:aft DAU_init tuxdev FAIL");
			return NULL;
	}
	patt_copy(tdev_tpl,tdev_DAU.srm.tp,"devname"); //生成子模板,想选几个字段用，分割列表
	ret=DAU_init(&u_DAU[0],SQL_Connect,"dev_shift a",0,0);
	if(ret) {
			DAU_free(&tdev_DAU);
			ShowLog(1,"get_shift:aft DAU_init dev_shift FAIL");
			return NULL;
	}
	DAU_init(&u_DAU[1],SQL_Connect,"tuxdev b",tdev_DAU.srm.rec,tdev_tpl);
	sprintf(msg,"WHERE a.devid=b.devid");
	ret=DAU_getm(2,u_DAU,msg,0);
	if(ret <= 0) {
printf("get_shift:aft getm ret=%d,stmt=%s\n",ret,msg);
			DAU_freem(2,u_DAU);
			DAU_free(&tdev_DAU);
			return NULL;
	}
	result=json_object_new_array();
	while(!DAU_nextm(2,u_DAU)) {
		json=json_object_new_object();
		DAU_toJSON(&u_DAU[0],json,0);//user全部打包到json
		DAU_toJSON(&u_DAU[1],json,"devname");//组合dev_name到json
		json_object_array_add(result,json);//在JSON数组中加入一条记录
	}
	DAU_freem(2,u_DAU);
	DAU_free(&tdev_DAU);
	return result;
}

int loadfile(T_SQL_Connect *SQL_Connect,char *tablename,FILE *ifd,FILE *ofd,int Pflg,char *buf,int buflen)
{
JSON_OBJECT result;

	result=get_shift(SQL_Connect,buf);
	if(!result) {
			printf("get_shift fail!\n");
			return -1;
	}
	printf("buf=%s\n",buf);
	printf("result=%s\n",json_object_to_json_string(result));
	json_object_put(result);
	return 0;
}

static char my_showid[200];
main(int argc,char *argv[])
{
int ret,i;
T_SQL_Connect SQL_Connect;
int Pflg=0;
char *tabname=0;
FILE *ifd,*ofd;
INT64 now;
char buf[4096];//最大数据长度有限

	tzset();
	sprintf(my_showid,"%s:%d",
		argv[0],getpid());
	Showid=my_showid;
	ifd=0;
	ofd=0;

/*******************************************************************
 *  get Opt
 *******************************************************************/
	for(i=1;i<argc;i++) {
		if(*argv[i]=='-') {
			switch(argv[i][1]) {
			case 'f':
				if(argv[i][2]) ret=envcfg(argv[i]+2);
				else {
					i++;
					ret=envcfg(argv[i]);
				}
				continue;
			case 'P':  //输出 不能加载的记录
				Pflg=1;
				if(argv[i][2]) {
					ofd=fopen(argv[i]+2,"w");
				} else {
					i++;
					if(i<argc) ofd=fopen(argv[i],"w");
				}
				continue;
					
			default:
				fprintf(stderr,"no know option:%s",argv[i]);
 				fprintf(stderr,"Usage:%s -f 配置文件 [-P]  输出文件名 ",
					argv[0]);
				continue;
			}
		}
		tabname=argv[i];
	}
	
ShowLog(5,"tabname=%s",tabname);
	ret=db_open(&SQL_Connect);
	if(ret) {
		ShowLog(1,"Open Database err=%d.%s",
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		return 1;
	}
ShowLog(5,"DB=%s",SQL_Connect.DBOWN);
	if(!ofd) ofd=stdout;
	ifd=stdin;
	
	now=now_sec();
	ret=loadfile(&SQL_Connect,tabname,ifd,ofd,Pflg,buf,sizeof(buf));
	ShowLog(2,"loadasc:load %d rec's time=%d,buf=%s",ret,(int)(now_sec()-now),buf);
	trans_commit(&SQL_Connect);

	if(ofd && ofd != stdout) fclose(ofd);
	ret=___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
