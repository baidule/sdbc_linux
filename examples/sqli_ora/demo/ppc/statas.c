#include <stdio.h>
#include <string.h>
#include <strproc.h>

main(int argc,char *argv[])
{
FILE *fd;
long num=0,n,max=0,min=0;
INT64 total=0;
char buf[2048],*p;

	fd=stdin;
	if(argc > 1) {
		fd=fopen(argv[1],"r");
		if(!fd) {
			perror(argv[1]);
			return 1;
		}
	}
	while(!ferror(fd)) {
		fgets(buf,sizeof(buf),fd);
		if(feof(fd)) break;
		p=strrchr(buf,'=');
		if(!p || *p!='=') continue;
		p++;
		n=atoi(p);
		if(n>max) max=n;
		if(min==0 || n<min) min=n;
		num++;
		total+=n;
	}
	printf("num=%d,total=%ld,min=%d,max=%d,avg=%.2lf\n",num,total,min,max,(double)total/num);
	if(fd != stdin) fclose(fd);
	return 0;
}
