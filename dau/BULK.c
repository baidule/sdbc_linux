#define _GNU_SOURCE
#include <DAU.h>
#include <BULK.h>
#include <regex.h>
#include <ctype.h>

extern int do_regexp(char *stmt,regmatch_t *match);
extern char * mark_subst(char *vp,T_PkgType *typ,int *bindnum);
extern void set_dbo(char *buf,char *DBOWN);

static int BULK_alloc(BULK *bulk);
static int bind_proc(void *content);   //bind处理
static int define(BULK *bulk);


static void bind_rows_free(BULK *bulk)
{
int i;
	if(bulk->cb) {
		for(i=0;i<bulk->cols;i++) {
			if(bulk->cb[i].ind) free( bulk->cb[i].ind);
			if(bulk->cb[i].r_code) free( bulk->cb[i].r_code);
			if(bulk->cb[i].r_len) free( bulk->cb[i].r_len);
			if(bulk->cb[i].a_col) free( bulk->cb[i].a_col);
		}
		free(bulk->cb);
		bulk->cb=NULL;
	}
	bulk->cols=0;
}
static void dcb_free(BULK *bulk)
{
int i;
	if(bulk->cb) {
		for(i=0;i<bulk->dcols;i++) {
			if(bulk->dcb[i].ind) free( bulk->dcb[i].ind);
			if(bulk->dcb[i].r_code) free( bulk->dcb[i].r_code);
			if(bulk->dcb[i].r_len) free( bulk->dcb[i].r_len);
			if(bulk->dcb[i].a_col) free( bulk->dcb[i].a_col);
		}
		free(bulk->dcb);
		bulk->dcb=NULL;
	}
	bulk->dcols=0;
}

void BULK_init(BULK *bulk,DAU *DP,void *recs,int max_rows_of_fetch)
{
	bulk->recs=recs;
	bulk->rows=0;
	bulk->bind_rows=0;
    if(DP) {
	bulk->max_rows_of_fetch= max_rows_of_fetch;
	bulk->SQL_Connect=DP->SQL_Connect;
	bulk->srm=&DP->srm;
	bulk->reclen=SRM_RecSize(bulk->srm);
	bulk->sth=SQLO_STH_INIT;
	bulk->cb=NULL;
	bulk->dcb=NULL;
	bulk->dcols=abs(bulk->srm->Aflg);
	bulk->cols=0;
	bulk->bind_tree=NULL;
	bulk->prows=0;
    } else {//次级初始化 设置新的结果集
	bind_rows_free(bulk);//清除查询条件
	if(bulk->max_rows_of_fetch < max_rows_of_fetch) {
		dcb_free(bulk);//清除旧的结果集
		bulk->dcols=abs(bulk->srm->Aflg);
		bulk->max_rows_of_fetch= max_rows_of_fetch;
		BULK_alloc(bulk);//重新分配
	} else bulk->max_rows_of_fetch= max_rows_of_fetch;
	define(bulk);//定义新的结果集
    }
//ShowLog(5,"%s:recs=%016lX",__FUNCTION__,bulk->recs);
}
void BULK_free(BULK *bulk)
{
int error=0;

	if(!bulk) return;
	if(bulk->SQL_Connect)
		error=bulk->SQL_Connect->Errno;
	bulk->max_rows_of_fetch=0;
	bulk->recs=0;
	BB_Tree_Free(&bulk->bind_tree,NULL);
	bind_rows_free(bulk);
	dcb_free(bulk);
	if(bulk->sth != SQLO_STH_INIT) {
		___SQL_Close__(bulk->SQL_Connect,bulk->sth);
		bulk->sth=SQLO_STH_INIT;
	}
	bulk->cols=0;
	bulk->dcols=0;
	bulk->srm=NULL;
	bulk->reclen=0;
	bulk->SQL_Connect=NULL;
}

