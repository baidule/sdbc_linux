#######################################
## file: Makefile
## author: weihua.liu (lwhjava@vip.qq.com)
## date: 2016/04/23
#######################################
SDBCDIR=$(shell pwd)

CC=gcc
SUBDIRS=string conf ds crypto socket sccli scsrv pack dau sdbc
INCDIR=$(SDBCDIR)/include
LIBDIR=$(SDBCDIR)/lib
CFLAGS= -m64 -w -fPIC -I$(INCDIR) -I$(INCDIR)/ldap -g

export CC SUBDIRS SDBCDIR LIBDIR INCDIR CFLAGS

all:CHECK_DIR $(SUBDIRS)

CHECK_DIR:
	mkdir -p $(LIBDIR)
$(SUBDIRS):ECHO
	make -C $@
ECHO:
	@echo $(SUBDIRS)
	@echo begin compile
clean:
	@for subdir in $(SUBDIRS); \
	do $(RM) $$subdir/*.o; \
	done
	@rm -rf $(LIBDIR)/*
