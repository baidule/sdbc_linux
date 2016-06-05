#include <sys/select.h>
#include <sys/time.h>
#include <datejul.h>
#include <tree.h>

#define NUM 65535

void BB_Tree_print(T_Tree *sp)
{
long *ip;

    if(!sp)return ;
    if(sp->Left){
        BB_Tree_print(sp->Left);
    }
        ip=(long *)sp->Content;
        printf("sp=%ld,Content=%ld,\t\tL=%d\tR=%d,left=%ld,right=%ld\n",sp,
		*ip,sp->Ldepth,sp->Rdepth,sp->Left,sp->Right);
    if(sp->Right)
      BB_Tree_print(sp->Right);
    return ;
}

static int t_cmp(void *s,void *d,int len)
{
long n1,n2;
	n1=*(long *)s;
	n2=*(long *)d;
	if(n1==n2)return 0;
	if(n1>n2) return 1;
	else return -1;
}

static int scan(void *cont)
{
long n=*(long *)cont;

	printf("scan cont=%ld\n",n);
	return 0;
}

main()
{
long a[NUM];
int i,n;
T_Tree *root=NULL;
INT64 now;

	for(i=0;i<NUM;i++) {
		a[i]=now_usec();
		while(i && a[i]==a[i-1]) a[i]=now_usec();
		root=BB_Tree_Add(root,&a[i],sizeof(long),t_cmp,0);
	}
printf("time=%ld\n",a[NUM-1] - a[0]);
now=now_usec();
	for(i=0;i<NUM;i++) {
		n=0;
		root=BB_Tree_Del(root,&a[i],sizeof(long),t_cmp,0,&n);
		printf("a[%d]=%ld tier %ld\n",i,a[i],n);
	}
	printf("Del %d TIMEVAL=%lld\n",NUM,now_usec()-now);
}
