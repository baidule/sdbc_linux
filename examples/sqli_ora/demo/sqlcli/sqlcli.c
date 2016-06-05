#include <unistd.h> 
#include "sqlcli.h"

static char my_showid[100];
int main(int argc,char *argv[])
{
T_Connect *conn;
int cc,poolno;
char *cp;

	if(argc < 2) {
		fprintf(stderr,"Usage: %s cfgfile\n",argv[0]);
		return 1;
	}
	if(envcfg(argv[1])) {
		perror(argv[1]);
		return 1;
	}
		
	scpool_init(1);
	sprintf(my_showid,"%s:%d",argv[0],getpid());
	Showid=my_showid;
	poolno=0;
//DNODE意为dest node，目标节点，就是交易路由
	cp=getenv("DNODE");
	if(cp) {
		cc=atoi(cp);
		poolno=get_scpool_no(cc);
		if(poolno<0) {
			ShowLog(1,"DNODE=%d: 所指定的连接不存在！",cc);
			return 2;
		}
	}

	conn=get_SC_connect(poolno,-1);
	if(!conn) {
		printf("get_SC_connect %d fault!\n",poolno);
		scpool_free();
		return 1;
	}

	cc=testselect(conn);
	cc=cli_prepare(conn);

	release_SC_connect(&conn,poolno);
	scpool_free();
	ShowLog(2,"TASK finished!");
	return 0;
}
