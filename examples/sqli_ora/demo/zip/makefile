CFLAGS= -m64 -I$(SDBCDIR)/include -O3
LFLAGS=-L$(SDBCDIR)/lib -lsc -lpthread

tzip:tzip.o memzip.o
	cc -m64 -o tzip tzip.o memzip.o $(LFLAGS)

#compress_file.c  decompress_file.c  quicklz.c  stream_compress.c  stream_decompress.c

compress_file:compress_file.o quicklz.o
	cc -m64 -o compress_file compress_file.o quicklz.o $(LFLAGS)
