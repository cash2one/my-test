TOP=../..
include $(TOP)/mk/env.mk
include $(TOP)/mk/cdpi.mk

dir = $(TOP)

DIR=x_pstk
TARGET=$(TOP)/lib/lib$(DIR).a
BUILD_DIR=$(TOP)/build/$(DIR)
SOURCES:=$(wildcard *.c)
OBJS:=$(addprefix $(BUILD_DIR)/,$(subst .c,.o,$(SOURCES)))
DEPS:=$(addprefix $(BUILD_DIR)/,$(subst .c,.d,$(SOURCES)))

PROJ_CFLAGS += -I. -I$(logw_inc) -I$(TOP)/include -I$(TOP)/src -I$(TOP)/src/monitor -I$(TOP)/src/http -I$(TOP)/third/cdpi/include -I$(TOP)/third/cdpi/include/yara -I$(TOP)/src/ddos

.PHONY: all clean

all:$(TARGET)

include $(TOP)/mk/rule.mk
