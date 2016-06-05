#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
void err_sys(const char *errmsg);
int getmac(char *out,int sockfd)
{               
        int i;
        struct ifreq ifr;
        struct arpreq arpr;
	char *p;
       
	 strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name));

        /* get hardware address */
        ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
        if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1)
		return -1;
       
        /* output hardware address */
	p=out;
        for (i = 0; i < 6; i++) {
                        unsigned char *mac = (unsigned char *) ifr.ifr_hwaddr.sa_data;
                p+=sprintf(p,"%02x", (int) mac[i]);
                if (i != 5)
                        p+=sprintf(p,"%c", ':');
        }
	return 0;
}



