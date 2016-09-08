AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib
DB_LIB_DIR		= $(top_srcdir)/../../database_lib
PSQL_INCLUDE	= /usr/local/pgsql/include
PSQL_LIB		= /usr/local/pgsql/lib
PSQL_HOME_INCLUDE = /home/postgresql/include
PSQL_HOME_LIB = /home/postgresql/lib
STORE_PROC_DIR	= $(top_srcdir)/../store_proc
LOCAL_DIR       = /usr/local/
XML_INCLUDE_DIR = /usr/include/libxml2
APT_LIB         =../../../../public_libs/third_party_engines/apt/online/bin


INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(DB_LIB_DIR)/include -I$(PSQL_INCLUDE) -I$(STORE_PROC_DIR)/include -I$(PSQL_HOME_INCLUDE) -I$(LOCAL_DIR)/include -I$(XML_INCLUDE_DIR)
noinst_PROGRAMS = flow_proc_apt

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W -Werror -Wunused -Wno-unused-parameter -fsigned-char 
CFLAGS          = $(DEBUG_FLAGS)

flow_proc_apt_LDADD        = -L$(DB_LIB_DIR)/lib -L$(PSQL_LIB) -L$(PSQL_HOME_LIB) -L$(LOCAL_DIR)/lib -L$(APT_LIB) -lcpsql -lpq -lcredis -lhiredis -L$(COMMON_LIB_DIR)/lib -ladt -lconfig -ltrace -lcommon -lflowutil -lflowcore -lflowbag-sdpart2 -lflowdb2 -lflowsql -llog -lxml2 -lpthread #-lz -ldl -lsignal  -ltimer  -laes
flow_proc_apt_SOURCES = \
	     $(top_srcdir)/src/flow_main.c \
	     $(top_srcdir)/src/flow_cfg.c \
	     $(top_srcdir)/src/flow_store.c \
	     $(top_srcdir)/src/flow_statistics.c \
		 $(STORE_PROC_DIR)/src/store_dirwalk.c \
		 $(STORE_PROC_DIR)/src/store_file_op.c 
