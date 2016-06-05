
SRVCOMM=$(SDBCDIR)/utility/srvcomm
CFLAGS= -Wall -g -I$(SDBCDIR)/include -I$(SRVCOMM) -I../tpc
LFLAGS= -L$(SDBCDIR)/lib -L$(ORACLE_HOME)/lib 

#通用多线程带连接池SQL服务器 
SRVOBJS= set_showid.o ctx_login.o login.o tpool_svc.o \
	$(SRVCOMM)/ctxlib.o $(SRVCOMM)/newfiles.o $(SRVCOMM)/net_showlog.o

#usage: tpoolsrv sql.cfg &
tpoolsrv:tpoolsrv.o $(SRVOBJS)
	gcc -o tpoolsrv tpoolsrv.o $(SRVOBJS) $(SDBCDIR)/lib/AIO_fd.o $(LFLAGS) -lsdbc -lsqli -lsc -lscbase -lclntsh -ljson -lpthread -lrt /lib64/libaio.so.1
tpoolsrv.o $(SRVOBJS):tpoolsrv.h ../tpc/server.h $(SRVCOMM)/ctxlib.h
tpoolsrv.o:userfunc.c

install:tpoolsrv
	-mv $(HOME)/bin/tpoolsrv $(HOME)/bin/tpoolsrv.old
	mv tpoolsrv $(HOME)/bin
