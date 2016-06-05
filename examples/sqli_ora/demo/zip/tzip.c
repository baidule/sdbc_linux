#include <pack.h>
#include <scry.h>

int main(int argc,char *argv[])
{
int ret,len;
INT64 now;
FILE *fd;
//char *filename="proteins.txt";
char *filename="small.txt";
char *cp,*buf,*compr;

	if(argc>1) filename=argv[1];
        fd=fopen(filename,"r");
        if(!fd) {
                perror(filename);
                return 1;
        }
        fseek(fd, 0, SEEK_END);
        len=ftell(fd);
        fseek(fd, 0, SEEK_SET);
	buf=malloc(len+10);
	if(!buf) {
		fprintf(stderr,"buf malloc error!\n");
		return 2;
	} 
	compr=malloc(len+10);
	if(!compr) {
		fprintf(stderr,"compr malloc error!\n");
		return 2;
	} 
	char *buf2=malloc(len);
 
	fread(buf,1,len,fd);
	fclose(fd);

//int memzip(char *dest,char *src,int srcLen);
//int memunzip(char *compr_str,int t_len,char *buf,int pkg_len);
	now=now_usec();
	ret=memzip(compr,buf,len);
	printf("compress %d to %d,TIME=%d\n",len,ret,(int)(now_usec() - now));
	cp=buf2+(len-ret);
	memcpy(cp,compr,ret);
	now=now_usec();
	len=memunzip(cp,ret,buf2,len);
	if(memcmp(buf,buf2,len)) {
		printf("uncompress Error!\n");
	} else printf("uncompress %d to %d,TIME=%d\n",ret,len,(int)(now_usec() - now));
	free(compr);
	free(buf);
	free(buf2);
	return 0;
}
