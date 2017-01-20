#include <DAU.h>
#include <ctype.h>
#include "getsrm.h"

#define THROW goto

extern T_PkgType *patt_dup(T_PkgType *tp);
extern int reset_bind(void *content);
extern int bind_delete(register DAU *DP,char *where);
extern int bind_update(register DAU *DP,char *where);
extern int bind_prepare(register DAU *DP,char *stmt);

static void DAU_free2(DAU *DP)
{
	if(DP->srm.rp) {
		if(DP->srm.result) free(DP->srm.result);
		DP->srm.result=0;
		DP->srm.rp=0;
	}
	if(DP->cursor >= 0) {
		___SQL_Close__(DP->SQL_Connect,DP->cursor);
		DP->cursor=SQLO_STH_INIT;
		BB_Tree_Free(&DP->bt_pre,0);
	}
	if(DP->ins_sth >= 0) {
		___SQL_Close__(DP->SQL_Connect,DP->ins_sth);
		DP->ins_sth=SQLO_STH_INIT;
		BB_Tree_Free(&DP->bt_ins,0);
	}
	if(DP->upd_sth >= 0) {
		___SQL_Close__(DP->SQL_Connect,DP->upd_sth);
		DP->upd_sth=SQLO_STH_INIT;
		BB_Tree_Free(&DP->bt_upd,0);
	}
	if(DP->del_sth >= 0) {
		___SQL_Close__(DP->SQL_Connect,DP->del_sth);
		DP->del_sth=SQLO_STH_INIT;
		BB_Tree_Free(&DP->bt_del,0);
	}
	if(DP->srm.tp) clean_bindtype(DP->srm.tp,ALL_BINDTYPE);
}
int DAU_init(DAU *DP,T_SQL_Connect *SQL_Connect,const char *tabname,void *rec,T_PkgType *tp)
{
int n;

	if(!DP) return -1;
	if(SQL_Connect) { //原始初始化
		DP->SQL_Connect=SQL_Connect;
		DP->srm.rec=rec;
		DP->srm.tp=NULL;
		DP->cursor=SQLO_STH_INIT;
		DP->ins_sth=SQLO_STH_INIT;
		DP->upd_sth=SQLO_STH_INIT;
		DP->del_sth=SQLO_STH_INIT;
		DP->tail=0;
		DP->bt_pre=0;
		DP->bt_ins=0;
		DP->bt_upd=0;
		DP->bt_del=0;
		DP->srm.hint=0;
		DP->srm.befor=0;
		DP->srm.Aflg=0;
		DP->srm.pks=0;
		DP->srm.tabname=tabname;
		DP->srm.rp=0;
		DP->srm.result=0;
		if(tp) {
		int n=set_offset(tp);
			DP->srm.tp=patt_dup(tp);
			if(!DP->srm.tp) return -1;
			DP->srm.Aflg = -n;
			DP->srm.colidx=mk_col_idx(DP->srm.tp);
			DP->srm.pks = (char *)tp[n].format;
			if(!tabname && tp[n].name)
				DP->srm.tabname=(char *)tp[n].name;
			return 0;
		}
		return mksrm(&DP->srm,DP->SQL_Connect);
	}
//次级初始化
	if(tabname) {
		DP->srm.tabname=tabname;
		if(tp) {
init_tp:

			DAU_free(DP); //表名、模板改了，原来的上下文没有意义了，必须清除
			n=set_offset(tp);
			if(!tabname && tp[n].name)
				DP->srm.tabname=(char *)tp[n].name;
			DP->srm.tp=patt_dup(tp);
			DP->srm.Aflg = -n;
			DP->srm.pks = (char *)tp[n].format;
			DP->srm.rec=rec; //模板改了,rec强制更换
			if(DP->srm.colidx) {
				free(DP->srm.colidx);
			}
			DP->srm.colidx=mk_col_idx(DP->srm.tp);
			return 0;
		}
//必须是同构的表
		DAU_free2(DP);
		return 0;
	}
	if(tp) {
		THROW init_tp;
	} else if(rec) {
		if(DP->srm.Aflg>0) return -1; //如果原来是分配的，不允许更换记录
		DP->srm.rec=rec;
		BB_Tree_Scan(DP->bt_pre,reset_bind);	//重新bind
		BB_Tree_Scan(DP->bt_ins,reset_bind);	//重新bind
		BB_Tree_Scan(DP->bt_upd,reset_bind);	//重新bind
		BB_Tree_Scan(DP->bt_del,reset_bind);	//重新bind
	} else {	//参数全空，全清除
		DP->srm.hint=NULL;
		DP->srm.tp=NULL;
		DP->srm.rec=NULL;
		DP->srm.result=NULL;
		DP->srm.Aflg=0;
		DP->srm.befor=NULL;
		DP->SQL_Connect=NULL;
		DP->cursor=SQLO_STH_INIT;
		DP->ins_sth=SQLO_STH_INIT;
		DP->upd_sth=SQLO_STH_INIT;
		DP->del_sth=SQLO_STH_INIT;
		DP->srm.colidx=0;
		DP->bt_pre=NULL;
		DP->bt_ins=NULL;
		DP->bt_upd=NULL;
		DP->bt_del=NULL;
		DP->srm.tabname=NULL;
	}
	return 0;
}
static void DAU_free1(DAU *DP)
{
	if(DP->srm.rp) {
		if(DP->srm.result) free(DP->srm.result);
		DP->srm.result=0;
		DP->srm.rp=0;
	}
}

