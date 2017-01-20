
#define _GNU_SOURCE

#include <DAU.h>
#include <ctype.h>
#include <regex.h>
#include <unistd.h>

//搜索占位符的
static char *regstr="^[$:]([A-Za-z_][0-9A-Za-z_]*)";  //占位符
static regex_t zwreg;
static volatile int zwflg=0;
static pthread_mutex_t bind_mutex=PTHREAD_MUTEX_INITIALIZER;

//这里定义bind树的数据结构和回调函数
struct bindnod {  //回调函数不能取得外部资源，所有资源必须定义在节点里
	int bindnum;
	T_PkgType *tp;
	char **rec;
	char *last_bindp;
	int *sth;
	short flg;
	short ind;
	char **tail;
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

static char *pack_mk_fmt(const char *format,char *buf)
{
	ext_copy(buf,format);
	return buf;
}

int reset_bind(void *content) //复位标志，准备重新bind
{
struct bindnod *bp;

	bp=(struct bindnod *)content;
	bp->tp->bindtype=0;
	bp->flg=0;
	return 0;
}

int print_bind(void *content)  //打印bind参数
{
struct bindnod *bp;

	bp=(struct bindnod *)content;
	*bp->tail+=sprintf(*bp->tail,"%d:%s=",bp->bindnum,
		plain_name(bp->tp->name));
	if(bp->tp->bindtype & RETURNING) {
		*bp->tail=stpcpy(*bp->tail,"<RETURNING>");
	} else {
		*bp->tail+=get_one_str(*bp->tail,*bp->rec,bp->tp,0);
	}
	*bp->tail+=sprintf(*bp->tail,",");
	return 0;
}

static int bind_proc(void *content)   //bind处理
{
register struct bindnod *bp;
T_PkgType *tp;
char *p,*bindp;
int ret,n;
short nf;
//char errb[512];
	bp=(struct bindnod *)content;
	p=*bp->tail;
	tp=bp->tp;
	bindp=*(bp->rec)+tp->offset;
//ShowLog(5,"bind_proc sth=%d,%s:%d",*bp->sth,tp->name,bp->bindnum);

//bind NULL
	nf=isnull(bindp,tp->type)?-1:0;
	if(1&bp->flg) bp->flg=!(nf ^ bp->ind); //如果NULL状态有变化，清空免bind标志
	bp->ind=nf;
	ret=0;
        if(!(1&bp->flg)) {
//ShowLog(5,"bind_proc sth=%d,%s:%d",*bp->sth,tp->name,bp->bindnum);
	    switch(tp->type) {
#ifdef SDBC_PTR_64
		case CH_INT64:
#endif

		case CH_LONG:
		case CH_TINY:
		case CH_SHORT:
		case CH_INT:
		    ret=sqlo_bind_by_pos(*bp->sth, bp->bindnum, SQLOT_INT, bindp, tp->len, &bp->ind, 0);
			bp->flg=1; //下次免bind
                break;
		case CH_FLOAT:
		case CH_DOUBLE:
		    ret=sqlo_bind_by_pos(*bp->sth, bp->bindnum, SQLOT_FLT, bindp, tp->len, &bp->ind, 0);
			bp->flg=1; //下次免bind
		        break;
		case CH_BYTE:
		    ret=sqlo_bind_by_pos(*bp->sth, bp->bindnum, SQLOT_BIN, bindp, tp->len, &bp->ind, 0);
			bp->flg=1; //下次免bind
		        break;
		case CH_CHAR:
		case CH_CNUM:
		case CH_DATE:
//ShowLog(5,"%s:bind %d:%s=%s,len=%d",__FUNCTION__,bp->bindnum,tp->name,bindp,tp->len);
		    ret=sqlo_bind_by_pos(*bp->sth, bp->bindnum, SQLOT_STR, bindp, tp->len, &bp->ind, 0);
			bp->flg=1; //下次免bind
		        break;
		default:	//类型与ORACLE不符，需变换
			bindp=p;
			if(tp->bindtype & RETURNING) { //bind RETURNING
				n=40;
				*bindp=0;
				p=bindp+n;  //CH_JUL...CH_USEC,CH_INT64.....
				bp->flg=2;
			} else {
				p+=get_one_str(p,*bp->rec,tp,0);
				n=strlen(bindp)+1;
				if(n<40) n=40;
				bp->flg=0;
			}
			ret=0;
			if(bindp != bp->last_bindp) {
		           ret=sqlo_bind_by_pos(*bp->sth, bp->bindnum, SQLOT_STR, bindp,n,&bp->ind, 0);
	    		   bp->last_bindp=bindp;
			}
		        *(++p)=0;
		        break;
	    }
	}
	*bp->tail=p;
	if(ret) {
		ShowLog(1,"bind_proc %s:ret=%d,bindnum=%d,err=%s",tp->name,ret,
			bp->bindnum,(char *)sqlo_geterror((*bp->sth>>16)));
	}
//ShowLog(5,"bind_proc sth=%d,%s:%d,ret=%d",*bp->sth,tp->name,bp->bindnum,ret);
	return 0;
}

static int bind_returning(void *content)   //bind RETURNING 后处理
{
register struct bindnod *bp;
T_PkgType *tp;
char *bindp;

	bp=(struct bindnod *)content;
	if(bp->flg != 2) return 0;
	tp=bp->tp;
	bindp=(*bp->rec)+tp->offset;
	put_str_one(*bp->rec,bp->last_bindp,tp,0);

	return 0;
}

//定义bind树的数据结构和回调函数完毕,下边是bind操作函数
//查找占位符
int do_regexp(char *stmt,regmatch_t *match)
{
int ret;
char errbuf[200];
	if(zwflg==-1) {
		sprintf(stmt+strlen(stmt)," :占位符编译失败");
		return -1;
	} else if(zwflg==0) {
		pthread_mutex_lock(&bind_mutex);
		if(!zwflg) {
			zwflg++;
			ret=regcomp(&zwreg,regstr, REG_EXTENDED);
			if(ret) {
				regerror(ret, &zwreg, errbuf, sizeof(errbuf));
				sprintf(stmt+strlen(stmt)," :%s",errbuf);
				zwflg=-1;
				regfree(&zwreg);
				pthread_mutex_unlock(&bind_mutex);
				return -1;
			}
			zwflg++;
		}
		pthread_mutex_unlock(&bind_mutex);
		if(zwflg==-1) {
			sprintf(stmt+strlen(stmt)," :占位符编译失败");
			return -1;
		}
	}
	while (zwflg==1) usleep(1000); //等别人编译zwreg完成
	ret=regexec(&zwreg,stmt,3,match,0);
	return ret;
}
//占位符替换为bind POS,并构建bind树
static char *mk_col_bind(char *values, T_PkgType *tp,int *num,struct bindnod *bnode,T_Tree **bind_tree)
{
T_PkgType *typ;
char *vp,buf[100];
int i;

        if(!values || !tp ->type<0) return values;
        vp=values;
        *vp=0;
        if(tp->offset<0) set_offset(tp);
        for(i=0,typ=tp;typ->type != -1;i++,typ++) {
            if((typ->bindtype&NOINS) || typ->type == CH_CLOB || !strcmp(typ->name,"ROWID")) {
/* can not insert CLOB *p and ROWID into database,skip it*/
                continue;
            }
	    if(!typ->format) goto dflt;
	    switch(typ->type) {
	    case CH_USEC:
		vp += sprintf(vp,"TO_TIMESTAMP(:%d,'%s'),",++*num,pack_mk_fmt(typ->format,buf));
		break;
	    case CH_DATE:
	    	if(is_timestamp((char *)typ->format)) {
			vp += sprintf(vp,"TO_TIMESTAMP(:%d,'%s'),",++*num,pack_mk_fmt(typ->format,buf));
			break;
		}
	    case CH_JUL:
	    case CH_TIME:
	    case CH_MINUTS:
		vp += sprintf(vp,"TO_DATE(:%d,'%s'),",++*num,pack_mk_fmt(typ->format,buf));
	    	break;
	    default:
dflt:
	    	vp+=sprintf(vp," :%d,",++*num);
		break;
	    }
	    bnode->tp=typ;
	    bnode->bindnum=*num;
	    *bind_tree=BB_Tree_Add(*bind_tree,bnode,sizeof(struct bindnod),bind_Cmp,0);
        }
	if(*(vp-1) == ',') *(--vp)=0;
        return vp;
}

/* typ是单个模板，为它生成占位符 */
char * mark_subst(char *vp,T_PkgType *typ,int *bindnum)
{
char buf[100];

	switch(typ->type) {
            case CH_DATE:
	    	if(is_timestamp((char *)typ->format)) {
                	vp += sprintf(vp,"TO_TIMESTAMP(:%d,'%s')",(*bindnum)+1,pack_mk_fmt(typ->format,buf));
			break;
		}
            case CH_JUL:
            case CH_TIME:
            case CH_MINUTS:
                vp += sprintf(vp,"TO_DATE(:%d,'%s')",(*bindnum)+1,pack_mk_fmt(typ->format,buf));
                break;
	    case CH_USEC:
               	vp += sprintf(vp,"TO_TIMESTAMP(:%d,'%s')",(*bindnum)+1,pack_mk_fmt(typ->format,buf));
		break;
            default:
                vp += sprintf(vp,":%d",(*bindnum)+1);
                break;
        }
	return vp;
}

static T_Tree * pre_bind(DAU *DP,int *sth,int *bindnum,char *stmt,struct bindnod *bnode,T_Tree ** broot)
{
register char *rep,*ep;
char *bindp,c;
regmatch_t match[3];
int n,colnum;
char buf[100],*into;
T_PkgType *tp;

// ShowLog(5,"pre_bind:stmt=%s",stmt);
	DP->SQL_Connect->Errno=0;
	ep=stmt;
	colnum=abs(DP->srm.Aflg);
//ShowLog(5,"pre_bind:stmt=%s",ep);
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
			n=index_col(DP->srm.colidx,colnum,bindp,DP->srm.tp);
			tp=&DP->srm.tp[n];
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
				sprintf(DP->SQL_Connect->ErrMsg,"pre_bind:%.*s 无效列名",(int)(ep-bindp),bindp);
				DP->SQL_Connect->Errno=FORMATERR;
				BB_Tree_Free(broot,0);
				return NULL;
			}
//get_one(buf,DP->srm.rec,DP->srm.tp,n,0);
//ShowLog(5,"mark_subst %s:%s=%s",DP->srm.tabname,tp->name,buf);
			if(!into || bindp<into)
				 mark_subst(buf,tp,bindnum);
			else sprintf(buf,":%d",(*bindnum)+1);
			bindp--;
			ep=strsubst(bindp,(int)(ep-bindp),buf);//替换成数字
			bnode->bindnum=++(*bindnum);
			bnode->tp=tp;
			*broot=BB_Tree_Add(*broot,bnode,sizeof(struct bindnod),bind_Cmp,0); //保存bind参数
		} else ep++;
	}
	return *broot;
}
//set DBOWN  at SRM.c
extern void set_dbo(char *buf,char *DBOWN);
//以下是插、查、改、删操作函数
int bind_ins(register DAU *DP,char *buf)
{
int ret=0;
register char *p;

	p=buf;
	if(DP->ins_sth==SQLO_STH_INIT) {
	int bindnum=0;
	struct bindnod bnode;
	char *returning=0;
//建立bind节点
		bnode.rec=(char **)&DP->srm.rec;
		bnode.sth=&DP->ins_sth;
		bnode.tail=&DP->tail;
		bnode.last_bindp=0;
		bnode.flg=0;
		bnode.ind=0;
		if(*p) { //有 RETURNING 子句
			returning=strdup(p);
		}
//制造语句
		if(DP->srm.befor) {
			p=stpcpy(p,DP->srm.befor);
			*p++ = ' ';
			DP->srm.befor=0;
		}

		p=stpcpy(p,"INSERT ");
		if(DP->srm.hint&&*DP->srm.hint) {
			p=stpcpy(p,DP->srm.hint);
			*p++=' ';
			*p=0;
		}
		p=stpcpy(p,"INTO ");
		if(*DP->SQL_Connect->DBOWN&&!strchr(DP->srm.tabname,'.')) {
			p=stpcpy(p,DP->SQL_Connect->DBOWN);
			*p++='.';
			*p=0;
		}
		p=stpcpy(mkset(stpcpy(stpcpy(p,DP->srm.tabname)," ("),DP->srm.tp),") VALUES (");
//生成占位符
		p=stpcpy(mk_col_bind(p,DP->srm.tp,&bindnum,&bnode,&DP->bt_ins),")");
		if(returning) {
		char *	p1=p;
			p=stpcpy(p,returning);
			free(returning);
			returning=p1;
		}
//prepare
		set_dbo(buf,DP->SQL_Connect->DBOWN);
		if(returning) {
			DP->bt_ins=pre_bind(DP,&DP->ins_sth,&bindnum,returning,&bnode,&DP->bt_ins);
			returning=0;
		}
		p=buf+strlen(buf);
		DP->ins_sth=sqlo_prepare(DP->SQL_Connect->dbh, (CONST char *)buf);
		if(DP->ins_sth<0) {
			___SQL_GetError(DP->SQL_Connect);
			sprintf(p," bind_ins:sqlo_prepare=%d,err=%d,%s",DP->ins_sth,
				DP->SQL_Connect->Errno,
				DP->SQL_Connect->ErrMsg);
			return DP->ins_sth;
		} else ShowLog(5,"bind_ins:prepare sth=%d,stmt=%s",DP->ins_sth,buf);
		*(++p)=0;
	}
//bind
	DP->srm.hint=0;
	DP->tail=p;
	BB_Tree_Scan(DP->bt_ins,bind_proc);
//exec
	DP->SQL_Connect->Errno = 0;
	if(0!=(ret = sqlo_execute(DP->ins_sth, 1))) {
		___SQL_GetError(DP->SQL_Connect);
		p=buf+strlen(buf);
		p+=sprintf(p," bind_ins:sqlo_execute=%d,errmsg=%s,tabneme=%s,bind=",ret,
			DP->SQL_Connect->ErrMsg,DP->srm.tabname);
		DP->tail=p;
		BB_Tree_Scan(DP->bt_ins,print_bind);//打印bind区
		return -abs(ret);
	}
//收集RETURNING变量值
	BB_Tree_Scan(DP->bt_ins,bind_returning);

//	sprintf(buf,"bind_ins:ret=%d",ret);
	return ret;
}

