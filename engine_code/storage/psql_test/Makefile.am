AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib
DB_LIB_DIR		= $(top_srcdir)/../../database_lib
PSQL_INCLUDE	= /usr/local/pgsql/include
PSQL_LIB		= /usr/local/pgsql/lib


INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(DB_LIB_DIR)/include -I$(PSQL_INCLUDE)
noinst_PROGRAMS = db_test

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char  
CFLAGS          = $(DEBUG_FLAGS)

db_test_LDADD        =-L$(COMMON_LIB_DIR)/lib -L$(DB_LIB_DIR)/lib -L$(PSQL_LIB) -lcpsql -lpq -llog#-lpthread -lz -ldl   -L$(COMMON_LIB_DIR)/lib -lsignal -ladt -ltimer -lconfig -laes
#db_test_SOURCES = \
#	     $(top_srcdir)/src/test_main.c	\
#	     $(top_srcdir)/src/test_db_helper.c	\
#	     $(top_srcdir)/src/store_common.c	
db_test_SOURCES = \
	     $(top_srcdir)/src/test_main.c	
