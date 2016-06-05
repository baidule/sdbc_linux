CFLAGS= -m64 -g -I$(SDBCDIR)/include

LFLAGS=-L$(SDBCDIR)/lib
itoa:itoa.o
	cc -m64 -o itoa itoa.o $(LFLAGS) -lscbase  -lpthread

t:t.o
	cc -m64 -o t t.o $(LFLAGS) -lscbase  -lpthread


