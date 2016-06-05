#include <ctype.h>
#include "sqlsrv.h"
/*********************************
 * 计算甲种本
 ********************************/
typedef struct {
	double YZ_fee;
	double YZ_half_fee;
	double RZ_fee;
	double RZ_half_fee;
	double K;
	double T;
	double YWS_fee;
	double YWS_half_fee;
	double YWZ_fee;
	double YWZ_half_fee;
	double YWX_fee;
	double YWX_half_fee;
	double RWS_fee;
	double RWS_half_fee;
	double RWX_fee;
	double RWX_half_fee;
	double G2S_fee;	//二人高包上  
	double G2X_fee;	//二人高包下  
	double acc;
} ticket_fee;

//计算递远递减
typedef struct {
	int D;
	int N;
	int K;
} fall_rate;

static const fall_rate fall_tab[]= {
	{0,0,0},
	{201,1,20},
	{501,2,70},
	{1001,3,170},
	{1501,4,320},
	{2501,5,570}
};
static const int  FALLNUM=(sizeof(fall_tab)/sizeof(fall_rate));
//基本·里程分段表
static const int dist_segmt[]={20,30,40,50,60,
			 70,80,90,100,110,
			120,130,140,150,160,
			170,180,190,200,220,
			240,260,280,300,320,
			340,360,380,400,430,
			460,490,520,550,580,
			610,640,670,700,740,
			780,820,860,900,940,
			980,1020,1060,1100,1150,
			1200,1250,1300,1350,1400,
			1450,1500,1550,1600,1660,
			1720,1780,1840,1900,1960,
			2020,2080,2140,2200,2270,
			2340,2410,2480,2550,2620,
			2690,2760,2830,2900,2980,
			3060,3140,3220,3300,3380,
			3460,3540,3620,3700,3790,
			3880,3970,4060,4150,4240,
			4330,4420,4510,4600,4700,
			4800,4900,5000,5100,5200,
			5300,5400,5500,5600,5700,
			5800,5900,6000};


static float jflc(int distance) //计算计费里程
{
int *ip,i,v;

	if(distance>6000) return (float)(3595+100*((distance-6001)/100));
	for(i=0,ip=(int *)dist_segmt;*ip < distance;ip++) i++;
	if(i>0) v=((ip[-1]+*ip)/2);
	else v=dist_segmt[0];
	for(i=0;i<FALLNUM-1;i++) 
		if(fall_tab[i+1].D>v) break;
//printf("%d->%d:%d,%d,%d\n",v,i,
//	 fall_tab[i].D, fall_tab[i].N, fall_tab[i].K);
//计算递远递减
//	return (float)(f_round((v*10-v*fall_tab[i].N)/10.0,5,0)+fall_tab[i].K);
	return (float)((v*10-v*fall_tab[i].N)/10.0+fall_tab[i].K);
}

#define BASE_FEE 0.05861

/* 最后，加入软票费 */
#define RPF(fee) ((fee)>5.0?1.0:0.5)
//计算票价，分票种基本票价表返回计费里程
//fac:基本=0,新空调=5, 折扣一档=4,折扣二档=3

