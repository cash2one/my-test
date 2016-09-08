AUTOMAKE_OPTIONS = foreign

CC  = gcc

XML_INCLUDE_DIR = /usr/include/libxml2

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(XML_INCLUDE_DIR) 
noinst_PROGRAMS = term

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W -Werror -Wunused -Wno-unused-parameter -fsigned-char 
CFLAGS          = $(DEBUG_FLAGS)
term_LDADD        =  -lxml2 -lpthread -lcrypt

term_SOURCES = \
	     $(top_srcdir)/src/term_main.c	\
	     $(top_srcdir)/src/term_ttyrd.c	\
	     $(top_srcdir)/src/term_ip.c	\
	     $(top_srcdir)/src/term_config.c	\
	     $(top_srcdir)/src/term_login.c	\
	     $(top_srcdir)/src/port.c \
	     $(top_srcdir)/src/term_ftp.c 


