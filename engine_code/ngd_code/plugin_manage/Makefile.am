AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/../include -I$(COMMON_LIB_DIR)/include -I$(top_srcdir)/../../include
noinst_LIBRARIES= libplugin_manage.a

COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char   
libplugin_manage_a_CFLAGS  = $(COMMON_FLAGS)

libplugin_manage_a_LIBADD = $(COMMON_LIB_DIR)/lib/libadt.a  
#libplugin_manage_a_LIBADD = -L$(COMMON_LIB_DIR)/lib -lsignal -ladt -ltimer -lconfig -laes
libplugin_manage_a_SOURCES = \
	     $(top_srcdir)/plugin.c	
