AUTOMAKE_OPTIONS = foreign

CC  = gcc
top_srcdir = `pwd`
COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib
DB_LIB_DIR		= $(top_srcdir)/../../database_lib
PSQL_INCLUDE	= /usr/local/pgsql/include
PSQL_LIB		= /usr/local/pgsql/lib
PSQL_HOME_INCLUDE = /home/postgresql/include
PSQL_HOME_LIB = /home/postgresql/lib


INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(DB_LIB_DIR)/include -I$(PSQL_INCLUDE) -I$(PSQL_HOME_INCLUDE)
noinst_PROGRAMS = merge_proc

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W -Werror -Wunused -Wno-unused-parameter -fsigned-char 
CFLAGS          = $(DEBUG_FLAGS)

merge_proc_LDADD        = -L$(DB_LIB_DIR)/lib -L$(PSQL_LIB) -L$(PSQL_HOME_LIB) -L$(COMMON_LIB_DIR)/lib/ -lcpsql -lpq -lcredis -lhiredis -ladt -ltrace -lconfig -lcommon -llog -lpthread #-lz -ldl   -lsignal -ltimer  -laes
merge_proc_SOURCES = \
	     $(top_srcdir)/src/merge_main.c	\
	     $(top_srcdir)/src/merge_attack.c \
	     $(top_srcdir)/src/merge_flow.c \
	     $(top_srcdir)/src/merge_time_op.c \
	     $(top_srcdir)/src/merge_db_op.c \
	     $(top_srcdir)/src/merge_user.c \
	     $(top_srcdir)/src/merge_cfg.c
all:
	$(CC) $(INCLUDES) -o merge_proc $(merge_proc_SOURCES) $(merge_proc_LDADD) $(CFLAGS)
_install:all
	mkdir -p ../../gms/storage/ ../../gms/storage/conf
	cp -a merge_proc ../../gms/storage/
	cp -a conf/*.conf ../../gms/storage/conf/
clean:
	rm -rf merge_proc
