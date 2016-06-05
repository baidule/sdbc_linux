
INCLTDL=-I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public
SHELL = /bin/sh
srcdir = .
CC = cc
CPP = cc -E
ECHO = echo
LEX = flex
LFLAGS =  -i
LIBADD_CRYPT = -lcrypt
LIBTOOL = $(SHELL) ../libtool
OBJEXT = o
RANLIB = ranlib
YACC = bison -y
INCLUDES = -I$(SDBCDIR)/include $(INCLTDL)



USERLIB=$(ORACLE_HOME)/lib/

LDFLAGS=-L$(LIBHOME) -L$(RDBMSLIB)
LLIBPSO=`cat $(ORACLE_HOME)/rdbms/lib/psoliblist`

# libraries for linking oci programs
OCISHAREDLIBS=$(TTLIBS) $(LLIBTHREAD)
#OCICALLBACK=$(RDBMSLIB)kpucb.o
OCISTATICLIBS=$(DEF_OPT) $(OCICALLBACK) $(NAETTOBJS) $(STATICTTLIBS)

PSOLIBLIST=$(ORACLE_HOME)/rdbms/lib/psoliblist
CLEANPSO=rm -f $(PSOLIBLIST); $(GENPSOLIB)


.SUFFIXES: .o
SHARED_LDFLAG=$(LDFLAGS) -shared -o



CFLAGS = -m64 -w -g -fPIC
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --mode=compile $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --mode=link $(CCLD) $(LDFLAGS) -o $@
DIST_COMMON =  ""


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best


.SUFFIXES:
.SUFFIXES: .S .c .lo .o .obj .s

.c.o:
	$(COMPILE) -c $<

.c.obj:
	$(COMPILE) -c `cygpath -w $<`

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<


.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.s.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

.S.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) -c $<

LIBOBJS= sql.o select.o ora_rpc.o insert_db.o pool.o \
	dbopen.o transation.o sqlora.o
$(SDBCDIR)/lib/libsqli.a:$(LIBOBJS)
	ar -rv $(SDBCDIR)/lib/libsqli.a $?
$(LIBOBJS):$(SDBCDIR)/include/sqli.h $(SDBCDIR)/include/pack.h $(SDBCDIR)/include/strproc.h
sdbc.o sqlo_type.o: $(SDBCDIR)/include/sdbc.h
sqlora.o: config.h sqlora_add.c

#tauth: tauth.o
#	cc -o tauth tauth.o -L$(SDBCDIR)/lib  -lldap -llber -lsqli -lscry -lstr -lsqlora8 
