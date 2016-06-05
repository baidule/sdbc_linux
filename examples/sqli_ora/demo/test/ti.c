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
测试批量插入 
***************************************/

#include <sqli.h>

#define BATCH 1000

typedef struct {
	int i1;
	int i2;
	int i3;
	int i4;
	short i1_ind;
	short i2_ind;
	short i3_ind;
	short i4_ind;
} ti_stu;

main(int argc,char *argv[])
{
int i,j,num=0;
int ret;
T_SQL_Connect SQL_Connect;
INT64 now;
sqlo_stmt_handle_t cursor;
char *result=0,stmt[1024];
ti_stu ti[BATCH];

	if(argc>1) ret=envcfg(argv[1]);

	ret=db_open(&SQL_Connect);
	if(ret) {
		printf("OPEN_Database %s,err=%d.%s\n",
			SQL_Connect.DBOWN,
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		return 1;
	}

	for(i=0;i<BATCH;i++)
		ti[i].i1_ind=ti[i].i2_ind=ti[i].i3_ind=ti[i].i4_ind=0;
	sprintf(stmt,"INSERT INTO %s.T (I1,I2,I3,I4) VALUES(:1,:2,:3,:4)",SQL_Connect.DBOWN);
	cursor=sqlo_prepare(SQL_Connect.dbh,stmt);
	if(cursor < 0) {
		___SQL_GetError(&SQL_Connect);
		printf("sqlo_prepare err=%d,%s\n",
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		___SQL_CloseDatabase__(&SQL_Connect);
		return 2;
	}
printf("stmt=%s\n",stmt);
	if((0>sqlo_bind_by_pos2(cursor,1,SQLOT_INT,&ti[0].i1,sizeof(int),&ti[0].i1_ind,0,sizeof(ti_stu))) ||
	   (0>sqlo_bind_by_pos2(cursor,2,SQLOT_INT,&ti[0].i2,sizeof(int),&ti[0].i2_ind,0,sizeof(ti_stu))) ||
	   (0>sqlo_bind_by_pos2(cursor,3,SQLOT_INT,&ti[0].i3,sizeof(int),&ti[0].i3_ind,0,sizeof(ti_stu))) ||
	   (0>sqlo_bind_by_pos2(cursor,4,SQLOT_INT,&ti[0].i4,sizeof(int),&ti[0].i4_ind,0,sizeof(ti_stu)))) {
		___SQL_GetError(&SQL_Connect);
		printf("sqlo_bind err=%d,%s\n",
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		___SQL_Close__(&SQL_Connect,cursor);
		___SQL_CloseDatabase__(&SQL_Connect);
		return 3;
	}
		
	j=0;
	now=now_usec();
	for(i=0;i<100000;i++) {
	int n;
		ti[j].i1=ti[j].i2=ti[j].i3=ti[j].i4=i;
		if(BATCH==++j) {
			n=0;
			do {
			    ret=sqlo_execute(cursor,j);
			    if(ret) {
				___SQL_GetError(&SQL_Connect);
				printf("sqlo_execute err=%d,%s\n",
					SQL_Connect.Errno,
					SQL_Connect.ErrMsg);
				break;
			    }
			    n=sqlo_prows(cursor);
			    if(n!=j) {
				___SQL_GetError(&SQL_Connect);
				if(SQL_Connect.Errno==DUPKEY) {
				//重码，修改之。
				}
			    }
			} while (0);
			j=0;
			num+=n;
		}
	}
	printf("insert %dRec's num=%d,TIMEVAL=%lldus\n",i,num,now_usec()-now);

	strcpy(stmt,"TICKET.TI");
	now=now_usec();
	ret=ORA_Rpc(&SQL_Connect,stmt,&result);
	printf("ORA_Rpc:return=%d,TIMEVAL=%lldus\n",ret,now_usec()-now);

	___SQL_Transaction__(&SQL_Connect,TRANROLLBACK);
	___SQL_CloseDatabase__(&SQL_Connect);
}