int bind_select(register DAU *DP,char *stmt,int recnum)
{
int ret,olderrno,bindnum=0,cursor=SQLO_STH_INIT;
char *tail,*p;
T_Tree *broot=0;
struct bindnod bnode;

	bnode.rec=(char **)&DP->srm.rec;
	bnode.sth=&cursor;
	bnode.tail=&tail;
	bnode.last_bindp=0;
	bnode.flg=0;
	bnode.ind=0;

	DP->SQL_Connect->Errno=0;
	*DP->SQL_Connect->ErrMsg=0;
	ret=SRM_mk_select(&DP->srm,DP->SQL_Connect->DBOWN,stmt);
	DP->srm.hint=0;
	broot=pre_bind(DP,&cursor,&bindnum,stmt,&bnode,&broot);
	if(0!=(ret=DP->SQL_Connect->Errno)) return ret;
	cursor=sqlo_prepare(DP->SQL_Connect->dbh, (CONST char *)stmt);
	if(cursor<0) {
		___SQL_GetError(DP->SQL_Connect);
		ShowLog(1,"bind_select:sqlo_prepare=%s,cursor=%d",stmt,cursor);
		BB_Tree_Free(&broot,0);
		return cursor;
	} else ShowLog(5,"bind_select:cursor=%d,sqlo_prepare=%s",cursor,stmt);

	tail=stmt+strlen(stmt)+1;
	*tail=0;  //为bind_proc准备空间
	if(broot) {
		BB_Tree_Scan(broot,bind_proc);  //bind
	}
	ret=sqlo_reopen(cursor,0,0);
	if(ret) {
		___SQL_GetError(DP->SQL_Connect);
		tail=stmt+strlen(stmt);
		tail+=sprintf(tail,":sqlo_execute ret=%d,err=%d,%s,tabname=%s,bind=",ret,
			DP->SQL_Connect->Errno,
			DP->SQL_Connect->ErrMsg,
			DP->srm.tabname);
		ret=DP->SQL_Connect->Errno;
		___SQL_Close__(DP->SQL_Connect,cursor);
		if(broot) BB_Tree_Scan(broot,print_bind);  //print bind
		BB_Tree_Free(&broot,0);
		DP->SQL_Connect->Errno=ret;
		return -abs(ret);
	}
	p=___SQL_Fetch(DP->SQL_Connect,cursor,&recnum);
	olderrno=0;
	if(!p) {
		olderrno= DP->SQL_Connect->Errno;
		tail=stmt+strlen(stmt);
		if(broot) {
			tail+=sprintf(tail," bind=");
			BB_Tree_Scan(broot,print_bind);
			BB_Tree_Free(&broot,0);
		}
		tail+=sprintf(tail," Fetch err=%d,%s",
       	 	DP->SQL_Connect->Errno,
                DP->SQL_Connect->ErrMsg);
		DP->srm.result=0;
		DP->srm.rp=0;
		DP->SQL_Connect->NativeError=0;
		if(olderrno!=SQLNOTFOUND) recnum=-abs(olderrno);
	} else {
		DP->srm.result=p;
		DP->srm.rp=p;
	}
	___SQL_Close__(DP->SQL_Connect,cursor);
	DP->SQL_Connect->Errno=olderrno;
	if(broot) {
		BB_Tree_Free(&broot,0);
	}
	return recnum;
}