static float  base_fee(int distance,ticket_fee *fee_p,int fac)
{
double tmp,YZ_base;
float lc;
double insum;
double r=(10.0+fac)/10.0;
	lc=jflc(distance); //计算计费里程
	tmp=YZ_base=BASE_FEE * lc; //许多票价依赖YZ_base
//	insum=f_round(tmp*0.02,4,1);//保险费
	insum=0; //保险费取消了
	fee_p->YZ_fee=f_round(f_round(tmp+insum,5,0)*r,5,0);
	fee_p->YZ_half_fee=0.5*fee_p->YZ_fee;
//加软票费
	fee_p->YZ_fee+=RPF(fee_p->YZ_fee);
	fee_p->YZ_half_fee+=RPF(fee_p->YZ_half_fee);
	if(lc>200) { //硬座+车站空调费
		fee_p->YZ_fee+=1.0;
		fee_p->YZ_half_fee+=1.0;
	}
//计算软座
//	fee_p->RZ_fee=f_round(f_round(r*f_round(2.0*tmp,5,0),5,0)+insum,5,0);//先上浮再加保险费
	fee_p->RZ_fee=f_round(r*f_round(2.0*tmp+insum,5,0),5,0);//先加保险费再上浮 
	fee_p->RZ_half_fee=0.5*fee_p->RZ_fee;
	fee_p->RZ_fee+=RPF(fee_p->RZ_fee);
	fee_p->RZ_half_fee+=RPF(fee_p->RZ_half_fee);

//计算空调费
	fee_p->acc=f_round(tmp*0.25,5,0);
	if(fee_p->acc<1.0) fee_p->acc=1.0;
	fee_p->acc=f_round(r*fee_p->acc,5,0);
	
// 计算加快
	fee_p->K=f_round(tmp*0.2,5,0);
	if(fee_p->K<1.0) fee_p->K=1.0;
	fee_p->K=f_round(r*fee_p->K,5,0);
	fee_p->T=2*fee_p->K;
//计算硬卧
	tmp=lc>=380?tmp:BASE_FEE*380;//400KM以内的计费里程
	fee_p->YWS_fee=f_round(r*f_round(tmp*1.1,5,0),5,0);
	fee_p->YWS_half_fee=0.5*fee_p->YWS_fee;
//+卧铺订票费
	fee_p->YWS_fee += 10.0;
	fee_p->YWS_half_fee += 10.0;

	fee_p->YWZ_fee=f_round(r*f_round(tmp*1.2,5,0),5,0);
	fee_p->YWZ_half_fee=0.5*fee_p->YWZ_fee;
//+卧铺订票费
	fee_p->YWZ_fee += 10.0;
	fee_p->YWZ_half_fee += 10.0;

	fee_p->YWX_fee=f_round(r*f_round(tmp*1.3,5,0),5,0);
	fee_p->YWX_half_fee=0.5*fee_p->YWX_fee;
//+卧铺订票费
	fee_p->YWX_fee += 10.0;
	fee_p->YWX_half_fee += 10.0;
//计算软卧
	fee_p->RWS_fee=f_round(r*f_round(tmp*1.75,5,0),5,0);
	fee_p->RWS_half_fee=0.5*fee_p->RWS_fee;
//计算二人高包
	fee_p->G2S_fee=f_round(fee_p->RWS_fee*1.80,5,0)+10.0;
//+卧铺订票费
	fee_p->RWS_fee += 10.0;
	fee_p->RWS_half_fee += 10.0;
	fee_p->RWX_fee=f_round(r*f_round(tmp*1.95,5,0),5,0);
	fee_p->RWX_half_fee=0.5*fee_p->RWX_fee;
	fee_p->G2X_fee=f_round(fee_p->RWX_fee*1.80,5,0)+10.0;
//+卧铺订票费
	fee_p->RWX_fee += 10.0;
	fee_p->RWX_half_fee += 10.0;
	return lc;
}

int price_app(int distance,int fac,char *result)
{
ticket_fee tfee;
int lc,i;
double *dp;
char *cp;

	if(fac<0) {
	JSON_OBJECT res;
		res=json_object_new_object();
		json_object_object_add(res,"info",jerr(-1,"错误的票价模式"));
		strcpy(result,json_object_to_json_string(res));
		json_object_put(res);
		return -1;
	}
	lc=base_fee(distance,&tfee,fac);//最后的参数：基本=0,新空调=5, 折扣一档=4,折扣二档=3 
	dp=(double *)&tfee;
	cp=result;
	for(i=0;i<(sizeof(tfee)/sizeof(double));i++) {
		cp+=sprintf(cp,"%.2f|",*dp++);
	}
//ShowLog(5,"%s:lc=%d,%s,size=%d",__FUNCTION__,lc,result,sizeof(tfee)/sizeof(double));
	return lc;
}

/*
main(int argc,char *argv[])
{
ticket_fee tfee;
float lc;
char buf[100];
int distance,f=0;

	if(argc>1) f=atoi(argv[1]);
	while(!ferror(stdin)) {
		if(isatty(0)) {
			printf("Input distabce:");
			fflush(stdout);
		}
		fgets(buf,sizeof(buf),stdin);
		if(feof(stdin)) break;
		if(!isdigit(*buf)) continue;
		lc=base_fee(atoi(buf),&tfee,f);//最后的参数：基本=0,新空调=5, 折扣一档=4,折扣二档=3 
		printf("计费里程 硬  全  硬  半  软  全  软  半  普  快  特  快 "
			"硬卧上全硬卧上半硬卧中全硬卧中半硬卧下全硬卧下半软卧上全软卧上半软卧下全软卧下半 空  调高二上全高二下全 \n");
		printf("%.1lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf"
			"\t%.2lf\t%.2lf"
			"\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t%.2lf"
			"\t%.2lf\t%.2lf\t%.2lf\t%.2lf"
			"\t%.2lf\t%.2lf\t%.2lf\n",
			lc,tfee.YZ_fee,tfee.YZ_half_fee,
			tfee.RZ_fee,tfee.RZ_half_fee,
			tfee.K,tfee.T,
			tfee.YWS_fee,tfee.YWS_half_fee,
			tfee.YWZ_fee,tfee.YWZ_half_fee,
			tfee.YWX_fee,tfee.YWX_half_fee,
			tfee.RWS_fee,tfee.RWS_half_fee,
			tfee.RWX_fee,tfee.RWX_half_fee,
			tfee.acc,
			tfee.G2S_fee,tfee.G2X_fee);

	}

	return 0;
}
*/
