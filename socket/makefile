
LIBOBJS= freevar.o gethostaddr.o net_pkg.o AIO_tcp.o tcpopen.o 

$(LIBDIR)/libsc.a:$(LIBOBJS)
	ar -rv $(LIBDIR)/libsc.a $?
$(LIBOBJS):$(INCDIR)/scsrv.h $(INCDIR)/sc.h $(INCDIR)/strproc.h