int bind_prepare(register DAU *DP,char *stmt)
{
int ret=0;
	DP->SQL_Connect->Errno=0;
	if(DP->cursor == SQLO_STH_INIT) {
	struct bindnod bnode;
	int bindnum=0;

		bnode.rec=(char **)&DP->srm.rec;
		bnode.sth=&DP->cursor;
		bnode.tail=&DP->tail;
		bnode.last_bindp=0;
		bnode.flg=bnode.ind=0;

		ret=SRM_mk_select(&DP->srm,DP->SQL_Connect->DBOWN,stmt);
		DP->srm.hint=0;
//ShowLog(5,"%s:befor pre_bind stmt=%s",__FUNCTION__,stmt);
		DP->bt_pre=pre_bind(DP,&DP->cursor,&bindnum,stmt,&bnode,&DP->bt_pre);
		if(0!=(ret=DP->SQL_Connect->Errno)) {
			ShowLog(1,"%s:stmt=%s,err=%d",__FUNCTION__,stmt,ret);
			return ret;
		}
		if(!DP->bt_pre) { //如果没有bind
			DP->cursor=___SQL_Prepare__(DP->SQL_Connect,stmt);
			if(DP->cursor < 0) {
				ShowLog(1,"bind_prepare no_bind:CURSOR=%d,%s",DP->cursor,stmt);
				return DP->cursor;
			}
			DP->srm.result=0;
			DP->srm.rp=0;
     		return 0;
		}
		DP->cursor=sqlo_prepare(DP->SQL_Connect->dbh, (CONST char *)stmt);
		if(DP->cursor<0) {
			___SQL_GetError(DP->SQL_Connect);
			ShowLog(1,"bind_prepare:sqlo_prepare=%s,cursor=%d",stmt,DP->cursor);
			BB_Tree_Free(&DP->bt_pre,0);
			return DP->cursor;
		} else ShowLog(5,"%s:cursor=%d,%s",__FUNCTION__,DP->cursor,stmt);
		DP->tail=stmt+strlen(stmt)+1;
	} else  DP->tail=stmt,*stmt=0;  //为bind_proc准备空间

	if(DP->bt_pre) BB_Tree_Scan(DP->bt_pre,bind_proc);  //bind
	ret=sqlo_reopen(DP->cursor,0,0);
	if(ret) {
		___SQL_GetError(DP->SQL_Connect);
		int err=DP->SQL_Connect->Errno;
		DP->tail=stmt;
		DP->tail+=sprintf(DP->tail,"bind_select:sqlo_execute:ret=%d,err=%d,%s,tabname=%s,bind=",ret,
			DP->SQL_Connect->Errno,
			DP->SQL_Connect->ErrMsg,
			DP->srm.tabname);
		___SQL_Close__(DP->SQL_Connect,DP->cursor);
		BB_Tree_Scan(DP->bt_pre,print_bind);  //print bind
		DP->SQL_Connect->Errno=err;
		return ret;
	}
	DP->srm.result=0;
	DP->srm.rp=0;
    return 0;
}

