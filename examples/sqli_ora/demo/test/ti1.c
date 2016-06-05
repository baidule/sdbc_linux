/*************************************

create table T (
        I1 number(6),
        I2 number(6),
        I3 number(6),
        I4 number(6)
);

PK:
create or replace procedure ti
IS
I   integer;
BEGIN
     FOR I IN 1..100000 LOOP
      INSERT INTO T VALUES (I,I,I,I);
     END LOOP;
END;
/	
这个OCI程序比存储过程慢很多 
***************************************/

#include <DAU.h>

main(int argc,char *argv[])
{
int i,*ip;
DAU dau;
int ret;
T_SQL_Connect SQL_Connect;
INT64 now;
char *result=0,stmt[1024];

	if(argc>1) ret=envcfg(argv[1]);

	ret=db_open(&SQL_Connect);
	if(ret) {
		printf("OPEN_Database %s,err=%d.%s\n",
			SQL_Connect.DBOWN,
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		return 1;
	}
	ret=DAU_init(&dau,&SQL_Connect,"T",0,0);
	if(ret) {
		printf("Table T not exist!\n");
		___SQL_CloseDatabase__(&SQL_Connect);
		return 2;
	}
	ip=dau.srm.rec;
	now=now_usec();
	for(i=0;i<100000;i++) {
		ip[0]=ip[1]=ip[2]=ip[3]=i;	
		ret=bind_ins(&dau,stmt);
		if(ret) {
			printf("insert error %s\n",stmt);
			break;
		}
	}
	printf("insert %dRec's TIMEVAL=%lldus\n",i,now_usec()-now);

	strcpy(stmt,"TICKET.TI");
	now=now_usec();
	ret=ORA_Rpc(&SQL_Connect,stmt,&result);
	printf("ORA_Rpc:return=%d,TIMEVAL=%lldus\n",ret,now_usec()-now);

	DAU_free(&dau);
	___SQL_Transaction__(&SQL_Connect,TRANROLLBACK);
	___SQL_CloseDatabase__(&SQL_Connect);
}