int DAU_select(DAU *DP,char *where,int num)
{

	if(!DP) return -1;
	if(!where) {
		DAU_free1(DP);
		return 0;
	}
	if(DP->cursor >= 0) {
		___SQL_Close__(DP->SQL_Connect,DP->cursor);
		DP->cursor=SQLO_STH_INIT;
		BB_Tree_Free(&DP->bt_pre,0);
	}
	if(DP->srm.rp) DAU_free1(DP);
	return bind_select(DP,where,num);
}

int DAU_prepare(DAU *DP,char *where)
{

	if(!DP) return -1;
	if(DP->srm.rp) DAU_free1(DP);
	if(!where) {
	int ret=0;
		BB_Tree_Free(&DP->bt_pre,0);
		if(DP->cursor>=0) {
			ret=___SQL_Close__(DP->SQL_Connect,DP->cursor);
			DP->cursor=SQLO_STH_INIT;
		}
		return ret;
	}
	return bind_prepare(DP,where);
}

int DAU_Fetch(DAU *DP)
{
int ret;
unsigned coln;
register int i;
unsigned short *vn;
char  **val;
register T_PkgType *tp;

	ret=sqlo_query_result(DP->cursor, &coln, &val, &vn, 0,0);
	if(ret) {
		 ___SQL_GetError(DP->SQL_Connect);
		if(DP->SQL_Connect->Errno == SQLNOTFOUND)
			strcpy(DP->SQL_Connect->ErrMsg,"没找到记录");
		 return -1;
	}
	i=0;
	for(tp=DP->srm.tp;i<coln && tp->type>-1;tp++) {
		if(tp->bindtype & NOSELECT) continue;
		put_str_one(DP->srm.rec,val[i],tp,0);
		i++;
	}
	DP->SQL_Connect->NativeError=coln;
	DP->SQL_Connect->Errno=0;
	*DP->SQL_Connect->ErrMsg=0;
	return 0;
}
int DAU_next(register DAU *DP)
{

	if(!DP||!DP->srm.rec||!DP->srm.tp) return -1;
	if(DP->cursor<0) {
		if(!DP->srm.rp || !*DP->srm.rp) return -1;
		DP->srm.rp += net_dispack(DP->srm.rec,DP->srm.rp,DP->srm.tp);
		if(!*DP->srm.rp) {
			free(DP->srm.result);
			DP->srm.result=0;
			DP->srm.rp=0;
		}
		return 0;
	}
// DP是prepare的
	return DAU_Fetch(DP);
}

int DAU_insert(DAU *DP,char *msg)
{

	if(!DP||!DP->srm.rec||!DP->srm.tp) return -1;

	if(!msg) { //任务结束，关闭游标
	int ret=0;
		if(DP->ins_sth>=0) ret=___SQL_Close__(DP->SQL_Connect,DP->ins_sth);
		DP->ins_sth=SQLO_STH_INIT;
		BB_Tree_Free(&DP->bt_ins,0);
		clean_bindtype(DP->srm.tp,NOINS|RETURNING);
		return ret;
	}
	*msg=0;

	return bind_ins(DP,msg);
}

int DAU_update(DAU *DP,char *where)
{
int ret=0;

	if(!DP) return -1;
	if(!where) { //任务结束，关闭游标
		BB_Tree_Free(&DP->bt_upd,0);
		if(DP->upd_sth>=0) ret=___SQL_Close__(DP->SQL_Connect,DP->upd_sth);
		DP->upd_sth=SQLO_STH_INIT;
		//clean_bindtype(DP->srm.tp,NOINS|RETURNING);
		return ret;
	}
	ret=bind_update(DP,where);
	if(ret<1) {
		sprintf(where+strlen(where),",ret=%d,err=%d,%s",ret,
			DP->SQL_Connect->Errno,
			DP->SQL_Connect->ErrMsg);
//		ShowLog(1,"DAU_update:%s",where);
	}
//ShowLog(5,"DAU_update:%s,ret=%d",where,ret);
	return ret;
}

