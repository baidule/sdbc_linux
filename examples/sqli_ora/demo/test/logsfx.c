#include <strproc.h>

char * psfx(int bh,int eh,int n,int now)
{
int i,j,bm,em;
int len;
static char buf[5];
char *p;

	if(n<1) return NULL;
	bm=bh*60;
	em=eh*60;
	len=em-bm;
	if(n>len) n=len;
	if(len==0 || now<bm || now>=em) return NULL;
	j=bm+(now-bm)*n/len*len/n;
	p=buf;
	p+=sprintf(p,"%02d",j/60);
	if((eh-bh) % n) {
		sprintf(p,"%02d",j%60);
	}
	return buf;
}

char *logsfx(int now)
{
int bh,eh,m,n;
int ret;
static char buf[5];
int n1,n2;
char *p;

	p=getenv("LOGSEG");
	if(!p || !*p) return "log";
	fprintf(stderr,"LOGSEG=%s\n",p);
	
		ret=sscanf(p,"%d?%d:%d",&bh,&m,&n);
		if(ret<1) return "log";
		else if(ret == 1 ) {
				ret=sscanf(p,"%d-%d?%d:%d",&bh,&eh,&m,&n);
				if(ret==2) {
					m=1;
					ret=5;
				} else if(ret==3) ret=5;
		}
		switch(ret) {
		case 1:
				return psfx(0,24,bh,now);
				break;
		case 2:
				if(bh>23) bh=23;
				if(now/60<bh) return psfx(0,bh,1,now);
				else return psfx(bh,24,m,now);
				break;
		case 3:
				eh=24;
		case 4:
				if(eh>24) eh=24;
				if(bh>eh) bh=eh;
				n1=24-eh;
				n2=n*n1/bh;
				if(eh<24 && n2<1) n2=1;
				n1=n-n2;
				if(n1<1) n1=1;
				ret=now/60;
				if(ret<bh) return psfx(0,bh,n1,now);
				else if(ret <eh ||eh >=24) return psfx(bh,eh,m,now);
				else return psfx(eh,24,n2,now);
				break;
		case 5:
				if(eh>24) eh=24;
				if(bh>(eh-1)) bh=eh-1;
				ret=now/60;
				if(ret<bh) return psfx(0,bh,1,now);
				else if(ret<eh || eh==24) return psfx(bh,eh,m,now);
				else  return psfx(eh,24,1,now);
				break;

		}
	return "log";
}

#include <sys/time.h>
long interval(struct timeval *begtime,struct timeval *endtime)
{
		long ret;
        ret=endtime->tv_sec-begtime->tv_sec;
        ret*=1000000;
        ret += endtime->tv_usec - begtime->tv_usec;
        return ret;
}


main(int argc,char *argv[])
{
char buf[100];
int n1,n2;
char *p;
struct timeval begtime,endtime;

	if(argc>1) envcfg(argv[1]);
	Showid=argv[0];
ShowLog(1,"%s",argv[0]);
fprintf(stderr,"STDERR:pid=%d\n",getpid());
system("../bin/mkpatt");
	printf("input: hh:mm\n");
	while(fgets(buf,sizeof(buf),stdin)) {
		sscanf(buf,"%d:%d",&n1,&n2);
		gettimeofday(&begtime,0);
		p=logsfx(n1*60+n2);
		gettimeofday(&endtime,0);
		printf("suffix=%s,TIMEVAL=%ld\n",p,interval(&begtime,&endtime));
		printf("input: hh:mm\n");
	}
	ShowLog(-1,0);
}
