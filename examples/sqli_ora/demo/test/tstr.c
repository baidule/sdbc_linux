#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
//#define STR
int main(int argc,char *argv[])
{
    int i,j;
    const char a[] = "abcdwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww";
	char buf[50000];
	char *p;
	int flg=0;
    
	if(argc>1) {
		if(*argv[1]=='s') flg=1;
	}
    for( i = 0; i < 0xfffff; i++ )
    {
        buf[0] = 0;
		p=buf;
		for(j=0;j<50;j++)  {
#ifdef STR
			 strcat(buf,a);
			 strcat(buf,"|");
#else
		if(flg) {
        	 p=stpcpy( p, a);
			 *p++='|';
			 *p=0;
		} else {
				p+=sprintf(p,"%s|",a);
		}
#endif
		}
    }
    return 0;
}

