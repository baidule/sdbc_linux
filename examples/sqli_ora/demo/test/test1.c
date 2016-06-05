#include <DAU.h>
#include <sys/time.h>
long interval(struct timeval *begtime,struct timeval *endtime)
{
long ret;
        ret=endtime->tv_sec-begtime->tv_sec;
        ret*=1000000;
        ret += endtime->tv_usec - begtime->tv_usec;
        return ret;
}

T_PkgType sum_tpl[]={
	{CH_DOUBLE,sizeof(double),"sum(v1+v2) sum","%.2lf",-1},
	{CH_DOUBLE,sizeof(double),"count(*) count","%.2lf"},
	{-1,0,"tcol100",0}
};

typedef struct {
	double sum;
	double count;
} sum_stu;


main(int argc,char *argv[])
{
DAU _DAU,sum_DAU;
int ret,i,j;
T_SQL_Connect sql;
char buf[1000];
double v1,*dp1,*dp2;
sum_stu sum;
struct timeval begtime,endtime;
T_PkgType v_tpl[3];

	if(argc>1) envcfg(argv[1]);
	ret=db_open(&sql);
	if(ret) return 1;
/*
	ret=DAU_init(&_DAU,&sql,"tcol100",0,0);
	if(ret) {
		printf("DAU_init fail!\n");
		___SQL_CloseDatabase__(&sql);
		return 2;
	}
	patt_copy(v_tpl,_DAU.srm.tp,"v1,v2");
	DAU_init(&sum_DAU,&sql,0,_DAU.srm.rec,v_tpl);
*/
	DAU_init(&sum_DAU,&sql,0,&sum,sum_tpl);
	sum.sum=1000;
	sum.count=2000;
	strcpy(buf,"WHERE v1>=:sum and v1 <:count");
	gettimeofday(&begtime,0);
	ret=DAU_select(&sum_DAU,buf,0);
	v1=0;
	if(ret >0) {
		DAU_next(&sum_DAU);
	}
	gettimeofday(&endtime,0);
	printf("ret=%d,sum=%.2lf, count=%.2lf,TIMEVAL=%ld\n",ret,sum.sum,sum.count,interval(&begtime,&endtime));
	
/*
	for(i=0;i<20000;i++) {
		sprintf(buf,"%d",i);
		for(j=0;j<=100;j++) DAU_putOne(&_DAU,buf,j);
		ret=DAU_insert(&_DAU,buf);
	}
*/



	___SQL_Transaction__(&sql,TRANCOMMIT);
	DAU_free(&sum_DAU);
//`	DAU_free(&_DAU);
	___SQL_CloseDatabase__(&sql);
	return 0;
}
