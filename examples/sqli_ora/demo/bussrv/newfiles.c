#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pack.h>
#include <scsrv.h>
extern char *getenv();
extern FILE *logfile;
extern int chkexec(char *str);
extern int substitute_env(char *line);
#define PROTO_DELIMITER "|"
#define PROTO_DELIMITER_CHAR '|'
#define DEFAULT_TMPFILE_SIZE 1024

int WriteTmpFile(char * ls_tmpfile,size_t len,char * ls_time)
{
	FILE *fd = fopen(ls_tmpfile,"w");
	if(fd == NULL)  return -1 ; 
	fprintf(fd,"%lu|%s|\n",len,ls_time);
	fclose(fd);
	return 0 ;
}

int ReadTmpFile(char * ls_tmpfile,char * ls_len,char * ls_time)
{
	char *p,ls_buf[DEFAULT_TMPFILE_SIZE];
	
	FILE *fd = fopen(ls_tmpfile,"r");
	if(fd == NULL)  return -1 ;
	fgets(ls_buf,sizeof(ls_buf),fd);
	fclose(fd);
	p=ls_buf;
	p = stptok(p,ls_len,21,PROTO_DELIMITER);
	if(*p != PROTO_DELIMITER_CHAR) return FORMATERR;
	p = stptok(++p,ls_time,YEAR_TO_SEC_LEN,PROTO_DELIMITER);	
	
	return 0 ;
}
//busToServer用
int ProtoCheck(char *fullname,size_t len,char * ls_time,size_t pos,char * ls_retbuf)
{
int li_ret;
int fd ;
char ls_tmpfile[DEFAULT_TMPFILE_SIZE];
char ls_oldlen[21];
char ls_oldtime[YEAR_TO_SEC_LEN];
size_t old_len=0;

	sprintf(ls_tmpfile, "%s.   ",fullname);
	li_ret=ReadTmpFile(ls_tmpfile, ls_oldlen,ls_oldtime);
	if(!li_ret) sscanf(ls_oldlen,"%ld",&old_len);
	if(li_ret || strcmp(ls_oldtime,ls_time) || old_len != len) { 
//新传文件
		if(!li_ret) unlink(ls_tmpfile);
		pos=0;
		fd=open(fullname,O_CREAT|O_WRONLY|O_TRUNC,0644);
		if(fd<0 || (0>(li_ret = WriteTmpFile(ls_tmpfile,len,ls_time)))) {
			if(fd>=0) {
				sprintf(ls_retbuf,"%s:err=%d,%s",ls_tmpfile,errno,strerror(errno));
				close(fd);
			} else sprintf(ls_retbuf,"%s:err=%d,%s",fullname,errno,strerror(errno));
			return -1;
		}
		sprintf(ls_retbuf,"%s|%lu|%s|%lu|",fullname,len,ls_time,pos);
		return fd;
	}
		
	if(0>(fd=open(fullname,O_WRONLY|O_APPEND|O_CREAT,0644))) {
		sprintf(ls_retbuf,"%s:err=%d,%s",fullname,errno,strerror(errno));
		unlink(ls_tmpfile);
		return -1;
	}		
//断点续传
	pos=lseek(fd,0,SEEK_END);
ShowLog(5,"断点续传:pos=%ld",pos);
	sprintf(ls_retbuf,"%s|%ld|%s|%ld|",fullname,len,ls_time,pos);
	return fd;
}
	
