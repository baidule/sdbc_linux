/************************************************************
 * 本程序对ORACLE的存储过程和存储函数进行包装。
 * 实现了接口串行化。
 ************************************************************/

#include <sqli.h>
#include <BB_tree.h>
#include <malloc.h>
#include <oci.h>

#define MAXNAMELEN 31
extern const char * _get_data_type_str (int dtype); //modify sqlora.c,change to extern

#define ENCODE_STH(_sth, _dbh) ((int)(_dbh << (sizeof(sqlo_stmt_handle_t)/2 * 8) | _sth))

#define DECODE_STH(_sth) ((ub4) _sth & 0x0000ffff)
#define DECODE_DBH(_sth) ((ub4) (_sth >> (sizeof(sqlo_stmt_handle_t)/2 * 8)) & 0x007fff)


struct rpc_node {
	int key;
	char name[MAXNAMELEN];
	int len;
	char format[40];
	int type;
	char *bufp;
	int buf[10];
	T_SQL_Connect *SQL_Connect;
	sqlo_stmt_handle_t *sthp;
	int *ret;
	int *res_num;
	short ind;
	char **packp;
};

static int toklen(char *str,char *tok)
{
char *p;
	p=stptok(str,0,0,tok);
	return p-str;
}

static void rpc_free(void *node)
{
struct rpc_node *np;
	if(!node) return;
	np=(struct rpc_node *)node;
/* free cursor on error 
*/
	if(np->type==SQLOT_CUR && 0>*np->ret) {
	int cur;
		if((cur=*(int *)np->bufp)>=0 ) {
			ShowLog(1,"%s:CLOSE ret=%d,cursor %d",__FUNCTION__,*np->ret,cur);
			sqlo_close(cur);
		}
	}
	else if(np->type==SQLOT_STR && np->bufp != np->buf) {
		if(np->bufp) free(np->bufp);
	}
}

static int buf_Cmp(rec1,rec2,len)
void *rec1;
void *rec2;
int len;
{
struct rpc_node *dap1,*dap2;
	dap1=(struct rpc_node *)rec1;
	dap2=(struct rpc_node *)rec2;
	if(dap1->key<dap2->key) return -2;
	else if (dap1->key>dap2->key) return 2;
	return 0;
}

/* 在执行之前 bind 变量 */
static int mk_rpc_bind(void *node)
{
struct rpc_node *np;
int status;
OCIError *err;
char buf[100];
	if(!node) return 0;
	np=(struct rpc_node *)node;
	if(*np->ret) return 0;
	np->ind=0;
	sqlo_get_oci_handle(np->SQL_Connect->dbh,&err,SQLO_OCI_HTYPE_ERROR);
	if(!np->bufp) np->bufp=(char *)np->buf;
	switch(np->type) {
	case SQLOT_ODT:
		if(!*np->format) strcpy(np->format,"YYYY-MM-DD HH24:MI:SS");
		if(*(char *)np->buf) {
			strcpy(buf,(char *)np->buf);
			status=OCIDateFromText(err,(unsigned char *)buf,strlen(buf)+1,
				(unsigned char *)np->format,strlen(np->format)+1,0,0,
				(OCIDate *)np->buf);
		}
		np->bufp=(char *)np->buf;
       		status=sqlo_bind_by_name(*np->sthp, np->name,
			np->type, np->bufp, sizeof(OCIDate), &np->ind, 0);
		break;
	case SQLOT_INT:
       		status=sqlo_bind_by_name(*np->sthp, np->name,
			np->type, np->buf, sizeof(int), &np->ind, 0);
		break;
	case SQLOT_FLT:
       		status=sqlo_bind_by_name(*np->sthp, np->name,
			np->type, np->buf, sizeof(double), &np->ind, 0);
		break;
	case SQLOT_CUR:
		status=sqlo_bind_ref_cursor(*np->sthp, np->name,
			np->buf);
		if(status!=SQLO_SUCCESS)
			*np->buf=-1;
		break;
	case SQLOT_NUM:
       		status=sqlo_bind_by_name(*np->sthp, np->name,
			SQLOT_STR, np->bufp, sizeof(np->buf), &np->ind, 0);
		break;
	default:  // as CHAR
       		status=sqlo_bind_by_name(*np->sthp, np->name,
			np->type, np->bufp,
			np->len>0?np->len:strlen(np->bufp)+1, &np->ind, 0);
		break;
	}
	if(status!=SQLO_SUCCESS) {
		___SQL_GetError(np->SQL_Connect);
		ShowLog(1,"mk_rpc_bind %s type=%d err=%s",
			np->name,np->type,np->SQL_Connect->ErrMsg);
		if(np->ret) *np->ret=np->SQL_Connect->Errno;
	}
	return 0;
}

