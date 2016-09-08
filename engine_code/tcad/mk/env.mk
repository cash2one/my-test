CPAG_VERSION = y
PLATFORM = HOST
BITS_FLAG = $(shell uname -m)

# wdb_calc222
CFLAGS = -Wstrict-prototypes -Wall -Wunused -Wcast-align -Wint-to-pointer-cast -Wunused-but-set-variable

ifeq ($(PLATFORM), HOST)
    CROSS_COMPILE =
# wdb_calc222
    CFLAGS += -D_PLATFORM_HOST_ -DLINUX -D_GNU_SOURCE
    ifeq ($(BITS_FLAG),x86_64)
        LIBPATH = hostlib64
        BITS_FLAG = 64
    else
        LIBPATH = hostlib
        BITS_FLAG = 32
    endif
endif

ifeq ($(PLATFORM),PPC)
    CROSS_COMPILE = ppc_82xx-
# wdb_calc222
    CFLAGS += -D_PLATFORM_PPC_ -DLINUX -DBIG_ENDIAN_BITFIELD -D_GNU_SOURCE
    LIBPATH = ppclib
    BITS_FLAG = 32
endif

ifeq ($(PLATFORM),MIPS)
    CROSS_COMPILE = mips64-octeon-linux-gnu-
# wdb_calc222
    CFLAGS += -D_PLATFORM_MIPS_ -DLINUX -DBIG_ENDIAN_BITFIELD -D_GNU_SOURCE
    LIBPATH = mipslib
    BITS_FLAG = 64
endif

NASM = $(CROSS_COMPILE)nasm
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
STRIP = $(CROSS_COMPILE)strip
OBJDUMP = $(CROSS_COMPILE)objdump
NM = $(CROSS_COMPILE)nm

BUILD_VERBOSE = n
DEBUG_VERSION = n
GPROF_VERSION = n

ifeq ($(BUILD_VERBOSE),y)
    Q =
else
    Q = @
endif

ifeq ($(DEBUG_VERSION),y)
    DEBUG_FLAGS = -g
else
    DEBUG_FLAGS =
endif

ifeq ($(GPROF_VERSION),y)
    GPROF_FLAGS = -pg
else
    GPROF_FLAGS =
endif

ifeq ($(CPAG_VERSION),y)
    CPAG_FLAGS = -D CPAG
else
    CPAG_FLAGS =
endif

# PROJ_CFLAGS = -I$(TOP)/include ##wdb_no_calc: change to
PROJ_CFLAGS = -I$(TOP)/include -I$(TOP)/src/common

CFLAGS += $(DEBUG_FLAGS) $(GPROF_FLAGS) $(CPAG_FLAGS) -Wall $(PROJ_CFLAGS)
