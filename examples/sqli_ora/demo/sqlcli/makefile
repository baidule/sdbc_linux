
SRVCOMM=$(SDBCDIR)/utility/srvcomm
CFLAGS= -Wall -g -I$(SDBCDIR)/include  -I$(SRVCOMM)
LFLAGS= -L$(SDBCDIR)/lib 

#≤‚ ‘”√øÕªß∂À 
CLIOBJS=  testselect.o test_status.o

sqlcli:sqlcli.o $(CLIOBJS)
	cc -s -o sqlcli sqlcli.o $(CLIOBJS) $(LFLAGS) -lsc -lscbase -ljson -lpthread -lrt
sqlcli.o $(CLIOBJS):sqlcli.h

