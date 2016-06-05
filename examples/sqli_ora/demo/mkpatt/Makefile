CFLAGS = -O3 -m64 -w -I$(SDBCDIR)/include
LIBS = -L$(SDBCDIR)/lib -L$(ORACLE_HOME)/lib

all:mkpatt mktpl

mkpatt:mkpatt.o prt_tpl.o
	cc -m64 -o mkpatt mkpatt.o prt_tpl.o $(LIBS) -lsqli -lscbase -lclntsh -lpthread

mktpl:mktpl.o meta_tpl.o prt_tpl.o
	cc -m64 -o mktpl mktpl.o meta_tpl.o prt_tpl.o $(LIBS) -lscbase -lpthread

mkpatt mktpl:prt_tpl.h

t_struct:t_struct.o
	cc -m64 -o t_struct t_struct.o $(LIBS) -lscbase -ljson
