#define _GNU_SOURCE
#include <DAU.h>
#include <OAD.h>
#include <regex.h>
#include <ctype.h>

extern int do_regexp(char *stmt,regmatch_t *match);
extern char * mark_subst(char *vp,T_PkgType *typ,int *bindnum);
extern void set_dbo(char *buf,char *DBOWN);
static int OAD_alloc(OAD *oad);
static int bind_proc(void *content);   //bind处理

void OAD_init(OAD *oad,DAU *DP,void *recs,int max_rows_of_batch)
{
int i;
	oad->recs=recs;
    if(DP) {
	oad->SQL_Connect=DP->SQL_Connect;
	oad->srm=&DP->srm;
	oad->reclen=SRM_RecSize(oad->srm);
	oad->sth=SQLO_STH_INIT;
	oad->max_rows_of_batch=max_rows_of_batch;
	oad->cols=0;
	oad->rows=0;
	oad->cb=0;
	oad->bind_tree=0;
	oad->begin=0;
	oad->a_col_flg=0;
    } else {//次级初始化
	if(oad->max_rows_of_batch < max_rows_of_batch) {
	    if(oad->cb) {
	    col_bag *colp=oad->cb;
		for(i=0;i<oad->cols;i++,colp++) {
			if(colp->ind) free( colp->ind);
			if(colp->r_code) free( colp->r_code);
			if(colp->r_len) free( colp->r_len);
			if(colp->a_col) free( colp->a_col);
		}
		free(oad->cb);
		oad->cb=NULL;
	    }
	    oad->max_rows_of_batch=max_rows_of_batch;
	    OAD_alloc(oad);
	} else oad->max_rows_of_batch=max_rows_of_batch;
	BB_Tree_Scan(oad->bind_tree,bind_proc);
    }
}
void OAD_free(OAD *oad)
{
int i,error=0;

	if(!oad) return;
	if(oad->SQL_Connect)
		error=oad->SQL_Connect->Errno;
	oad->max_rows_of_batch=0;
	oad->recs=0;
	BB_Tree_Free(&oad->bind_tree,0);
	if(oad->cb) {
		for(i=0;i<oad->cols;i++) {
			if(oad->cb[i].ind) free( oad->cb[i].ind);
			if(oad->cb[i].r_code) free( oad->cb[i].r_code);
			if(oad->cb[i].r_len) free( oad->cb[i].r_len);
			if(oad->cb[i].a_col) free( oad->cb[i].a_col);
		}
		free(oad->cb);
		oad->cb=0;
	}
	if(oad->sth != SQLO_STH_INIT) {
		___SQL_Close__(oad->SQL_Connect,oad->sth);
		oad->sth=SQLO_STH_INIT;
	}
	oad->a_col_flg=0;
	oad->cols=0;
	oad->begin=0;
	oad->srm=NULL;
	oad->reclen=0;
	if(error) oad->SQL_Connect->Errno=error;
	oad->SQL_Connect=NULL;
}

//这里定义bind树的数据结构和回调函数
struct bindnod {  //回调函数不能取得外部资源，所有资源必须定义在节点里
	int bindnum;
	T_PkgType *tp;
	char *last_bindp;
	short flg;
	OAD *oad;
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
OAD *oad=bp->oad;
char *p,*bindp;
int ret,n,rows,reclen;
short *ind;
col_bag *cb;

	n=bp->bindnum-1;
	rows=oad->max_rows_of_batch;
	reclen=oad->reclen;
	p=oad->recs+tp->offset;
//ShowLog(5,"bind_proc sth=%d,n=%d:%s,rows=%d,cb=%16llX",oad->sth,n,tp->name,rows,(long)oad->cb);
	cb=&oad->cb[n];
	cb->name=(char *)plain_name(tp->name);
//ShowLog(5,"bind_proc n=%d:%s",n,cb->name);
	bindp=p;
	ind=cb->ind;

