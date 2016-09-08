AUTOMAKE_OPTIONS = foreign

COMMON_LIB_DIR=$(top_srcdir)/../../../common_lib

CC			 = gcc
INCLUDES 	 = -I$(top_srcdir) -I$(top_srcdir)/include  -I$(COMMON_LIB_DIR)/include
LDFLAGS		 = -L$(top_srcdir)/lib/ 
LIBS		 = -lhiredis

noinst_LIBRARIES = libcredis.a

libcredis_a_CFLAGS  = -O2 -static -Werror -Wall -finline-functions -Wno-unused-parameter -Wunused 
libcredis_a_LIBADD 	= $(COMMON_LIB_DIR)/lib/liblog.a

libcredis_a_SOURCES = \
	$(top_srcdir)/src/redis_conn.c \
	$(top_srcdir)/src/redis_op.c \
	$(top_srcdir)/src/redis_hash.c \
	$(top_srcdir)/src/redis_zset.c \
	$(top_srcdir)/src/redis_list.c 
