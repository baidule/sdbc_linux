#include <sqli.h>

typedef int S32_t;

int DateInt2Int(S32_t intDate, S32_t *uiDate)
{
    char tmpStr[10];
    S32_t year = intDate/10000;
    S32_t month = (intDate%10000)/100;
    S32_t day = intDate%100;

    const static S32_t d_of_m[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    S32_t total = 0;
    S32_t i = 0;
    total = (year - 1900)*365;    //先计算整年的天数
    //每个闰年多加一天
    
    for(i = 1900; i < year; ++i)
	    if((i%4==0 && i%100!=0) || i%400==0)
		    ++total;
    //超过2月 并且本年是闰年, 天数加一
    if (month > 2 && ((year%4==0 && year%100!=0) || year%400==0))
	    total += 1;
    //计算剩下的月的天数
    for (i = 0; i < month -1; i++)
	    total += d_of_m[i];
    //把天数加上
    total += day;

    *uiDate = total;
    return 1;
}

static char lg[100];
int main(int argc,char *argv[])
{
int ret,res,day;
T_SQL_Connect *sql;
char cmd[4096],*recp=NULL;
INT64 now;

	tzset();
	if(argc>1) ret=envcfg(argv[1]);
	sprintf(lg,"%s:%d",sc_basename(argv[0]),getpid());
	Showid=lg;
	ret=19700101;
	ret=DateInt2Int(ret, &res);
	day=rstrfmttojul("19700101","YYYYMMDD");
	printf("int=%d,date=%d,day=%d\n",ret,res,day);
	DB_pool_init();
	ret=get_DB_connect(&sql,0);
	if(ret) {
		fprintf(stderr,"get_DB_Connect fault!");
		return 1;
	}
	now=now_usec();
//	sprintf(cmd,"BIZ_AUDIT_FUNC.updateLostPackageTable( '20140514142806', '04', :int)");
	sprintf(cmd,"CMS_PROD.TESTFEE( '20140514142806', :char(100), :char(100))");
	ret=ORA_Rpc(sql,cmd,&recp);
	if(ret<0) {
		printf("%s:ret=%d\nerr=%d,%s\n",cmd,ret,sql->Errno,sql->ErrMsg);
	} else {
		printf("%s:ret=%d,recp=%s,results=%d,INTERVAL=%d\n",
			cmd,ret,recp,sql->NativeError,INTERVAL(now));
	}
	release_DB_connect(&sql,0);
	DB_pool_free();
	if(recp) free(recp);
	return 0;
}
