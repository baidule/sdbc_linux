CFLAGS=-g -I$(SDBCDIR)/include

tproc:tproc.o
	cc -o tproc tproc.o -L$(SDBCDIR)/lib -L$(ORACLE_HOME)/lib -lsqli -lscbase -lclntsh -lpthread
