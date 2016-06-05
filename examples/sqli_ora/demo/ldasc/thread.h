#ifndef THREAD_H
#define THREAD_H

#include "pthread.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

//thr_ctx_siz:线程context区尺寸
//thr_entry:线程进入时所需的工作
//thr_exit:线程退出是所需的工作
//callback:线程队列激活时的工作
int thread_init(int (*callback)(void *thr_ctx,int n,void *user_ctx),int thr_ctx_siz,int(*thr_entry)(void **),int(*thr_exit)(void **));
int thread_start();
void thread_finish();
int thread_end();
//val必须是malloc的
void put_to_thread(int num,void *user_ctx);

#ifdef __cplusplus
}
#endif

#endif