int busToServer(connect,NetHead)
T_NetHead *NetHead;
T_Connect *connect;
{
char errbuf[512],buffer[SDBC_BLKSZ];
char tmp[DEFAULT_TMPFILE_SIZE];
char ls_time[21];
size_t len=0,pos=0;
int flg,i,count,e_node;
int fd;
int Event_no;

	e_node=LocalAddr(connect->Socket,0);
	Event_no=NetHead->PROTO_NUM;
	if(!NetHead->PKG_LEN) {
		sprintf(tmp,"Filename EMPTY!");
		NetHead->ERRNO1=EINVAL;
errret:
		NetHead->data=tmp;
		NetHead->PKG_LEN=strlen(NetHead->data);
		NetHead->PROTO_NUM=PutEvent(connect,Event_no);
		NetHead->ERRNO1=-1;
   		NetHead->ERRNO2=-1; 
	   	NetHead->PKG_REC_NUM=0;
   		NetHead->D_NODE=0;
   		NetHead->O_NODE=e_node;
		i=SendPack(connect,NetHead);
		ShowLog(1,"%s:%s",__FUNCTION__,NetHead->data);
		return 0;  
	}
ShowLog(5,"%s:data=%s",__FUNCTION__,NetHead->data);
	
	
	memset((void*)ls_time,0,sizeof(ls_time));
	
//  breakflag=NetHead->PKG_REC_NUM;
//	strncpy(buffer,NetHead->data,sizeof(buffer)-1);
	char * p ;
	p = stptok(NetHead->data,buffer,SDBC_BLKSZ,PROTO_DELIMITER);
	p = stptok(++p,tmp,sizeof(tmp),PROTO_DELIMITER);
	len=atoi(tmp);
	p = stptok(++p,ls_time,sizeof(ls_time),PROTO_DELIMITER);
	p = stptok(++p,tmp,sizeof(tmp),PROTO_DELIMITER);
	pos=atoi(tmp);
	
	if(*buffer=='$') 
	{
		if(buffer[1]=='/') strsubst(buffer+1,0,"RECVDIR");
		else if(buffer[1]=='@') strsubst(buffer+1,1,"HOME");
		else ;
	}
	if(!strncmp(buffer,"@/",2)) strsubst(buffer,1,"$HOME");
	substitute_env(buffer);
	fd=0;

	fd = ProtoCheck(buffer,len,ls_time,pos,tmp);
	if(fd == -1) {
		goto errret;
	}
		
	NetHead->PROTO_NUM=PutEvent(connect,Event_no);
		
	NetHead->ERRNO1=0;
	NetHead->ERRNO2=PACK_STATUS; 
  	NetHead->O_NODE=e_node;
	NetHead->D_NODE=0;
	NetHead->data=tmp;
	NetHead->PKG_LEN= strlen(NetHead->data);
	i=SendPack(connect,NetHead);
ShowLog(5,"%s:%s",__FUNCTION__,NetHead->data);
  
    
	count=1;
	do {
		i=RecvPack(connect,NetHead);
		if(i) {
			sprintf(tmp,"aft RecvPack,ret=%d,%s",i,buffer);
			NetHead->ERRNO1=errno;
			close(fd);
			goto errret;
		}
		if(NetHead->PKG_LEN) {
			i=write(fd,NetHead->data,NetHead->PKG_LEN);
			if(i<0) {
				flg=errno;
				NetHead->ERRNO1=errno;
				ShowLog(1,"%s:Write File:%s count=%d i=%d,errno:%d,%s",
					__FUNCTION__,
					buffer,count,i,errno,
					strerror_r(errno,errbuf,sizeof(errbuf)));
			}
			count+=i;
		}
	} while (NetHead->ERRNO2 == PACK_CONTINUE);
	if(NetHead->PROTO_NUM == 65535) return -1;
	pos=lseek(fd,0, SEEK_END);
	close(fd);
	stptok(tmp,buffer,sizeof(buffer), PROTO_DELIMITER);
	sprintf(errbuf,"%s.   ",buffer);
	unlink(errbuf);
	sprintf(tmp,"%s|%ld|%s|%ld|",buffer,len,ls_time,pos);
	
	NetHead->PROTO_NUM=PutEvent(connect,Event_no);
 	NetHead->O_NODE=e_node;
 	NetHead->D_NODE=0;
	NetHead->data=tmp;
 	NetHead->PKG_LEN=strlen(NetHead->data);
	NetHead->ERRNO2=0;
	NetHead->ERRNO1=0;
	i=SendPack(connect,NetHead);
  ShowLog(2,"%s:%s succes",__FUNCTION__,tmp);
  return 0;
}

int GetFileInfo(char * fname, size_t flen,char *ls_ftime,size_t *pos,char *buf)
{
int fd;
int li_ret ;
struct stat l_stat ;
char ftime[27];
int o_err=0;

	fd =  open(fname,O_RDONLY);
	if(fd ==-1) {
		o_err=-abs(errno);
		if(!o_err) o_err=-1;
		sprintf(buf,"open %s:err=%d,%s",fname,errno,strerror(errno));
		return o_err;
	}
	
	li_ret = fstat(fd, &l_stat);
	if(li_ret !=0) {
		o_err=-abs(errno);
		if(!o_err) o_err=-1;
		sprintf(buf,"fstat %s:err=%d,%s",fname,errno,strerror(errno));
		close(fd);
		return o_err;
	}
	
	strftime(ftime, sizeof(ftime), "%Y-%m-%d %H:%M:%S", localtime(&l_stat.st_mtime));
	if(flen != l_stat.st_size || strcmp(ftime,ls_ftime)) {
		*pos=0;
	} 
	lseek(fd,*pos,SEEK_SET);
	sprintf(buf,"%s|%lu|%s|%lu|",fname,l_stat.st_size,ftime,*pos);
	
	return fd ;
}

