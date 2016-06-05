#include <stdio.h>

void psfx(int bh,int eh,int n)
{
int i,j;
int len;
char buf[5],*p;
	if(n<1) return;
	len=(eh - bh)*60;
	if(len==0) return;
	for(i=0;i<n;i++) {
		j=bh*60+len*i/n;
		p=buf;
		p+=sprintf(p,"%02d",j/60);
		if((eh-bh) % n) {
			sprintf(p,"%02d",j%60);
		}
		printf("%s ",buf);
	}
	putchar('\n');
}

char *logsfx(char *param)
{
int bh,eh,m,n;
int ret;
static char buf[5];
int n1,n2;
		ret=sscanf(param,"%d?%d:%d",&bh,&m,&n);
		if(ret<1) return "log";
		else if(ret == 1 ) {
				ret=sscanf(param,"%d-%d?%d:%d",&bh,&eh,&m,&n);
				if(ret==2) {
					m=1;
					ret=5;
				} else if(ret==3) ret=5;
		}
		switch(ret) {
		case 1:
				psfx(0,24,bh);
				break;
		case 2:
				psfx(0,bh,1);
				psfx(bh,24,m);
				break;
		case 3:
				eh=24;
		case 4:
				n1=24-eh;
				n2=n*n1/bh;
				if(eh<24 && n2<1) n2=1;
				n1=n-n2;
				if(n1<1) n1=1;
				psfx(0,bh,n1);
				psfx(bh,eh,m);
				psfx(eh,24,n2);
				break;
		case 5:
				psfx(0,bh,1);
				psfx(bh,eh,m);
				if(eh<24) psfx(eh,24,1);
				break;

		}
	return NULL;
}
main(int argc,char *argv[])
{
char buf[100];
	printf("input: bh-eh?m:n\n");
	while(fgets(buf,sizeof(buf),stdin)) {
		logsfx(buf);
		printf("input: bh-eh?m:n\n");
	}
}
