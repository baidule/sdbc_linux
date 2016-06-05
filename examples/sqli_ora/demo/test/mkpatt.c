#include <DAU.h>
#include <ctype.h>

extern int mksrm(SRM *srmp,T_SQL_Connect *SQL_Connect);

int print_type(FILE *fd,T_PkgType *tp,char *tabname)
{
char buf[256];
int i;
int flg;
	fprintf(fd,"\nT_PkgType %s_tpl[]={\n",tabname);
	i=0;
	while(tp->type != -1) {
//		if(strcmp(tp->name,"ROWID")) strlower(tp->name);
		if(tp->format) {
		    if(!strcmp(tp->format,YEAR_TO_DAY)) {
			sprintf(buf,"YEAR_TO_DAY");
		    } else if(!strcmp(tp->format,YEAR_TO_MIN)) {
			sprintf(buf,"YEAR_TO_MIN");
		    } else if(!strcmp(tp->format,YEAR_TO_SEC)) {
			sprintf(buf,"YEAR_TO_SEC");
		    } else sprintf(buf,"\"%s\"",tp->format);
		} else sprintf(buf,"0");
		switch(tp->type) {
		case CH_BYTE:
			fprintf(fd,"\t{CH_BYTE,");
			break;
		case CH_CHAR:
			fprintf(fd,"\t{CH_CHAR,");
			break;
		case CH_CLOB:
			fprintf(fd,"\t{CH_CLOB,");
			break;
		case CH_DATE:
			fprintf(fd,"\t{CH_DATE,");
			break;
		case CH_INT:
			fprintf(fd,"\t{CH_INT,");
			break;
		case CH_SHORT:
			fprintf(fd,"\t{CH_SHORT,");
			break;
		case CH_TINY:
			fprintf(fd,"\t{CH_TINY,");
			break;
		case CH_LONG:
			fprintf(fd,"\t{CH_LONG,");
			break;
		case CH_DOUBLE:
			fprintf(fd,"\t{CH_DOUBLE,");
			break;
		case CH_FLOAT:
			fprintf(fd,"\t{CH_FLOAT,");
			break;
		case CH_CNUM:
			fprintf(fd,"\t{CH_CNUM,");
			break;
		case CH_JUL:
			fprintf(fd,"\t{CH_JUL,");
			break;
		case CH_CJUL:
			fprintf(fd,"\t{CH_CJUL,");
			break;
		case CH_MINUTS:
			fprintf(fd,"\t{CH_MINUTS,");
			break;
		case CH_CMINUTS:
			fprintf(fd,"\t{CH_CMINUTS,");
			break;
		case CH_TIME:
			fprintf(fd,"\t{CH_TIME,");
			break;
		case CH_CTIME:
			fprintf(fd,"\t{CH_CTIME,");
			break;
		case CH_INT64:
			fprintf(fd,"\t{CH_INT64,");
			break;
		default:
			tp++;
			continue;
		}

		flg=0;
		switch(tp->type) {
                case CH_SHORT:
                        fprintf(fd,"sizeof(short),\"%s\"",tp->name);
			flg=1;
                        break;
                case CH_LONG:
                        fprintf(fd,"sizeof(long),\"%s\"",tp->name);
			flg=1;
                        break;
                case CH_INT:
                        fprintf(fd,"sizeof(int),\"%s\"",tp->name);
			flg=1;
                        break;
                case CH_INT64:
                        fprintf(fd,"sizeof(INT64),\"%s\"",tp->name);
			flg=1;
                        break;
                case CH_DOUBLE:
			if(tp->format) 
                        	fprintf(fd,"sizeof(double),\"%s\",%s",tp->name,buf);
			else {
                        	fprintf(fd,"sizeof(double),\"%s\"",tp->name);
				flg=1;
			}
                        break;
                case CH_FLOAT:
			if(tp->format) 
                        	fprintf(fd,"sizeof(float),\"%s\",%s",tp->name,buf);
			else {
                        	fprintf(fd,"sizeof(float),\"%s\"",tp->name);
				flg=1;
			}
                        break;
                case CH_JUL:
                case CH_CJUL:
                case CH_MINUTS:
                case CH_CMINUTS:
                        fprintf(fd,"sizeof(INT4),\"%s\",%s",tp->name,buf);
                        break;
                case CH_TIME:
                case CH_CTIME:
                        fprintf(fd,"sizeof(INT64),\"%s\",%s",tp->name,buf);
                        break;
                case CH_DATE:
			if(tp->format) {
			    if(!strcmp(tp->format,YEAR_TO_DAY)) {
				fprintf(fd,"YEAR_TO_DAY_LEN,\"%s\",%s",tp->name,buf);
			    } else if(!strcmp(tp->format,YEAR_TO_MIN)) {
				fprintf(fd,"YEAR_TO_MIN_LEN,\"%s\",%s",tp->name,buf);
			    } else if(!strcmp(tp->format,YEAR_TO_SEC)) {
				fprintf(fd,"YEAR_TO_SEC_LEN,\"%s\",%s",tp->name,buf);
			    } else fprintf(fd,"%d,\"%s\",%s",tp->len,tp->name,buf);
			    break;
			} 
                default:
			fprintf(fd,"%d,\"%s\"",tp->len, tp->name);
			flg=1;
			break;
                }

		if(!i) {
			if(flg) fprintf(fd,",0,-1},\n",tp->len, tp->name,buf);
			else fprintf(fd,",-1},\n",tp->len, tp->name,buf);
		} else fprintf(fd,"},\n",tp->len, tp->name,buf);
		tp++;
		i++;
	}
	fprintf(fd,"\t{-1,0,");
	(tp->name&&*tp->name)?fprintf(fd,"\"%s\",",tp->name):fprintf(fd,"0,");
	(tp->format&&*tp->format)?fprintf(fd,"\"%s\"",tp->format):fprintf(fd,"0");
	fprintf(fd,"}\n};\n");
	return i;
}
int print_struct(FILE *fd,T_PkgType *tp,char *tabname)
{
char buf[256];
int i;
char *name;
	fprintf(fd,"\nextern T_PkgType %s_tpl[];\ntypedef struct {\n",tabname);
	i=0;
	while(tp->type != -1) {
		name=plain_name(tp->name);
		switch(tp->type) {
			case CH_CHAR:
			case CH_BYTE:
			case CH_DATE:
				if(tp->format) {
				    if(!strcmp(tp->format,YEAR_TO_DAY)) {
					fprintf(fd,"\tchar %s[YEAR_TO_DAY_LEN];\n",name);
				    } else if(!strcmp(tp->format,YEAR_TO_MIN)) {
					fprintf(fd,"\tchar %s[YEAR_TO_MIN_LEN];\n",name);
				    } else if(!strcmp(tp->format,YEAR_TO_SEC)) {
					fprintf(fd,"\tchar %s[YEAR_TO_SEC_LEN];\n",name);
				    } else fprintf(fd,"\tchar %s[%d];\n",name,tp->len);
				} else fprintf(fd,"\tchar %s[%d];\n",name,tp->len);
				break;
			case CH_CLOB:
				fprintf(fd,"\tchar *%s;\n",name);
				break;
			case CH_INT:
				fprintf(fd,"\tint %s;\n",name);
				break;
			case CH_SHORT:
				fprintf(fd,"\tshort %s;\n",name);
				break;
			case CH_LONG:
				fprintf(fd,"\tlong %s;\n",name);
				break;
			case CH_DOUBLE:
				fprintf(fd,"\tdouble %s;\n",name);
				break;
			case CH_FLOAT:
				fprintf(fd,"\tfloat %s;\n",name);
				break;
			case CH_CNUM:
				fprintf(fd,"\tchar %s[%d];\n",name,tp->len);
				break;
			case CH_TINY:
				fprintf(fd,"\tchar %s;\n",name);
				break;
			case CH_JUL:
			case CH_CJUL:
			case CH_MINUTS:
			case CH_CMINUTS:
				fprintf(fd,"\tINT4 %s;\n",name);
				break;
			case CH_INT64:
				fprintf(fd,"\tINT64 %s;\n",name);
				break;
			case CH_TIME:
			case CH_CTIME:
				fprintf(fd,"\tINT64 %s;\n",name);
				break;
			default:
				tp++;
				continue;
			}
			tp++;
			i++;
		}
		fprintf(fd,"} %s_stu;\n",tabname);
		return i;
}

