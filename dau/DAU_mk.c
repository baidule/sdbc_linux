#include <DAU.h>
#include <SRM_json.h>
#include <BB_tree.h>

//针对表名的乐观锁表
typedef struct {
	char tabname[128];
	INT64 timestamp;
	int stat;
} tab_node;
static T_Tree *tab_list;
static pthread_mutex_t tab_mux=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t tab_cond=PTHREAD_COND_INITIALIZER;

static int tab_cmp(void *s,void *d,int len)
{
tab_node *sp,*dp;
	sp=(tab_node *)s;
	dp=(tab_node *)d;
	return strcmp(sp->tabname,dp->tabname);
}

static int mkDauFromDB(DAU *dp,const char *tabname)
{
int ret=-1,flg=0;
	if(!dp->SQL_Connect) {
		ret=get_DB_connect(&dp->SQL_Connect,dp->pos);
		if(ret) return -1;
		flg=1;
	}
	ret=DAU_init(dp,dp->SQL_Connect,tabname,NULL,NULL);
	if(ret) ShowLog(1,"%s:DAU_init [%s] err=%d,%s",__FUNCTION__,
		tabname,dp->SQL_Connect->Errno,dp->SQL_Connect->ErrMsg);
	if(flg) {
		release_DB_connect(&dp->SQL_Connect,dp->pos);
	}
	return ret;
}

int DAU_mk(DAU *DP,T_SQL_Connect *SQL_Connect,const char *tabname)
{
int ret;
tab_node node,*np;
T_Tree *treep;
INT64 now;

	DAU_init(DP,NULL,NULL,NULL,NULL);
	DP->SQL_Connect=SQL_Connect;
	stptok(tabname,node.tabname,sizeof(node.tabname),NULL);
	
	while(SRM_mk(&DP->srm,tabname)) { //先从模板库找
		node.timestamp=now_usec();
		node.stat=0;
		pthread_mutex_lock(&tab_mux);
		treep=BB_Tree_Find(tab_list,&node,sizeof(node),tab_cmp);//检查乐观锁
		if(!treep) {//没有锁
			node.timestamp=now_usec();
			node.stat=0;
			tab_list=BB_Tree_Add(tab_list,&node,sizeof(node),tab_cmp,NULL);//加乐观锁
ShowLog(5,"%s:tab %s 加入乐观锁",__FUNCTION__,node.tabname);
again:
			pthread_mutex_unlock(&tab_mux);
			ret=mkDauFromDB(DP,tabname);//读数据库生成模板
			if(ret) {//失败
				treep=BB_Tree_Find(tab_list,&node,sizeof(node),tab_cmp);
				np=(tab_node *)treep->Content;
				np->stat=-1;//标记，以后也不要再找这个表了
				np->timestamp=now_usec();
				ret=-1;
			} else {//成功了
				ret=tpl_to_lib(DP->srm.tp,tabname); //加入到模板库
				pthread_mutex_lock(&tab_mux);
				tab_list=BB_Tree_Del(tab_list,&node,sizeof(node),tab_cmp,NULL,&ret);//解锁
				pthread_mutex_unlock(&tab_mux);
				ret=0;
			}
			pthread_cond_broadcast(&tab_cond);//通知所有等此表者
			break;
		} else {
			np=(tab_node *)treep->Content;
			if(np->stat==-1) {//失败的表
				now=now_usec();
				if((now-np->timestamp)>300000000) {//超过一定时间，可以再试试
ShowLog(5,"%s:reread tab %s ",__FUNCTION__,node.tabname);
					np->timestamp=now;
					goto again;
				}  else {
					ret=-1;
					pthread_mutex_unlock(&tab_mux);
					break;
				} 
			} else { //有人正在加载，等一会
				pthread_cond_wait(&tab_cond,&tab_mux);
				pthread_mutex_unlock(&tab_mux);
			}
		}
	}
	return ret;
}
