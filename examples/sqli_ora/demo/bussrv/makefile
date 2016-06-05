CFLAGS= -Wall -I$(SDBCDIR)/include -I$(MYSQLDIR)/include -O
LFLAG=-L$(SDBCDIR)/lib

all:bussrv  
#简单的通信服务器
#当含有family_old.o模块时，xcomd具有与xcom不同的内码，因此不能互相通信，必须通过midware转发 发 
bussrv:bussrv.o f_srv.o event_s.o chkexec.o newfiles.o
	cc -o $@ $^ $(LFLAG) -lsc -lpthread

bussrv.o getlist.o f_srv.o : xcomd.h $(SDBCDIR)/include/sc.h $(SDBCDIR)/include/scsrv.h $(SDBCDIR)/include/strproc.h

t_dcomp:t_dcomp.o
	cc -o $@ $^ $(LFLAG) -lsc -lpthread

t:t.o
	cc -o $@ $^ $(LFLAG) -lsc  -lpthread

install:bussrv
	mv $(HOME)/bin/bussrv $(HOME)/bin/bussrv_old
	mv bussrv $(HOME)/bin
