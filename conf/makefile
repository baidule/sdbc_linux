
CFLAGS:= $(CFLAGS) -I$(INCDIR)/ldap

LIBOBJS= envcfg.o isdir.o showlog.o ldap_auth.o openauth.o decode.o encryptproc.o 

$(LIBDIR)/libscbase.a:$(LIBOBJS)
	ar -rv $(LIBDIR)/libscbase.a $?
$(LIBOBJS):$(INCDIR)/strproc.h

