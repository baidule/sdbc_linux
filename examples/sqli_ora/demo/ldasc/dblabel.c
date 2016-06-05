#include <sqli.h>
#include <libgen.h>

static char my_showid[200];
main(int argc,char *argv[])
{
int ret,i;
T_SQL_Connect SQL_Connect;
char *dblabel=0,*myname;

	tzset();
	myname=sc_basename(argv[0]);
	sprintf(my_showid,"%s:%d",
		myname,getpid());
	Showid=my_showid;

/*******************************************************************
 *  get Opt
 *******************************************************************/
	for(i=1;i<argc;i++) {
		if(*argv[i]=='-') {
			switch(argv[i][1]) {
			case 'f':	//配置文件
				if(argv[i][2]) ret=envcfg(argv[i]+2);
				else {
					i++;
					ret=envcfg(argv[i]);
				}
				continue;
			default:
				fprintf(stderr,"no know option:%s\n",argv[i]);
 				fprintf(stderr,"Usage:%s -f 配置文件 \n",
					argv[0]);
				continue;
			}
		}
		dblabel=argv[i];
	}

char *   p=decodeprepare(dblabel);
        if(!p) {
                SQL_Connect.Errno=-1;
                strcpy(SQL_Connect.ErrMsg,"Envenroment not init");
                return -1;
        }

        ret=SQL_AUTH(getenv("DATABASEAUTHFILE"),p,
                        SQL_Connect.DSN,
                        SQL_Connect.UID,
                        SQL_Connect.PWD,
                        SQL_Connect.DBOWN);
ShowLog(5,"dblabel=%s,p=%s,ret=%d",dblabel,p,ret);
        if(ret) {
                SQL_Connect.Errno=ret;
                strcpy(SQL_Connect.ErrMsg,"SQL_AUTH Error");
                return 2;
        }
	printf("%s/%s@%s\n",
                SQL_Connect.UID,
                SQL_Connect.PWD,
		SQL_Connect.DSN);

	return 0;
}
