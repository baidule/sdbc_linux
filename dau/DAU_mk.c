#include <DAU.h>
#include <SRM_json.h>
#include <BB_tree.h>

//��Ա������ֹ�����
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
	
	while(SRM_mk(&DP->srm,tabname)) { //�ȴ�ģ�����
		node.timestamp=now_usec();
		node.stat=0;
		pthread_mutex_lock(&tab_mux);
		treep=BB_Tree_Find(tab_list,&node,sizeof(node),tab_cmp);//����ֹ���
		if(!treep) {//û����
			node.timestamp=now_usec();
			node.stat=0;
			tab_list=BB_Tree_Add(tab_list,&node,sizeof(node),tab_cmp,NULL);//���ֹ���
ShowLog(5,"%s:tab %s �����ֹ���",__FUNCTION__,node.tabname);
again:
			pthread_mutex_unlock(&tab_mux);
			ret=mkDauFromDB(DP,tabname);//�����ݿ�����ģ��
			if(ret) {//ʧ��
				treep=BB_Tree_Find(tab_list,&node,sizeof(node),tab_cmp);
				np=(tab_node *)treep->Content;
				np->stat=-1;//��ǣ��Ժ�Ҳ��Ҫ�����������
				np->timestamp=now_usec();
				ret=-1;
			} else {//�ɹ���
				ret=tpl_to_lib(DP->srm.tp,tabname); //���뵽ģ���
				pthread_mutex_lock(&tab_mux);
				tab_list=BB_Tree_Del(tab_list,&node,sizeof(node),tab_cmp,NULL,&ret);//����
				pthread_mutex_unlock(&tab_mux);
				ret=0;
			}
			pthread_cond_broadcast(&tab_cond);//֪ͨ���еȴ˱���
			break;
		} else {
			np=(tab_node *)treep->Content;
			if(np->stat==-1) {//ʧ�ܵı�
				now=now_usec();
				if((now-np->timestamp)>300000000) {//����һ��ʱ�䣬����������
ShowLog(5,"%s:reread tab %s ",__FUNCTION__,node.tabname);
					np->timestamp=now;
					goto again;
				}  else {
					ret=-1;
					pthread_mutex_unlock(&tab_mux);
					break;
				} 
			} else { //�������ڼ��أ���һ��
				pthread_cond_wait(&tab_cond,&tab_mux);
				pthread_mutex_unlock(&tab_mux);
			}
		}
	}
	return ret;
}