//这里定义bind树的数据结构和回调函数
struct bindnod {  //回调函数不能取得外部资源，所有资源必须定义在节点里
	int bindnum;
	T_PkgType *tp;
	char *last_bindp;
	short flg;
	BULK *bulk;
};
//比较函数
static int bind_Cmp(void *rec1,void *rec2,int len)
{
register struct bindnod *dap1,*dap2;
int cc;

        dap1=(struct bindnod *)rec1;
        dap2=(struct bindnod *)rec2;
        cc=dap1->bindnum - dap2->bindnum;
        if(cc<0) return -2;
        else if (cc>0) return 2;
        return 0;
}

static int bind_proc(void *content)   //bind处理
{
register struct bindnod *bp=(struct bindnod *)content;
T_PkgType *tp=bp->tp;
BULK *bulk=bp->bulk;
char *p,*bindp;
int ret,n,rows,reclen,i;
col_bag *cb;

	n=bp->bindnum-1;
	rows=bulk->bind_rows;
	reclen=bulk->reclen;
	p=bulk->recs+tp->offset;
//ShowLog(5,"bind_proc sth=%d,n=%d:%s,rows=%d,cb=%16llX",bulk->sth,n,tp->name,rows,(long)bulk->cb);
	cb=&bulk->cb[n];
	cb->name=(char *)plain_name(tp->name);
//ShowLog(5,"bind_proc n=%d:%s",n,cb->name);
	bindp=p;

    ret=0;
       switch(tp->type) {

#ifdef SDBC_PTR_64
        case CH_INT64:
#endif

        case CH_LONG:
        case CH_TINY:
        case CH_SHORT:
        case CH_INT:
            ret=sqlo_bind_by_pos(bulk->sth, bp->bindnum, SQLOT_INT, bindp, tp->len, cb->ind, reclen);
                break;
        case CH_FLOAT:
        case CH_DOUBLE:
            ret=sqlo_bind_by_pos(bulk->sth, bp->bindnum, SQLOT_FLT, bindp, tp->len, cb->ind, reclen);
                break;
        case CH_BYTE:
            ret=sqlo_bind_by_pos(bulk->sth, bp->bindnum, SQLOT_BIN, bindp, tp->len, cb->ind, reclen);
                break;
        case CH_CHAR:
        case CH_CNUM:
        case CH_DATE:
            ret=sqlo_bind_by_pos(bulk->sth, bp->bindnum, SQLOT_STR, bindp, tp->len, cb->ind, reclen);
                break;
        default:    //类型与ORACLE不符，需变换
//ShowLog(5,"%s name=%s",__FUNCTION__,tp->name);
			if(!cb->a_col) cb->a_col=(char *)calloc(bulk->bind_rows,BULK_A_COL_LEN);
			bindp=cb->a_col;
			for(i=0;i<bulk->bind_rows;i++) {//把所有值拿到变换数组
				get_one_str(bindp,p,tp,0);
				p+=reclen;
				bindp+=BULK_A_COL_LEN;
			}
			ret=sqlo_bind_by_pos(bulk->sth, bp->bindnum, SQLOT_STR, cb->a_col,BULK_A_COL_LEN,cb->ind, BULK_A_COL_LEN);
            break;
    	}
//ShowLog(5,"bind_proc END sth=%d,%s:%d",bulk->sth,tp->name,bp->bindnum);
    	if(ret) {
        	ShowLog(1,"%s %s:ret=%d,bindnum=%d",__FUNCTION__,tp->name,ret,bp->bindnum);
    	} else {//如果没有错误，设置ind.
		for(i=0;i<bulk->bind_rows;i++) {
			if(isnull(bindp,tp->type)) cb->ind[i]=-1;
			else cb->ind[i]=0;
		}
	}
//ShowLog(5,"bind_proc sth=%d,%s:%d,%s,ret=%d",sth,tp->name,bp->bindnum,errb,ret);
    return 0;
}

