AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib
DB_LIB_DIR              = $(top_srcdir)/../../database_lib
PSQL_INCLUDE    = /usr/local/pgsql/include
PSQL_LIB                = /usr/local/pgsql/lib
PSQL_HOME_INCLUDE = /home/postgresql/include
PSQL_HOME_LIB	= /home/postgresql/lib
XML2_INCLUDE_DIR = /usr/include/libxml2

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(DB_LIB_DIR)/include -I$(PSQL_INCLUDE) -I$(PSQL_HOME_INCLUDE) -I$(XML2_INCLUDE_DIR)
noinst_PROGRAMS = sys_guard

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char   -Werror -Wall
CFLAGS          = $(DEBUG_FLAGS)

sys_guard_LDADD        =  -L$(DB_LIB_DIR)/lib -L$(PSQL_LIB) -L$(PSQL_HOME_LIB) -lcpsql -lpq  -L$(COMMON_LIB_DIR)/lib -lsignal -ladt -ltimer -laes -lconfig  -llog -lxml2#-lpthread -lz -ldl 
sys_guard_SOURCES = \
	     $(top_srcdir)/src/sys_guard_main.c     \
             $(top_srcdir)/src/read.c     \
             $(top_srcdir)/src/sysstatus_file.c     \
             $(top_srcdir)/src/regular_guard.c     \
             $(top_srcdir)/src/file_op.c     \
             $(top_srcdir)/src/guard_cfg.c     \
             $(top_srcdir)/src/process_src.c     \
             $(top_srcdir)/src/parse.c     \
             $(top_srcdir)/src/service_class.c     
 
		
		
