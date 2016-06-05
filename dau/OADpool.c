/**************************************************
 * OAD连接池管理
 * 因为与应用关系紧密，没有加入到框架系统，在此供需要者参考
 **************************************************/
#include <OAD.h>
/*********************
#include <newcms.h>
#ifdef __cplusplus
extern "C" {
#endif
int init_OAD_pool();
void free_OAD_pool(void);
void oadpool_check();
int get_OADpoolnum();
void release_OAD(OAD **oadpp,int n);
OAD * get_OAD(int n,int flg)
#ifdef __cplusplus
}
#endif

外置的模板库，应用系统的例子：
static T_PkgType *tpl_lib[] = {
        CM_UD_ROUTE_tpl,
        PI_EXIT_tpl,
        CM_DIAGRAM_TRIP_NO_0_tpl,
        CM_DIAGRAM_TRIP_NO_1_tpl,
        NULL,
};

T_PkgType *get_tpl(char *tplname)
{
T_PkgType **tpp,*tp=NULL;
int i;
        for(tpp=tpl_lib;tpp;tpp++) {
                tp=*tpp;
                i=set_offset(tp);
                if(!strcmp(tp[i].name,tplname))
                        return tp;
        }
        return NULL;
}

配置文件：
#OAD池设置
OADPOOL_LOGLEVEL=5
OADPOOLNUM=1
#表名
OADFN0=CM_UD_ROUTE_TEMP_3
#模板名
OADTN0=CM_UD_ROUTE
OADBATCH0=200
OADDBPOOLNO0_NUM=0
OADPOOL0_NUM=2

*/

static int log_level=0;

typedef struct {
	int next;
	pthread_t tid;
	char *recs;
	DAU t_dau;
	OAD t_oad;
	INT64 timestamp;
} resource;

typedef struct {
	pthread_mutex_t mut;
	pthread_cond_t cond;
	char OADFN[41];
	int dbpool_no;
	int batch;
	int resource_num;
	T_PkgType *tp;
	resource *lnk;
	int free_q;
}pool;

static int OADPOOLNUM=0;
static pool *oadpool=NULL;
//释放OAD池  
#ifdef __cplusplus
extern "C"
#endif
void free_OAD_pool()
{
int i,n,ret;
T_SQL_Connect *SQL_Connect;

	if(!oadpool) return;
	for(n=0;n<OADPOOLNUM;n++) {
		pthread_cond_destroy(&oadpool[n].cond);
		pthread_mutex_destroy(&oadpool[n].mut);
		if(oadpool[n].lnk) {
			for(i=0;i<oadpool[n].resource_num;i++) {
				SQL_Connect=oadpool[n].lnk[i].t_dau.SQL_Connect;
				if(SQL_Connect) {
					OAD_free(&oadpool[n].lnk[i].t_oad);
					DAU_free(&oadpool[n].lnk[i].t_dau);
					release_DB_connect(&SQL_Connect,oadpool[n].dbpool_no);
				}
				if(oadpool[n].lnk[i].recs) {
					free(oadpool[n].lnk[i].recs);
				}
			}
			free(oadpool[n].lnk);
		}
	}
	free(oadpool);
	oadpool=NULL;
}