    ret=0;
       switch(tp->type) {

#ifdef SDBC_PTR_64
        case CH_INT64:
#endif

        case CH_LONG:
        case CH_TINY:
        case CH_SHORT:
        case CH_INT:
            ret=sqlo_bind_by_pos(oad->sth, bp->bindnum, SQLOT_INT, bindp, tp->len, ind, reclen);
                break;
        case CH_FLOAT:
        case CH_DOUBLE:
            ret=sqlo_bind_by_pos(oad->sth, bp->bindnum, SQLOT_FLT, bindp, tp->len, ind, reclen);
                break;
        case CH_BYTE:
            ret=sqlo_bind_by_pos(oad->sth, bp->bindnum, SQLOT_BIN, bindp, tp->len, ind, reclen);
                break;
        case CH_CHAR:
        case CH_CNUM:
        case CH_DATE:
            ret=sqlo_bind_by_pos(oad->sth, bp->bindnum, SQLOT_STR, bindp, tp->len, ind, reclen);
                break;
        default:    //类型与ORACLE不符，需变换
//ShowLog(5,"%s name=%s",__FUNCTION__,tp->name);
			if(!cb->a_col) cb->a_col=(char *)calloc(oad->max_rows_of_batch,A_COL_LEN);
			bindp=cb->a_col;
			ret=sqlo_bind_by_pos(oad->sth, bp->bindnum, SQLOT_STR, bindp,A_COL_LEN,ind, A_COL_LEN);
            break;
    	}
if(ret) ShowLog(1,"bind_proc:ret=%d sth=%d,%s:%d",ret,oad->sth,tp->name,bp->bindnum);
    	if(ret) {
        	ShowLog(1,"%s %s:ret=%d,bindnum=%d",__FUNCTION__,tp->name,ret,bp->bindnum);
    	}
//ShowLog(5,"bind_proc sth=%d,%s:%d,%s,ret=%d",sth,tp->name,bp->bindnum,errb,ret);
    return 0;
}

static int bind_data(void *content)   //null和附加列处理
{
struct bindnod *bp=(struct bindnod *)content;
T_PkgType *tp=bp->tp;
OAD *oad=bp->oad;
char *p,*bindp;
int ret,n,rows,reclen;
col_bag *cb;

	n=bp->bindnum-1;
	rows=oad->rows;
	reclen=oad->reclen;
	bindp=oad->recs+tp->offset;
	cb=&oad->cb[n];
//set indicator for null
	for(ret=0;ret<rows;ret++) {
		if(!(tp->bindtype & RETURNING) && isnull(bindp,tp->type))
			cb->ind[ret]=-1;
		else cb->ind[ret]=0;
		bindp += reclen;
	}

//ShowLog(5,"bind_data sth=%d,%s:%d,type=0X%04X",oad->sth,tp->name,bp->bindnum,tp->type);
    ret=0;
    switch(tp->type) {

#ifdef SDBC_PTR_64
        case CH_INT64:
#endif

        case CH_LONG:
        case CH_TINY:
        case CH_SHORT:
        case CH_INT:
        case CH_FLOAT:
        case CH_DOUBLE:
        case CH_BYTE:
        case CH_CHAR:
        case CH_CNUM:
        case CH_DATE:
                break;
        default:    //类型与ORACLE不符，需变换
			oad->a_col_flg |= 1;
			bindp=cb->a_col;
			p=oad->recs;
			for(ret=0;ret<oad->rows;ret++) {
            			get_one_str(bindp,p,tp,0);
				p += reclen;
				bindp += A_COL_LEN;
			}
            break;
	}
//ShowLog(5,"bind_proc sth=%d,%s:%d,%s,ret=%d",sth,tp->name,bp->bindnum,errb,ret);
    return 0;
}

static int get_ret(void *content)	//get_returning 处理
{
struct bindnod *bp=(struct bindnod *)content;
T_PkgType *tp=bp->tp;
int i;
char *a_col,*rec;
OAD *oad=bp->oad;

	if(!(tp->bindtype & RETURNING)) return 0;
	a_col=oad->cb[bp->bindnum-1].a_col;
	if(!a_col) return 0;
	rec=oad->recs+tp->offset+oad->begin*oad->reclen;
	for(i=0;i<oad->rows;i++) {
		put_str_one(rec,a_col,tp,0);
		rec += oad->reclen;
		a_col += A_COL_LEN;
	}
	return 0;
}

