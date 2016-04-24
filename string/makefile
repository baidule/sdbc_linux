
CFLAGS:= $(CFLAGS) -I/usr/local/include -O1 

LIBOBJS= basename.o  dateconvert.o  datejul.o  fround.o  skipblk.o  strproc.o  sysjul.o  trim.o \
		 strapp.o strrev.o strtcpy.o itoa.o trimall.o stctok.o GBK_flag.o

$(LIBDIR)/libscbase.a:$(LIBOBJS)
	ar -rv $(LIBDIR)/libscbase.a $?
$(LIBOBJS):$(INCDIR)/strproc.h
datejul.o dateconvert.o:$(INCDIR)/datejul.h

