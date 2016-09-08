AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib
OPENSSL_INCLUDE_DIR = /usr/local/ssl
XML2_INCLUDE_DIR = /usr/include/libxml2

#AUTH_COM_DIR    = $(top_srcdir)/../authtool

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(OPENSSL_INCLUDE_DIR)/include -I$(XML2_INCLUDE_DIR)/ #-I$(AUTH_COM_DIR)/include 
noinst_PROGRAMS = logmon

COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char -Werror -Wall 
CFLAGS          = $(DEBUG_FLAGS)

logmon_LDADD        =   -L$(COMMON_LIB_DIR)/lib -lsignal -ladt -ltimer -lauth -lconfig -laes  -llog -lssl -lxml2#-lpthread -lz -ldl 

#libauthcheck_so_LDADD   =   -L$(COMMON_LIB_DIR)/lib -lsignal -ladt -ltimer -lauth -lconfig -laes -lssl #-lpthread -lz -ldl 

logmon_SOURCES = \
	     $(top_srcdir)/src/logmon_main.c     \
	     $(top_srcdir)/src/time.c     \
	     $(top_srcdir)/src/status.c     \
	     $(top_srcdir)/src/get_sig.c         \
	     $(top_srcdir)/src/authfile_info.c    \
	     $(top_srcdir)/src/compare_id.c   \
	     $(top_srcdir)/src/parse.c   \
	     $(top_srcdir)/src/cap_des.c   \
	     $(top_srcdir)/src/auth_cfg.c   \
	     $(top_srcdir)/src/decrypt.c     \
	     #$(top_srcdir)/src/md5.c   \
	     #$(top_srcdir)/src/make_serial.c
