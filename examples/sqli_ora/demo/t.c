#include <pack.h>
#include <scry.h>

int main(int ac,char *av[])
{
INT64 now;
char day[30],day1[30],buf[256];
int ret;
	tzset();
	if(ac>1) envcfg(av[1]);
	Showid=sc_basename(av[0]);
	now=now_usec();
	ShowLog(5,"TIMEVAL=%d",(int)(now_usec() - now));
printf("%s:ShowLog,TIMEVAL=%d\n",av[0],(int)(now_usec() - now));
	strcpy(day,"20120804050302123456");

	now=rstrusecfmt(day,"yyyymmddhh24missff6");
	
	rusecstrfmt(day1,now,YEAR_TO_USEC);
	printf("day=%s,now=%lld,day1=%s\n",day,now,day1);
	
	ret=fingerprint(buf);
	printf("buf=%s\n",buf);
	return 0;
}
