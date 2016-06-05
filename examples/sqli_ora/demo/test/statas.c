#include <stdio.h>
#include <string.h>

main(int argc,char *argv[])
{
FILE *fd;
long num=0,total=0,n,max=0;
char buf[256],*p;

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
		p=strchr(buf,'#');
		if(!p || *p!='#') continue;
		p++;
		n=atoi(p);
		if(n>max) max=n;
		num++;
		total+=n;
	}
	printf("num=%d,total=%ld,max=%d,avg=%.2lf\n",num,total,max,(double)total/num);
	if(fd != stdin) fclose(fd);
	return 0;
}
