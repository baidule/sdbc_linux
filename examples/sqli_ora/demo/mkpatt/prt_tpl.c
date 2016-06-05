#include <DAU.h>
#include <ctype.h>
#include "prt_tpl.h"

static char *ext_quote(char *buf,const char *str)
{
char *p;
	p=buf;
	while(*str) {
		if(*str=='\"') *p++='\\';
		*p++=*str++;
	}
	*p=0;
	return buf;
}

int print_type(FILE *fd,T_PkgType *tp,char *tabname)
{
char buf[512],datebuf[31];
int i;
int flg;
	fprintf(fd,"\nT_PkgType %s_tpl[]={\n",tabname);
	i=0;
	while(tp->type != -1) {
		if(tp->format) {
		    if(!strcmp(tp->format,YEAR_TO_DAY)) {
				sprintf(datebuf,"YEAR_TO_DAY");
		    } else if(!strcmp(tp->format,YEAR_TO_MIN)) {
				sprintf(datebuf,"YEAR_TO_MIN");
		    } else if(!strcmp(tp->format,YEAR_TO_SEC)) {
				sprintf(datebuf,"YEAR_TO_SEC");
		    } else sprintf(datebuf,"\"%s\"",tp->format);
		} else sprintf(datebuf,"0");
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
		case CH_STRUCT:
			fprintf(fd,"\t{CH_STRUCT,");
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
		case CH_USEC:
			fprintf(fd,"\t{CH_USEC,");
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
                        fprintf(fd,"sizeof(short),\"%s\"",ext_quote(buf,tp->name));
						if(tp->format) fprintf(fd,",\"%s\"",tp->format);
						else flg=1;
                        break;
                case CH_LONG:
                        fprintf(fd,"sizeof(long),\"%s\"",ext_quote(buf,tp->name));
						if(tp->format) fprintf(fd,",\"%s\"",tp->format);
						else flg=1;
                        break;
                case CH_INT:
                        fprintf(fd,"sizeof(int),\"%s\"",ext_quote(buf,tp->name));
						if(tp->format) fprintf(fd,",\"%s\"",tp->format);
						else flg=1;
                        break;
                case CH_INT64:
                        fprintf(fd,"sizeof(INT64),\"%s\"",ext_quote(buf,tp->name));
						if(tp->format) fprintf(fd,",\"%s\"",tp->format);
						else flg=1;
                        break;
                case CH_DOUBLE:
			if(tp->format) 
                        	fprintf(fd,"sizeof(double),\"%s\",\"%s\"",ext_quote(buf,tp->name),tp->format);
			else {
                        	fprintf(fd,"sizeof(double),\"%s\"",ext_quote(buf,tp->name));
				flg=1;
			}
                        break;
                case CH_FLOAT:
			if(tp->format) 
                        	fprintf(fd,"sizeof(float),\"%s\",\"%s\"",ext_quote(buf,tp->name),tp->format);
			else {
               	fprintf(fd,"sizeof(float),\"%s\"",ext_quote(buf,tp->name));
				flg=1;
			}
                        break;
                case CH_JUL:
                case CH_CJUL:
                case CH_MINUTS:
                case CH_CMINUTS:
                        fprintf(fd,"sizeof(INT4),\"%s\",%s",ext_quote(buf,tp->name),datebuf);
                        break;
                case CH_TIME:
                case CH_USEC:
                case CH_CTIME:
                        fprintf(fd,"sizeof(INT64),\"%s\",%s",ext_quote(buf,tp->name),datebuf);
                        break;
                case CH_DATE:
			if(tp->format) {
			    if(!strcmp(tp->format,YEAR_TO_DAY)) {
				fprintf(fd,"YEAR_TO_DAY_LEN,\"%s\",%s",ext_quote(buf,tp->name),datebuf);
			    } else if(!strcmp(tp->format,YEAR_TO_MIN)) {
				fprintf(fd,"YEAR_TO_MIN_LEN,\"%s\",%s",ext_quote(buf,tp->name),datebuf);
			    } else if(!strcmp(tp->format,YEAR_TO_SEC)) {
				fprintf(fd,"YEAR_TO_SEC_LEN,\"%s\",%s",ext_quote(buf,tp->name),datebuf);
			    } else if(!strcmp(tp->format,YEAR_TO_USEC)) {
				fprintf(fd,"YEAR_TO_USEC_LEN,\"%s\",%s",ext_quote(buf,tp->name),datebuf);
			    } else fprintf(fd,"%d,\"%s\",%s",tp->len,ext_quote(buf,tp->name),datebuf);
			    break;
			} 
                case CH_CLOB:
                        fprintf(fd,"sizeof(char *),\"%s\"",ext_quote(buf,tp->name));
            flg=1;
                        break;

                case CH_STRUCT:
                        fprintf(fd,"0,\"%s\",%s",tp->name,tp->format);
                        break;
                default:
			fprintf(fd,"%d,\"%s\"",tp->len, ext_quote(buf,tp->name));
			flg=1;
			break;
                }

		if(!i) {	//模板第一项，-1标志 
			if(flg) fprintf(fd,",0,-1},\n");//无格式语句 
			else fprintf(fd,",-1},\n");	//有格式语句 
		} else fprintf(fd,"},\n");
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
int i;
const char *name;
char *p;
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
			case CH_STRUCT:
				p=(char *)tp->format + 14;
				fprintf(fd,"\t%.*s_stu %s;\n",(int)strlen(p)-4,p,name);
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
			case CH_USEC:
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
	fprintf(fd,"    \"%s:[ \" +\n",tabname);
	for(i=0;tp[i].type>=0;i++) {
		if(i>0) fprintf(fd,",\" +\n");
		fprintf(fd,"\t\"{name:'%s'",plain_name(tp[i].name));
		switch(tp[i].type) {
		case CH_DOUBLE:
		case CH_FLOAT:
		case CH_TINY:
		case CH_SHORT:
		case CH_INT:
		case CH_LONG:
		case CH_INT64:
		case CH_CNUM:
			fprintf(fd,",type:'CH_NUMBER'");
			break;
		case CH_DATE:
		case CH_JUL:
		case CH_MINUTS:
		case CH_TIME:
		case CH_USEC:
			fprintf(fd,",type:'CH_DATE',format:'%s'",
				tp[i].format);
			break;
		default: break;
		}
		fprintf(fd,"}");
	}
	fprintf(fd,"\" +\n    \"]");
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
	fprintf(fd,"    \"%s:[\n",tabname);
	for(i=0;tp[i].type>=0;i++) {
		if(i>0) fprintf(fd,",\n");
		if(strcmp(tp[i].name,"ROWID"))
			fprintf(fd,"\t{name:'%s'",up1((char *)plain_name(tp[i].name)));
		else 
			fprintf(fd,"\t{name:'%s'",tp[i].name);
		switch(tp[i].type) {
		case CH_DOUBLE:
		case CH_FLOAT:
		case CH_TINY:
		case CH_SHORT:
		case CH_INT:
		case CH_LONG:
		case CH_INT64:
		case CH_CNUM:
			fprintf(fd,",type:'CH_NUMBER'");
			break;
		case CH_DATE:
		case CH_JUL:
		case CH_MINUTS:
		case CH_TIME:
		case CH_USEC:
			fprintf(fd,",type:'CH_DATE',format:'%s'",
				tp[i].format);
			break;
		default: break;
		}
		fprintf(fd,"}");
	}
	fprintf(fd,"\" +\n    \"]");
}

void print_JAVA(FILE *fd,T_PkgType *tp,char *tabname)
{
int i;
	fprintf(fd,"\nclass %s {\n",tabname);
	for(i=0;tp[i].type>=0;i++) {
		const char *name=plain_name(tp[i].name);
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
	fprintf(fd, "}\n");
}
