
CFLAGS= -w -m64 -I. -O3

LIBOBJS=  crc32.o enigma.o enigma2.o 

libenigma.a:$(LIBOBJS)
	ar -rv libenigma.a $?

enigma.o enigma2.o : enigma.h
crc32.o:crc32.h

#enigmaº”√‹≤‚ ‘ 
teni: teni.o libenigma.a
	cc -m64 -o teni teni.o -L. -lenigma 

#enigma2º”√‹≤‚ ‘ 
teni2: teni2.o libenigma.a
	cc -m64 -o teni2 teni2.o -L. -lenigma 

teni3: teni3.o libenigma.a
	cc -m64 -o teni3 teni3.o -L. -lenigma 

