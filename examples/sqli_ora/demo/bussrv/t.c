#include <stdio.h>
#include <bignum.h>
#include <ctype.h>
#include <strproc.h>
#include <crc32.h>
#include <sc.h>

extern const unsigned int  family[];
char str[]="EDW2Hm000004DVSA000nmG0100000000W00000000001sW00";
static int chknum(u_int *para,u_int init)
{
int i,old,sum,*ip;
        ip=(int *)para;
        old=*ip;
        *ip &= 0xFFFF;
        sum=(int)init;
        for(i=0;i<PARANUM;i++) 
                sum += *ip++;
        *para |= sum << 16;
        return old;
}

static int NetHeadDispack(T_NetHead *nethead,char *buf,u_int init)
{
char *p;
int old;
        if(!buf)return POINTERR;
        if(isspace(*buf)) {
                return FORMATERR;
        }
        p=str_a64n(PARANUM,buf,nethead->para);
        old=p-buf;
        if(old<(HEADPACKLENGTH) && old<strlen(buf)) {
                return old;
        }
        old=chknum(nethead->para,init);
        if(old != nethead->para[0]) {
                ShowLog(1,"PKGERR:old=%08X,para=%08X",old,nethead->para[0]);
                return FORMATERR;
        }
        nethead->para[0] &= 0xFFFF;
        return 0;
}

int main()
{
char *cp="/fileserver/Develop/sdbc/send/BIRD";
char *p;
int i,CRC;
T_NetHead head;
char buf[200];

	CRC=ssh_crc32(cp,strlen(cp));
	printf("CRC=%08X,len=%ld\n",CRC,strlen(cp)+1);
	i=NetHeadDispack(&head,str,family[29]);
	strhex(PARANUM,head.para,buf);
	printf("NetHeadDispack:len=%ld,ret=%d,buf=%s\n",strlen(str),i,buf);
	str_n64a(PARANUM,head.para,buf);
	printf("str=%s,buf=%s\n",str,buf);
	return 0;
}