int bind_update(register DAU *DP,char *where)
{
int ret=0;
register char *p;

    p=where;
	DP->SQL_Connect->Errno=0;
    if(DP->upd_sth==SQLO_STH_INIT) {
	char *save_where=0;
	struct bindnod bnode;
	int bindnum=0;
		bnode.rec=(char **)&DP->srm.rec;
		bnode.sth=&DP->upd_sth;
		bnode.tail=&DP->tail;
		bnode.last_bindp=0;
		bnode.flg=bnode.ind=0;
		if(toupper(*p) != 'U') {
			save_where=strdup(p);
			p=DAU_mk_update(DP,where);
			p+=sprintf(p,"SET(");
      		p=mkset(p,DP->srm.tp);
       	  	p+=sprintf(p,")=(SELECT ");
       		p=mk_col_bind(p,DP->srm.tp,&bindnum,&bnode,&DP->bt_upd);
			p+=sprintf(p," FROM DUAL) ");
			if(save_where) {
				strcat(p,save_where);
				free(save_where);
			}
		}
		DP->srm.hint=0;
		set_dbo(where,DP->SQL_Connect->DBOWN);
		p+=strlen(p);
        DP->bt_upd=pre_bind(DP,&DP->upd_sth,&bindnum,where,&bnode,&DP->bt_upd);
		if(0!=(ret=DP->SQL_Connect->Errno)) return ret;
        DP->upd_sth=sqlo_prepare(DP->SQL_Connect->dbh, (CONST char *)where);
        if(DP->upd_sth<0) {
                ___SQL_GetError(DP->SQL_Connect);
                sprintf(p," bind_update:sqlo_prepare=%d,err=%d,%s",DP->upd_sth,
                        DP->SQL_Connect->Errno,
                        DP->SQL_Connect->ErrMsg);
                return DP->upd_sth;
        } else ShowLog(5,"bind_update:bind=%d,sth=%d,stmt=%s,",bindnum,DP->upd_sth,where);
		p+=100;
        *p=0;
		ret=0;
    }
	DP->tail=p;
	if(DP->bt_upd && !DP->srm.rec) {
		sprintf(DP->SQL_Connect->ErrMsg,"bind_update:#错误：有bind变量，没有数据区，无法bind！#");
		DP->SQL_Connect->Errno=FORMATERR;
		ShowLog(1,"%s",DP->SQL_Connect->ErrMsg);
		BB_Tree_Free(&DP->bt_upd,0);
		___SQL_Close__(DP->SQL_Connect,DP->upd_sth);
		DP->upd_sth=SQLO_STH_INIT;
		return FORMATERR;
	}
    BB_Tree_Scan(DP->bt_upd,bind_proc);  //bind
    if(0!=(ret = sqlo_execute(DP->upd_sth, 1))) {
	___SQL_GetError(DP->SQL_Connect);
	p=where+strlen(where);
	p+=sprintf(p,"bind_update:sqlo_execute=%d,errmsg=%s,tabname=%s,bind=",
                    ret,DP->SQL_Connect->ErrMsg,
				DP->srm.tabname);
	DP->tail=p;
	BB_Tree_Scan(DP->bt_upd,print_bind);  //print bind
        return -abs(ret);
    }
    ret= sqlo_prows(DP->upd_sth);
    if(ret<0) {
	___SQL_GetError(DP->SQL_Connect);
	return ret;
    }
    DP->SQL_Connect->Errno = 0;
    *DP->SQL_Connect->ErrMsg = 0;
    DP->SQL_Connect->NativeError=ret;
	if(!DP->bt_upd) {
		___SQL_Close__(DP->SQL_Connect,DP->upd_sth);
		DP->upd_sth=SQLO_STH_INIT;
	}
	else if(!ret) {
		DP->tail=DP->SQL_Connect->ErrMsg;
//		BB_Tree_Scan(DP->bt_upd,print_bind);  //print bind
	}
    return ret;
}

