/* enigma 加密测试 
 * 比较3种方法的性能，观察周期性
 */
#include <stdio.h>
#include <string.h>
#include <enigma.h>
#include <sys/time.h>

#define TEST_SPEED

long interval(struct timeval *begtime,struct timeval *endtime)
{
long ret;
        ret=endtime->tv_sec-begtime->tv_sec;
        ret*=1000000;
        ret += endtime->tv_usec - begtime->tv_usec;
        return ret;
}

static const char key[]= { 0,15,38,97,122,233,127,255,
			12,192,113,22,95,111,245,
			44,27,92,156,161,31,59,73,
			13,2,41,7,199,250,114,214,
			17,118,193,8,254,43,98,177,
			34,18,94,19,120,143,198,166,
			116,14,174,11,121,141,191,201,
			106,64,164,16,126,146,196,206,
			105,65,165,15,125,145,195,205,
			5,55,155,35,135,245,185,233,
			22,182,173,92,95,121,248,
			42,24,97,157,163,38,58,78,
			13,2,41,7,99,250,114,214,
			150,118,193,9,254,43,98,102,
			34,18,194,19,120,143,198,202,
			116,14,174,50,201,111,191,178,
			107,61,162,86,206,156,196,183,
			104,62,168,85,215,145,200,172,
			240,51,101,36,131,241,181,231,
			247,237,63,241,243,221,41,211,
			6,126,175,29,129,229,169,219,
			167,74,171,20,30,222,160,217,
			117,70,32,28,124,220,168,102,
			253,252,123,21,91,114,241,
			47,27,97,157,164,33,55,77,
			14,29,49,6,196,251,118,218,
			10,110,190,69,151,46,93,103,
			39,218,197,15,128,148,108,242,
			6,126,175,29,129,229,169,219,
			167,74,171,20,30,222,160,100,
			117,70,32,28,124,220,168,214,
			253,252,123,21,91,114,241,
			47,27,97,157,164,33,55,77,
			14,29,49,6,196,251,118,218,
			10,110,190,69,151,46,93,103,
			39,218,197,15,128,148,108,242,
			176,184,179,211,121,166,141,234,
			186,60,169,116,129,146,196,8,
			195,69,168,115,175,85,225,37,
			137,78,170,23,120,223,165,215};

int main(int ac,char *av[])
{
char buf[131702];
int len,i,len1;
ENIGMA2 egm;
struct timeval beg,end;

		gettimeofday(&beg,0);
//	enigma2_init(&egm," 永 垂 不 朽 的 Enigma",0);
	enigma2_init(&egm,key,sizeof(key));
		gettimeofday(&end,0);
	printf("t1:");
	for(i=0;i<256;i++) {
		if(!(i&7)) putchar('\n');
		printf("%3d:%d\t",i,255&egm.r[0][i]);
	}
	printf("\nt3:");
	for(i=0;i<256;i++) {
		if(!(i&7)) putchar('\n');
		printf("%3d:%d\t",i,255&egm.r[2][i]);
	}
	printf("\ncrc=%d,INTERVAL=%ld\n",egm.crc,interval(&beg,&end));
	memset(buf,'B',sizeof(buf));
	buf[sizeof(buf)-1]=0;
#ifndef TEST_SPEED
	while(!ferror(stdin)) {
		fgets(buf,sizeof(buf),stdin);
		if(feof(stdin)) break;
		TRIM(buf);
#endif
		len=strlen(buf);
		gettimeofday(&beg,0);
		enigma(egm.r,buf,len);
		gettimeofday(&end,0);
		len1=len>32?32:len;
		printf("enigma encode     :");
		for(i=0;i<len1;i++) printf("%02X ",buf[i]&255);
		printf("\nenigma encode  64K:");
		for(i=0;i<len1;i++) printf("%02X ",buf[i+65536]&255);
		printf("\ntimeval=%ld\n",interval(&beg,&end));
		enigma(egm.r,buf,len);
		printf("enigma decode:\n%.100s\n",buf);
//test frenz
		gettimeofday(&beg,0);
		enigma_encrypt(egm.r,buf,len);
		gettimeofday(&end,0);
		printf("enigma_encrypt    :");
		for(i=0;i<len1;i++) printf("%02X ",buf[i]&255);
		printf("\nenigma_encrypt 64K:");
		for(i=0;i<len1;i++) printf("%02X ",buf[i+65536]&255);
		printf("\ntimeval=%ld\n",interval(&beg,&end));
		enigma_decrypt(egm.r,buf,len);
		printf("enigma_decrypt:\n%.100s\n",buf);
//test enigma2
		gettimeofday(&beg,0);
		enigma2_encrypt(&egm,buf,len);
		gettimeofday(&end,0);
		printf("enigma2 enccrypt  :");
		for(i=0;i<len1;i++) printf("%02X ",buf[i]&255);
		printf("\nenigma2 encode 64K:");
		for(i=0;i<len1;i++) printf("%02X ",buf[i+65536]&255);
		printf("\ntimeval=%ld\n",interval(&beg,&end));
		enigma2_decrypt(&egm,buf,len);
		printf("enigma2 decrypt:\n%.100s\n",buf);
#ifndef TEST_SPEED
	}
#endif
	return 0;
}
