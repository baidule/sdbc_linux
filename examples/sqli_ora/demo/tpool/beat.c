#include <unistd.h>
#include <sc.h>
//¹¥»÷ÊÔÑé
int main()
{
int sock,i,len,ret;
char buf[1025];
	strcpy(buf,"      ");
	len=strlen(buf);
	for(i=0;i<10;i++) {
	sock=tcpopen("localhost","11455");
	if(sock <0) {
		perror("Can't open socket");
		exit(1);
	}
		ret=write(sock,buf,len);
		if(errno==0) {
		char rcvbuf[100];
			ret=read(sock,rcvbuf,sizeof(rcvbuf));
			printf("read:%s\n",rcvbuf);
		}
	close(sock);
	}
	return 0;
}
