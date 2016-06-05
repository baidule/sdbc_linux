/**************************************************
 *DAU池管理
 **************************************************/
#include <ctype.h>
#include "tjsTPC.h"

static int log_level=0;

typedef struct {
	int next;
	pthread_t tid;
	DAU dau;
	INT64 timestamp;
} resource;

typedef struct {
	pthread_mutex_t mut;
	pthread_cond_t cond;
	int poolno;
	int resource_num;
	resource *lnk;
	int free_q;
}pool_t;

static int DAUPOOLNUM=0;
static pool_t *pool=0;
//释放DAU连接池  
void DAUpool_free()
{
int i,n;
char *cp;

	if(!pool) return;
	for(n=0;n<DAUPOOLNUM;n++) {
		pthread_cond_destroy(&pool[n].cond);
		pthread_mutex_destroy(&pool[n].mut);
		if(pool[n].lnk) {
			for(i=0;i<pool[n].resource_num;i++) {
			    if(pool[n].lnk[i].dau.SQL_Connect) {
				cp=DAU_getRec(&pool[n].lnk[i].dau);
				if(cp) free(cp);
				DAU_getRec(&pool[n].lnk[i].dau)=NULL;
				DAU_free(&pool[n].lnk[i].dau);
				release_DB_connect(&pool[n].lnk[i].dau.SQL_Connect,pool[n].poolno);
			    }
			}
			free(pool[n].lnk);
		}
	}
	free(pool);
	pool=0;
}

//初始化DAU连接池  
int DAUpool_init()
{
int n,i,num;;
char *p,buf[256];
INT64 now;
resource *rs;

	if(pool) return 0;
	p=getenv("DAUPOOLNUM");
	if(!p||!isdigit((unsigned)*p)) {
		ShowLog(1,"%s:缺少环境变量 DAUPOOLNUM 缺省设为1",__FUNCTION__);
		DAUPOOLNUM=1;
	} else DAUPOOLNUM=atoi(p);

	pool=(pool_t *)malloc(DAUPOOLNUM * sizeof(pool_t));
	if(!pool) {
		DAUPOOLNUM=0;
		return -1;
	}

	now=now_usec();
	p=getenv("DBPOOL_LOGLEVEL");
	if(p && isdigit(*p)) log_level=atoi(p);

    for(n=0;n<DAUPOOLNUM;n++) {
	pool[n].free_q=-1;
	if(0!=(i=pthread_mutex_init(&pool[n].mut,NULL))) {
		ShowLog(1,"%s:mutex_init err %s",__FUNCTION__,
			strerror(i));
		return -2;
	}
	
	if(0!=(i=pthread_cond_init(&pool[n].cond,NULL))) {
		ShowLog(1,"%s:cond init  err %s",__FUNCTION__,
			strerror(i));
		return -3;
	}

	sprintf(buf,"POOLNO%d",n);
	p=getenv(buf);
	if(!p||!isdigit((int)*p)) {
		ShowLog(1,"%s:缺少环境变量  %s,DAU池[%d] poolno设为0",__FUNCTION__,buf,n);
		pool[n].lnk=0;
		pool[n].poolno=0;
	} else pool[n].poolno=atoi(p);

	sprintf(buf,"DAUPOOL%d_NUM",n);
	p=getenv(buf);
	if(!p || !isdigit((int)*p)) {
		ShowLog(1,"%s:缺少环境变量  %s,连接数设为1",__FUNCTION__,buf);
		pool[n].resource_num=1;
	} else pool[n].resource_num=atoi(p);
ShowLog(5,"%s:pool[%d].resource_num=%d",__FUNCTION__,n,pool[n].resource_num);
	pool[n].lnk=(resource *)malloc(pool[n].resource_num * sizeof(resource));
	if(!pool[n].lnk) {
		ShowLog(1,"%s:malloc lnk error!",__FUNCTION__);
		pool[n].resource_num=0;
		continue;
	}
	pool[n].free_q=pool[n].resource_num-1;
	num=pool[n].resource_num;
	rs=pool[n].lnk;
	for(i=0;i<num;i++,rs++) {
		rs->tid=0;
		DAU_init(&rs->dau,NULL,NULL,NULL,NULL);
		rs->timestamp=now;
		if(i<num-1) rs->next=i+1;
                else rs->next=0;
	}
    }
	return 0;
}

