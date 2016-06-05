#include <unistd.h>
#include <DAU.h>
#include <ctype.h>
#include "prt_tpl.h"

extern int mksrm(SRM *srmp,T_SQL_Connect *SQL_Connect);

int main(int argc,char *argv[])
{
char buf[100],cfile[100],hfile[100];
int ret,i;
SRM srm;
T_SQL_Connect SQL_Connect;
int Jflg=0;
FILE *cfd,*hfd;
int lines=0;
char *p;
INT64 now;
char *tplname=NULL;

	*cfile=0,*hfile=0;
	cfd=hfd=0;
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
			case 'J':  //输出 JSON 脚本
				Jflg=1;
				continue;
					
			case 'j':  //输出 JSON 脚本
				Jflg=3;
				continue;
					
			default:
				fprintf(stderr,"no know option:%s",argv[i]);
 				fprintf(stderr,"Usage:%s -f 配置文件 [-J]  输出文件名 ",
					argv[0]);
				continue;
			}
		}
		if(Jflg) {
			sprintf(cfile,"%s.json",argv[i]);
			sprintf(hfile,"%s.java",argv[i]);
			tplname=argv[i];
		} else {
			sprintf(cfile,"%s.tpl",argv[i]);
			sprintf(hfile,"%s.stu",argv[i]);
		}
	}
	p=getenv("COL_TO_LOWER");
	if(p && isdigit(*p)) col_to_lower=atoi(p);

	Showid=argv[0];
	ret=db_open(&SQL_Connect);
	if(ret) {
		ShowLog(1,"Open Database err=%d,%s",
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		return 1;
	}
	if(!*cfile) {
		cfd=stdout;
	} else {
		if(!(cfd=fopen(cfile,"w"))) {
			perror(cfile);
			___SQL_CloseDatabase__(&SQL_Connect);
			return 1;
		}
	}
	
	if(!*hfile) {
		hfd=stdout;
	} else {
		if(!(hfd=fopen(hfile,"w"))) {
			perror(hfile);
			___SQL_CloseDatabase__(&SQL_Connect);
			return 1;
		}
	}

	for(lines=0;!ferror(stdin);lines++) {
		if(isatty(0)) {
			printf("输入表名： ");
			fflush(stdout);
		}
		fgets(buf,sizeof(buf),stdin);
		if(feof(stdin)) break;
		TRIM(buf);
		if(!*buf) continue;
		strupper(buf);
		srm.tabname=buf;
now=now_usec();
		ret=mksrm(&srm,&SQL_Connect);
ShowLog(5,"mksrm %s ,cols=%d,ret=%d,INTERVAL=%d",srm.tabname,abs(srm.Aflg),ret,INTERVAL(now));
		if(ret) {
			printf("/* Can not find the table %s! */\n",buf);
			ShowLog(1,"%s:表名错：err=%d,%s",buf,
				SQL_Connect.Errno,SQL_Connect.ErrMsg);
			continue;
		}
		p=strchr(srm.tabname,'.');
		if(!p) p=(char *)srm.tabname;
		else p++;
		if(Jflg) {	//JSON模板
			if(lines==0) {
				if(tplname) fprintf(cfd,"String %s=",tplname);
				fprintf(cfd,"\"{\" +\n");
			} else fprintf(cfd,",\" +\n");
			if(Jflg==1) {  //生成最方便的SDBC POJO
				print_JSON(cfd,srm.tp,p);
				print_JAVA(hfd,srm.tp,p);
			} else if(Jflg==3) {	//生成标准POJO
				print_json(cfd,srm.tp,p);
				print_java(hfd,srm.tp,p);
			}
			
		} else {      //C模板
			print_type(cfd,srm.tp,p);
			print_struct(hfd,srm.tp,p);
		}
		SRM_free(&srm);
	}
	if(Jflg) fprintf(cfd,"}\";\n");

	if(cfd && cfd != stdout) fclose(cfd);
	if(hfd && hfd != stdout) fclose(hfd);
	ret=___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
