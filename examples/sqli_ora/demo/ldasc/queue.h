#ifndef QUEUE_H
#define QUEUE_H

//#define type_size(data_type) sizeof(data_type),#data_type 

typedef struct {
	int next;
	int num;
	void *val;
} node_stu;


#ifdef __cplusplus
extern "C" {
#endif

//初始化org_buf，返回elements,失败返回-1
int  queue_init(node_stu **org_buf,int elements);
void queue_free(node_stu **org_buf);
//归还节点,n为节点在org_buf的下标
void queue_release(node_stu *org_buf,int n,int *queue);
node_stu * queue_get(node_stu *org_buf,int *queue);

#ifdef __cplusplus
}
#endif

#endif