int serverToBus(connect,NetHead)
T_NetHead *NetHead;
T_Connect *connect;
{
char ls_flen[20];
char ls_ftime[20];
char ls_fpos[20];
char fname[1024],buffer[SDBC_BLKSZ];
char *p;
int i,e_node;
int fd;
int Event_no=NetHead->PROTO_NUM;
size_t pos,len;

	e_node=LocalAddr(connect->Socket,0);
	ShowLog(5,"%s:data=%s,errno2=%d",__FUNCTION__,NetHead->data,NetHead->ERRNO2);
	p=stptok(NetHead->data,fname,sizeof(fname),PROTO_DELIMITER);
	if(*p!='|') {
err1:
		sprintf(buffer,"data format error!");
		NetHead->ERRNO1=-1;
errret:
		NetHead->ERRNO2=-1;
		NetHead->data=buffer;
		NetHead->PKG_LEN=strlen(NetHead->data);
		NetHead->PROTO_NUM=PutEvent(connect,Event_no);
		NetHead->D_NODE=0;
		NetHead->O_NODE=e_node;
		NetHead->PKG_REC_NUM=0;
		i=SendPack(connect,NetHead);
		ShowLog(1,"%s:%s",__FUNCTION__,NetHead->data);
		return 0;
	}
	substitute_env(fname);
	ShowLog(5,"%s:%s,p=%s",__FUNCTION__,fname,p);
	p=stptok(++p,ls_flen,sizeof(ls_flen),PROTO_DELIMITER);
	if(*p!='|') goto err1;
	len=0;
	sscanf(ls_flen,"%lu",&len);
	p=stptok(++p,ls_ftime,sizeof(ls_ftime),PROTO_DELIMITER);
	if(*p!='|') goto err1;
	p=stptok(++p,ls_fpos,sizeof(ls_fpos),PROTO_DELIMITER);
	pos=0;
	sscanf(ls_fpos,"%lu",&pos);
	
	fd = GetFileInfo(fname, len,ls_ftime,&pos,buffer);
	if(fd<0) {
		NetHead->ERRNO1=fd;
		goto errret;
	} 

ShowLog(5,"%s:buffer=%s",__FUNCTION__,buffer);
	NetHead->PKG_REC_NUM=0;
	NetHead->ERRNO1=0; 
	NetHead->O_NODE=e_node;
	NetHead->D_NODE=0;	
	NetHead->PROTO_NUM=0;
	NetHead->data=buffer;
	NetHead->PKG_LEN=strlen(NetHead->data) ;
	NetHead->ERRNO2= PACK_CONTINUE;
	int li_ret = SendPack(connect,NetHead);	
	if(li_ret !=0) {
		ShowLog(1,"%s: SendPack ret=%d,err=%d,%s",__FUNCTION__,li_ret,
				errno,strerror(errno));
		close(fd);
		return -1 ;
	}
	
size_t r_len;
int num=0;
	do {
		*buffer=0;
		r_len=read(fd,buffer,sizeof(buffer));
		if(r_len<=0)break;

		if((++num %20)==0) sleep(1);

		NetHead->PROTO_NUM=0;
		NetHead->data=buffer;
		NetHead->PKG_LEN=r_len;
		NetHead->ERRNO1=0;
		if(r_len==sizeof(buffer))
			NetHead->ERRNO2=PACK_CONTINUE;
		else
			NetHead->ERRNO2=0;
		li_ret=SendPack(connect,NetHead);
		if(li_ret) {
			ShowLog(1,"%s: SendPack2 ret=%d,err=%d,%s",__FUNCTION__,li_ret,
				errno,strerror(errno));
			close(fd);
			return -1;
		}
	} while(r_len==sizeof(buffer));

	close(fd);
	if(NetHead->ERRNO2==PACK_CONTINUE) {
		NetHead->PROTO_NUM=PutEvent(connect,Event_no);
		NetHead->ERRNO1=0;
		NetHead->ERRNO2=0;
		NetHead->data=0;
		NetHead->PKG_LEN=0;
		i=SendPack(connect,NetHead);
	}
	ShowLog(2,"%s: %s success,len=%lu",__FUNCTION__,fname,len);   
	return 0;
}

