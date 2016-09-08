TOP=../..
include $(TOP)/mk/env.mk

ASM_INCLUDE=0

DIR=common
TARGET=$(TOP)/lib/lib$(DIR).a
BUILD_DIR=$(TOP)/build/$(DIR)
SOURCES:=$(wildcard *.c)
ASM_SOURCES:=$(wildcard *.s)
OBJS:=$(addprefix $(BUILD_DIR)/,$(subst .c,.o,$(SOURCES)))
DEPS:=$(addprefix $(BUILD_DIR)/,$(subst .c,.d,$(SOURCES)))
ASM_OBJS:=$(addprefix $(BUILD_DIR)/,$(subst .s,.o,$(ASM_SOURCES)))

PROJ_CFLAGS += 

.PHONY: all clean

all:$(TARGET)

include $(TOP)/mk/rule.mk