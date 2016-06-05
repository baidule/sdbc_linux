#include <datejul.h>

 #include <stdlib.h>
/*
       div_t div(int numerator, int denominator);
       ldiv_t ldiv(long numerator, long denominator);
       lldiv_t lldiv(long long numerator, long long denominator);
*/

char * i_toStr(int n, char *s)
{
char *p;
int sign;
div_t div_r;
	p=s;
        if ((sign = n) < 0) n = -n;
        do {
		div_r=div(n,10);
                *p++ = div_r.rem + '0';
		n=div_r.quot;
        } while (n != 0);
        if (sign < 0) *p++ = '-';
        *p = '\0';
        strrevers(s);
	return p;
}

char * ll_toStr(register INT64 n, char *s)
{
register char *p;
int sign;

	p=s;
	sign=(n<0);
        if (sign ) n = -n;
        do {
                *p++ = n % 10 + '0';
        } while ((n /= 10) > 0);
        if (sign) *p++ = '-';
        *p = '\0';
        strrevers(s);
	return p;
}

int main(int argc,char *argv[])
{
int i,a=-310511;
char result[30];
INT64 now;

	tzset();
	now=now_usec();
	for(i=0;i<10000;i++) {
		i_toStr(i+a,result);
	}
	printf("%s,TIME=%d\n",result,INTERVAL(now));
	return 0;
}
