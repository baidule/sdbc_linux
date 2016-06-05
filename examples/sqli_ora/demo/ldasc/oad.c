#include <DAU.h>
#include <OAD.h>

#define BATCH_NUM 10000
#if BATCH_NUM < 10000
#define COMMIT_NUM (10000/BATCH_NUM)
#else
#define COMMIT_NUM 1
#endif

#define DEF_BUF_SIZ 10240

static DAU _DAU;
static int buf_len=DEF_BUF_SIZ;
static int Pflg=0;
static FILE *ofd;
static volatile int rows=0,upd=0,loss=0;

static pthread_mutex_t oadLock=PTHREAD_MUTEX_INITIALIZER;

/* 插入重码就修改 
 返回小于0出错，在SQL_Connect里 
 >=0插入的条数   
*/

//批量插入，遇重码修改之。在很多重码的场合，效率相当低
int insert_DAO(OAD *oad,int n,char *stmt)
{
char *p;
int cc,ret,num;
int beg;
DAU *DP;

	beg=
	ret=num=0;
  do {
	ret=OAD_exec(oad,beg,n);
	if(ret>0) num+=ret;
	if(ret == n-beg) {
		pthread_mutex_lock(&oadLock);
		rows += num;
		pthread_mutex_unlock(&oadLock);
		return num;
	}
	if(oad->SQL_Connect->Errno != DUPKEY) {
		ShowLog(1,"%s:OAD_exec n=%d,beg=%d,ret=%d err=%d,%s",__FUNCTION__,
			n,beg,ret,
			oad->SQL_Connect->Errno,
			oad->SQL_Connect->ErrMsg);
		loss+=n-num;
		return ret;
	}
	DP=OAD_get_DAU(oad);
	if(ret<0) ret=0;
	p=(char *)oad->recs;
	p+=(beg+ret)*(oad->reclen);
	memcpy(DP->srm.rec,p,oad->reclen);
	*stmt=0;
	cc=update_by_PK(DP,stmt);
	if(cc<=0) {
		p=stmt+strlen(stmt);
		p=stpcpy(p,";bind=");
		cc=DAU_print_bind(DP,p);
		ShowLog(1,"%s:update stmt=%s,err=%d,%s,bind_status=%04X",__FUNCTION__,
			stmt,
			oad->SQL_Connect->Errno,
			oad->SQL_Connect->ErrMsg,
			cc);
		loss += n-num;
		return num;
	}
	upd+=cc;
	beg+=ret+1;
//ShowLog(5,"%s:beg=%d",__FUNCTION__,beg);
  } while(beg<n);
	return num;
}

struct thr_ctx {
	DAU dau;
	OAD oad;
};

//线程开始
static int entry_thr(void **ctx)
{
struct thr_ctx *ctxp=*(struct thr_ctx **)ctx;
T_SQL_Connect *SQL_Connect;
int ret;
char buf[buf_len];
	
	ret=get_DB_connect(&SQL_Connect,0);
	if(ret) {
		ShowLog(1,"%s:get_DB_connect fault ret=%d",__FUNCTION__,ret);
		return -1;
	}
	DAU_init(&ctxp->dau,SQL_Connect,NULL,_DAU.srm.rec,_DAU.srm.tp);
	OAD_init(&ctxp->oad,&ctxp->dau,_DAU.srm.rec,1);
	ret=OAD_mk_ins(&ctxp->oad,buf);
	if(ret) {
		release_DB_connect(&SQL_Connect,0);
		ShowLog(1,"aft OAD_mk_ins:stmt=%s,err=%d,%s\n",buf,
			ctxp->dau.SQL_Connect->Errno,
			ctxp->dau.SQL_Connect->ErrMsg);
			OAD_free(&ctxp->oad);
			DAU_free(&ctxp->dau);
			return -2;
	}
	return ret;
}
//线程工作
static int load_to_db(void *ctx,int n,char *val)
{
struct thr_ctx *ctxp=(struct thr_ctx *)ctx;
int ret;
char buf[buf_len];
INT64 now;

	if(n<=0) return 0;
	now=now_usec();
	OAD_init(&ctxp->oad,NULL,val,n);//重新绑定
	ret=insert_DAO(&ctxp->oad,n,buf);
	if(___SQL_Transaction__(ctxp->dau.SQL_Connect,TRANCOMMIT)) {
		ShowLog(1,"%s:commit fault",__FUNCTION__);
	}
	free(val); //在这里释放了。
	ShowLog(5,"%s:insert %d Rec's,INTERVAL=%d",__FUNCTION__,ret,INTERVAL(now));
	return ret;
}
//线程结束
static int exit_thr(void **ctx)
{
struct thr_ctx *ctxp=*(struct thr_ctx **)ctx;
T_SQL_Connect *sql;
	
		sql=ctxp->dau.SQL_Connect;
		DAU_free(&ctxp->dau);
		OAD_free(&ctxp->oad);
		release_DB_connect(&sql,0);
		return 0;
}

