#include <pack.h>

enum t_e {
	Zero,
	First,
	Second
};
int main(int ac,char *av[])
{
INT64 now;
char day[40],day1[40];
	tzset();
	if(ac>1) envcfg(av[1]);
	Showid=sc_basename(av[0]);
	now=now_usec();
	ShowLog(5,"TIMEVAL=%d",(int)(now_usec() - now));
printf("%s:ShowLog,TIMEVAL=%d\n",av[0],(int)(now_usec() - now));

	strcpy(day,"2012-8-4 5:3:2.123456");
	now=rstrusecfmt(day,"yyyy-mm-dd hh24:mi:ss.ff6");
	
	rusecstrfmt(day1,now,YEAR_TO_USEC);
	printf("day=%s,now=%lld,day1=%s\n",day,now,day1);

	printf("enum: First=%d\n",First);
	return 0;
}
