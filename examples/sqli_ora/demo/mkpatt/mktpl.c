#include <unistd.h>
#include <SRM.h>
#include <ctype.h>
#include "prt_tpl.h"

extern char *meta_tpl(SRM *srmp,char *str);

int main(int argc,char *argv[])
{
char buf[100],cfile[100],hfile[100],*p;
int ret,i;
SRM srm;
int Jflg=0;
FILE *cfd,*hfd;
int lines=0;
char *namep;

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
			case 'J':  //输出 SDBC JSON 脚本
				Jflg=1;
				continue;
					
			case 'j':  //输出 标准JSON 脚本
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
		} else {
			sprintf(cfile,"%s.tpl",argv[i]);
			sprintf(hfile,"%s.stu",argv[i]);
		}
	}

	p=getenv("GBK_FLAG");
        if(p && *p=='T') GBK_flag=1;

	Showid=argv[0];
	if(!*cfile) {
		cfd=stdout;
	} else {
		if(!(cfd=fopen(cfile,"w"))) {
			perror(cfile);
			return 1;
		}
	}
	
	if(!*hfile) {
		hfd=stdout;
	} else {
		if(!(hfd=fopen(hfile,"w"))) {
			perror(hfile);
			return 1;
		}
	}

	for(lines=0;!ferror(stdin);lines++) {
		if(isatty(0)) {
			printf("输入元数据文件名： ");
			fflush(stdout);
		}
		fgets(buf,sizeof(buf),stdin);
		if(feof(stdin)) break;
		TRIM(buf);
		if(!*buf) continue;
		namep=meta_tpl(&srm,buf);
		if(!namep) {
			printf("/* Can not find the file %s! */\n",buf);
			continue;
		}
		if(Jflg) {	//JSON模板
			if(lines==0) fprintf(cfd,"{");
			else fprintf(cfd,",\n");
			if(Jflg==1) {  //生成最方便的SDBC POJO
				print_JSON(cfd,srm.tp,namep);
				print_JAVA(hfd,srm.tp,namep);
			} else if(Jflg==3) {	//生成标准POJO
				print_json(cfd,srm.tp,namep);
				print_java(hfd,srm.tp,namep);
			}
			
		} else {      //C模板
			print_type(cfd,srm.tp,namep);
			print_struct(hfd,srm.tp,namep);
		}
		SRM_free(&srm);
	}
	if(Jflg) fprintf(cfd,"}\n");

	if(cfd && cfd != stdout) fclose(cfd);
	if(hfd && hfd != stdout) fclose(hfd);
	return 0;
}
