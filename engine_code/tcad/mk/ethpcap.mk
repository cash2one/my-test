pag_inc = $(PROJ_DIR)/third/ethpcap/include

ifeq ($(CPAG_VERSION),y)
pag_lpath = $(PROJ_DIR)/third/ethpcap/$(LIBPATH)/shared
else
pag_lpath = $(PROJ_DIR)/third/ethpcap/$(LIBPATH)/static
endif