static int pre_bind_array(BULK *bulk,char *stmt,struct bindnod *bnode)
{
char *rep,*ep;
char *bindp,c,buf[100];
regmatch_t match[3];
int n,colnum;
T_PkgType *tp;

	bulk->SQL_Connect->Errno=0;
	ep=stmt;
	colnum=abs(bulk->srm->Aflg);
	while(*(ep=stptok(ep,0,0,"$:\'"))) {
		if(*ep=='\'') {	//剔除引号里的占位符
			ep++;
			ep=stptok(ep,0,0,"\'");
			if(*ep=='\'') {
				ep++;
				continue;
			}
			break;
		}
		if((ep[-1]&0x80) || isalpha(ep[-1])) {
			ep++;
			continue;
		}
		if(!do_regexp(ep,match)) { //查找占位符
			rep=ep+match[0].rm_so;
			bindp=ep+match[1].rm_so;
			ep+=match[0].rm_eo;
			c=*ep;
			*ep=0;	//临时给尾0
			n=index_col(bulk->srm->colidx,colnum,bindp,bulk->srm->tp);
			tp=&bulk->srm->tp[n];
			*ep=c;
			if(*rep=='$') {   //伪列名替换成真列名
			char *trup;
				if(n<0) continue;		//如果没有该列名，不替换
				trup=(char *)tp->name;
				strtcpy(buf,&trup,' ');
//ShowLog(5,"pre_bind:aft pkg_getType %s type=%d,name=%s,buf=%s",bindp,tp->type,tp->name,buf);
				ep=strsubst(rep,(int)(ep-rep),buf);//替换成 真列名
				continue;
			}
			if(n<0) {
				sprintf(bulk->SQL_Connect->ErrMsg," :pre_bind:%.*s 无效列名",(int)(ep-bindp),bindp);
				bulk->SQL_Connect->Errno=FORMATERR;
				BB_Tree_Free(&bulk->bind_tree,0);
				return FORMATERR;
			}
			mark_subst(buf,tp,&bulk->cols);
			bindp--;
			ep=strsubst(bindp,(int)(ep-bindp),buf);//替换成数字
			bnode->bindnum = ++bulk->cols;
			bnode->tp=tp;
			bulk->bind_tree=BB_Tree_Add(bulk->bind_tree,bnode,sizeof(struct bindnod),bind_Cmp,0); //保存bind参数
		} else ep++;
	}

	return 0;
}
//分配bulk空间
static int BULK_alloc(BULK *bulk)
{
int n;
	if(bulk->cols>0 && !bulk->cb && 0!=(bulk->cb=malloc(bulk->cols * sizeof(col_bag)))) {
ShowLog(5,"%s:cols=%d,bind_rows=%d",__FUNCTION__,bulk->cols,bulk->bind_rows);
		for(n=0;n<bulk->cols;n++) {
			bulk->cb[n].name=0;
			bulk->cb[n].a_col=0;
			bulk->cb[n].r_code=bulk->cb[n].r_len=0;
			if(!(bulk->cb[n].ind=(short *)calloc(bulk->bind_rows,sizeof(short)))) {
			int i;
				ShowLog(1,"%s calloc cb fail at n=%d",__FUNCTION__,n);
				for(i=0;i<n;i++) free(bulk->cb[i].ind);
				free(bulk->cb);
				bulk->cb=0;
				BB_Tree_Free(&bulk->bind_tree,0);
				return MEMERR;
			}
		}
	}
	if(!bulk->dcb) {
		if(0!=(bulk->dcb=malloc(bulk->dcols * sizeof(col_bag)))) {
			for(n=0;n<bulk->dcols;n++) {
				bulk->dcb[n].name=0;
				bulk->dcb[n].a_col=NULL;
				bulk->dcb[n].r_code=bulk->dcb[n].r_len=NULL;
				if(!(bulk->dcb[n].ind=(short *)malloc(bulk->max_rows_of_fetch*sizeof(short)))) {
				int i;
					ShowLog(1,"%s calloc dcb fail at n=%d",__FUNCTION__,n);
					for(i=0;i<n-1;i++) free(bulk->dcb[i].ind);
					free(bulk->dcb);
					bulk->dcb=NULL;
					bind_rows_free(bulk);
					return MEMERR;
				}
			}
		} else {
			ShowLog(1,"%s malloc dcb fail!",__FUNCTION__);
			bind_rows_free(bulk);
			return MEMERR;
		}
	}
	if(!bulk->cb) {
		ShowLog(5,"%s cb empty!",__FUNCTION__);
	}
	return 0;
}

