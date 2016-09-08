AUTOMAKE_OPTIONS = foreign

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
#APT_LIB         =../../../../public_libs/third_party_engines/apt/online/bin


INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(DB_LIB_DIR)/include -I$(PSQL_INCLUDE) -I$(STORE_PROC_DIR)/include -I$(PSQL_HOME_INCLUDE) -I$(LOCAL_DIR)/include -I$(XML_INCLUDE_DIR)
noinst_PROGRAMS = flow_proc

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W -Werror -Wunused -Wno-unused-parameter -fsigned-char 
CFLAGS          = -g

flow_proc_LDADD        = -L$(DB_LIB_DIR)/lib -L$(PSQL_LIB) -L$(PSQL_HOME_LIB) -L$(LOCAL_DIR)/lib  -lcpsql -lpq -lcredis -lhiredis -L$(COMMON_LIB_DIR)/lib -ladt -lconfig -ltrace -lcommon  -llog -lxml2 -lpthread #-lz -ldl -lsignal  -ltimer  -laes
flow_proc_SOURCES = \
	     $(top_srcdir)/src/flow_main.c \
	     $(top_srcdir)/src/flow_sock.c \
	     $(top_srcdir)/src/flow_store.c \
		 $(top_srcdir)/src/flow_cfg.c \
		 $(top_srcdir)/src/flow_statistics.c \
		 $(top_srcdir)/src/flow_event_sock.c \
		 $(top_srcdir)/src/flow_common.c \
		 $(top_srcdir)/src/monitor_ip_list.c \
		 $(top_srcdir)/src/socket_rev.c \
		 $(top_srcdir)/src/ddos_trie.c \
		 $(top_srcdir)/src/net_conn.c \
		 $(top_srcdir)/src/flow_event_store.c \
		 $(STORE_PROC_DIR)/src/store_dirwalk.c  \
		 $(STORE_PROC_DIR)/src/store_file_op.c \
		 $(top_srcdir)/src/rwlock.c \
		 $(top_srcdir)/src/thunk.S \
		 $(top_srcdir)/src/gms_udp_sock.c
	
all:flow_conf
	$(CC) $(INCLUDES)  $(flow_proc_SOURCES) -o $(noinst_PROGRAMS) $(flow_proc_LDADD)
flow_conf:
	$(CC) $(top_srcdir)/src/socket_cli.c -o flow_config
_install:all
	mkdir -p ../../gms/storage/ ../../gms/storage/conf/ ../../gms/conf/ ../../gms/bin
	cp -a flow_proc  ../../gms/storage/
	cp -a flow_proc  ../../gms/storage/flow_proc_tcad
	cp -a conf/flow_trace.conf ../../gms/storage/conf/
	cp -a conf/flow_time.conf ../../gms/storage/conf/
	cp -a conf/gms_udp_sock.conf ../../gms/storage/conf/
	cp -a conf/custom_flow.conf ../../gms/conf/
	cp -a flow_config ../../gms/bin/
clean:
	rm -rf flow_proc
	
