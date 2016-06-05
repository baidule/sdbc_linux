/*****************************************************
 * SDBC 4.0 数据库包装函数库。
 * 适用于 ORACLE。通过第三方软件sqlora使用OCI接口
 * 访问数据库。
 ****************************************************/

#include <time.h>
#include <sqli.h>
#include <unistd.h>

static int _abort_flag;
void sigint_handler(void);
int sdbc_debug=0;

//T_User_Pthread User_Pthread[MAXTHREAD];
pthread_mutex_t thread_id_lock;
void sigint_handler(void) {
  _abort_flag++;
}
int ___SQL_GetError(T_SQL_Connect *SQL_Connect)
{
	strncpy(SQL_Connect->ErrMsg,(char *)sqlo_geterror(SQL_Connect->dbh),sizeof(SQL_Connect->ErrMsg));
	SQL_Connect->ErrMsg[sizeof(SQL_Connect->ErrMsg)-1]=0;
	TRIM(SQL_Connect->ErrMsg);
	SQL_Connect->Errno= sqlo_geterrcode(SQL_Connect->dbh); 
	if(SQL_Connect->Errno) {
	    if(!(SQL_Connect->Errno==DUPKEY ||
		SQL_Connect->Errno==SQLNOTFOUND ||
		SQL_Connect->Errno==FETCHEND))
	    		ShowLog(1,"%s: dbh=%d,err=%d:%s",__FUNCTION__,
				SQL_Connect->dbh,SQL_Connect->Errno,SQL_Connect->ErrMsg);
	    return SQL_Connect->Errno;
	}
	return 0;
}

int ___SQL_Init_SQL_Connect(T_SQL_Connect *SQL_Connect)
{
  	SQL_Connect->dbh=-1;
	SQL_Connect->Errno=0;
	SQL_Connect->pos=-1;
	*SQL_Connect->ErrMsg=0;
	SQL_Connect->NativeError=0;
	*SQL_Connect->SqlState=0;
	return 0;
}

static int initflg=-1;
static int handle;                   /* handle of the interrupt handler */
extern char _errmsg[];   //in sqlora.c

int ___SQL_init_sqlora(int maxconnect,int maxcurs)
{

	if(initflg<0) {
		ShowLog(2,"%s:maxConn's=%d,maxCursor's=%d",__FUNCTION__,maxconnect,maxcurs);
        	if (SQLO_SUCCESS != sqlo_init(SQLO_ON, maxconnect, maxcurs)) {
         		ShowLog(1,"%s:Failed to init libsqlora",__FUNCTION__);
         		return -1;
        	}
  // register the interrupt handler 
        	sqlo_register_int_handler(&handle, sigint_handler);
        	initflg=0;
  	}
	return 0;
}
/*******************************************************/
/* OpenDatabase - 连接数据库到 sql_var.dbh             */
/*******************************************************/
int ___SQL_OpenDatabase__( T_SQL_Connect *sql_var)
{
char cstr[81],*p;	 /*注册串  用户名/口令 */
char server_version[1024];	/*版本号*/
int ret;

  ___SQL_Init_SQL_Connect(sql_var);
  
  ret=  ___SQL_init_sqlora(MAXCONNECT, MAXCURSOR);
  if(ret<0) return ret;

  TRIM((void *)sql_var->DSN);
  if(sql_var->DSN[0])
      sprintf(cstr,"%s/%s@%s",sql_var->UID, sql_var->PWD,sql_var->DSN); /*组注册串*/
  else 
      sprintf(cstr,"%s/%s",sql_var->UID, sql_var->PWD); /*组注册串*/
  /* login */
  strcpy(sql_var->PWD,"        ");
  if(SQLO_SUCCESS != (ret=sqlo_connect(&sql_var->dbh, cstr))) {  /*连接数据库*/
  	ShowLog(1,"sqlo_connect fail:ret=%d,dbh=%d,%s,%s",ret,sql_var->dbh,cstr,_errmsg);
    	if(ret!=SQLO_SUCCESS_WITH_INFO) {
		sql_var->Errno=ret; 
		sprintf(sql_var->ErrMsg,"CONNECT FAIL:%s,%s",cstr,_errmsg);
  		if(sql_var->dbh>=0) sqlo_finish(sql_var->dbh);
  		sql_var->dbh=-1;
		return -1;
	}
  }
  if (_abort_flag) { 
	sql_var->Errno=-1; 
	strcpy(sql_var->ErrMsg,"ABORT");
  	if(sql_var->dbh>=0) sqlo_finish(sql_var->dbh);
  	sql_var->dbh=-1;
	return -2;
  }
  if (SQLO_SUCCESS != (ret=sqlo_server_version(sql_var->dbh, 
		server_version, sizeof(server_version)))) {	 /*检查数据库版本*/
    	if(ret!=SQLO_SUCCESS_WITH_INFO) {
	  	ShowLog(1,"sqlo_server_version fail:ret=%d,%s,%s",ret,cstr,_errmsg);
    		___SQL_GetError(sql_var);
		sql_var->NativeError=-1; 
		strcpy(sql_var->SqlState,"DATABASE VERSION ERROR");
  		sqlo_finish(sql_var->dbh);
  		sql_var->dbh=-1;
    		return ret;
	}
  }
  p=getenv("COMMIT");
  if(p&&*p) {
	sprintf(cstr,"ALTER SESSION SET COMMIT_WRITE = %s",p);
	ret=___SQL_Exec(sql_var,cstr);
	if(ret<0) {
		ShowLog(1,"%s:%s,err=%d,%s",__FUNCTION__,cstr,
			sql_var->Errno,sql_var->ErrMsg);
	}
  }
  ShowLog(5,"___SQL_OpenDatabase__: %s",server_version);
	sql_var->Errno=0; 
	*sql_var->ErrMsg=0;
  return 0;
}