static int define(BULK *bulk)
{
T_PkgType *tp=bulk->srm->tp;
int i,ret=0;
char *p;
col_bag *cb;

	cb=bulk->dcb;
	for(i=0;tp->type>-1;i++,tp++) {
		if(tp->bindtype & NOSELECT) {
			i--;
			continue;
		}
		p=((char *)bulk->recs);
		p+=tp->offset;
		switch(tp->type) {
		case CH_FLOAT:
		case CH_DOUBLE:
		case CH_LDOUBLE:
			ret=sqlo_define_by_pos(bulk->sth, i+1, SQLOT_FLT,
				p,tp->len, cb->ind, 0,
				bulk->reclen);
			break;
		case CH_TINY:
		case CH_SHORT:
		case CH_INT:
		case CH_LONG:
#ifdef SDBC_PTR_64
	        case CH_INT64:
#endif
			ret=sqlo_define_by_pos(bulk->sth, i+1, SQLOT_INT,
				p,tp->len, cb->ind, 0,
				bulk->reclen);
			break;
		case CH_JUL:
		case CH_CJUL:
		case CH_MINUTS:
		case CH_CMINUTS:
		case CH_TIME:
		case CH_CTIME:
		case CH_USEC:
			if(!cb->a_col)
				cb->a_col=malloc(bulk->max_rows_of_fetch*BULK_A_COL_LEN);
			if(!cb->r_len)
				cb->r_len=(short *)malloc(bulk->max_rows_of_fetch * sizeof(short));
//			if(!cb->r_code)
//				cb->r_code=(short *)malloc(bulk->max_rows_of_fetch * sizeof(short));
			ret=sqlo_define_by_pos(bulk->sth, i+1, SQLOT_STR,
				cb->a_col, BULK_A_COL_LEN, cb->ind, cb->r_len,
//				cb->r_code,
				1);	//1=列式绑定数组
//ShowLog(5,"%s: %d BULK_A_COL_LEN=%d,ret=%d",__FUNCTION__,i+1,BULK_A_COL_LEN,ret);
			break;
		case CH_DATE:
		case CH_BYTE:
		case CH_CNUM:
		case CH_CHAR:
			if(!cb->r_len)
				cb->r_len=(short *)malloc(bulk->max_rows_of_fetch * sizeof(short));
//			if(!cb->r_code)
//				cb->r_code=(short *)malloc(bulk->max_rows_of_fetch * sizeof(short));
				ret=sqlo_define_by_pos(bulk->sth, i+1, SQLOT_STR,
					p, tp->len, cb->ind, cb->r_len,
//                           		cb->r_code,
					bulk->reclen);
			break;
		default:
//CH_CLOB????
			i--;
			break;
		}
		if(ret<0) {
			___SQL_GetError(bulk->SQL_Connect);
			ShowLog(1,"%s:sqlo_define %d,err=%d,%s",__FUNCTION__, i,
				bulk->SQL_Connect->Errno,bulk->SQL_Connect->ErrMsg);
			return ret;
		}
		cb++;
	}
	bulk->dcols=i;
	sqlo_set_prefetch_rows(bulk->sth,bulk->max_rows_of_fetch);
	return i;
}

