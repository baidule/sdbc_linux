#include <stdio.h>
#include <sdbc.h>
#include <ctype.h>
//计算递远递减
typedef struct {
    int D;
    int N;
    int K;
} fall_rate1;
 
fall_rate1 fall_tab[]= {//递远递减
    {0,0,0},
    {201,1,20},//从201到500公里递减率为10%。例如当前为300公里：200+(300-200)(1-10%)==290==300*(1-10%)+20。这里的20就是这样来的
    {501,2,70},
    {1001,3,170},
    {1501,4,320},
    {2501,5,570}
};
static int  FALLNUM=(sizeof(fall_tab)/sizeof(fall_rate1));
//基本·里程分段表
static int dist_segmt[]={20,30,40,50,60,
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
 
 
float jflc(int distance) //计算计费里程
{
int *ip,i,v;
 
    if(distance>6000) return (float)(3595+100*((distance-6001)/100));
    for(i=0,ip=dist_segmt;*ip < distance;ip++) i++;
    if(i>0) v=((ip[-1]+*ip)/2);//规定用distance所在区间的中值，比如：distance＝51,则按(50+60)/2=55计算
    else v=dist_segmt[0];
    for(i=0;i<FALLNUM-1;i++)
        if(fall_tab[i+1].D>v) break;
//printf("%d->%d:%d,%d,%d\n",v,i,
//   fall_tab[i].D, fall_tab[i].N, fall_tab[i].K);
//计算递远递减
    return (float)(fround((v*10-v*fall_tab[i].N)/10.0,5,0)+fall_tab[i].K);
}
 
typedef struct {
    double YZ_fee;//硬座
    double YZ_half_fee;
    double RZ_fee;//软座
    double RZ_half_fee;
    double K;//加快
    double T;//特快
    double YWS_fee;//硬卧上
    double YWS_half_fee;
    double YWZ_fee;//硬卧中
    double YWZ_half_fee;
    double YWX_fee;//硬卧下
    double YWX_half_fee;
    double RWS_fee;//软卧上
    double RWS_half_fee;
    double RWX_fee;//软卧下
    double RWX_half_fee;
    double G2S_fee; //二人高包上
    double G2X_fee; //二人高包下
    double acc;//空调费
} ticket_fee;
 
#define BASE_FEE 0.05861
 
/* 最后，加入软票费 */
//#define RPF(fee) ((fee)>5.0?1.0:0.5)//1元还是.5元。
//计算票价，分票种基本票价表返回计费里程
//fac:基本=0,新空调=5, 折扣一档=4,折扣二档=3
 
float  base_fee(int distance,ticket_fee *fee_p,int fac)
{
double tmp,YZ_base;
float lc;
double insum;
double r=(10.0+fac)/10.0;
    lc=jflc(distance); //计算计费里程
    tmp=YZ_base=BASE_FEE * lc; //许多票价依赖YZ_base
    insum=fround(tmp*0.02,4,1);//保险费
    fee_p->YZ_fee=fround(fround(tmp+insum,5,0)*r,5,0);
    //fee_p->YZ_half_fee=0.5*fee_p->YZ_fee;
//加软票费
    //fee_p->YZ_fee+=RPF(fee_p->YZ_fee);
    //fee_p->YZ_half_fee+=RPF(fee_p->YZ_half_fee);
    if(lc>200) { //硬座+车站空调费
        fee_p->YZ_fee+=1.0;
        //fee_p->YZ_half_fee+=1.0;
    }
//计算软座
    fee_p->RZ_fee=fround(r*fround(2.0*tmp+insum,5,0),5,0);
    //fee_p->RZ_half_fee=0.5*fee_p->RZ_fee;
    //fee_p->RZ_fee+=RPF(fee_p->RZ_fee);
    //fee_p->RZ_half_fee+=RPF(fee_p->RZ_half_fee);
 
//计算空调费
    fee_p->acc=fround(tmp*0.25,5,0);
    if(fee_p->acc<1.0) fee_p->acc=1.0;
    fee_p->acc=fround(r*fee_p->acc,5,0);
 
// 计算加快
    fee_p->K=fround(tmp*0.2,5,0);
    if(fee_p->K<1.0) fee_p->K=1.0;
    fee_p->K=fround(r*fee_p->K,5,0);
    //fee_p->T=2*fee_p->K;
//计算硬卧
    tmp=lc>=380?tmp:BASE_FEE*380;//400KM以内的计费里程
    fee_p->YWS_fee=fround(r*fround(tmp*1.1,5,0),5,0);
    //fee_p->YWS_half_fee=0.5*fee_p->YWS_fee;
//+卧铺订票费
    //fee_p->YWS_fee += 10.0;
    //fee_p->YWS_half_fee += 10.0;
 
    fee_p->YWZ_fee=fround(r*fround(tmp*1.2,5,0),5,0);
    //fee_p->YWZ_half_fee=0.5*fee_p->YWZ_fee;
//+卧铺订票费
    //fee_p->YWZ_fee += 10.0;
    //fee_p->YWZ_half_fee += 10.0;
 
    fee_p->YWX_fee=fround(r*fround(tmp*1.3,5,0),5,0);
    //fee_p->YWX_half_fee=0.5*fee_p->YWX_fee;
//+卧铺订票费
    //fee_p->YWX_fee += 10.0;
    //fee_p->YWX_half_fee += 10.0;
//计算软卧
    fee_p->RWS_fee=fround(r*fround(tmp*1.75,5,0),5,0);
    //fee_p->RWS_half_fee=0.5*fee_p->RWS_fee;
//计算二人高包
    fee_p->G2S_fee=fround(fee_p->RWS_fee*1.80,5,0);
//+卧铺订票费
    //fee_p->RWS_fee += 10.0;
    //fee_p->RWS_half_fee += 10.0;
    fee_p->RWX_fee=fround(r*fround(tmp*1.95,5,0),5,0);
    //fee_p->RWX_half_fee=0.5*fee_p->RWX_fee;
    fee_p->G2X_fee=fround(fee_p->RWX_fee*1.80,5,0);
//+卧铺订票费
    //fee_p->RWX_fee += 10.0;
    //fee_p->RWX_half_fee += 10.0;
    return lc;
}
 
 
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
        printf("计费里程  硬全  软全   普快  "
            "硬卧上全  硬卧中全  硬卧下全  软卧上全  软卧下全 空调费 高二上全  高二下全 \n");
        printf("%.1lf\t%4.2lf\t%4.2lf"
            "\t%4.2lf"
            "\t%.2lf\t%.2lf\t%8.2lf\t%.2lf\t%.2lf"          
            "\t%.2lf\t%.2lf\t%.2lf\n",
            lc,tfee.YZ_fee,
            tfee.RZ_fee,
            tfee.K,
            tfee.YWS_fee,
            tfee.YWZ_fee,
            tfee.YWX_fee,
            tfee.RWS_fee,
            tfee.RWX_fee,
            tfee.acc,
            tfee.G2S_fee,tfee.G2X_fee);
 
    }
 
    return 0;
}