void print_JSON(FILE *fd,T_PkgType *tp,char *tabname)
{
int i;
	fprintf(fd,"\"%s\":[\n",tabname);
	for(i=0;tp[i].type>=0;i++) {
		if(i>0) fprintf(fd,",\n");
		fprintf(fd,"\t{\"name\":\"%s\"",tp[i].name);
		switch(tp[i].type) {
		case CH_DOUBLE:
		case CH_FLOAT:
		case CH_TINY:
		case CH_SHORT:
		case CH_INT:
		case CH_LONG:
		case CH_INT64:
		case CH_CNUM:
			fprintf(fd,",\"type\":\"CH_NUMBER\"");
			break;
		case CH_DATE:
		case CH_JUL:
		case CH_MINUTS:
		case CH_TIME:
			fprintf(fd,",\"type\":\"CH_DATE\",\"format\":\"%s\"",
				tp[i].format);
			break;
		default: break;
		}
		fprintf(fd,"}");
	}
	fprintf(fd,"\n]");
}
char *up1(char *str)
{
	*str=toupper(*str);
	strlower(str+1);
	return str;
}
void print_json(FILE *fd,T_PkgType *tp,char *tabname)
{
int i;
	up1(tabname);
	fprintf(fd,"\"%s\":[\n",tabname);
	for(i=0;tp[i].type>=0;i++) {
		if(i>0) fprintf(fd,",\n");
		if(strcmp(tp[i].name,"ROWID"))
			fprintf(fd,"\t{\"name\":\"%s\"",up1(tp[i].name));
		else 
			fprintf(fd,"\t{\"name\":\"%s\"",tp[i].name);
		switch(tp[i].type) {
		case CH_DOUBLE:
		case CH_FLOAT:
		case CH_TINY:
		case CH_SHORT:
		case CH_INT:
		case CH_LONG:
		case CH_INT64:
		case CH_CNUM:
			fprintf(fd,",\"type\":\"CH_NUMBER\"");
			break;
		case CH_DATE:
		case CH_JUL:
		case CH_MINUTS:
		case CH_TIME:
			fprintf(fd,",\"type\":\"CH_DATE\",\"format\":\"%s\"",
				tp[i].format);
			break;
		default: break;
		}
		fprintf(fd,"}");
	}
	fprintf(fd,"\n]");
}


