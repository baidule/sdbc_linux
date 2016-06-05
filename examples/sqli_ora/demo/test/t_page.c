#define SERVER
#include <sdbc.h>
#include <stdio.h>

extern char *get_page(char *stmt,int page_no,int page_size);

main(int argc,char *argv[])
{
T_SQL_Connect SQL_Connect;
int ret;
char stmt[4096];
char *rec;

	tzset();
	if(argc>1) envcfg(argv[1]);
	ret=db_open(&SQL_Connect);
	if(ret) {
		printf("Open Database error\n");
		return 1;
	}

	sprintf(stmt,"select * from ticket.seat");
	printf("stmt=%s\n",get_page(stmt,9000,100));
	
	ret=___SQL_Select__(&SQL_Connect,stmt,&rec,0);
	printf("ret=%d,%s\n",ret,rec);
	if(rec) free(rec);
	___SQL_CloseDatabase__(&SQL_Connect);
}
