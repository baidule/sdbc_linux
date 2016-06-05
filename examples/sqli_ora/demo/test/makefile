CFLAGS= -m64 -g -I$(SDBCDIR)/include

LFLAGS=-L$(SDBCDIR)/lib
t_time:t_time.o
	cc -m64 -o t_time t_time.o $(LFLAGS) -lscbase -lpthread
t:t.o
	cc -m64 -o t t.o $(LFLAGS) -lscbase -lpthread
