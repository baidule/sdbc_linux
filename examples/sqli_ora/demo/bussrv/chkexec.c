#include <string.h>
int chkexec(char *str)
{
	if(strncmp(str,"mkdir",5)) return -1;
	return 0;
}
