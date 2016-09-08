AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib
DB_LIB_DIR		= $(top_srcdir)/../../database_lib
PSQL_INCLUDE	= /usr/local/pgsql/include
PSQL_LIB		= /usr/local/pgsql/lib
PSQL_HOME_INCLUDE = /home/postgresql/include
PSQL_HOME_LIB = /home/postgresql/lib
REDIS_INCLUDE	= /usr/local/include/hiredis
REDIS_LIB		= /usr/local/lib
MYSQL_INCLUDE	= /usr/include/mysql
MYSQL_LIB		= /usr/lib64/mysql

XML_INCLUDE_DIR = /usr/include/libxml2


INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(DB_LIB_DIR)/include -I$(PSQL_INCLUDE) -I$(REDIS_INCLUDE) -I$(PSQL_HOME_INCLUDE) -I$(XML_INCLUDE_DIR) -I$(MYSQL_INCLUDE)
noinst_PROGRAMS = store_proc

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W -Werror  -Wno-unused-parameter -fsigned-char 
CFLAGS          = $(DEBUG_FLAGS)

store_proc_LDADD        = -L$(DB_LIB_DIR)/lib  -L$(PSQL_LIB) -L$(REDIS_LIB) -L$(PSQL_HOME_LIB) -L$(MYSQL_LIB) -lcpsql -lpq -lcredis -lhiredis -L$(COMMON_LIB_DIR)/lib -ladt -lconfig -ltrace -lcommon -llog -lxml2 -lpthread -lmysqlclient #-lz -ldl -lsignal  -ltimer  -laes
store_proc_SOURCES = \
	     $(top_srcdir)/src/store_main.c	\
	     $(top_srcdir)/src/store_3rd.c	\
	     $(top_srcdir)/src/store_attack.c	\
	     $(top_srcdir)/src/store_av.c	\
	     $(top_srcdir)/src/store_abb.c	\
	     $(top_srcdir)/src/store_black.c \
	     $(top_srcdir)/src/store_dev_status.c \
	     $(top_srcdir)/src/store_cfg.c	\
	     $(top_srcdir)/src/store_file_op.c	\
	     $(top_srcdir)/src/store_db_op.c	\
	     $(top_srcdir)/src/store_dirwalk.c