// 归还连接时，测试在连接池的位置
static int lnk_no(pool_t *pl,DAU *DP)
{
int i,e;
resource *rs;

	rs=pl->lnk;
	e=pl->resource_num-1;
	for(i=0;i<=e;i++,rs++) {
		if(DP == &rs->dau) {
			return i;
		}
	}
	return -1;	//不是连接池里的
}
//取连接时获取空闲的连接
static int get_lnk_no(pool_t *pl)
{
int i,*ip,*np;
resource *rs;

        if(pl->free_q<0) return -1;
	ip=&pl->free_q;
	rs=&pl->lnk[*ip];
        i=rs->next;
	np=&pl->lnk[i].next;
        if(i==*ip) *ip=rs->next=-1;
        else {
		rs->next=*np;
	}
        *np=-1;
        return i;
}
//归还连接时加入空闲队列
static void add_lnk(pool_t *pl,resource *rs)
{
int *np,*ip;
int i;
char *cp;
resource *rs1;

	for(i=0;i<pl->resource_num;i++) {
		if(rs==&pl->lnk[i]) break;
	}
	if(i==pl->resource_num) {
		ShowLog(1,"%s:rs not in DAUpool",__FUNCTION__);
		return;
	}
	ip=&pl->lnk[i].next;
        if(*ip>=0) {
		ShowLog(1,"%s:DAUlnk[%d] 已经在队列里 free_q=%d",__FUNCTION__,i,*ip);
		return; //已经在队列里
	}
	np=&pl->free_q;
        if(*np < 0) {
                *np=i;
                *ip=i;
	} else { //插入队头  
		rs1=&pl->lnk[*np];
                *ip=rs1->next;
                rs1->next=i;
		if(!rs->dau.SQL_Connect || rs->dau.SQL_Connect->Errno<0) {
			if(rs->dau.SQL_Connect) { //肯定是rs->dau.SQL_Connect->Errno<0
				cp=DAU_getRec(&rs->dau);
				if(cp) free(cp);
				DAU_getRec(&rs->dau)=NULL;
				DAU_free(&rs->dau);
				rs->dau.srm.tabname=NULL;
				release_DB_connect(&rs->dau.SQL_Connect,pl->poolno);
			}
ShowLog(5,"DAU%s:rs[%d]排到队尾",__FUNCTION__,i);
			*np=i;//坏连接排队尾 
		}
        }
}
//取数据库 连接 
static int get_db(DAU *DP,int dbpoolno)
{
int ret;
	ret=get_DB_connect(&DP->SQL_Connect,dbpoolno);
	return ret;
}