//占位符替换为bind POS,并构建bind树
static char *mk_col_bind(char *values, OAD *oad,struct bindnod *bnode)
{
T_PkgType *typ;
char *vp;
int i;

        if(!values) return values;
        vp=values;
        *vp=0;
		typ=oad->srm->tp;
        if(typ->offset<0) set_offset(typ);
        for(i=0;typ->type != -1;i++,typ++) {
            if((typ->bindtype&NOINS) || typ->type == CH_CLOB || !strcmp(typ->name,"ROWID")) {
/* can not insert CLOB *p and ROWID into database,skip it*/
                continue;
            }
	    if(!typ->format) goto dflt;
	    switch(typ->type) {
	    case CH_USEC:
			vp += sprintf(vp,"TO_TIMESTAMP(:%d,'",++oad->cols);
			vp = stpcpy(ext_copy(vp,typ->format),"'),");
			break;
	    case CH_DATE:
	    	if(is_timestamp((char *)typ->format)) {
				vp += sprintf(vp,"TO_TIMESTAMP(:%d,'",++oad->cols);
				vp = stpcpy(ext_copy(vp,typ->format),"'),");
				break;
			}
	    case CH_JUL:
	    case CH_TIME:
	    case CH_MINUTS:
			vp += sprintf(vp,"TO_DATE(:%d,'",++oad->cols);
			vp = stpcpy(ext_copy(vp,typ->format),"'),");
	    	break;
	    default:
dflt:
	    	vp+=sprintf(vp," :%d,",++oad->cols);
		break;
	    }
	    bnode->tp=typ;
	    bnode->bindnum=oad->cols;
	    oad->bind_tree=BB_Tree_Add(oad->bind_tree,bnode,sizeof(struct bindnod),bind_Cmp,0);
	}
	if(*(vp-1) == ',') *(--vp)=0;
        return vp;
}

static int pre_bind_array(OAD *oad,char *stmt,struct bindnod *bnode)
{
char *rep,*ep;
char *bindp,c,buf[100];
regmatch_t match[3];
int n,colnum;
char *into;
T_PkgType *tp;

	oad->SQL_Connect->Errno=0;
	ep=stmt;
	colnum=abs(oad->srm->Aflg);
	into=strcasestr(stmt," INTO :");
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
			n=index_col(oad->srm->colidx,colnum,bindp,oad->srm->tp);
			tp=&oad->srm->tp[n];
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
				sprintf(oad->SQL_Connect->ErrMsg," :pre_bind:%.*s 无效列名",(int)(ep-bindp),bindp);
				oad->SQL_Connect->Errno=FORMATERR;
				BB_Tree_Free(&oad->bind_tree,0);
				return FORMATERR;
			}
			if(!into || bindp<into)
				 mark_subst(buf,tp,&oad->cols);
			else sprintf(buf,":%d",oad->cols+1);
			bindp--;
			ep=strsubst(bindp,(int)(ep-bindp),buf);//替换成数字
			bnode->bindnum = ++oad->cols;
			bnode->tp=tp;
			oad->bind_tree=BB_Tree_Add(oad->bind_tree,bnode,sizeof(struct bindnod),bind_Cmp,0); //保存bind参数
		} else ep++;
	}

//	BB_Tree_Free(&oad->bind_tree,0);
	return 0;
}
//分配oad空间
static int OAD_alloc(OAD *oad)
{
int n;
col_bag *colp;
	if(oad->cols<=0) return LENGERR;
	if(0!=(oad->cb=malloc(oad->cols * sizeof(col_bag)))) {
		colp=oad->cb;
		for(n=0;n<oad->cols;n++,colp++) {
			colp->name=colp->a_col=0;
			colp->r_code=colp->r_len=0;
			if(!(colp->ind=(short *)calloc(oad->max_rows_of_batch,sizeof(short)))) {
			int i;
				ShowLog(1,"%s calloc cb fail at n=%d",__FUNCTION__,n);
				for(i=0;i<n;i++) free(oad->cb[i].ind);
				free(oad->cb);
				oad->cb=NULL;
				BB_Tree_Free(&oad->bind_tree,0);
				return MEMERR;
			}
		}
	} else {
		ShowLog(1,"%s malloc cb fail!",__FUNCTION__);
		return MEMERR;
	}
	return 0;
}

