/**************************************************
 * 数据库连接池管理
 **************************************************/
// #include <pthread.h>
#include <ctype.h>
#include <sqli.h>
#include <bignum.h>
#include <enigma.h>
#include <crc32.h>
#include <dw.h>

static int log_level=0;

typedef struct {
	int next;
	pthread_t tid;
	T_SQL_Connect SQL_Connect;
	INT64 timestamp;
} resource;

typedef struct {
	pthread_mutex_t mut;
	pthread_cond_t cond;
	char DBLABEL[41];
	int resource_num;
	resource *lnk;
	int free_q;
}pool;

static int DBPOOLNUM=0;
static pool *dbpool=0;
//释放数据库连接池  
void DB_pool_free()
{
int i,n,ret;

	if(!dbpool) return;
	for(n=0;n<DBPOOLNUM;n++) {
		pthread_cond_destroy(&dbpool[n].cond);
		pthread_mutex_destroy(&dbpool[n].mut);
		if(dbpool[n].lnk) {
			for(i=0;i<dbpool[n].resource_num;i++) {
			    if(dbpool[n].lnk[i].SQL_Connect.dbh > -1) {
				ret=___SQL_CloseDatabase__(&dbpool[n].lnk[i].SQL_Connect);
			    }
			}
			free(dbpool[n].lnk);
		}
	}
	free(dbpool);
	dbpool=0;
}

//初始化数据库 连接池  
int DB_pool_init()
{
int n,i,res_num,num;;
char *p,buf[256];
INT64 now;
resource *rs;

	if(dbpool) return 0;
	p=getenv("DBPOOLNUM");
	if(!p||!isdigit((unsigned)*p)) {
		ShowLog(1,"%s:缺少环境变量 DBPOOLNUM 缺省设为1",__FUNCTION__);
		DBPOOLNUM=1;
	} else DBPOOLNUM=atoi(p);

	dbpool=(pool *)malloc(DBPOOLNUM * sizeof(pool));
	if(!dbpool) {
		DBPOOLNUM=0;
		return -1;
	}

	now=now_usec();
	p=getenv("DBPOOL_LOGLEVEL");
	if(p && isdigit(*p)) log_level=atoi(p);

	res_num=0;
    for(n=0;n<DBPOOLNUM;n++) {
	dbpool[n].free_q=-1;
	if(0!=(i=pthread_mutex_init(&dbpool[n].mut,NULL))) {
		ShowLog(1,"%s:mutex_init err %s",__FUNCTION__,
			strerror(i));
		return -2;
	}
	
	if(0!=(i=pthread_cond_init(&dbpool[n].cond,NULL))) {
		ShowLog(1,"%s:cond init  err %s",__FUNCTION__,
			strerror(i));
		return -3;
	}

	if(DBPOOLNUM==1)  strcpy(buf,"DBLABEL");
	else sprintf(buf,"DBLABEL%d",n);
	p=getenv(buf);
	if(!p||!*p) {
		ShowLog(1,"%s:缺少环境变量  %s,数据库池[%d]不能使用",__FUNCTION__,buf,n);
		dbpool[n].lnk=0;
		dbpool[n].resource_num=0;
		*dbpool[n].DBLABEL=0;
		continue;
	} 

	stptok(p,dbpool[n].DBLABEL,sizeof(dbpool[n].DBLABEL),0);

	sprintf(buf,"DBPOOL%d_NUM",n);
	p=getenv(buf);
	if(!p || !isdigit(*p)) {
		ShowLog(1,"%s:缺少环境变量  %s,连接数设为1",__FUNCTION__,buf);
		dbpool[n].resource_num=1;
	} else dbpool[n].resource_num=atoi(p);
	dbpool[n].lnk=(resource *)malloc(dbpool[n].resource_num * sizeof(resource));
	if(!dbpool[n].lnk) {
		ShowLog(1,"%s:malloc lnk error!",__FUNCTION__);
		dbpool[n].resource_num=0;
		continue;
	}
ShowLog(5,"%s:begin init dbpool[%d]",__FUNCTION__,n);
	dbpool[n].free_q=dbpool[n].resource_num-1;
	num=dbpool[n].resource_num;
	rs=dbpool[n].lnk;
	for(i=0;i<num;i++,rs++) {
		rs->tid=0;
		rs->SQL_Connect.pos=i;
		rs->SQL_Connect.dbh=-1;
		rs->SQL_Connect.Errno=0;
		rs->SQL_Connect.NativeError=0;
		*rs->SQL_Connect.ErrMsg=0;
		*rs->SQL_Connect.SqlState=0;
		rs->timestamp=now;
		if(i<num-1) rs->next=i+1;
                else rs->next=0;
//ShowLog(5,"%s:dbpool[%d].%d,next=%d",__FUNCTION__,n,i,rs->next);
	}
	res_num += num;
    }
	res_num += DBPOOLNUM;
	if(0>___SQL_init_sqlora(res_num,res_num * 8)) {
		DB_pool_free();
		return -4;
	}	
	return 0;
}

