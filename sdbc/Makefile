
CFLAGS = -g -Wall -fPIC -I$(SDBCDIR)/include 
CC=cc

OBJS =  sdbc.o locktab.o sqlo_type.o mod_DB.o bind_DB.o dui.o pageselect.o get_tpl.o

lib:$(SDBCDIR)/lib/libsdbc.a
$(SDBCDIR)/lib/libsdbc.a:$(OBJS)
	ar -rv $(SDBCDIR)/lib/libsdbc.a $?

$(OBJS):$(SDBCDIR)/include/sdbc.h