//初始化OAD 连接池  
#ifdef __cplusplus
extern "C"
#endif
int init_OAD_pool()
{
int n,i,num;;
char *p,buf[256];
INT64 now;
resource *rs;

	if(oadpool) return 0;
	p=getenv("OADPOOLNUM");
	if(!p||!isdigit((unsigned)*p)) {
		ShowLog(1,"%s:缺少环境变量 OADPOOLNUM 缺省设为1",__FILE__);
		OADPOOLNUM=1;
	} else OADPOOLNUM=atoi(p);

	oadpool=(pool *)malloc(OADPOOLNUM * sizeof(pool));
	if(!oadpool) {
		OADPOOLNUM=0;
		return -1;
	}

	now=now_usec();
	p=getenv("OADPOOL_LOGLEVEL");
	if(p && isdigit(*p)) log_level=atoi(p);

    for(n=0;n<OADPOOLNUM;n++) {
	oadpool[n].free_q=-1;
	if(0!=(i=pthread_mutex_init(&oadpool[n].mut,NULL))) {
		ShowLog(1,"%s:mutex_init err %s",__FILE__,
			strerror(i));
		return -2;
	}
	
	if(0!=(i=pthread_cond_init(&oadpool[n].cond,NULL))) {
		ShowLog(1,"%s:cond init  err %s",__FILE__,
			strerror(i));
		return -3;
	}

	sprintf(buf,"OADBATCH%d",n);
	p=getenv(buf);
	if(!p||!isdigit((unsigned)*p)) {
		ShowLog(1,"%s:缺少环境变量 %s 缺省设为1000",__FILE__,buf);
		oadpool[n].batch=1000;
	} else oadpool[n].batch=atoi(p);
	sprintf(buf,"OADFN%d",n);
	p=getenv(buf);
	if(!p||!*p) {
		ShowLog(1,"%s:缺少环境变量  %s,OAD池[%d]不能使用",__FILE__,buf,n);
		oadpool[n].lnk=0;
		oadpool[n].resource_num=0;
		*oadpool[n].OADFN=0;
		continue;
	} 
	stptok(p,oadpool[n].OADFN,sizeof(oadpool[n].OADFN),0);

	sprintf(buf,"OADTN%d",n);
	p=getenv(buf);
	if(!p||!*p) {
		p=oadpool[n].OADFN;
	} 
	oadpool[n].tp=get_tpl(p);
	if(!oadpool[n].tp) {
		ShowLog(1,"%s:OAD pool[%d}:no such template %s",
			__FILE__,n,oadpool[n].OADFN);
		oadpool[n].lnk=NULL;
		oadpool[n].resource_num=0;
		continue;
	}

	sprintf(buf,"OADDBPOOLNO%d_NUM",n);
	p=getenv(buf);
	if(!p || !isdigit((int)*p)) {
		ShowLog(1,"%s:缺少环境变量  %s,数据库池号设为0",__FILE__,buf);
		oadpool[n].dbpool_no=0;
	} else oadpool[n].dbpool_no=atoi(p);

	sprintf(buf,"OADPOOL%d_NUM",n);
	p=getenv(buf);
	if(!p || !isdigit((int)*p)) {
		ShowLog(1,"%s:缺少环境变量  %s,连接数设为1",__FILE__,buf);
		oadpool[n].resource_num=1;
	} else oadpool[n].resource_num=atoi(p);
	oadpool[n].lnk=(resource *)malloc(oadpool[n].resource_num * sizeof(resource));
	if(!oadpool[n].lnk) {
		ShowLog(1,"%s:malloc lnk error!",__FILE__);
		oadpool[n].resource_num=0;
		continue;
	}
	oadpool[n].free_q=oadpool[n].resource_num-1;
	num=oadpool[n].resource_num;
ShowLog(5,"%s:begin init oadpool[%d]=%d",__FILE__,n,num);
	rs=oadpool[n].lnk;
	for(i=0;i<num;i++,rs++) {
		rs->tid=0;
		oadpool[n].lnk[i].recs=NULL;
		DAU_init(&rs->t_dau,NULL,NULL,NULL,NULL);
		rs->t_oad.pos=i;
		rs->recs=NULL;
		rs->timestamp=now;
		if(i<num-1) rs->next=i+1;
                else rs->next=0;
	}
    }
	return 0;
}

