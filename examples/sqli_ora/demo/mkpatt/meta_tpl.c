
/*********************************************************************
 * 将元数据转换成模板
 * 元数据格式：
 * TEMPLATE 模板名 列数 
 * 列名:类型 长度 格式 
 * ..................
 *********************************************************************/

#include <SRM.h>

static int type_len(int type)
{
	switch(type & 127) {
	case CH_TINY:   return 1;
	case CH_SHORT:	return sizeof(short);
	case CH_INT:	return sizeof(int);
	case CH_LONG:	return sizeof(long);
	case CH_INT64:	return sizeof(INT64);
	case CH_FLOAT:	return sizeof(float);
	case CH_DOUBLE:	return sizeof(double);
//	case CH_LDOUBLE:	return sizeof(long double);
	case CH_CLOB:	return sizeof(char *);
	case CH_STRUCT: return 0;
	default: return -1;
	}
}
static const char *mk_format(int type)
{
	switch(type) {
	case CH_JUL:
	case CH_CJUL:
		return YEAR_TO_DAY;
	case CH_MINUTS: 
	case CH_CMINUTS: 
		return YEAR_TO_MIN;
	case CH_TIME:
	case CH_CTIME:
		return YEAR_TO_SEC;
	case CH_USEC:
		return YEAR_TO_USEC;
	case CH_DOUBLE:
		return "%.2lf";
	case CH_FLOAT:
		return "%.2f";
	default: return NULL;
	}
}
char * str_dup(char *cp)
{
	if(!cp) return NULL;
	return strdup(cp);
}

char * meta_tpl(SRM *srmp,char *file)
{
FILE *fd;
char buf[2048],*p,*p1;
static char name[2000];
int i,n,ret;

	if(!srmp) return NULL;
	if(!(fd=fopen(file,"r"))) {
		return NULL;
	}
	*name=0;
	ret=n=i=0;
	while(!ferror(fd)) {
		fgets(buf,sizeof(buf),fd);
		if(feof(fd)) break;
		TRIM(buf);
		if(!n) {
			ret=sscanf(buf,"TEMPLATE %s%d",name,&n);
			if(ret==2 && n>0) {
				srmp->tp=(T_PkgType *)malloc(sizeof(T_PkgType) * (n+1));
				if(!srmp->tp) {
					ShowLog(1,"%s malloc tp error!",__FUNCTION__);
					fclose(fd);
					return NULL;
				}
				for(ret=0;ret<=n;ret++) {
					srmp->tp[ret].type=-1;
					srmp->tp[ret].name=0;
					srmp->tp[ret].format=0;
					srmp->tp[ret].offset=-1;
				}
			}
			if(n<0) break;
			continue;
		}
		p=skipblk(buf);
		if(*p=='#') continue;
		p1=p;
		if(i<n) {
			do {
				p=stptok(p,0,0,":");
				if(!*p) continue;
			} while (p[1] != '=' && *++p) ;
			if(!*p) continue;
			*p++=0;
			if(!*++p) continue;
			p=skipblk(p);
			srmp->tp[i].name=strdup(p1);
			p1=p;
			p=stptok(p,0,0," ");
			if(*p==' ') *p++=0;
			p=skipblk(p);

			srmp->tp[i].type=mk_sdbc_type(p1);
			if(srmp->tp[i].type < 0) {
				ShowLog(1,"无效数据类型：%s",p1);
				continue;
			}
//printf("数据类型：%s,type=%02X\n",p1,srmp->tp[i].type);
			srmp->tp[i].len=type_len(srmp->tp[i].type);
			if(srmp->tp[i].len<=0) {
				if(!*p) {
					if(srmp->tp[i].type == CH_CNUM)	srmp->tp[i].len=40;
					else srmp->tp[i].len=28;
					++i;
					continue;
				}
				p1=p;
				p=stptok(p,0,0," ");
				if(*p==' ') *p++=0;
				p=skipblk(p);
				if(srmp->tp[i].type != CH_STRUCT) 
					srmp->tp[i].len=atoi(p1);
				else p=p1;
			}
			p1=p;
			if(*p1) {
				if(srmp->tp[i].type == CH_STRUCT &&
				 	NULL != (srmp->tp[i].format=malloc(strlen(p1)+20))) {
						sprintf((char *)srmp->tp[i].format,"(const char *)%s_tpl",p1);
				} else srmp->tp[i].format=strdup(p1);
			} else srmp->tp[i].format=str_dup((char *)mk_format(srmp->tp[i].type));
			++i;
		} else {
			p=stptok(p,0,0,"\t");
			if(*p=='\t') {
				*p++=0;
				p=skipblk(p);
			}
			srmp->tp[n].name=strdup(p1); //表名 
			if(*p) {
				srmp->tp[n].format=strdup(p);	//pks
			}
			break;
		}
	}
	fclose(fd);
//	srmp->Aflg=set_offset(srmp->tp);
//	srmp->rec=malloc(srmp->tp[srmp->Aflg].offset);
	srmp->Aflg=n;
	srmp->rec=0;
	srmp->colidx=0;
	srmp->tabname=(char *)srmp->tp[srmp->Aflg].name;
	srmp->pks=(char *)srmp->tp[srmp->Aflg].format;
	srmp->rp=0;
	srmp->result=0;
	return name;
}

