#include <unistd.h>
#include <strproc.h>
#include <quicklz.h>

/*
 * size_t qlz_size_decompressed(const char *source);
 * size_t qlz_size_compressed(const char *source);
 * size_t qlz_compress(const void *source, char *destination, size_t size);
 * size_t qlz_decompress(const char *source, void *destination);
 */
int main(int argc,char *argv[])
{
char buf[65536],tmp[65536];
size_t ret,cc;
	tzset();
	if(argc>1) envcfg(argv[1]);
	
	ret=read(0,tmp,sizeof(tmp));
	if(ret>0) {
		cc=qlz_decompress(tmp,buf);
printf("%s\n",buf);
		ShowLog(1,"de_compress:ret=%ld,cc=%ld,siz=%ld\n",
			ret,cc,qlz_size_decompressed(tmp));
	}
	return 0;
}
