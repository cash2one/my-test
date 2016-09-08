include $(PROJ_DIR)/mk/env.mk
include $(PROJ_DIR)/mk/ethpcap.mk
include $(PROJ_DIR)/mk/cdpi.mk
include $(PROJ_DIR)/mk/cityhash.mk

TOP = ..
psql_include=/db/postgresql/lib
PROG_DATAPATH_LDFLAGS += -L$(pag_lpath) -L$(psql_include) -lpag -L$(cityhash_lpath) -lcityhash -L$(cdpi_lpath) -lcdpi -lyara -lthunk -liplib -lstdc++ -lpthread -lrt -lpq -lcrypto

tcad_gms:FORCE
# wdb: change this
	$(Q) $(CC) -o $(PROJ_DIR)/bin/$@ $(PROG_DATAPATH_LDFLAGS)
# wdb: to
#	$(Q) $(CC) -o $(PROJ_DIR)/bin/$@ $(PROG_DATAPATH_LDFLAGS) -lnuma
# wdb: end of changing

monitor:FORCE
	$(Q) $(CC) -o $(PROJ_DIR)/bin/$@ $(PROG_MONITOR_LDFLAGS)

.PHONY: tcad_gms monitor FORCE