int loadfile(char *tablename,FILE *ifd,char *buf,int buflen,char dlmt)
{
char *p,tabn[512];
OAD oad;
int ret;
int n,num;
INT64 now;
T_SQL_Connect *SQL_Connect;
struct batch_data *bp;
char *recs=NULL;
int rec_siz;

    ret=get_DB_connect(&SQL_Connect,0);
    if(ret) return -1;
//    ShowLog(5,"loadfile:entry dlmt=0X%02X",dlmt&255);
	now=now_usec();
    ret=DAU_init(&_DAU,SQL_Connect,tablename,0,0);
    if(ret) {
        ShowLog(1,"loadfile:DAU_init tabname=%s,ret=%d",tablename,ret);
        return -1;
    }
	rec_siz=DAU_RecSize(&_DAU);
    release_DB_connect(&SQL_Connect,0);
	ShowLog(5,"DAU_init ret=%d,TIMEVAL=%d,reclen=%d",ret,INTERVAL(now),rec_siz);

	ret=thread_init(load_to_db,sizeof(struct thr_ctx),entry_thr,exit_thr);

	upd=loss=0;
	n=num=0;
	now=now_usec();
	thread_start();
    	for(rows=0;!ferror(ifd);) {
		if(!recs) {
			recs=malloc(BATCH_NUM*rec_siz);//用完由thread释放
			p=recs;
		}
        	if(!fgets(buf,buflen,ifd)) break;
		TRIM(buf);
		if(!*buf) continue;
		pkg_dispack(p,buf,_DAU.srm.tp,dlmt);
		p+=rec_siz;
		if(BATCH_NUM == ++n) {
			ret=put_to_thread(n,recs);
			recs=NULL;
			n=0;
		}
    	}
	if(n) {
		ret=put_to_thread(n,recs);
		n=0;
	} else if(recs) free(recs);

	thread_end();
	thread_finish();
	thread_free();
    	DAU_free(&_DAU);
    ShowLog(2,"loadfile:rows=%d,upd=%d,loss=%d,TIMEVAL=%lld",rows,upd,loss,now_usec()-now);
    return rows;
}

static char my_showid[200];
main(int argc,char *argv[])
{
int ret,i;
char *tabname=0;
FILE *ifd;
INT64 now;
char *cp,*myname;
char *dlmt="|";

    tzset();
	myname=getenv("GBK_FLAG");
        if(myname && *myname) GBK_flag=1;

    myname=sc_basename(argv[0]);
    sprintf(my_showid,"%s:%d",
        myname,getpid());
    Showid=my_showid;
    ifd=0;
    ofd=0;

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
            case 'P':  //输出 不能加载的记录
                Pflg=1;
                if(argv[i][2]) {
                    ofd=fopen(argv[i]+2,"w");
                } else {
                    i++;
                    if(i<argc) ofd=fopen(argv[i],"w");
                }
                continue;
            case 'K':  //设置工作buff，Kbyte。 
                if(argv[i][2]) {
                    buf_len=atoi(argv[i]+2);
                } else {
                    i++;
                    if(i<argc) buf_len=atoi(argv[i]);;
                }
                continue;
	     case 'd':  //设置分隔符
                                if(argv[i][2]) {
                                        dlmt=&argv[i][2];
                                } else {
                                        i++;
                                        if(i<argc) dlmt=argv[i];
                                }
                                continue;

             default:
                     fprintf(stderr,"no know option:%s\n",argv[i]);
                     fprintf(stderr,"Usage:%s -f 配置文件 [-P] 输出文件名 [-Kkbytes] [-d分隔符]\n",

                    argv[0]);
                continue;
            }
        }
        tabname=argv[i];
    }
    if(buf_len < DEF_BUF_SIZ) buf_len=DEF_BUF_SIZ;
char buf[buf_len];
    *buf=0;

ShowLog(5,"tabname=%s",tabname);
      if(*dlmt=='\\') switch(dlmt[1]) {
                case 't':
                        *dlmt='\t';
                        break;
                case 'b':
                        *dlmt=' ';
                        break;
                case 'n':
                        *dlmt='\n';
                        break;
                case '0':
                case 0:
                        *dlmt='\0';
                        break;
                default:
                        *dlmt='|';
                        break;
        }

    cp=getenv("COL_TO_LOWER");
    if(cp && isdigit(*cp)) col_to_lower=atoi(cp);
    if(!ofd) ofd=stdout;
    ifd=stdin;

	DB_pool_init();
	if(ret<0) {
		ShowLog(1,"thread_init fault!");
		DB_pool_free();
		return 1;
	}

    now=now_usec();
int cc;
        ret=loadfile(tabname,ifd,buf,sizeof(buf),*dlmt);
        ShowLog(2,"loadasc:load %d rec's time=%d,buf=%s",ret,INTERVAL(now),buf);

    if(ofd && ofd != stdout) fclose(ofd);
	DB_pool_free();
    return 0;
}

