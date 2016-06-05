#include <datejul.h>

main()
{
char time_buf[64];

	tzset();
	printf("%s\n",rusecstrfmt(time_buf,now_usec(),"YYYY/MM/DD HH24:MI:SS.FF6"));

}