int DAU_delete(DAU *DP,char *where)
{

	if(!DP) return -1;
        if(!where) { //任务结束，关闭游标
	int ret=0;
                BB_Tree_Free(&DP->bt_del,0);
                if(DP->del_sth>=0) ret=___SQL_Close__(DP->SQL_Connect,DP->del_sth);
                DP->del_sth=SQLO_STH_INIT;
                return ret;
        }
	return bind_delete(DP,where);
}
/*****************************************************
 * 执行无返回结果集的SQL语句，使用DAU_delete的游标，
 * 不要与DAU_delete同时使用
 * DAU中的结构和模板需满足bind的需要。
 * 涉及表名的$DB.符号将用DBOWN取代。
 *****************************************************/
int DAU_exec(DAU *DP,char *stmt)
{
	if(!DP) return -1;
	return bind_exec(DP,stmt);
}

int DAU_getm(int n,DAU *DP,char *where,int rownum)
{
int ret;
char *p,*whp;

	if(!DP || !DP[0].srm.tp) return -1;

	if(DP[0].srm.result) {
		free(DP[0].srm.result);
		DP[0].srm.result=0;
		DP[0].srm.rp=0;
	}
	whp=0;
	if(!*where || (toupper(*where)!='S')) { //如果是select,不作处理
	    if(*where) {
			whp=strdup(where);
			if(!whp) {
			  sprintf(DP[0].SQL_Connect->ErrMsg,"MEM alloc error");
			  DP[0].SQL_Connect->Errno=MEMERR;
			  return MEMERR;
			}
	    }
	    p=where;
            p+=sprintf(p,"SELECT %s ",DP[0].srm.hint?DP[0].srm.hint:"");
	    for(ret=0;ret<n;ret++) {
		if(ret>0) {
			*p++=',';
			*p=0;
		}
		mkfield(p,DP[ret].srm.tp,(char *)plain_name(DP[ret].srm.tabname));
		p+=strlen(p);
	    }
	    p+=sprintf(p," FROM ");
	    for(ret=0;ret<n;ret++) {
                if(ret>0) {
                        *p++=',';
                        *p=0;
                }
        	if(*(DP[0].SQL_Connect->DBOWN)) {
			p+=sprintf(p,"%s.",DP[0].SQL_Connect->DBOWN);
		}
                p+=sprintf(p,"%s",DP[ret].srm.tabname);
            }
	    *p++=' ';
	    *p=0;
	    if(whp) {
		strcat(p,whp);
		free(whp);
	    }
	}
	DP[0].cursor=SQLO_STH_INIT;
	DP[0].srm.hint=0;
	ret=___SQL_Select__(DP[0].SQL_Connect,where,&DP[0].srm.result,rownum);
	if(ret <=0) {
		sprintf(where,",result=%s",DP[0].srm.result);
		//ShowLog(1,"DAU_prepare:%s",where);
		if(DP[0].srm.result) free(DP[0].srm.result);
		DP[0].srm.result=0;
		DP[0].srm.rp=0;
		return ret;
	}
	DP[0].srm.rp=DP[0].srm.result;
	return ret;
}

int DAU_nextm(int n,DAU *DP)
{
int i;

	if(!DP) return -1;
	if(!DP[0].srm.rp || !*DP[0].srm.rp) return -1;
	for(i=0;i<n;i++) {
		if(!DP[i].srm.rec || !DP[i].srm.tp) return -1;
		DP[0].srm.rp += net_dispack(DP[i].srm.rec,DP[0].srm.rp,DP[i].srm.tp);
	}
	if(!*DP[0].srm.rp) {
		free(DP[0].srm.result);
		DP[0].srm.result=0;
		DP[0].srm.rp=0;
	}
	return 0;
}

void DAU_free(DAU *DP)
{
int error=0;

	if(!DP) return;
	if(DP->SQL_Connect) error=DP->SQL_Connect->Errno;
	SRM_free(&DP->srm);
	if(DP->cursor >= 0) {
		BB_Tree_Free(&DP->bt_pre,0);
		___SQL_Close__(DP->SQL_Connect,DP->cursor);
		DP->cursor=SQLO_STH_INIT;
	}
	if(DP->ins_sth >= 0) {
		BB_Tree_Free(&DP->bt_ins,0);
		___SQL_Close__(DP->SQL_Connect,DP->ins_sth);
		DP->ins_sth=SQLO_STH_INIT;
	}
	if(DP->upd_sth >= 0) {
		BB_Tree_Free(&DP->bt_upd,0);
		___SQL_Close__(DP->SQL_Connect,DP->upd_sth);
		DP->upd_sth=SQLO_STH_INIT;
	}
	if(DP->del_sth >= 0) {
		BB_Tree_Free(&DP->bt_del,0);
		___SQL_Close__(DP->SQL_Connect,DP->del_sth);
		DP->del_sth=SQLO_STH_INIT;
	}
	if(errno && DP->SQL_Connect) DP->SQL_Connect->Errno=error;
	DP->SQL_Connect=NULL;
}

void DAU_freem(int n,DAU *DP)
{
int i;

	for(i=0;i<n;i++) {
		DAU_free(&DP[i]);
	}
}
