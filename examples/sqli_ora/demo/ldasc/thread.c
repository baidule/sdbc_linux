#include "thread.h"

static int fn_q;
static node_stu *fn_buf=0;

static pthread_mutex_t fnLock=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t fnCond=PTHREAD_COND_INITIALIZER;

node_stu *get_buf(int *queue)
{
node_stu *fnp;
	if(!queue) {
		pthread_mutex_lock(&fnLock);
		do {
			fnp=queue_get(fn_buf,&fn_q);
			if(fnp) {
				pthread_mutex_unlock(&fnLock);
				return fnp;
			}
			pthread_cond_wait(&fnCond,&fnLock);
		} while(1);
	}
	return queue_get(fn_buf,queue);
}

void release_buf(node_stu *fnp,int *queue)
{
	if(!queue) {
		fnp->val=NULL;
		pthread_mutex_lock(&fnLock);
		queue_release(fn_buf,fnp->next,&fn_q);
		pthread_mutex_unlock(&fnLock);
		pthread_cond_signal(&fnCond);
	} else queue_release(fn_buf,fnp->next,queue);
}

static pthread_mutex_t threadLock=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t threadCond=PTHREAD_COND_INITIALIZER;
static int threadQueue=-1;

int maxThreadNum = 0;
static int threadEnd = 0;
static struct tpool {
        pthread_t tid;
	int (*callback)(void *thr_ctx,int n,void *usr_ctx);
	void *thr_ctx;
	int (*thr_entry)(void **thr_ctx);
	int (*thr_exit)(void **thr_ctx);
        int status;
} *tidp=NULL;

int thread_init(int (*callback)(void *,int n,void *),int thr_ctx_siz,int(*thr_entry)(void **),int(*thr_exit)(void **))
{
char *envp;
int i,ret;

	threadEnd=1;
        envp=getenv("MAXTHREADS");
        if(envp && isdigit((int)*envp))
                maxThreadNum=atoi(envp);
	if(maxThreadNum<1) maxThreadNum=1;
	ret=queue_init(&fn_buf,maxThreadNum<<2);
	if(ret<=0) return -1;
	fn_q=ret-1;
        tidp=(struct tpool *)malloc(sizeof(struct tpool) * maxThreadNum);
        if(!tidp) {
		queue_free(&fn_buf);
		return -1;
	}
        for(i=0;i<maxThreadNum;i++) {
		tidp[i].status=-1;
		tidp[i].callback=callback;
		tidp[i].thr_entry=thr_entry;
		tidp[i].thr_exit=thr_exit;
		tidp[i].thr_ctx=thr_ctx_siz<=0?NULL:(void *)malloc(thr_ctx_siz);
        }
ShowLog(5,"%s:maxthread=%d",__FUNCTION__,maxThreadNum);
        return 0;
}

static void * thread_work(void *arg)
{
pthread_t tid=pthread_self();
char showid[100];
int iRet;
int tno=*(int *)arg;
node_stu *fn_rec;
char file_name[92],*line;
int rowno;
struct tpool *tpp;

    pthread_detach(tid);
    sprintf(showid,"Tid%lu",tid);
    mthr_showid_add(tid,showid);
    ShowLog(5,"%s(%d) start",__FUNCTION__,tno);

    tpp=tidp+tno;
    if(tpp->thr_entry) {
	iRet=tpp->thr_entry(&tpp->thr_ctx);
	if(iRet<0) {
    		ShowLog(1,"%s(%d) fault,iRet=%d",__FUNCTION__,tno,iRet);
	   	mthr_showid_del(tid);
    		if(tidp) tpp->status=-1;
    		return NULL;
	}
    }
    while(1) {
	pthread_mutex_lock(&threadLock);
	tpp->status=1;
	while(NULL==(fn_rec=queue_get(fn_buf,&threadQueue))) {
		if(threadEnd) break;
		tpp->status=0;//置等待标志
		pthread_cond_wait(&threadCond,&threadLock);
		if(!tidp) break;
		tpp->status=1;
	}
	pthread_mutex_unlock(&threadLock);
	if(!fn_rec) break;//通知结束任务
	iRet = tpp->callback(tpp->thr_ctx,fn_rec->num,fn_rec->val);
	release_buf(fn_rec,NULL);
    }
    if(tpp->thr_exit) {
	iRet=tpp->thr_exit(&tpp->thr_ctx);
    }
    ShowLog(5,"%s(%d) finish",__FUNCTION__,tno);
    mthr_showid_del(tid);
    if(tidp) tpp->status=-1;
    return NULL;
}

//启动线程
int thread_start()
{
int i,ret,n;
	threadEnd=0;
	n=0;
        for(i=0;i<maxThreadNum;i++) {
	    if(tidp[i].status==-1) {
                ret=pthread_create(&tidp[i].tid,NULL,thread_work,&i);
		n++;
                usleep(5000);
	    }
        }
	return n;
}

void thread_finish()
{
int i;
	if(!tidp) return;
	do {//等所有任务完成
	    usleep(100000);
	    for(i=0;i<maxThreadNum;i++) {
		if(tidp[i].status>0) break;
	    }
	    if(i==maxThreadNum) break;
	}while(1);
}
//关闭线程
int thread_end()
{
int i,n;
	if(!tidp) return -1;
	threadEnd=1;
	n=0;
	for(i=0;i<maxThreadNum;i++) {
		if(tidp[i].status==0) {
			pthread_cond_signal(&threadCond);
			n++;
		}
	}
	return 0;
}
//
int thread_free()
{
int i;
    if(tidp) {
	thread_end();
	for(i=0;i<maxThreadNum;i++) {
		if(tidp[i].thr_ctx) {
			free(tidp[i].thr_ctx);
			tidp[i].thr_ctx=NULL;
		}
	}
	free(tidp);
	tidp=NULL;
    }
    queue_free(&fn_buf);
    return 0;
}
//val必须是malloc的
void put_to_thread(int num,void *val)
{
node_stu *fnp;

	fnp=get_buf(NULL);
	if(!fnp) {
		ShowLog(1,"%s:get_buf fault!",__FUNCTION__);
		return;
	}
	fnp->val=val;
	fnp->num=num;
        pthread_mutex_lock(&threadLock);
	release_buf(fnp,&threadQueue);
        pthread_mutex_unlock(&threadLock);
	pthread_cond_signal(&threadCond);
}
