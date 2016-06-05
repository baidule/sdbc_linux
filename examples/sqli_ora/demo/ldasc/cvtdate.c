#include <stdio.h>
#include <ctype.h>
#include <datejul.h>
#include <sys/types.h>
#include <regex.h>
extern char delimit; // in fdbc:date.c
regex_t preg;
/*
usage:
$ cvtdate ([-dDmMyYju[refdate]] date) ...
*/
int centry=19,year=70;
char * y2kextend(char *str);

INT4  today;
//char *getenv(char *);

main(argc,argv)
int argc;
char *argv[];
{
char buf[12],flg=0,*penv;
int i;
int flg1;
char date1[]="^([0-9][0-9])[-./]{0,1}[01][0-9][-./]{0,1}[0-3][0-9]";
time_t day;
char *p;
struct tm *timp;
	flg1=0;
	*buf=0;
	tzset();
/* Y2K */
	if((penv=getenv("Y2KSTART")) && *penv)
		sscanf(penv,"%2d%2d",&centry,&year);
	if(i=regcomp(&preg,date1,REG_EXTENDED)) {
		fprintf(stderr,"regcomp err %d\n",i);
		exit(1);
	}
/*
time(&day);
day += (8*3600);
i = day%(24*3600);
day = day/(24*3600);

day= day*(24*3600);
day -= (8*3600);
day += i;

timp=localtime(&day);
printf("2-- daynum=%d,day=%04d-%02d-%02d %02d:%02d:%02d\n",
	day,
	timp->tm_year+1900,timp->tm_mon+1,timp->tm_mday,
	timp->tm_hour,timp->tm_min,timp->tm_sec);

timp=localtime(&day);
today=mktime(timp);
printf("1--day=%d,today=%d\n",day,today);
printf("day=%04d-%02d-%02d %02d:%02d:%02d\n",
	timp->tm_year+1900,timp->tm_mon+1,timp->tm_mday,
	timp->tm_hour,timp->tm_min,timp->tm_sec);
i=day%(24*3600);
day = day/(24*3600)+1;
printf("22---day=%d,today=%d,i=%d\n",day,today,i);
printf("33---day=%d,today=%d\n",day,today);
day = (day-1)*(24*3600)+i;
timp=localtime(&day);
printf("2-- daynum=%d,day=%04d-%02d-%02d %02d:%02d:%02d\n",
	day,
	timp->tm_year+1900,timp->tm_mon+1,timp->tm_mday,
	timp->tm_hour,timp->tm_min,timp->tm_sec);
*/

	today=rtoday();
	if(argc<2) {
		prday(".",'u');
		exit(0);
	}
	for(i=1;i<argc;i++) {
		if(*argv[i] == '-' && isalpha(argv[i][1])) {
			flg=*(argv[i]+1);
			if(argv[i][2])  {
			   today=cvtdate(y2kextend(argv[i])+2,today);
//fprintf(stderr,"refdate=%d\n",today);
			}
 			continue;
		}
		else if(*argv[i]!='+') {
			day=strtol(argv[i],&p,10);
			if(p>argv[i] && !*p) { // is uniq digit as juldate
				if(day>31) {
					prday(".",flg,day);
					continue;
				} else day=0;
			} else day=0;
		}
		prday(y2kextend(argv[i]),flg,today);
		flg=0;
	}
	regfree(&preg);
}
prday(arg,flg,today)
char *arg;
int flg;
time_t today; 
{
long day;
short ymd[3];
char buf[14];
//printf("prday(%s,%d,%d)\n",arg,flg,today);
		day=cvtdate(arg,today);
		rjulymd(day,ymd);
		switch(flg) {
		case 'U':
			printf("%u\n",day);
			break;
		case 'y':
			printf("%d\n",ymd[0]);
			break;
		case 'm':
			printf("%02d\n",ymd[1]);
			break;
		case 'd':
			printf("%02d\n",ymd[2]);
			break;
		case 'M':
			printf("%02d\n",mday(day));
			break;
		case 'Y':
			printf("%03d\n",yday(day));
			break;
		case 'j':
			printf("%02d\n",jday(day));
			break;
		case 'w':
			printf("%d\n",day%7);
			break;
		default:
			puts(rjulstr(buf,day));
			flg=1;
			break;
		}
}
char *
y2kextend(str)
char *str;
{
static char date[20];
regmatch_t match[3];
int cc;
char *p,buf[20];
	strncpy(date,str,19);
	if(regexec(&preg,date,3,match,0)) return(str);
//fprintf(stderr,"MATHED!befor=%s",date);
	p=date+match[1].rm_so;
	sscanf(p,"%d",&cc);
	if(cc>1000) return(str);
	if(cc<year) cc+=(centry+1)*100;
	else cc+=centry*100;
	sprintf(buf,"%d",cc);
	strsubst(p,match[1].rm_eo-match[1].rm_so,buf);
//	sprintf(p,"%d",cc);
//fprintf(stderr,"after=%s\n",date);
	return date;
}
