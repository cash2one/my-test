CC  = gcc
top_srcdir=`pwd`
COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib
DB_LIB_DIR		= $(top_srcdir)/../../database_lib
PSQL_INCLUDE	= /usr/local/pgsql/include
PSQL_LIB		= /usr/local/pgsql/lib
PSQL_HOME_INCLUDE = /home/postgresql/include
PSQL_HOME_LIB = /home/postgresql/lib
STORE_PROC_DIR	= $(top_srcdir)/../store_proc
LOCAL_DIR       = /usr/local/
XML_INCLUDE_DIR = /usr/include/libxml2


INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(DB_LIB_DIR)/include -I$(PSQL_INCLUDE) -I$(STORE_PROC_DIR)/include -I$(PSQL_HOME_INCLUDE) -I$(LOCAL_DIR)/include -I$(XML_INCLUDE_DIR)
noinst_PROGRAMS = flow_def

DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W -Werror -Wunused -Wno-unused-parameter -fsigned-char 
CFLAGS          = -g

flow_proc_LDADD        = -L$(DB_LIB_DIR)/lib -L$(PSQL_LIB) -L$(PSQL_HOME_LIB) -L$(LOCAL_DIR)/lib  -lcpsql -lpq -lcredis -lhiredis -L$(COMMON_LIB_DIR)/lib -ladt -lconfig -ltrace -lcommon  -llog -lxml2 -lpthread #-lz -ldl -lsignal  -ltimer  -laes
flow_def_SOURCES = \
		$(top_srcdir)/src/flow_def_main.c \
		$(top_srcdir)/src/hex.c \
		$(top_srcdir)/src/xmlread.c  

all:
	$(CC) $(INCLUDES)  $(flow_def_SOURCES) -o $(noinst_PROGRAMS) $(flow_proc_LDADD)
_install:all
	mkdir -p ../../gms/storage/ ../../gms/storage/conf/ ../../gms/conf/
	cp -a conf/*.conf   ../../gms/conf/
	cp -a $(noinst_PROGRAMS)  ../../gms/storage/
clean:
	rm -rf $(noinst_PROGRAMS)
