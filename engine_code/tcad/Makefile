TOP=.
include $(TOP)/mk/env.mk
export PROJ_DIR=$(PWD)
export STRIP
export OPT=$(HOME)

################PROGS##################
PROGS = $(TOP)/bin/tcad_gms $(TOP)/bin/monitor

################DIRS##################
THIRD_DIRS = $(TOP)/third
COMMON_DIRS = $(TOP)/src
DATAPATH_DIRS += $(TOP)/src/common
DATAPATH_DIRS += $(TOP)/src/http
DATAPATH_DIRS += $(TOP)/src/x_pstk
DATAPATH_DIRS += $(TOP)/src/ddos
#DATAPATH_DIRS += $(TOP)/src/trie

MONITOR_DIRS = $(TOP)/src/monitor

SRC_DIRS = $(THIRD_DIRS) $(COMMON_DIRS) $(DATAPATH_DIRS) $(MONITOR_DIRS)

################PROGS USE LOCAL LIBS##################
PROG_DATAPATH_LDFLAGS = $(TOP)/lib/libdp.a
PROG_DATAPATH_LDFLAGS += $(TOP)/lib/libx_pstk.a
PROG_DATAPATH_LDFLAGS += $(TOP)/lib/libhttp.a
PROG_DATAPATH_LDFLAGS += $(TOP)/lib/libcommon.a
PROG_DATAPATH_LDFLAGS += $(TOP)/lib/libddos.a

PROG_MONITOR_LDFLAGS = $(TOP)/lib/libmonitor.a
PROG_MONITOR_LDFLAGS += $(TOP)/lib/libcommon.a

export PROG_DATAPATH_LDFLAGS 
export PROG_MONITOR_LDFLAGS


.PHONY: all print_platform $(SRC_DIRS) clean dirclean FORCE

all: print_platform $(SRC_DIRS) $(PROGS) 

print_platform:
	@echo "###################################"
	@echo "             $(PLATFORM)"
	@echo "###################################"
	@echo $(MAKE)
$(SRC_DIRS):
	@mkdir -p $(TOP)/bin $(TOP)/lib
	$(MAKE) --directory=$@

$(TOP)/bin/tcad_gms: $(COMMON_DIRS) $(DATAPATH_DIRS)
	@echo [LD] $@
	@$(MAKE) -f $(TOP)/mk/prog.mk tcad_gms
	
$(TOP)/bin/monitor: $(COMMON_DIRS) $(MONITOR_DIRS)
	@echo [LD] $@
	@$(MAKE) -f $(TOP)/mk/prog.mk monitor

clean:
	$(Q) for d in $(SRC_DIRS); \
	do \
	$(MAKE) clean --directory=$$d; \
	done \
 
	$(Q) -rm -f $(PROGS)