// 归还连接时，测试在连接池的位置
static resource * lnk_no(pool *pl,T_SQL_Connect *sql)
{
int i,e;
resource *rs;

	i=sql->pos;
	if(i > -1 && i < pl->resource_num) {
	    rs=&pl->lnk[i];
       	    if(sql != &rs->SQL_Connect) {
       	 	ShowLog(1,"%s:SQL_Connect not equal pos=%d",__FUNCTION__,i);
       	    } else return rs;
	}
	rs=pl->lnk;
	e=pl->resource_num-1;
	for(i=0;i<=e;i++,rs++) {
		if(sql == &rs->SQL_Connect) {
			rs->SQL_Connect.pos=i;
ShowLog(1,"%s:SQL_Connect fix to pos=%d",__FUNCTION__,i);
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
		ShowLog(1,"%s:lnk[%d] 已经在队列里 free_q=%d",__FUNCTION__,i,*ip);
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
		if(pl->lnk[i].SQL_Connect.dbh<0) *np=i;//坏连接排队尾 
        }
}
extern char *(*encryptproc)(char *mstr);
static ENIGMA2 egm;
static char *encryptpass(char *mstr)
{
int ret;
char tmp[41];
        ret=a64_byte(tmp,mstr);
	enigma2_decrypt(&egm,tmp,ret);
        tmp[ret]=0;
        strcpy(mstr,tmp);
        return mstr;
}

static pthread_mutex_t db_mutex=PTHREAD_MUTEX_INITIALIZER;
//连接数据库  
int connect_db(T_SQL_Connect *SQL_Connect,char *DBLABEL)
{
char *p;
DWS dw;
int ret,crc;
/********************************************************************
 * 用户口令解密准备
 ********************************************************************/
        p=getenv("KEYFILE");
        if(!p||!*p) {
                ShowLog(1,"%s:缺少环境变量 KEYFILE",__FUNCTION__);
		ret=-1;
        } else {
                ret=initdw(p,&dw);
                if(ret) {
                        ShowLog(1,"%s:Init dw %s error %d",__FUNCTION__,p,ret);
                }
        }

	pthread_mutex_lock(&db_mutex);
        encryptproc=0;
        if(!ret) {
		crc=ssh_crc32((const unsigned char *)DBLABEL,strlen(DBLABEL));
		p=getdw(crc,&dw);
        	if(!p) {
              	  ShowLog(1,"无效的 KEYID");
		} else {
//ShowLog(1,"%s:key=%s",__FUNCTION__,p);
			enigma2_init(&egm,p,0);
                	encryptproc=encryptpass;
       		}
        	freedw(&dw);
	}
//取得数据库用户名/口令  
	ret=SQL_AUTH(getenv("DATABASEAUTHFILE"),DBLABEL,
                        SQL_Connect->DSN,
                        SQL_Connect->UID,
                        SQL_Connect->PWD,
                        SQL_Connect->DBOWN);

	pthread_mutex_unlock(&db_mutex);
	if(ret) {
		ShowLog(1,"%s:无效的DBLABEL %s,SQL_AUTH ret=%d",__FUNCTION__,DBLABEL,ret);
		return -3;
	}
/*
ShowLog(1,"%s:DSN=%s,UID=%s,PWD=%s,DBOWN=%s",__FUNCTION__,
	 SQL_Connect->DSN,
	 SQL_Connect->UID,
         SQL_Connect->PWD,
         SQL_Connect->DBOWN);
*/
//打开数据库 
	ret=___SQL_OpenDatabase__(SQL_Connect);
	return ret;
}

//取数据库连接  
int _get_DB_connect(T_SQL_Connect **SQL_Connect,int n,int flg)
{
int i,num;
pool *pl;
resource *rs;
pthread_t tid=pthread_self();
T_SQL_Connect *SQL;

	if(!SQL_Connect) return FORMATERR;
//ShowLog(5,"%s:poolno=%d,flg=%d",__FUNCTION__,n,flg);
	*SQL_Connect=NULL;
	if(!dbpool || n<0 || n>=DBPOOLNUM) return -1;
	pl=&dbpool[n];
	if(!pl->lnk) {
		ShowLog(1,"%s:无效的数据库池[%d]",__FUNCTION__,n);
		return FORMATERR;
	}
	num=pl->resource_num;
	if(0!=pthread_mutex_lock(&pl->mut)) return -1;
	while(NULL == (rs=get_lnk_no(pl))) {
		if(flg) {
			pthread_mutex_unlock(&pl->mut);
			return 1;
		}
//		if(log_level) ShowLog(log_level,"%s tid=%lX pool[%d] suspend",
//			__FUNCTION__,tid,n);
		pthread_cond_wait(&pl->cond,&pl->mut); //没有资源，等待 
//		if(log_level) ShowLog(log_level,"%s tid=%lX pool[%d] weakup",__FUNCTION__,tid,n);
    	}
	pthread_mutex_unlock(&pl->mut);
	SQL=&rs->SQL_Connect;
	i=SQL->pos;
	if(SQL->dbh<0) {
		if(connect_db(SQL,pl->DBLABEL)) {
			SQL->pos=i;//连接时pos破坏了
			add_lnk(pl,i);
			pthread_mutex_unlock(&pl->mut);
			ShowLog(1,"%s:dbpool[%d] 打开数据库%s错:err=%d,%s",
				__FUNCTION__,n,pl->DBLABEL,
				SQL->Errno,
				SQL->ErrMsg);
			return SYSERR;
		}
		SQL->pos=i;
	}
	*SQL_Connect=SQL;
	rs->tid=tid;
	rs->timestamp=now_usec();
	if(log_level) ShowLog(log_level,"%s tid=%lX,pool[%d].%d,USEC=%llu",
				__FUNCTION__, rs->tid,n,i,rs->timestamp);
	SQL->Errno=0;
	*SQL->ErrMsg=0;
	return 0;
}
//归还数据库连接  
void release_DB_connect(T_SQL_Connect **SQL_Connect,int n)
{
pthread_t tid=pthread_self();
pool *pl;
resource *rs;

	if(!SQL_Connect || !*SQL_Connect || !dbpool || n<0 || n>=DBPOOLNUM) return;
	pl=&dbpool[n];
	if(!pl->lnk) {
		ShowLog(1,"%s:无效的数据库池[%d]",__FUNCTION__,n);
		return;
	}
	sqlo_close_all_db_cursors((*SQL_Connect)->dbh);
	___SQL_Transaction__(*SQL_Connect,TRANROLLBACK);//未完事务夭折
	if(NULL != (rs=lnk_no(pl, *SQL_Connect))) {
	int i=rs->SQL_Connect.pos;
		rs->tid=0; 

		switch((*SQL_Connect)->Errno) {
		case DBFAULT:
		case -30001:  //数据库没打开
			if((*SQL_Connect)->Errno!=-30001) {
				___SQL_CloseDatabase__(*SQL_Connect);
			        ShowLog(1,"%s:SQL_Connect[%d][%d] closed!",__FUNCTION__,
	                                n,(*SQL_Connect)->pos);
			}
			(*SQL_Connect)->dbh=-1;
			break;
		default: break;
		}

		if(0!=pthread_mutex_lock(&pl->mut)) return;
		add_lnk(pl,i);
		pthread_mutex_unlock(&pl->mut);
		pthread_cond_signal(&pl->cond); //如果有等待连接的线程就唤醒它 
  		rs->timestamp=now_usec();
		*SQL_Connect=NULL;
		if(log_level) ShowLog(log_level,"%s tid=%lX,pool[%d].%d,USEC=%llu",
					__FUNCTION__, tid,n,i,rs->timestamp);
		return;
	}
}
//数据库池监控 
void dbpool_check()
{
int n,i,num;
pool *pl;
resource *rs;
INT64 now;
char buf[32];

	if(!dbpool) return;
	now=now_usec();
	pl=dbpool;

	for(n=0;n<DBPOOLNUM;n++,pl++) {
		if(!pl->lnk) continue;
		rs=pl->lnk;
		num=pl->resource_num;
//		if(log_level) ShowLog(log_level,"%s:dbpool[%d],num=%d",__FUNCTION__,n,num);
		pthread_mutex_lock(&pl->mut);
		for(i=0;i<num;i++,rs++) {
//ShowLog(5,"%s:dbpool[%d].%d,tid=%lX",__FUNCTION__,n,i,rs->tid);
		    if(rs->next >= 0) {
			if(rs->SQL_Connect.dbh>-1 && (now-rs->timestamp)>299000000) {
//空闲时间太长了     
				___SQL_CloseDatabase__(&rs->SQL_Connect);
				if(log_level)
				ShowLog(log_level,"%s:Close DBpool[%d].lnk[%d],since %s",__FUNCTION__,
					n,i,rusecstrfmt(buf,rs->timestamp,YEAR_TO_USEC));
			}
		    } else {
			if(rs->SQL_Connect.dbh>-1 && (now-rs->timestamp)>299000000) {
//占用时间太长了     
				ShowLog(3,"%s:dbpool[%d].lnk[%d] used by tid=%lX,since %s",
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
 */
int get_dbpool_no(char *DBLABEL)
{
int n;
	if(!dbpool) return -1;
	for(n=0;n<DBPOOLNUM;n++) 
		if(!strcmp(dbpool[n].DBLABEL,DBLABEL)) return n;
	return -1;
}

int get_DBpoolnum()
{
	return DBPOOLNUM;
}

int get_rs_num(int poolno)
{
        if(!dbpool) return 0;
        if(poolno<0 || poolno>=DBPOOLNUM) return 0;
        return dbpool[poolno].resource_num;
}
char * get_DBLABEL(int poolno)
{
        if(!dbpool) return NULL;
        if(poolno<0 || poolno>=DBPOOLNUM) return NULL;
        return dbpool[poolno].DBLABEL;
}