int OAD_mk_ins(OAD *oad,char *stmt)
{
int ret=0;
char *p=stmt;
struct bindnod bnode;

	if(oad->sth < 0) {
	char *returning=0;
		if(*p) { //有 RETURNING 子句
			returning=strdup(p);
		}
		bnode.bindnum=0;
		bnode.flg=0;
		bnode.last_bindp=0;
		bnode.oad=oad;
//制造语句
        	if(oad->srm->befor) {
            		p=stpcpy(p,oad->srm->befor);
            		*p++ = ' ';
            		oad->srm->befor=0;
        	}

        	p=stpcpy(p,"INSERT ");
        	if(oad->srm->hint&&*oad->srm->hint) {
            		p=stpcpy(p,oad->srm->hint);
            		*p++=' ';
            		*p=0;
        	}
        	p=stpcpy(p,"INTO ");
        	if(*oad->SQL_Connect->DBOWN&&!strchr(oad->srm->tabname,'.')) {
            		p=stpcpy(p,oad->SQL_Connect->DBOWN);
            		*p++='.';
            		*p=0;
        	}
        	p=stpcpy(mkset(stpcpy(stpcpy(p,oad->srm->tabname)," ("),oad->srm->tp),") VALUES (");
//生成占位符
        	p=stpcpy(mk_col_bind(p,oad,&bnode),")");
        	if(returning) {
        		char *  p1=p;
            		p=stpcpy(p,returning);
            		free(returning);
            		returning=p1;
        	}
        	set_dbo(stmt,oad->SQL_Connect->DBOWN);
        	if(returning) {
			ret=pre_bind_array(oad,returning,&bnode);
			if(ret) {
				return ret;
			}
            		returning=0;
        	}
//分配oad空间
		if(0!=(ret=OAD_alloc(oad))) {
			ShowLog(1,"%s malloc cb fail %d !",__FUNCTION__,ret);
			return MEMERR;
		}
//prepare
		oad->sth=sqlo_prepare(oad->SQL_Connect->dbh,stmt);
		if(oad->sth < 0) {
			___SQL_GetError(oad->SQL_Connect);
			sprintf(stmt+strlen(stmt),",err=%d,%s",
				oad->SQL_Connect->Errno,
				oad->SQL_Connect->ErrMsg);
			OAD_free(oad);
			return(-1);
		}
		ShowLog(5,"%s sth=%d,%s",__FUNCTION__,oad->sth,stmt);
//bind
		BB_Tree_Scan(oad->bind_tree,bind_proc);
	}
	return 0;
}

int OAD_mk_update(OAD *oadp,char *where)
{
int ret=0;
register char *p;
struct bindnod bnode;
char *save_where=NULL;

	p=where;
	if(!p) return -1;
	oadp->SQL_Connect->Errno=0;
    	*oadp->SQL_Connect->ErrMsg = 0;

	bnode.bindnum=0;
	bnode.last_bindp=0;
	bnode.oad=oadp;
	bnode.flg=0;

	if(*p) save_where=strdup(p);

	if(toupper(*p) != 'U') {
		p=stpcpy(mk_col_bind(stpcpy(mkset(stpcpy(DAU_mk_update(OAD_get_DAU(oadp),where), " SET("),
			oadp->srm->tp),")=(SELECT "),oadp,&bnode)," FROM DUAL) ");

		if(save_where) {
			strcpy(p,save_where);
			free(save_where);
			save_where=NULL;
		}
	}
	oadp->srm->hint=0;
	set_dbo(where,oadp->SQL_Connect->DBOWN);
       	ret=pre_bind_array(oadp,p,&bnode);
	if(0!=ret) {
		BB_Tree_Free(&oadp->bind_tree,0);
		return ret;
	}
//分配oad空间
	if(0!=(ret=OAD_alloc(oadp))) {
		ShowLog(1,"%s:malloc cb fail %d !",__FUNCTION__,ret);
		return MEMERR;
	}

       	oadp->sth=sqlo_prepare(oadp->SQL_Connect->dbh, (CONST char *)where);
       	if(oadp->sth<0) {
	        ___SQL_GetError(oadp->SQL_Connect);
	        sprintf(p+strlen(p)," %s:sqlo_prepare=%d,err=%d,%s",__FUNCTION__,oadp->sth,
	                oadp->SQL_Connect->Errno,
	                oadp->SQL_Connect->ErrMsg);
	        return -1;
	} else ShowLog(5,"%s:sth=%d,stmt=%s,",__FUNCTION__,oadp->sth,where);
    	oadp->SQL_Connect->NativeError=0;
    	BB_Tree_Scan(oadp->bind_tree,bind_proc);  //bind
	return 0;
}

