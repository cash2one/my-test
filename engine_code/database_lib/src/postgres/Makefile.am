AUTOMAKE_OPTIONS = foreign

COMMMON_LIB_DIR=$(top_srcdir)/../../../common_lib
CC			 = gcc
INCLUDES 	 = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMMON_LIB_DIR)/include/ -I/usr/local/pgsql/include/ -I/home/postgresql/include/
LDFLAGS		 = -L/usr/local/pgsql/lib/ -L/home/postgresql/lib/ -L$(COMMMON_LIB_DIR)/lib/
LIBS		 = -lpq

noinst_LIBRARIES = libcpsql.a

libcpsql_a_CFLAGS  = -O2 -static -Werror -Wall -finline-functions -Wno-unused-parameter -Wunused 
libcpsql_a_LIBADD  = $(COMMMON_LIB_DIR)/lib/liblog.a

libcpsql_a_SOURCES = \
	$(top_srcdir)/src/psql_conn.c \
	$(top_srcdir)/src/psql_result.c \
	$(top_srcdir)/src/store_common.c