int get_upd_returning(DAU *DP)
{
int ret;

	ret=sqlo_fetch(DP->upd_sth,1);
	if(0==ret) {
		 BB_Tree_Scan(DP->bt_upd,bind_returning);
		return ret;
	}

	___SQL_GetError(DP->SQL_Connect);
	ShowLog(1,"%s err=%d.%s",__FUNCTION__,
		DP->SQL_Connect->Errno,
		DP->SQL_Connect->ErrMsg);
	return ret;
}

int bind_delete(register DAU *DP,char *where)
{
int ret=0;
register char *p;

        p=where;
	DP->SQL_Connect->Errno=0;
	if(DP->del_sth==SQLO_STH_INIT) {
	struct bindnod bnode;
	int bindnum=0;

		if(toupper(*p) != 'D') {
			ret=SRM_mk_delete(&DP->srm,DP->SQL_Connect->DBOWN,where);
			if(ret) return ret;
		}
		set_dbo(where,DP->SQL_Connect->DBOWN);
		p+=strlen(p)+30;
		*p=0;
		bnode.rec=(char **)&DP->srm.rec;
		bnode.sth=&DP->del_sth;
		bnode.tail=&DP->tail;
		bnode.last_bindp=0;
		bnode.flg=bnode.ind=0;
		DP->bt_del=pre_bind(DP,&DP->del_sth,&bindnum,where,&bnode,&DP->bt_del);
		if(0!=(ret=DP->SQL_Connect->Errno)) return ret;
		if(!DP->bt_del) return ___SQL_Exec(DP->SQL_Connect,where);
		DP->del_sth=sqlo_prepare(DP->SQL_Connect->dbh, (CONST char *)where);
		if(DP->del_sth<0) {
			___SQL_GetError(DP->SQL_Connect);
			sprintf(p," bind_delete:sqlo_prepare=%d,err=%d,%s",DP->del_sth,
                                DP->SQL_Connect->Errno,
                                DP->SQL_Connect->ErrMsg);
			return DP->del_sth;
		} else ShowLog(5,"bind_delete:bind=%d,sth=%d,stmt=%s,",bindnum,DP->del_sth,where);
		*(++p)=0;
		ret=0;
	}
	DP->srm.hint=0;
	DP->tail=p;
	if(DP->bt_del ) {
		if(!DP->srm.rec) {
			sprintf(DP->SQL_Connect->ErrMsg,"bind_delete:#错误：有bind变量，没有数据区，无法bind！#");
			DP->SQL_Connect->Errno=FORMATERR;
			ShowLog(1,"%s",DP->SQL_Connect->ErrMsg);
			BB_Tree_Free(&DP->bt_del,0);
			___SQL_Close__(DP->SQL_Connect,DP->del_sth);
			DP->del_sth=SQLO_STH_INIT;
			return FORMATERR;
		}
		BB_Tree_Scan(DP->bt_del,bind_proc);  //bind
	}
	if(0!=(ret = sqlo_execute(DP->del_sth, 1))) {
		___SQL_GetError(DP->SQL_Connect);
		int err=DP->SQL_Connect->Errno;
		p=where+strlen(where);
		p+=sprintf(p,",bind_delete:sqlo_execute=%d,errmsg=%s,tabname=%s,bind=",
                        ret,DP->SQL_Connect->ErrMsg, DP->srm.tabname);
		DP->tail=p;
		if(DP->bt_del) BB_Tree_Scan(DP->bt_del,print_bind);  //bind
		else {
			___SQL_Close__(DP->SQL_Connect,DP->del_sth);
			DP->del_sth=SQLO_STH_INIT;
		}
		DP->SQL_Connect->Errno=err;
		return -abs(ret);
	}
//      sprintf(where,"bind_delete:sqlo_execute ret=%d",ret);
	ret= sqlo_prows(DP->del_sth);
	if(!DP->bt_del) {
		___SQL_Close__(DP->SQL_Connect,DP->del_sth);
		DP->del_sth=SQLO_STH_INIT;
	}
	DP->SQL_Connect->Errno = 0;
	*DP->SQL_Connect->ErrMsg = 0;
	DP->SQL_Connect->NativeError=ret;
	return ret;
}