/* 在执行之后，结果数据打包输出 */
static int mk_rpc_pack(void *node)
{
struct rpc_node *np;
char buf[100],*p;
OCIError *err;
int ret;
ub4 len;

	if(!node) return 0;
	*buf=0;
	np=(struct rpc_node *)node;
	if(!np->packp) return 0;
	ret=sqlo_get_oci_handle(np->SQL_Connect->dbh,&err,SQLO_OCI_HTYPE_ERROR);
	if(ret!=SQLO_SUCCESS) {
		___SQL_GetError(np->SQL_Connect);
		ShowLog(1,"mk_rpc_pack get_oci_handle, %s,err=%s",
			np->name,np->type,np->SQL_Connect->ErrMsg);
		if(np->ret) *np->ret=np->SQL_Connect->Errno;
		return 0;
	}
	p=buf;
	switch(np->type) {
	case SQLOT_ODT:
		len=sizeof(buf);
		ret=OCIDateToText(err,(CONST OCIDate *)np->buf,
				(unsigned char *)"YYYY-MM-DD HH24:MI:SS",22,
				0,0,&len,(text *)buf);
		strcat(buf,"|");
		break;
	case SQLOT_CUR:
		{
int sth;
		sth= ENCODE_STH(*np->buf,np->SQL_Connect->dbh);
		np->bufp=(char *)np->buf;
		ret=sqlo_execute(sth, 1);
		if(ret!=SQLO_SUCCESS) {
			___SQL_GetError(np->SQL_Connect);
			ShowLog(1,"%s:ref_cur err=%d,%s",__FUNCTION__,
				np->SQL_Connect->Errno,
				np->SQL_Connect->ErrMsg);
			if(np->SQL_Connect->Errno>0)
				*np->buf=-np->SQL_Connect->Errno;
			else
				*np->buf=np->SQL_Connect->Errno;
			ret=___SQL_Close__(np->SQL_Connect,sth);
			if(ret) ShowLog(1,"%s:ref_cur %d err=%d,%s",__FUNCTION__,
					sth,
					np->SQL_Connect->Errno,
					np->SQL_Connect->ErrMsg);
		}
		sprintf(buf,"%d|",sth);
	}
		break;
	case SQLOT_INT:
		sprintf(buf,"%d|",*np->buf);
		break;
	case SQLOT_FLT:
		sprintf(buf,"%f|",*(double *)np->buf);
		break;
	case SQLOT_NUM:
	default:  // as CHAR
		p=np->bufp;
		strcat(p,"|");
		break;
	}
	if(ret!=SQLO_SUCCESS) {
		___SQL_GetError(np->SQL_Connect);
		ShowLog(1,"%s:%s type=%d err=%s",__FUNCTION__,
			np->name,np->type,np->SQL_Connect->ErrMsg);
		if(np->ret) *np->ret=np->SQL_Connect->Errno;
		return 0;
	}
	p=skipblk(p);
	if(!*np->packp) *np->packp=strdup(p);
	else {
		*np->packp=realloc(*np->packp,strlen(*np->packp)+strlen(p)+1);
		strcat(*np->packp,p);
	}
	if(np->res_num) (*np->res_num)++;
//ShowLog(5,"%s:final pack=%s,res_num=%d,np->ret=%d",__FUNCTION__,*np->packp,*np->res_num,np->ret?*np->ret:-1);
	return 0;
}

