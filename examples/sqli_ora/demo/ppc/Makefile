SRVCOMM=$(SDBCDIR)/utility/srvcomm
CFLAGS= -w -m64 -I$(SDBCDIR)/include -I$(SRVCOMM) -I../tpc
LFLAGS= -L$(SDBCDIR)/lib -L$(ORACLE_HOME)/lib 

all:sqlsrv sqlcli
#通用SQL服务器 
SRVOBJS= login.o $(SRVCOMM)/net_showlog.o price.o price_svc.o

sqlsrv:sqlsrv.o $(SRVOBJS)
	cc -m64 -s -o sqlsrv sqlsrv.o $(SRVOBJS) $(LFLAGS) -lsdbc -lsqli -lsc -lscbase -ljson  -lclntsh -lpthread -lnsl 
sqlsrv.o $(SRVUBJS):sqlsrv.h
sqlsrv.o:userfunc.c

#测试用客户端 
CLIOBJS=clilogin.o testpage.o testselect.o price_cli.o test_status.o

sqlcli:sqlcli.o $(CLIOBJS)
	cc -m64 -s -o sqlcli sqlcli.o $(CLIOBJS) $(LFLAGS) -lsc -lscbase -ljson -lpthread -lnsl
sqlcli.o $(CLIOBJS):sqlcli.h

statas:statas.o
	cc -o statas statas.o

install:sqlsrv
	-mv ~/bin/sqlsrv ~/bin/sqlsrv.old
	mv sqlsrv ~/bin
