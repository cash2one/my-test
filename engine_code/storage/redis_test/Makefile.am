AUTOMAKE_OPTIONS = foreign

CC  = gcc

COMMON_LIB_DIR  = $(top_srcdir)/../../common_lib
DB_LIB_DIR		= $(top_srcdir)/../../database_lib


INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(COMMON_LIB_DIR)/include -I$(DB_LIB_DIR)/include 
noinst_PROGRAMS = db_redis_test

DEBUG_FLAGS     = -g -rdynamic  -finline-functions -W  -Werror -Wunused -Wno-unused-parameter -fsigned-char 
CFLAGS          = $(DEBUG_FLAGS)

db_redis_test_LDADD        = -L$(DB_LIB_DIR)/lib -L$(COMMON_LIB_DIR)/lib -lcredis -lhiredis -llog
db_redis_test_SOURCES = \
	     $(top_srcdir)/src/test_redis_main.c
