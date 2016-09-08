AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(top_srcdir)/../include
noinst_PROGRAMS = ngd

COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char   
CFLAGS          = $(DEBUG_FLAGS)

ngd_LDADD       = -lpthread -lz -ldl   -L$(COMMON_LIB_DIR)/lib -lsignal -ladt -ltimer -lconfig -laes
ngd_SOURCES = \
	     $(top_srcdir)/data_frame/ngd_conf.c	\
	     $(top_srcdir)/data_frame/ngd_init.c	\
	     $(top_srcdir)/data_frame/ngd_main.c	\
	     $(top_srcdir)/plugin_manage/plugin.c	\
	     $(top_srcdir)/data_frame/thread.c		\
	     $(top_srcdir)/data_frame/argv.c
