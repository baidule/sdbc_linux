CFLAGS= -m64 -w -g -I$(SDBCDIR)/include 
LIBS = -L$(ORACLE_HOME)/lib -L$(SDBCDIR)/lib 
LANG=C
#通用数据库加载/卸载程序 
#执行程序名为unldasc完成卸载功能。 
#ldasc完成加载功能。
all:ldasc cvtdate dblabel oad
ldasc:ldasc.o 
	cc -m64 -o ldasc ldasc.o $(LIBS) -ljson -lsqli -lscbase -lclntsh -lpthread
#时间转换程序 
cvtdate:cvtdate.o 
	cc -m64 -o cvtdate cvtdate.o $(LIBS) -lscbase -lpthread
#多线程数组加载器
oad:oad.o thread.o queue.o
	cc -m64 -o oad oad.o thread.o queue.o $(LIBS) -ljson -lsqli -lscbase -lclntsh -lpthread

oad.o thread.o queue.o:thread.h queue.h
#读数据库用户口令，单点登录
dblabel:dblabel.o 
	cc -m64 -o dblabel dblabel.o $(LIBS) -lscbase -lpthread

install:ldasc
	mv ~/bin/ldasc ~/bin/ldasc_old
	mv ldasc ~/bin