int BULK_prepare(BULK *bulk,char *stmt,int bind_rows)
{
int ret=0;
//char *p=stmt;
	if(bulk->sth < 0 && stmt) {
	struct bindnod bnode;
		bnode.bindnum=0;
		bnode.flg=0;
		bnode.last_bindp=0;
		bnode.bulk=bulk;
//制造语句
	 	ret=SRM_mk_select(bulk->srm,bulk->SQL_Connect->DBOWN,stmt);
//生成绑定树
		if(bind_rows>0) {
			ret=pre_bind_array(bulk,stmt,&bnode);
			if(ret) {
				if(bulk->bind_tree) {
					BB_Tree_Free(&bulk->bind_tree,NULL);
				}
				return ret;
			}
		}
//prepare
		bulk->sth=sqlo_prepare(bulk->SQL_Connect->dbh,stmt);
		if(bulk->sth < 0) {
			___SQL_GetError(bulk->SQL_Connect);
			sprintf(stmt+strlen(stmt),",err=%d,%s",
				bulk->SQL_Connect->Errno,
				bulk->SQL_Connect->ErrMsg);
			return(-1);
		}
		ShowLog(5,"%s sth=%d,%s",__FUNCTION__,bulk->sth,stmt);
	}
	bulk->prows=0;
//分配bulk空间
	if(bulk->bind_rows != bind_rows) {
		if(bulk->bind_rows) bind_rows_free(bulk);
		bulk->bind_rows=bind_rows;
	}
	if(0!=(ret=BULK_alloc(bulk))) {
		ShowLog(1,"%s malloc bulk fail %d !",__FUNCTION__,ret);
		return MEMERR;
	}
//define 结果集
	int cc=define(bulk);
if(cc<0) {
	ShowLog(1,"%s:define fault cc=%d",__FUNCTION__,cc);
	return -1;
}
	ret=(bind_rows>0) ?
//bind 查询条件
		BB_Tree_Scan(bulk->bind_tree,bind_proc),
		sqlo_execute1(bulk->sth,0,bulk->bind_rows):
		sqlo_execute(bulk->sth,0);
	if(ret<0) {
		___SQL_GetError(bulk->SQL_Connect);
		ShowLog(1,"%s:execute ret=%d,err=%d,%s",__FUNCTION__,ret,
			bulk->SQL_Connect->Errno,
			bulk->SQL_Connect->ErrMsg);
		BB_Tree_Free(&bulk->bind_tree,NULL);
		BULK_free(bulk);
		return ret;
	}
ShowLog(5,"%s:prows=%d",__FUNCTION__,sqlo_prows(bulk->sth));
	return ret;
}

static int bind_data(BULK *bulk)   //null和附加列处理
{
T_PkgType *tp=bulk->srm->tp;
char *p,*bindp;
int ret,n,rows,reclen;
col_bag *cb;
short *indp;

	reclen=bulk->reclen;
	rows=bulk->rows;
	cb=bulk->dcb;

	for(n=0;tp->type>-1;tp++) {
		if(tp->bindtype & NOSELECT) {
			continue;
		}
		bindp=(char *)bulk->recs;
		indp=cb->ind;

    		switch(tp->type) {
		case CH_CLOB://暂时不处理CLOB
			n++;
			cb++;
			continue;
   //类型与ORACLE不符，需变换
       		 case CH_JUL:
       		 case CH_CJUL:
       		 case CH_MINUTS:
       		 case CH_CMINUTS:
       		 case CH_TIME:
       		 case CH_CTIME:
       		 case CH_USEC:
			p=cb->a_col;
			for(ret=0;ret<bulk->rows;ret++,indp++) {
				if(*indp<0) *p=0;
       				put_str_one(bindp,p,tp,0);
				bindp += reclen;
				p += BULK_A_COL_LEN;
			}
       			break;
        	default:
			for(ret=0;ret<bulk->rows;ret++,indp++) {
				if(*indp<0) put_str_one(bindp,"",tp,0);
				bindp += reclen;
			}
               		break;
		}
		n++;
		cb++;
	}
	return 0;
}

int BULK_fetch(BULK *bulk)
{
int ret,prows;
	ret=sqlo_fetch(bulk->sth, bulk->max_rows_of_fetch);
	if(ret<0) {
		___SQL_GetError(bulk->SQL_Connect);
		return -1;
	}
	bulk->rows=0;
	if(0<(prows=sqlo_prows(bulk->sth))) {
		bulk->rows=prows-bulk->prows;
		if(bulk->rows>0) {
			bind_data(bulk);
			bulk->prows=prows;
		}
	}
	return bulk->rows;
}

char * BULK_pkg_dispack(BULK *bulk,int n,char *buf,char delimit)
{
char *p;

	p=buf;
	p+=pkg_dispack(bulk->recs + bulk->reclen*n,buf,bulk->srm->tp,delimit);
	return p;
}

char * BULK_pkg_pack(BULK *bulk,int n,char *buf,char delimit)
{
char *p;

	p=buf;
	p+=pkg_pack(buf,bulk->recs + bulk->reclen*n,bulk->srm->tp,delimit);
	return p;
}