/* 取参数变量的数据格式 */
static char * getfmt(char *p,char *fmt)
{
char *p1;
	if(*p!='(') {
		return p;
	}
	p++;
	if(fmt) *fmt=0;
	p1=stptok(p,fmt,35,") ,");
	if(*p1==')') p1++;
	return p1;
}

/* 取输入参数的值 */
static char * getval(char *p,char *val,int siz)
{
char *p1;
	if(val) *val=0;
	if(!*p||*p==',') return p;
	p=skipblk(p);
	if(*p != '\'') return stptok(p,val,siz,",) ");
	p++;
	p1=stptok(p,val,siz,"\'");
	while(*p1=='\'' && p1[1]=='\'') {
		strcat(val,"\'");
		p1=stptok(p1+2,val+strlen(val),siz-(p1-p),"\'");
	}
	if(*p1=='\'') p1++;
//	if(*p1==',') p1++;
//ShowLog(5,"getval val=%s,p1=%s",val,p1);
	return p1;
}

/* 生成bind参数 */
static char * setbind(char *type_buf,char *p1,struct rpc_node *node)
{
int len;
	if(!strcmp(type_buf,"CURSOR")) {
		node->type=SQLOT_CUR;
		p1=getfmt(p1,0);
	} else if (!strcmp(type_buf,"DATE")) {
		node->type=SQLOT_ODT;
		p1=getfmt(p1,node->format);
		p1=getval(p1,(char *)node->buf,sizeof(node->buf));
	} else if (!strcmp(type_buf,"CHAR")) {
		node->type=SQLOT_STR;
		p1=getfmt(p1,node->format);
		node->len=atoi(node->format)+1;
		*node->format=0;
		p1=skipblk(p1);
		len=toklen(p1,",)");
//ShowLog(5,"%s:CHAR(%d) len=%d",__FUNCTION__,node->len,len);
		if(node->len > 0) {
			node->len=node->len<len?len:node->len;
			if(node->len>=(int)sizeof(node->buf)) 
				node->bufp=malloc(node->len+1);
			else node->bufp=(char *)node->buf;
			p1=getval(p1,node->bufp,node->len);
		} 
	} else if (!strcmp(type_buf,"INT")) {
		node->type=SQLOT_INT;
		p1=getfmt(p1,node->format);
		p1=getval(p1,type_buf,sizeof(type_buf));
		*node->buf=atoi(type_buf);
	} else if (!strcmp(type_buf,"DOUBLE")) {
		node->type=SQLOT_FLT;
		p1=getfmt(p1,node->format);
		p1=getval(p1,type_buf,sizeof(type_buf));
		*(double *)node->buf=atof(type_buf);
	} else if (!strncmp(type_buf,"NUM",3)) {
		node->type=SQLOT_NUM;
		p1=getfmt(p1,node->format);
		p1=getval(p1,(char *)node->buf,sizeof(node->buf));
	} else {
		node->type=SQLOT_STR;
		p1=getfmt(p1,node->format);
		p1=getval(p1,(char *)node->buf,sizeof(node->buf));
	}
	return p1;
}

	
/*
cmd: RPCname(value,:type,:type value)
cmd: type := FUNCname(value,:type,:type value)
the type is char(len),date(YYYY-MM-DD HH24:MI:SS),int,num,double,cursor,.....
*/
int ORA_Rpc(T_SQL_Connect *SQL_Connect,char *cmd,char **result)
{
char *stmt,*p;
char type_buf[20];
int ret;
struct rpc_node node;
T_Tree *root=0,*tnode;
sqlo_stmt_handle_t sth = SQLO_STH_INIT;
char stmt1[1024];
char *p1;
	p=cmd;
	if(!p||!*p) return -12;
	stmt=malloc(strlen(cmd)+256);
	if(!stmt) return MEMERR;
	ret=0;
	SQL_Connect->NativeError=0;
	node.SQL_Connect=SQL_Connect;
	node.sthp=&sth;
	node.ret=&ret;
	node.res_num=&SQL_Connect->NativeError;
	node.packp=result;
	node.key=0;
        node.len=-1;
        *node.format=0;
        *node.buf=0;
        node.bufp=0;
	p=strstr(p,":="); //find function
	if(p&&(!(p1=strchr(cmd,'\'')) || (p<p1))) { //function found
	char *p2;
                sprintf(node.name,":RE0");
		p1=skipblk(cmd);
		for(p2=stmt1;p1<p;) *p2++=*p1++;
		*p2=0;
		if(*stmt1) {
			p1=stptok(stmt1,type_buf,sizeof(type_buf),"( 	");
			if(!*type_buf) strcpy(type_buf,"NUM");
			else if(*type_buf==':') strsubst(type_buf,1,0);
		} else strcpy(type_buf,"NUM");
		strupper(type_buf);
/* bind 返回值，加入二叉树 */
		root=BB_Tree_Add(root,&node,sizeof(node),buf_Cmp,0);
		tnode=BB_Tree_Find(root,&node,sizeof(node),buf_Cmp);
		if(!tnode) {
			ShowLog(1,"%s:node can't add to BB_Tree!",__FUNCTION__);
			BB_Tree_Free(&root,rpc_free);
			return MEMERR;
		}
		p1=setbind(type_buf,p1,tnode->Content);
		strsubst(cmd,p-cmd,":RE0 "); // :RE0 := func(...)
		p=skipblk(p+2);  //p指向函数名
	} else p=cmd;

	p=stptok(p,node.name,sizeof(node.name),"(");
	TRIM(node.name);
	strupper(node.name);
	if(*p) {   //has args
	int flg;
		p++;
		while(*(p1=stptok(p,0,0,":\'"))) {
			flg=0;
			if(*p1=='\'') {		//排除单引号内的 ':'
				p=p1+1;
				do { //找下一个 '
					p1=stptok(p,0,0,"\'");
					if(!*p1) {
						p=p1;
						flg=1;
						break;
					}
					if(p1[1]!='\'') {
						p=p1+1;
						break;
					}
					p=p1+2;
				} while(*p);
				if(flg) break;  //出现了异常，退出
				continue;	//跳过了一个字符串，继续
			}
			p=p1;
/*  现在，找到了占位符 */
//ShowLog(5,":find p= %s",p);
			node.key++;
			node.len=-1;
			*node.format=0;
			*node.buf=0;
			node.bufp=0;
			sprintf(node.name,"%.3s%d",p,node.key);
			p1=stptok(p+1,type_buf,sizeof(type_buf),"( ,)");
			TRIM(type_buf);
			strupper(type_buf);
			root=BB_Tree_Add(root,&node,sizeof(node),buf_Cmp,0);
			tnode=BB_Tree_Find(root,&node,sizeof(node),buf_Cmp);
			if(!tnode) {
				ShowLog(1,"%s:node can't add to BB_Tree!",__FUNCTION__);
				BB_Tree_Free(&root,rpc_free);
				return MEMERR;
			}
			p1=setbind(type_buf,p1,tnode->Content);

			p1=skipblk(p1);
			p=strsubst(p,p1-p,node.name);
			p++;
		} // 一个占位符处理完毕 
	
	} // args
/*****************************************************************/
	sprintf(stmt,"BEGIN\n%s;\nEND;",cmd);

ShowLog(5,"ORA_Rpc:\n%s",stmt);
	sth = sqlo_prepare(SQL_Connect->dbh, stmt);
	if(sth>=0) {
		ret=0;
/* 在执行前BIND变量 */
		BB_Tree_Scan(root,mk_rpc_bind);
		if(ret) {
			ret=-abs(ret);
			goto rpc_exit;
		}
		ret = sqlo_execute(sth, 1);
		if (ret != SQLO_SUCCESS) {
			___SQL_GetError(SQL_Connect);
			ret=-13;
			goto rpc_exit;
		}
		if(result) *result=0;
/* 在执行后，结果集打包 */
		BB_Tree_Scan(root,mk_rpc_pack);
		ret=SQL_Connect->NativeError;
		sqlo_close(sth);
	} else {
		___SQL_GetError(SQL_Connect);
		ret=-abs(SQL_Connect->Errno);
	}

rpc_exit:
	BB_Tree_Free(&root,rpc_free);
	free(stmt);
	return ret;
}
