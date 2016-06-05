#include <datejul.h>
/********************************************************************************************
 * 感恩节（英语：Thanksgiving Day）是美国和加拿大共有的节日，由美国人民独创，
 * 原意是为了感谢上天赐予的好收成。在美国，自1941年起，感恩节是在
 * 每年11月的第四个星期四，并从这一天起将休假两天；而加拿大与美国的感恩节时间不同，
 * 10月第二个星期一。像中国的春节一样，在这一天，成千上万的人们不管多忙，都要和自己的家人团聚。
 *********************************************************************************************/
//美国感恩节
int th_a(int year)
{
short ymd[3];
int day,d;
	ymd[0]=year;
	ymd[1]=11;
	ymd[2]=1;
	day=rymdjul(ymd);
	if(day < 0) return -1;
	d=4-day%7;
	if(d<0) d+=7;
	day+=d; //第一个周四
	return day+21;
}


//加拿大感恩节
int th_c(int year)
{
short ymd[3];
int day,d;
	ymd[0]=year;
	ymd[1]=10;
	ymd[2]=1;
	day=rymdjul(ymd);
	if(day < 0) return -1;
	d=1-day%7;
	if(d<0) d+=7;
	day+=d; //第一个周一
	return day+7;
}
//每周二上午8:00 - 12:00有活动
main(int argc,char *argv[])
{
INT4 now,today;
int w,m;
char buf[256];
char a[30],c[30];
	tzset();
	printf("每周二上午8:00 - 12:00有活动\n");
	printf("input YYYY-MM-DD HH24:MI\n");
	while(fgets(buf,sizeof(buf),stdin)) {
		today=rstrfmttojul(buf,"YYYY-MM-DD");	
		now=rstrminfmt(buf,"YYYY-MM-DD HH24:MI");	
		w=today%7;//今天是周几
		m=(now-timezone/60)%1440;//现在是当日的第几分钟	
		if(w!=2 || m<480 || m>720) {//没有活动
			printf("没有活动\n");
		} else {
			printf("有活动\n");
		}

		sscanf(buf,"%d",&w);
		m=th_a(w);
		rjulstr(a,m);
		m=th_c(w);
		rjulstr(c,m);
		printf("%d这一年美国感恩节是：%s，加拿大感恩节是%s\n",w,a,c);
		printf("input YYYY-MM-DD HH24:MI\n");
	}
	return 0;
}


