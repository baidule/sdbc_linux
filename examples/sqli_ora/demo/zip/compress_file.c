// Sample demo for QuickLZ 1.5.x

// Remember to define QLZ_COMPRESSION_LEVEL and QLZ_STREAMING_MODE to the same values for the compressor and decompressor

#include <stdio.h>
#include <stdlib.h>
#include <datejul.h>

#include "quicklz.h"

int main(int argc, char* argv[])
{
FILE *ifile, *ofile;
char *src, *dst;
INT64 now;
//qlz_state_decompress state_decompress;
//qlz_state_compress state_compress ;
size_t len, len2; 
char *src2;

    if(!(ifile = fopen(argv[1], "rb"))) { 
		perror("file");
		return 1;
    }

    // allocate source buffer and read file
    fseek(ifile, 0, SEEK_END);
    len = ftell(ifile);
    fseek(ifile, 0, SEEK_SET);
    src = (char*) malloc(len);
    src2 = (char*) malloc(len);
    fread(src, 1, len, ifile);

    // allocate "uncompressed size" + 400 for the destination buffer
    dst = (char*) malloc(len + 100);

    // compress and write result
    now=now_usec();
    len2 = qlz_compress(src, dst, len); //, &state_compress);
    fprintf(stderr,"qlz_compress %d to %d,TIME=%d\n",len,len2,(int)(now_usec() - now));
    fclose(ifile);
    if(len2<len) {
char *cp=src2+(len-len2);
    memcpy(cp,dst,len2);
    size_t len3=len;
    now=now_usec();
    len = qlz_decompress(dst,src2);
long long end=now_usec();
    if(memcmp(src,src2,len3)) {
	fprintf(stderr,"decompress error!\n");
    } else 
    	fprintf(stderr,"qlz_decompress %d to %d,TIME=%d\n",len2,len,(int)(end - now));
    }
    free(dst);
    free(src);
    free(src2);
    return 0;
}
