AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include 
noinst_PROGRAMS = genkey

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char   -Werror -Wall
CFLAGS          = $(DEBUG_FLAGS)

genkey_LDADD        =   -L$(COMMON_LIB_DIR)/lib -lsignal -ladt -ltimer -laes -lauth -lconfig #-lpthread -lz -ldl 
genkey_SOURCES = \
	     $(top_srcdir)/src/genkey_main.c     
		
		
