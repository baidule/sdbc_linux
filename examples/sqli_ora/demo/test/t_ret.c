/**********************************************************
 * test RETURNING  
 **********************************************************/

#include <sqli.h>

#define M_ST_OFF(struct_type, member)    \
    ((void*) &((struct_type*) 0)->member)

typedef struct {
        char id;
        char i64[22];
        short i2;
        double d8;
        double d4;
        INT64 t1;
        char r5[128];
        char date1[YEAR_TO_SEC_LEN];
} TTYPE_stu;

main(int argc,char **argv)
{
T_SQL_Connect SQL_Connect;
TTYPE_stu tt;
int ret,sth;
char stmt[4096],timestamp[30];
struct timeval ts;
INT64 now;

	if(argc>1) ret=envcfg(argv[1]);
	gettimeofday(&ts,0);
	ret=db_open(&SQL_Connect);
	if(ret) {
		printf("oprn database error!\n");
		return 1;
	}
	___SQL_Transaction__(&SQL_Connect,TRANBEGIN);
	tt.id=14;
	tt.i2=0;
	tt.d8=1234567890.12;
	tt.d4=12.34;
	rsecstrfmt(tt.date1,now_sec(),"YYYY-MM-DD HH24:MI:SS");
	strcpy(stmt,"INSERT INTO TICKET.TTYPE (\"id\",\"i2\",\"d8\",\"d4\",\"date1\") "
		    "VALUES(:1,:2,:3,:4,TO_DATE(:5,'YYYY-MM-DD HH24:MI:SS')) "
		    "RETURNING TO_CHAR(\"t1\",'YYYY-MM-DD HH24:MI:SS.FF6') INTO :6");
	sth=sqlo_prepare(SQL_Connect.dbh,stmt);
	if(sth<0) {
		___SQL_GetError(&SQL_Connect);
		printf("prepare %s err=%d,%s\n",stmt,
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		 ___SQL_CloseDatabase__(&SQL_Connect);
		return 1;
	}
	if(SQLO_SUCCESS !=
		(sqlo_bind_by_pos(sth, 1, SQLOT_INT, &tt.id, sizeof(tt.id), NULL, 0)) ||
		(sqlo_bind_by_pos(sth, 2, SQLOT_INT, &tt.i2, sizeof(tt.i2), NULL, 0)) ||
		(sqlo_bind_by_pos(sth, 3, SQLOT_FLT, &tt.d8, sizeof(tt.d8), NULL, 0)) ||
		(sqlo_bind_by_pos(sth, 4, SQLOT_FLT, &tt.d4, sizeof(tt.d4), NULL, 0)) ||
		(sqlo_bind_by_pos(sth, 5, SQLOT_STR, tt.date1, sizeof(tt.date1), NULL, 0)) ||
		(sqlo_bind_by_pos(sth, 6, SQLOT_STR, timestamp, sizeof(timestamp), NULL, 0))) {
			
		___SQL_GetError(&SQL_Connect);
		printf("bind %s err=%d,%s\n",stmt,
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		___SQL_Close__(&SQL_Connect,sth);
		___SQL_CloseDatabase__(&SQL_Connect);
		return 2;
	}
	ret=sqlo_execute(sth,1);
	if(SQLO_SUCCESS != ret) {
		___SQL_GetError(&SQL_Connect);
		printf("execute %s err=%d,%s\n",stmt,
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		___SQL_Close__(&SQL_Connect,sth);
		___SQL_CloseDatabase__(&SQL_Connect);
		return 3;
	}

	printf("exec=%d,timestamp=%s\n",ret,timestamp);

	___SQL_Close__(&SQL_Connect,sth);
	___SQL_Transaction__(&SQL_Connect,TRANROLLBACK);
	___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