int OAD_mk_del(OAD *oadp,char *where)
{
int ret=0;
struct bindnod bnode;

	if(!oadp||!where) return -1;
	oadp->SQL_Connect->Errno=0;
    	*oadp->SQL_Connect->ErrMsg = 0;

	bnode.bindnum=0;
	bnode.last_bindp=0;
	bnode.oad=oadp;
	bnode.flg=0;

	ret=DAU_mk_delete(OAD_get_DAU(oadp),where);
	set_dbo(where,oadp->SQL_Connect->DBOWN);
       	ret=pre_bind_array(oadp,where,&bnode);
	if(0!=ret) {
		BB_Tree_Free(&oadp->bind_tree,0);
		return ret;
	}
//分配oad空间
	if(0!=(ret=OAD_alloc(oadp))) {
		ShowLog(1,"%s:malloc cb fail %d !",__FUNCTION__,ret);
		return MEMERR;
	}

       	oadp->sth=sqlo_prepare(oadp->SQL_Connect->dbh, (CONST char *)where);
       	if(oadp->sth<0) {
	        ___SQL_GetError(oadp->SQL_Connect);
	        sprintf(where+strlen(where)," %s:sqlo_prepare=%d,err=%d,%s",__FUNCTION__,oadp->sth,
	                oadp->SQL_Connect->Errno,
	                oadp->SQL_Connect->ErrMsg);
	        return -1;
	} else ShowLog(5,"%s:sth=%d,stmt=%s,",__FUNCTION__,oadp->sth,where);
    	oadp->SQL_Connect->NativeError=0;
    	BB_Tree_Scan(oadp->bind_tree,bind_proc);  //bind
	return 0;
}

int OAD_exec(OAD *oad,int begin,int n)
{
int ret;

	oad->begin=begin;
	oad->rows=(n<=(oad->max_rows_of_batch-begin))?n:oad->max_rows_of_batch-begin;
	if(!begin) { //只有原始的执行如此
//ShowLog(5,"%s:tabname=%s",__FUNCTION__,oad->srm->tabname);
		BB_Tree_Scan(oad->bind_tree,bind_data);
	}
	ret=sqlo_execute1(oad->sth,oad->begin,n);
	if(oad->a_col_flg) BB_Tree_Scan(oad->bind_tree,get_ret); //如果有RETURNING变量，取回。
	if(ret) {
		if(ret>0) {
			ret=sqlo_prows(oad->sth);
		}
		___SQL_GetError(oad->SQL_Connect);
		return ret;
	}
	return sqlo_prows(oad->sth);
}

char * OAD_pkg_dispack(OAD *oad,int n,char *buf,char delimit)
{
char *p;

	p=buf;
	p+=pkg_dispack(oad->recs + oad->reclen*n,buf,oad->srm->tp,delimit);
	return p;
}

char * OAD_pkg_pack(OAD *oad,int n,char *buf,char delimit)
{
char *p;

	p=buf;
	p+=pkg_pack(buf,oad->recs + oad->reclen*n,oad->srm->tp,delimit);
	return p;
}
