TOP := ..
THIRD := boost
CUR_DIR := .
TARGETS := $(addprefix $(CUR_DIR)/,$(THIRD))

curl_version := 7.21.2
protoc-c_version := 0.13
readline_version := 6.1
zlib_version := 1.2.5
zmq_version := 2.1.2
llconf_version := 0.4.6
event_version := 2.0.10
expat_version := 2.0.1
log4c_version := 1.2.1
net-snmp_version := 5.6

all:third_prepare

third_prepare: $(TARGETS)

define third/lib/prepare
$(CUR_DIR)/$(1):
#	@if [ ! -d $(CUR_DIR)/$(1) ]; then \
#		$(TOP)/scripts/uncompression.sh $(1).tar.gz; \
#		$(TOP)/scripts/gen_mk.sh $(1) $(TOP)/mk/ $(PROJ_DIR); \
#	fi
endef

$(foreach lib,$(THIRD),$(eval $(call third/lib/prepare,$(lib))))

clean:

distclean:
	rm -rf $(TARGETS) $(addsuffix .mk,$(addprefix $(TOP)/mk/,$(THIRD)))

.PHONY:third_prepare clean distclean
