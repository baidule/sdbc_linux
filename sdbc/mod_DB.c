/***********************************************
 * 线程池服务器 下的数据库连接池高级管理器
 ***********************************************/

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/resource.h>

#include <sys/unistd.h>
#include <ctype.h>
#include <sdbc.h>
#define __USE_GNU
#include <sys/time.h>

extern int _get_DB_connect(T_SQL_Connect **SQL_Connect,int poolno,int flg);


typedef struct wqueue {
	pthread_mutex_t mut;
	pthread_cond_t cond;
	pTCB queue;
	pthread_t tid;
} Qpool;

//资源线程池
typedef struct {
	pthread_mutex_t mut;
	int poolnum;
	Qpool *QP;
	pthread_attr_t attr;
} WTHREAD;
//mod_sc.c
extern void wthread_free(WTHREAD *wp);
//midsc.c
extern int bind_DB(int TCBno,T_SQL_Connect *sql); //由应用提供

//等待队列
static WTHREAD wpool={PTHREAD_MUTEX_INITIALIZER,0,NULL};

void mod_DB_free()
{
WTHREAD *wp=&wpool;
	if(wp->QP) {
		free(wp->QP);
		wp->QP=NULL;
	}
	wp->poolnum=0;
	pthread_attr_destroy(&wp->attr);
	pthread_mutex_destroy(&wp->mut);
}

static void wpool_init(WTHREAD *wp,int num)
{
int i;
	if(wp->QP) return;
	wp->poolnum=num;
	wp->QP=(Qpool *)malloc(wp->poolnum * sizeof(Qpool));
	for(i=0;i<wp->poolnum;i++) {
		wp->QP[i].queue=NULL;
		wp->QP[i].tid=0;
		pthread_mutex_init(&wp->QP[i].mut,NULL);
		pthread_cond_init(&wp->QP[i].cond,NULL);
	}
	
        i=pthread_attr_setdetachstate(&wp->attr,PTHREAD_CREATE_DETACHED);
        if(i) {
                ShowLog(1,"can't set pthread attr PTHREAD_CREATE_DETACHED:%s",strerror(i));
        }
//设置线程堆栈保护区 16K
        pthread_attr_setguardsize(&wp->attr,(size_t)(1024 * 16));

}

static void *wait_DB(void *para)
{
int cc,ret;
int poolno=(int)(long)para;
Qpool *qp=&wpool.QP[poolno];
int TCBno=-1;
T_SQL_Connect *sql;
pthread_t tid=pthread_self();

	ShowLog(5,"%s:DBpool[%d],tid=%lX created!",__FUNCTION__,poolno,tid);
	while(1) {
//从就绪队列取一个任务
		pthread_mutex_lock(&qp->mut);
		while(0>(TCBno=TCB_get(&qp->queue))) {
		struct timespec ts;
		struct timeval tv;
                        gettimeofday(&tv,0);
			ts.tv_sec=tv.tv_sec;
                        ts.tv_sec+=300; //等待5分钟
			ts.tv_nsec=0;
			
                        ret=pthread_cond_timedwait(&qp->cond,&qp->mut,&ts);  //没有任务，等待
                        if(ETIMEDOUT == ret)  {
				qp->tid=0;
				break;
			}
		} 
		pthread_mutex_unlock(&qp->mut);
		if(TCBno<0) break;

		sql=NULL;
//ShowLog(5,"%s[%d]:get_DB_connect for TCB:%d!",__FUNCTION__,poolno,TCBno);
		cc=get_DB_connect(&sql,poolno); //等待得到连接
		if(cc == -1) {
			ShowLog(1,"%s:tid=%lX,get poolno[%d] fault for TCB:%d",
				__FUNCTION__,tid,poolno,TCBno);
		} //else ShowLog(5,"%s[%d]:TCB:%d got connect!",__FUNCTION__,poolno,TCBno);
//ShowLog(5,"%s[%d]:TCB:%d get_DB_conn succeed!",__FUNCTION__,poolno,TCBno);
		ret=bind_DB(TCBno,sql);
		if(ret<0) {
			if(!cc) release_DB_connect(&sql,poolno);
			ShowLog(1,"%s:tid=%lX,bind_DB fault TCB:%d,ret=%d",
				__FUNCTION__, tid,TCBno,ret);
			qp->tid=0;
			break;
		}
		TCB_add(NULL,TCBno); //加入到主任务队列
		ShowLog(3,"%s[%d]:tid=%lX,TCB:%d USEC=%lX,queued to ready!",
			__FUNCTION__,poolno,tid,TCBno,now_usec());
	}
	ShowLog(3,"%s[%d]:tid=%lX cancel!",__FUNCTION__,poolno,tid);
	return NULL;
}

int DB_connect_MGR(int TCBno,int poolno,T_SQL_Connect **sqlp,int (*call_back)(T_Connect *,T_NetHead *))
{
int ret;
pthread_t *tp;

	if(TCBno<0) {
		ShowLog(1,"%s:bad TCB no!",__FUNCTION__);
		return -1;
	}
	ret=_get_DB_connect(sqlp,poolno,1);
	if(ret<=0) return ret;

	if(wpool.poolnum<=0) {
		ret=get_DBpoolnum();
		if(ret<=0) return -1;
		pthread_mutex_lock(&wpool.mut);
		wpool_init(&wpool,ret);
		pthread_mutex_unlock(&wpool.mut);
	}
	if(poolno < 0 || poolno >= wpool.poolnum) {
		ShowLog(1,"%s:TCB:%d bad poolno %d,poolnum=%d",__FUNCTION__,
			TCBno,poolno,wpool.poolnum);
		return -1;
	}
	set_callback(TCBno,call_back,0);
	pthread_mutex_lock(&wpool.QP[poolno].mut);
	TCB_add(&wpool.QP[poolno].queue,TCBno);
	tp=&wpool.QP[poolno].tid;
	if(*tp==0) *tp=TCBno+1;
	pthread_mutex_unlock(&wpool.QP[poolno].mut);
//new thread
	if(*tp==TCBno+1) {
		pthread_create(&wpool.QP[poolno].tid,&wpool.attr,wait_DB,(void *)(long)poolno);
	} else pthread_cond_signal(&wpool.QP[poolno].cond); //唤醒等待线程
	ShowLog(3,"%s:to queue get pool[%d] tid=%lX",__FUNCTION__,
			poolno,pthread_self());
	return 1;
}

