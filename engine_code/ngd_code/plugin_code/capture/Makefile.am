AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../../common_lib

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(top_srcdir)/../../include
sbin_PROGRAMS = libcaptor.so

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
COMMON_FLAGS    = -march=opteron -m64 -O2 -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 

#libcaptor_so_CFLAGS = $(COMMON_FLAGS) -Bsymbolic 
libcaptor_so_CFLAGS = $(COMMON_FLAGS) -Bsymbolic -fPIC
libcaptor_so_LDFLAGS = -fPIC -shared
#libcaptor_so_LDFLAGS = -shared
#libcaptor_so_LDADD = -L$(COMMON_LIB_DIR)/lib 

libcaptor_so_SOURCES = \
	     $(top_srcdir)/src/captor.c \
	     $(top_srcdir)/src/cap_record.c	\
	     $(top_srcdir)/src/cap_file_captor.c	\
	     $(top_srcdir)/src/raw_socket_captor.c	\
	     $(top_srcdir)/src/tcpdump_file_captor.c	\
	     $(top_srcdir)/src/tcpdump_filelist_captor.c	\
	     $(top_srcdir)/src/pcap_lib_captor.c	\
	     $(top_srcdir)/src/shuguang_drv_captor.c	\
	     $(top_srcdir)/src/special_drv_captor.c	\
	     $(top_srcdir)/src/task_queue_captor.c		