// 归还连接时，测试在连接池的位置
static resource * lnk_no(pool *pl,OAD *oadp)
{
int i,e;
resource *rs;

	i=oadp->pos;
	if(i > -1 && i < pl->resource_num) {
	    rs=&pl->lnk[i];
       	    if(oadp != &rs->t_oad) {
       	 	ShowLog(1,"%s:OAD not equal pos=%d",__FILE__,i);
       	    } else return rs;
	}
	rs=pl->lnk;
	e=pl->resource_num-1;
	for(i=0;i<=e;i++,rs++) {
		if(oadp == &rs->t_oad) {
			rs->t_oad.pos=i;
ShowLog(1,"%s:the OAD fix to pos=%d",__FILE__,i);
			return rs;
		}
	}
	return NULL;	//不是连接池里的
}
//取连接时获取空闲的连接
static resource * get_lnk_no(pool *pl)
{
int i,*ip,*np;
resource *rs;

        if(pl->free_q<0) return NULL;
	ip=&pl->free_q;
	rs=&pl->lnk[*ip];
        i=rs->next;
	np=&pl->lnk[i].next;
        if(i==*ip) *ip=rs->next=-1;
        else {
		rs->next=*np;
	}
	rs=&pl->lnk[i];
        *np=-1;
        return rs;
}
//归还连接时加入空闲队列
static void add_lnk(pool *pl,int i)
{
int *np,*ip=&pl->lnk[i].next;
        if(*ip>=0) {
		ShowLog(1,"%s:lnk[%d] 已经在队列里 free_q=%d",__FILE__,i,*ip);
		return; //已经在队列里
	}
	np=&pl->free_q;
        if(*np < 0) {
                *np=i;
                *ip=i;
	} else { //插入队头  
	resource *rs=&pl->lnk[*np];
                *ip=rs->next;
                rs->next=i;
		if(!pl->lnk[i].t_dau.SQL_Connect) *np=i;//坏连接排队尾 
        }
}
//连接OAD  
static int new_connect(pool *pl,resource *rs)
{
int ret=-1,len=0;
T_SQL_Connect *SQL_Connect;
	//这个不保证不死锁
	ret= _get_DB_connect(&SQL_Connect,pl->dbpool_no,0);
	if(ret) {
		ShowLog(1,"new_connect:get_DB_connect error!");
		return ret;
	}
	if(!rs->recs) {
		len=set_offset(pl->tp);
		len=pl->tp[len].offset;
		rs->recs=(char *)malloc(pl->batch * len);
		if(!rs->recs) {
			release_DB_connect(&SQL_Connect,pl->dbpool_no);
			ShowLog(1,"new_connect:malloc %d byte fail",len);
			return MEMERR;
		}
	}
	DAU_init(&rs->t_dau,SQL_Connect,pl->OADFN,rs->recs,pl->tp);
	OAD_init(&rs->t_oad,&rs->t_dau,rs->recs,pl->batch);
	*rs->recs=0;
	ret=OAD_mk_ins(&rs->t_oad,rs->recs);
	if(ret) {
		OAD_free(&rs->t_oad);
		DAU_free(&rs->t_dau);
		release_DB_connect(&rs->t_dau.SQL_Connect,pl->dbpool_no);
		ShowLog(1,"new_connect:OAD_mk_ins err=%d,%s",ret,rs->recs);
	}
	return ret;
}

/**
 * 根据OADFN取OAD池号  
 * 失败返回-1
 */
#ifdef __cplusplus
extern "C"
#endif
int get_oadpool_no(const char *OADFN)
{
int n;
	if(!oadpool) return -1;
	for(n=0;n<OADPOOLNUM;n++) 
		if(!strcmp(oadpool[n].OADFN,OADFN)) return n;
	return -1;
}

