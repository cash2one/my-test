TOP=../..
include $(TOP)/mk/env.mk
include $(TOP)/mk/gndpi.mk

DIR=http
TARGET=$(TOP)/lib/lib$(DIR).a
BUILD_DIR=$(TOP)/build/$(DIR)
SOURCES:=$(wildcard *.c)
OBJS:=$(addprefix $(BUILD_DIR)/,$(subst .c,.o,$(SOURCES)))
DEPS:=$(addprefix $(BUILD_DIR)/,$(subst .c,.d,$(SOURCES)))

PROJ_CFLAGS += -I. -I$(TOP)/include -I$(TOP)/src -I$(TOP)/src/x_pstk -I$(TOP)/third/cdpi/include -I$(TOP)/third/cdpi/include/yara

.PHONY: all clean

all:$(TARGET)

include $(TOP)/mk/rule.mk