//取DAU连接  
int get_DAU_connect(DAU **DPP,int poolno)
{
int i,num;
pool_t *pl;
resource *rs;
pthread_t tid=pthread_self();

	if(!DPP) return FORMATERR;
//ShowLog(5,"%s:poolno=%d,tid=%lu,flg=%d",__FUNCTION__,n,tid,flg);
	*DPP=NULL;
	if(!pool || poolno<0 || poolno>=DAUPOOLNUM) return -1;
	pl=&pool[poolno];
	if(!pl->lnk) {
		ShowLog(1,"%s:无效的DAU池[%d]",__FUNCTION__,poolno);
		return FORMATERR;
	}
	num=pl->resource_num;
	if(0!=pthread_mutex_lock(&pl->mut)) return -1;
	while(0 > (i=get_lnk_no(pl))) {
/*
		if(flg) {
			pthread_mutex_unlock(&pl->mut);
			return 1;
		}
*/
		if(log_level) ShowLog(log_level,"%s tid=%lu DAUpool[%d] suspend",
			__FUNCTION__,tid,poolno);
		pthread_cond_wait(&pl->cond,&pl->mut); //没有资源，等待 
		if(log_level) ShowLog(log_level,"%s tid=%lu DAUpool[%d] weakup",
			__FUNCTION__,tid,poolno);
    	}
	pthread_mutex_unlock(&pl->mut);
	rs=&pl->lnk[i];
	if(!rs->dau.SQL_Connect) {
		if(get_db(&rs->dau,pl->poolno)) {
			add_lnk(pl,rs);
			pthread_mutex_unlock(&pl->mut);
			ShowLog(1,"%s:DAUpool[%d] 取数据库错",
				__FUNCTION__,poolno);
			return SYSERR;
		}
	}
	*DPP=&rs->dau;
	rs->tid=tid;
	rs->timestamp=now_usec();
	if(log_level) ShowLog(log_level,"%s tid=%lu,DAUpool[%d].%d,USEC=%llu",
				__FUNCTION__, rs->tid,poolno,i,rs->timestamp);
	return 0;
}
//归还DAU连接  
void release_DAU_connect(DAU **DPP,int poolno)
{
pthread_t tid=pthread_self();
pool_t *pl;
resource *rs;
int i;

	if(!DPP || !pool || poolno<0 || poolno>=DAUPOOLNUM) return;
	pl=&pool[poolno];
	if(!pl->lnk || 0>(i=lnk_no(pl, *DPP))) {
		ShowLog(1,"%s:无效的DAU池[%d]",__FUNCTION__,poolno);
		return;
	}
	*DPP=NULL;
	rs=&pl->lnk[i];
	rs->tid=0; 
	if(0!=pthread_mutex_lock(&pl->mut)) return;
	add_lnk(pl,rs);
	pthread_mutex_unlock(&pl->mut);
	pthread_cond_signal(&pl->cond); //如果有等待连接的线程就唤醒它 
  	rs->timestamp=now_usec();
	if(log_level) ShowLog(log_level,"%s tid=%lu,DAUpool[%d].%d,USEC=%llu",
				__FUNCTION__, tid,poolno,i,rs->timestamp);
}
//DAU池监控 
void DAUpool_check()
{
int n,i,num;
pool_t *pl;
resource *rs;
INT64 now;
char buf[32],*cp;

	if(!pool) return;
	now=now_usec();
	pl=pool;

	for(n=0;n<DAUPOOLNUM;n++,pl++) {
		if(!pl->lnk) continue;
		rs=pl->lnk;
		num=pl->resource_num;
//		if(log_level) ShowLog(log_level,"%s:pool[%d],num=%d",__FUNCTION__,n,num);
		pthread_mutex_lock(&pl->mut);
		for(i=0;i<num;i++,rs++) {
//ShowLog(5,"%s:pool[%d].%d,tid=%lu",__FUNCTION__,n,i,rs->tid);
		    if(!rs->tid) {
			if(rs->dau.SQL_Connect && (now-rs->timestamp)>150000000) {
//空闲时间太长了     
				cp=DAU_getRec(&rs->dau);
				if(cp) free(cp);
				DAU_getRec(&rs->dau)=NULL;
				DAU_free(&rs->dau);
				rs->dau.srm.tabname=NULL;
				release_DB_connect(&rs->dau.SQL_Connect,pl->poolno);
				if(log_level)
				ShowLog(log_level,"%s:Close DAUpool[%d].lnk[%d],since %s",__FUNCTION__,
					n,i,rusecstrfmt(buf,rs->timestamp,YEAR_TO_USEC));
			}
		    } else {
			if(rs->dau.SQL_Connect && (now-rs->timestamp)>299000000) {
//占用时间太长了     
				ShowLog(3,"%s:DAUpool[%d].lnk[%d] used by tid=%lu,since %s",
					__FUNCTION__,n,i,rs->tid,
					rusecstrfmt(buf,rs->timestamp,YEAR_TO_USEC));
			}
		    }
		}
		pthread_mutex_unlock(&pl->mut);
	}
}
/**
 * 根据DBLABEL取数据库池号  
 * 失败返回-1
int get_pool_no(char *DBLABEL)
{
int n;
	if(!pool) return -1;
	for(n=0;n<DAUPOOLNUM;n++) 
		if(!strcmp(pool[n].DBLABEL,DBLABEL)) return n;
	return -1;
}
*/

int get_DAUpoolnum()
{
	return DAUPOOLNUM;
}