//取OAD连接  
#ifdef __cplusplus
extern "C"
#endif
OAD * get_OAD(int n,int flg)
{
int i;
pool *pl;
resource *rs;
pthread_t tid=pthread_self();
OAD *oadp=NULL;

//ShowLog(5,"get_OAD:poolno=%d,tid=%lu,flg=%d",n,tid,flg);
	if(!oadpool || n<0 || n>=OADPOOLNUM) return NULL;
	pl=&oadpool[n];
	if(!pl->lnk) {
		ShowLog(1,"%s:无效的OAD池[%d]",__FILE__,n);
		return NULL;
	}
	if(0!=pthread_mutex_lock(&pl->mut)) {
		ShowLog(1,"get_OAD:mutex err=%d,%s",
			errno,strerror(errno));
		return NULL;
	}
	while(NULL == (rs=get_lnk_no(pl))) {
		if(flg) {
			pthread_mutex_unlock(&pl->mut);
			return NULL;
		}
		if(log_level) ShowLog(log_level,"get_OAD:tid=%lu pool[%d] suspend",
			tid,n);
		pthread_cond_wait(&pl->cond,&pl->mut); //没有资源，等待 
		if(log_level) ShowLog(log_level,"get_OAD:tid=%lu pool[%d] weakup",tid,n);
    	}
	pthread_mutex_unlock(&pl->mut);
	oadp=&rs->t_oad;
	i=oadp->pos;
	if(!rs->t_dau.SQL_Connect) { //还没有数据库连接
		if(new_connect(pl,rs)) {
			oadp->pos=i;//连接时pos破坏了
			pthread_mutex_lock(&pl->mut);
			add_lnk(pl,i);
			pthread_mutex_unlock(&pl->mut);
			ShowLog(1,"%s:oadpool[%d] 打开OAD池%d错",
				__FILE__,n,pl->dbpool_no);
			return NULL;
		}
		oadp->pos=i;
	}
	rs->tid=tid;
	rs->timestamp=now_usec();
	if(log_level) ShowLog(log_level,"get_OAD:tid=%lu,pool[%d].%d,USEC=%llu",
				tid,n,i,rs->timestamp);
	return oadp;
}
//归还OAD连接  
#ifdef __cplusplus
extern "C"
#endif
void release_OAD(OAD **oadpp,int n)
{
pthread_t tid=pthread_self();
pool *pl;
resource *rs;
T_SQL_Connect *SQL_Connect;

	if(!oadpp || !*oadpp || !oadpool || n<0 || n>=OADPOOLNUM) return;
	pl=&oadpool[n];
	if(!pl->lnk) {
		ShowLog(1,"%s:无效的OAD池[%d]",__FILE__,n);
		return;
	}
	if(NULL != (rs=lnk_no(pl, *oadpp))) {
	int i=rs->t_oad.pos;
		rs->tid=0; 
		SQL_Connect=rs->t_dau.SQL_Connect;
		if(SQL_Connect->Errno==3114||SQL_Connect->Errno==-30001) {  //数据库没打开
			OAD_free(&rs->t_oad);
			DAU_free(&rs->t_dau);
			release_DB_connect(&rs->t_dau.SQL_Connect,pl->dbpool_no);
		}
		if(0!=pthread_mutex_lock(&pl->mut)) return;
		add_lnk(pl,i);
		pthread_mutex_unlock(&pl->mut);
		pthread_cond_signal(&pl->cond); //如果有等待连接的线程就唤醒它 
  		rs->timestamp=now_usec();
		*oadpp=NULL;
		if(log_level) ShowLog(log_level,"release_OAD:tid=%lu,pool[%d].%d,USEC=%llu",
					tid,n,i,rs->timestamp);
		return;
	}
}
//OAD池监控 
#ifdef __cplusplus
extern "C"
#endif
void oadpool_check()
{
int n,i,num;
pool *pl;
resource *rs;
INT64 now;
char buf[32];
OAD *oadp;

	if(!oadpool) return;
	now=now_usec();
	pl=oadpool;

	for(n=0;n<OADPOOLNUM;n++,pl++) {
		if(!pl->lnk) continue;
		rs=pl->lnk;
		num=pl->resource_num;
//		if(log_level) ShowLog(log_level,"%s:oadpool[%d],num=%d",__FILE__,n,num);
		pthread_mutex_lock(&pl->mut);
		for(i=0;i<num;i++,rs++) {
//ShowLog(5,"%s:oadpool[%d].%d,tid=%lu",__FILE__,n,i,rs->tid);
		    if(rs->next>=0) {
			if(rs->t_dau.SQL_Connect && (now-rs->timestamp)>150000000) {
//空闲时间太长了     
				rs->t_dau.SQL_Connect->Errno==3114;
                                OAD_free(&rs->t_oad);
				DAU_free(&rs->t_dau);
				release_DB_connect(&rs->t_dau.SQL_Connect,pl->dbpool_no);
				if(log_level)
				ShowLog(log_level,"%s:Close oadpool[%d].lnk[%d],since %s",__FILE__,
					n,i,rusecstrfmt(buf,rs->timestamp,YEAR_TO_USEC));
			}
		    } else {
			if(rs->t_dau.SQL_Connect && (now-rs->timestamp)>299000000) {
//占用时间太长了     
				ShowLog(3,"%s:oadpool[%d].lnk[%d] used by tid=%lu,since %s",
					__FILE__,n,i,rs->tid,
					rusecstrfmt(buf,rs->timestamp,YEAR_TO_USEC));
			}
		    }
		}
		pthread_mutex_unlock(&pl->mut);
	}
}
#ifdef __cplusplus
extern "C"
#endif
int get_OADpoolnum()
{
	return OADPOOLNUM;
}

