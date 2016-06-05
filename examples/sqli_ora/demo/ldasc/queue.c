#include "queue.h"

int queue_init(node_stu **org_buf,int elements)
{
int i;
node_stu *buf;
	if(!org_buf || elements<=0) return -1;
	buf=(node_stu *)malloc(sizeof(node_stu) * elements);
	if(!buf) return -1;
	for(i=0;i<elements-1;i++) {
		buf[i].next=i+1;
		buf[i].val=0;
	}
	buf[i].next=0;
	*org_buf=buf;
	return elements;
}
void queue_free(node_stu **org_buf)
{
	if(org_buf&&*org_buf) {
		free(*org_buf);
		*org_buf=0;
	}
}

node_stu * queue_get(node_stu *org_buf,int *queue)
{
int n;
node_stu *fnp,*fnp1;

	if(!org_buf || !queue || *queue<0) return 0;
	fnp=&org_buf[*queue]; //队尾
	n=fnp->next;
	fnp1=&org_buf[n];//取队头
	if(n==*queue) {//最后一个，取空
		*queue=-1;
	} else {
		fnp->next=fnp1->next;
	}
	fnp1->next=n;//记录自己的位置
        return fnp1;
}

void queue_release(node_stu *org_buf,int n,int *queue)
{
node_stu *fnp;
	if(!org_buf || !queue) return;
	if(*queue>-1) {
		org_buf[n].next=org_buf[*queue].next;
		org_buf[*queue].next=n;
	}
	*queue=n;//队尾
}
