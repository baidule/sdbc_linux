/* 演示enigma2的使用 */
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

	printf("sizeof long=%d,int=%d\n",sizeof(long),sizeof(int));
	pass=getpass("输入密钥：");
	//printf("输入密钥：:");
	//fflush(stdout);
	//fgets(buf,512,stdin);
	enigma2_init(&egm,pass,0);
	while(!ferror(stdin)) {
		if(isatty(0)) {
			fprintf(stderr,"输入数据:");
			fflush(stderr);
		}
		fgets(buf,sizeof(buf),stdin);
		if(feof(stdin)) break;
		len=strlen(buf);
		len1=len>32?32:len;

		enigma2_encrypt(&egm,buf,len);
		printf("enigma2 encrypt:");
		for(i=0;i<len1;i++) printf("%02X ",buf[i]&255);

		enigma2_decrypt(&egm,buf,len);
		printf("\nenigma2 decrypt:%.100s",buf);
	}
	return 0;
}
