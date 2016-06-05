
CFLAGS = -m64 -O3 -Wall -fPIC -I$(SDBCDIR)/include 
CC=cc

DAUOBJS =  DAU.o bind.o PK_dao.o getsrm.o SRM_to_DAU.o returning.o OAD.o BULK.o DAU_mk.o


lib:$(SDBCDIR)/lib/libsqli.a
$(SDBCDIR)/lib/libsqli.a:$(DAUOBJS)
	ar -rv $(SDBCDIR)/lib/libsqli.a $?

$(DAUOBJS):$(SDBCDIR)/include/DAU.h $(SDBCDIR)/include/SRM.h  
OAD.o:$(SDBCDIR)/include/OAD.h

getsrm.o DAU.o:getsrm.h descDAO.c

#≤‚ ‘¥¯RETURNINGµƒ≤Â»Î”Ôæ‰ 
t_ret:t_ret.o
	cc -s -o t_ret t_ret.o -L$(SDBCDIR)/lib -lsqli -lscry -lstr -lsqlora8
