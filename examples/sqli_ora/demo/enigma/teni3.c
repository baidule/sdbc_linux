/* enigma º”√‹≤‚ ‘ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <enigma.h>
#include <sys/time.h>

int main(int ac,char *av[])
{
char buf[131702];
int len,i,len1;
ENIGMA2 egm;
char *pass;

	pass=getpass(" ‰»Î√‹‘ø£∫");
	enigma2_init(&egm,pass,0);
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
	printf("\ncrc=%d\n",egm.crc);
	while(!ferror(stdin)) {
		fgets(buf,sizeof(buf),stdin);
		if(feof(stdin)) break;
		len=strlen(buf);
		enigma(egm.r,buf,len);
		len1=len>32?32:len;
		printf("enigma encode :");
		for(i=0;i<len1;i++) printf("%02X ",buf[i]&255);
		enigma(egm.r,buf,len);
		printf("\nenigma decode :%.100s",buf);
//test frenz
		enigma_encrypt(egm.r,buf,len);
		printf("enigma_encrypt:");
		for(i=0;i<len1;i++) printf("%02X ",buf[i]&255);
		enigma_decrypt(egm.r,buf,len);
		printf("\nenigma_decrypt:%.100s",buf);
//test enigma2
		enigma2_encrypt(&egm,buf,len);
		printf("enigma2 encode:");
		for(i=0;i<len1;i++) printf("%02X ",buf[i]&255);
		enigma2_decrypt(&egm,buf,len);
		printf("\nenigma2 decode:%.100s",buf);
	}
	return 0;
}
