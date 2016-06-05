
SRVCOMM=$(SDBCDIR)/utility/srvcomm
CFLAGS= -Wall -g -I$(SDBCDIR)/include  -I$(SRVCOMM)
LFLAGS= -m64 -L$(SDBCDIR)/lib -L$(ORACLE_HOME)/lib
CC=gcc

#通用多线程带连接池SQL服务器 
SRVOBJS= login.o ctx_login.o tpc_svc.o \
	$(SRVCOMM)/net_showlog.o $(SRVCOMM)/newfiles.o $(SRVCOMM)/ctxlib.o

#usage: tpcsrv sql.cfg &
tpcsrv:tpcsrv.o $(SRVOBJS)
	$(CC) -o tpcsrv tpcsrv.o $(SRVOBJS) $(LFLAGS) -lsdbc -lsqli -lsc -lscbase -ljson -lclntsh -lpthread 
tpcsrv.o $(SRVOBJS):tpcsrv.h userfunc.c

install:tpcsrv
	-mv $(HOME)/bin/tpcsrv $(HOME)/bin/tpcsrv.old
	mv tpcsrv $(HOME)/bin