void print_JAVA(FILE *fd,T_PkgType *tp,char *tabname)
{
int i;
	fprintf(fd,"\nclass %s {\n",tabname);
	for(i=0;tp[i].type>=0;i++) {
		char *name=plain_name(tp[i].name);
		fprintf(fd,"\tpublic ");
		switch(tp[i].type) {
		case CH_DOUBLE:
			fprintf(fd,"double %s;\n",name);
			break;
		case CH_FLOAT:
			fprintf(fd,"float %s;\n",name);
			break;
		case CH_TINY:
			fprintf(fd,"byte %s;\n",name);
			break;
		case CH_SHORT:
			fprintf(fd,"short %s;\n",name);
			break;
		case CH_INT:
			fprintf(fd,"int %s;\n",name);
			break;
		case CH_LONG:
		case CH_INT64:
			fprintf(fd,"long %s;\n",name);
			break;
		case CH_CNUM:
		default:
			fprintf(fd,"String %s;\n",name);
			break;
		}
	}
	fprintf(fd,"\tprivate static JSONArray PARTTERN;\n"
		   "\t%s(String jsonPartternLib) {\n"
		   "\tif(PARTTERN==null)\n"
			"\t\tPARTTERN=Jorm.getParttern(jsonPartternLib,this);\n"
		   "\t}\n\tpublic JSONArray getPARTTERN() { return PARTTERN; }\n"
		   "}\n",tabname);
}
void print_java(FILE *fd,T_PkgType *tp,char *tabname)
{
int i;
	fprintf(fd,"\nclass %s {\n",tabname);
	for(i=0;tp[i].type>=0;i++) {
		fprintf(fd,"\tprivate ");
		switch(tp[i].type) {
		case CH_DOUBLE:
			fprintf(fd,"double %s;\n\tpublic ",tp[i].name);
			fprintf(fd,"double set%s(double %s)\n\t{\n\t\tthis.%s=%s;\n\t}\n\tpublic ",
				tp[i].name,tp[i].name,tp[i].name,tp[i].name);
			fprintf(fd,"double get%s()\n\t{\n\t\treturn this.%s;\n\t}\n\n",
				tp[i].name,tp[i].name);
			break;
		case CH_FLOAT:
			fprintf(fd,"float %s;\n\tpublic ",tp[i].name);
			fprintf(fd,"float set%s(float %s)\n\t{\n\t\tthis.%s=%s;\n\t}\n\tpublic ",
				tp[i].name,tp[i].name,tp[i].name,tp[i].name);
			fprintf(fd,"float get%s()\n\t{\n\t\treturn this.%s;\n\t}\n\n",
				tp[i].name,tp[i].name);
			break;
		case CH_TINY:
			fprintf(fd,"byte %s;\n\tpublic ",tp[i].name);
			fprintf(fd,"byte set%s(byte %s)\n\t{\n\t\tthis.%s=%s;\n\t}\n\tpublic ",
				tp[i].name,tp[i].name,tp[i].name,tp[i].name);
			fprintf(fd,"byte get%s()\n\t{\n\t\treturn this.%s;\n\t}\n\n",
				tp[i].name,tp[i].name);
			break;
		case CH_SHORT:
			fprintf(fd,"short %s;\n\tpublic ",tp[i].name);
			fprintf(fd,"short set%s(short %s)\n\t{\n\t\tthis.%s=%s;\n\t}\n\tpublic ",
				tp[i].name,tp[i].name,tp[i].name,tp[i].name);
			fprintf(fd,"short get%s()\n\t{\n\t\treturn this.%s;\n\t}\n\n",
				tp[i].name,tp[i].name);
			break;
		case CH_INT:
			fprintf(fd,"int %s;\n\tpublic ",tp[i].name);
			fprintf(fd,"void set%s(int %s)\n\t{\n\t\tthis.%s=%s;\n\t}\n\tpublic ",
				tp[i].name,tp[i].name,tp[i].name,tp[i].name);
			fprintf(fd,"int get%s()\n\t{\n\t\treturn this.%s;\n\t}\n\n",
				tp[i].name,tp[i].name);
			break;
		case CH_LONG:
		case CH_INT64:
			fprintf(fd,"long %s;\n\tpublic ",tp[i].name);
			fprintf(fd,"void set%s(long %s)\n\t{\n\t\tthis.%s=%s;\n\t}\n\tpublic ",
				tp[i].name,tp[i].name,tp[i].name,tp[i].name);
			fprintf(fd,"long get%s()\n\t{\n\t\treturn this.%s;\n\t}\n\n",
				tp[i].name,tp[i].name);
			break;
		case CH_CNUM:
		default:
			fprintf(fd,"String %s;\n\tpublic ",tp[i].name);
			fprintf(fd,"void set%s(String %s)\n\t{\n\t\tthis.%s=%s;\n\t}\n\tpublic ",
				tp[i].name,tp[i].name,tp[i].name,tp[i].name);
			fprintf(fd,"String get%s()\n\t{\n\t\treturn this.%s;\n\t}\n\n",
				tp[i].name,tp[i].name);
			break;
		}
	}
	fprintf(fd, "}\n",tabname);
}

