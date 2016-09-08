AUTOMAKE_OPTIONS = foreign

CC  = gcc
COMMON_LIB_DIR= $(top_srcdir)/../../common_lib
DATABASE_LIB_DIR= $(top_srcdir)/../../database_lib
FLOW_PROC_DIR= $(top_srcdir)/../../storage/flow_proc
APT_LIB=../../../../public_libs/third_party_engines/apt/online/bin

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(FLOW_PROC_DIR)/include -I$(COMMON_LIB_DIR)/include -I$(DATABASE_LIB_DIR)/include
noinst_PROGRAMS = query_flow

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W -Werror -Wunused -Wno-unused-parameter -fsigned-char 
CFLAGS          = $(DEBUG_FLAGS)

query_flow_LDADD        = -L$(COMMON_LIB_DIR)/lib -L$(DATABASE_LIB_DIR)/lib -L$(APT_LIB) -ladt -lconfig -ltrace -lcommon -lflowutil -lflowcore -lflowbag-sdpart2 -lflowsql -lflowdb2 -lxml2 -llog -lpthread  -lcpsql
query_flow_SOURCES = \
	     $(top_srcdir)/src/query_main.c \
	     $(top_srcdir)/src/query_config.c \
	     $(top_srcdir)/src/query_entry.c


