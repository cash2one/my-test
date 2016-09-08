AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../../common_lib

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(top_srcdir)/../../include
sbin_PROGRAMS = libether_decode.so libsepcial_eth_decode.so

#COMMON_FLAGS    =-march=opteron -m64  -O2 -rdynamic  -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 
COMMON_FLAGS    = -march=opteron -m64 -O2 -finline-functions -W  -Wunused -Wno-unused-parameter -fsigned-char 

#libcaptor_so_CFLAGS = $(COMMON_FLAGS) -Bsymbolic 
libcaptor_so_CFLAGS = $(COMMON_FLAGS) -Bsymbolic -fPIC
libether_decode_so_LDFLAGS = -fPIC -shared
libsepcial_eth_decode_so_LDFLAGS = -fPIC -shared

#libcaptor_so_LDFLAGS = -shared
#libcaptor_so_LDADD = -L$(COMMON_LIB_DIR)/lib 

libether_decode_so_SOURCES = \
	    $(top_srcdir)/src/dfp_ether_decode.c 



libsepcial_eth_decode_so_SOURCES = \
	    $(top_srcdir)/src/dfp_special_ether_decode.c 