main(int argc,char *argv[])
{
char buf[100],cfile[100],hfile[100];
int ret,i;
SRM srm;
T_SQL_Connect SQL_Connect;
int Jflg=0;
FILE *cfd,*hfd;
int lines=0;

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
		} else {
			sprintf(cfile,"%s.tpl",argv[i]);
			sprintf(hfile,"%s.stu",argv[i]);
		}
	}
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
		ret=mksrm(&srm,&SQL_Connect);
		if(ret) {
			printf("/* Can not find the table %s! */\n",buf);
			ShowLog(1,"%s:表名错：err=%d,%s",buf,
				SQL_Connect.Errno,SQL_Connect.ErrMsg);
			continue;
		}
		if(Jflg) {	//JSON模板
			if(lines==0) fprintf(cfd,"{");
			else fprintf(cfd,",\n");
			if(Jflg==1) {  //生成最方便的SDBC POJO
				print_JSON(cfd,srm.tp,srm.tabname);
				print_JAVA(hfd,srm.tp,srm.tabname);
			} else if(Jflg==3) {	//生成标准POJO
				print_json(cfd,srm.tp,srm.tabname);
				print_java(hfd,srm.tp,srm.tabname);
			}
			
		} else {      //C模板
			print_type(cfd,srm.tp,srm.tabname);
			print_struct(hfd,srm.tp,srm.tabname);
		}
		SRM_free(&srm);
	}
	if(Jflg) fprintf(cfd,"}\n");

	if(cfd && cfd != stdout) fclose(cfd);
	if(hfd && hfd != stdout) fclose(hfd);
	ret=___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