int bind_exec(register DAU *DP,char *stmt)
{
int ret=0;
register char *p;

        if(!stmt) { //任务结束，关闭游标
                BB_Tree_Free(&DP->bt_del,0);
                if(DP->del_sth>=0) ret=___SQL_Close__(DP->SQL_Connect,DP->del_sth);
                DP->del_sth=SQLO_STH_INIT;
                return ret;
        }
        p=stmt;
	DP->SQL_Connect->Errno=0;
	if(DP->del_sth==SQLO_STH_INIT) {
	struct bindnod bnode;
	int bindnum=0;
//set DBOWN
		set_dbo(stmt,DP->SQL_Connect->DBOWN);
		p+=strlen(p)+30;
		*p=0;
		bnode.rec=(char **)&DP->srm.rec;
		bnode.sth=&DP->del_sth;
		bnode.tail=&DP->tail;
		bnode.last_bindp=0;
		bnode.flg=bnode.ind=0;
		DP->bt_del=pre_bind(DP,&DP->del_sth,&bindnum,stmt,&bnode,&DP->bt_del);
		if(0!=(ret=DP->SQL_Connect->Errno)) return ret;
		if(!DP->bt_del) return ___SQL_Exec(DP->SQL_Connect,stmt);
		DP->del_sth=sqlo_prepare(DP->SQL_Connect->dbh, (CONST char *)stmt);
		if(DP->del_sth<0) {
			___SQL_GetError(DP->SQL_Connect);
			sprintf(p," bind_exec:sqlo_prepare=%d,err=%d,%s",DP->del_sth,
                                DP->SQL_Connect->Errno,
                                DP->SQL_Connect->ErrMsg);
			return DP->del_sth;
		} else ShowLog(5,"bind_exec:bind=%d,sth=%d,stmt=%s,",bindnum,DP->del_sth,stmt);
		*(++p)=0;
		ret=0;
	}
	DP->srm.hint=0;
	DP->tail=p;
	if(DP->bt_del ) {
		if(!DP->srm.rec) {
			sprintf(DP->SQL_Connect->ErrMsg,"bind_exec:#错误：有bind变量，没有数据区，无法bind！#");
			DP->SQL_Connect->Errno=FORMATERR;
			ShowLog(1,"%s",DP->SQL_Connect->ErrMsg);
			BB_Tree_Free(&DP->bt_del,0);
			___SQL_Close__(DP->SQL_Connect,DP->del_sth);
			DP->del_sth=SQLO_STH_INIT;
			return FORMATERR;
		}
		BB_Tree_Scan(DP->bt_del,bind_proc);  //bind
	}
	if(0!=(ret = sqlo_execute(DP->del_sth, 1))) {
		___SQL_GetError(DP->SQL_Connect);
		p=stmt+strlen(stmt);
		p+=sprintf(p,",bind_exec:sqlo_execute=%d,errmsg=%s,tabname=%s,bind=",
                        ret,DP->SQL_Connect->ErrMsg, DP->srm.tabname);
		int err=DP->SQL_Connect->Errno;
		DP->tail=p;
		if(DP->bt_del) BB_Tree_Scan(DP->bt_del,print_bind);  //bind
		else {
			___SQL_Close__(DP->SQL_Connect,DP->del_sth);
			DP->del_sth=SQLO_STH_INIT;
		}
		DP->SQL_Connect->Errno=err;
		return -abs(ret);
	}
//      sprintf(stmt,"bind_exec:sqlo_execute ret=%d",ret);
	ret= sqlo_prows(DP->del_sth);
	if(!DP->bt_del) {
		___SQL_Close__(DP->SQL_Connect,DP->del_sth);
		DP->del_sth=SQLO_STH_INIT;
	}
	DP->SQL_Connect->Errno = 0;
	*DP->SQL_Connect->ErrMsg = 0;
	DP->SQL_Connect->NativeError=ret;
	return ret;
}
int DAU_print_bind(DAU *DP,char *stmt)
{
int i=0;

	DP->tail=stmt;
	if(DP->bt_pre) {
		BB_Tree_Scan(DP->bt_pre,print_bind);  //print bind
		i|=1;
	}
	if(DP->bt_upd) {
		BB_Tree_Scan(DP->bt_upd,print_bind);  //print bind
		i|=2;
	}
	if(DP->bt_ins) {
		BB_Tree_Scan(DP->bt_ins,print_bind);  //print bind
		i|=4;
	}
	if(DP->bt_del) {
		BB_Tree_Scan(DP->bt_del,print_bind);  //print bind
		i|=8;
	}
	return i;
}