int ___SQL_Prepare(T_SQL_Connect *SQL_Connect,char *stmt,int bind_num,char *bind_v[])
{
int cols;
sqlo_stmt_handle_t sth;

     if ( 0 > (sth = (sqlo_open(SQL_Connect->dbh, stmt, bind_num, (CONST char **)bind_v)))) {
	ShowLog(1,"___SQL_Prepare__: %s",stmt);
    	___SQL_GetError(SQL_Connect);
     	return sth;
     }
	cols=0;
	sqlo_ocol_names(sth, &cols);
	SQL_Connect->NativeError=cols; 
	SQL_Connect->Errno=0; 
	*SQL_Connect->ErrMsg=0;
     return sth;
}

char *___SQL_Fetch(T_SQL_Connect *SQL_Connect,int curno,int *recnum)
{
register char *p1,*p;
int ret,i,j,cc;
unsigned short *vn,*stp;
char  **val;
unsigned  int coln;
int len,lenw=0;
T_PkgType Char_Type[2]={{CH_CHAR,-1,0,0,0},{-1,0,0,0,0}};

	p1=p=0;
	len=0;
//	if(recnum && *recnum > 0) sqlo_set_prefetch_rows(curno, *recnum);
    for(i=0;!(cc=sqlo_query_result(curno, &coln, &val, &vn, 0,0));i++) {
		ret=coln+100;
		stp=vn;
		for(j=0;j<coln;j++) {
			ret += (*stp >> 3) + *stp;
			stp++;
		}
//if((i%10000)==0) ShowLog(5,"___SQL_Fetch: %d",i);

		if((lenw-len)<ret) {
			lenw=len+ret*200;
			p1=realloc(p,lenw);
			if(!p1) break;
			p=p1;
			p1 += len;
		}

		stp=vn;
        	for(j=0;j<coln;j++) {
			Char_Type->len=1+*stp++;
			p1+=get_one_str(p1,*val++,Char_Type,'|');
			*p1++='|';
			*p1=0;
		}
		len=p1-p;
		if(!recnum || (*recnum>0 && (i+1)>=*recnum)) {
			i++;
			break;
		}
	}
	SQL_Connect->NativeError=coln;
	if(cc<0) {
ShowLog(5,"%s:aft fetch dbh=%d,cur=%d,cc=%d,i=%d",__FUNCTION__,SQL_Connect->dbh,curno,cc,i);
		___SQL_GetError(SQL_Connect);
	}
	if(recnum) *recnum=i;
	if(!i||cc) {
	    if(SQL_Connect->Errno==0) {
		SQL_Connect->Errno=SQLNOTFOUND;
		strcpy(SQL_Connect->ErrMsg,"NOT FOUND");
	    }
	}
	return p;
}

int ___SQL_Close__(T_SQL_Connect *SQL_Connect,int CursorNo)
{
int ret;

     while (SQLO_STILL_EXECUTING == (ret = sqlo_close(CursorNo))) {
		                 usleep(1000);
     }

     if(ret!=SQLO_SUCCESS) ___SQL_GetError(SQL_Connect);
     else {
	     SQL_Connect->Errno=0;
	     *SQL_Connect->ErrMsg=0;
     } 
      return ret;
}

int	___SQL_CloseDatabase__( T_SQL_Connect *SQL_Connect )
{
int i;

  	if(SQL_Connect->dbh==-1) return -1;
	___SQL_Transaction__(SQL_Connect,TRANBEGIN);
  	i=sqlo_finish(SQL_Connect->dbh);
	if(i) {
    		___SQL_GetError(SQL_Connect);
	} else {
		SQL_Connect->Errno=0; 
		*SQL_Connect->ErrMsg=0;
	}
  	SQL_Connect->dbh=-1;
	return i;
}

int ___SQL_Exec(T_SQL_Connect *SQL_Connect ,char *stmt)
{
int ret;
    ret=sqlo_exec (SQL_Connect->dbh, stmt) ;
	if(ret<0) {
		___SQL_GetError(SQL_Connect);
    } else if(ret==0) {
		strncpy(SQL_Connect->ErrMsg,(char *)sqlo_geterror(SQL_Connect->dbh),sizeof(SQL_Connect->ErrMsg));
		SQL_Connect->ErrMsg[sizeof(SQL_Connect->ErrMsg)-1]=0;
		SQL_Connect->Errno= sqlo_geterrcode(SQL_Connect->dbh); 
    } else {
		SQL_Connect->Errno=0; 
		SQL_Connect->NativeError=ret; 
		*SQL_Connect->ErrMsg=0;
    }
    return ret;
}

