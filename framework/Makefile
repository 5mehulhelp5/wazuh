# Makefile for Wazuh Framework
# Copyright (C) 2015, Wazuh Inc.
# May 3, 2017
#
# Syntax: make [ all | build | install | examples | clean ]

WAZUH_GROUP       = wazuh
INSTALLDIR       ?= /var/lib/wazuh-server

CC           = gcc
CFLAGS       = -pipe -Wall -Wextra
THREAD_FLAGS = -pthread
MV_FILE      = mv -f
RM_FILE      = rm -f
INSTALL_DIR  = install -o root -g ${WAZUH_GROUP} -m 0750  -d
INSTALL_EXEC = install -o root -g ${WAZUH_GROUP} -m 0750
INSTALL_FILE = install -o root -g ${WAZUH_GROUP} -m 0640

ifdef DEBUG
	CFLAGS+=-g -I ../src
	LFLAGS+=-g
else
	CFLAGS+=-O2 -I ../src
	LFLAGS+=-O2
endif

.PHONY: all build install examples clean

all: build

install:
	$(INSTALL_DIR) $(INSTALLDIR)/framework
	$(INSTALL_DIR) $(INSTALLDIR)/framework/scripts
	$(INSTALL_DIR) $(INSTALLDIR)/framework/wazuh
	$(INSTALL_DIR) $(INSTALLDIR)/framework/wazuh/core/cluster
	$(INSTALL_DIR) $(INSTALLDIR)/framework/wazuh/core/cluster/dapi
	$(INSTALL_DIR) $(INSTALLDIR)/framework/wazuh/core/cluster/hap_helper
	$(INSTALL_DIR) $(INSTALLDIR)/logs
	$(INSTALL_DIR) $(INSTALLDIR)/etc
	$(INSTALL_DIR) $(INSTALLDIR)/var/run
	$(INSTALL_DIR) $(INSTALLDIR)/queue/cluster
	$(INSTALL_DIR) $(INSTALLDIR)/queue/sockets

	$(INSTALL_FILE) scripts/*.py ${INSTALLDIR}/framework/scripts
	$(INSTALL_FILE) wazuh/*.py ${INSTALLDIR}/framework/wazuh
	$(INSTALL_FILE) wazuh/core/cluster/*.json ${INSTALLDIR}/framework/wazuh/core/cluster
	$(INSTALL_FILE) wazuh/core/cluster/*.py ${INSTALLDIR}/framework/wazuh/core/cluster
	$(INSTALL_FILE) wazuh/core/cluster/dapi/*.py ${INSTALLDIR}/framework/wazuh/core/cluster/dapi
	$(INSTALL_FILE) wazuh/core/cluster/hap_helper/*.py ${INSTALLDIR}/framework/wazuh/core/cluster/hap_helper
	$(INSTALL_FILE) ../etc/ossec.conf ${INSTALLDIR}/etc

#	Install scripts/%.py on $(INSTALLDIR)/bin/%
	$(foreach script,$(wildcard scripts/*.py),$(INSTALL_EXEC) wrappers/generic_wrapper.sh $(patsubst scripts/%.py,/bin/%,$(script));)
#   Provisional name change for wazuh-logtest and wazuh-clusterd
	$(MV_FILE) /bin/wazuh_server /bin/wazuh-server
	$(RM_FILE) /bin/__init__

examples: install
	$(INSTALL_DIR) $(INSTALLDIR)/framework/examples
	$(INSTALL_EXEC) examples/*.py ${INSTALLDIR}/framework/examples

clean:
	$(RM_FILE) $(BUILD_TARGET) *.o *.so.0
