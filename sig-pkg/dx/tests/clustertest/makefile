################################################################################
#
# File:    makefile
# Project: OpenSonATA
# Authors: The OpenSonATA code is the result of many programmers
#          over many years
#
# Copyright 2011 The SETI Institute
#
# OpenSonATA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenSonATA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
# 
# Implementers of this code are requested to include the caption
# "Licensed through SETI" with a link to setiQuest.org.
# 
# For alternate licensing arrangements, please contact
# The SETI Institute at www.seti.org or setiquest.org. 
#
################################################################################

SHELL = /bin/sh

BASE=../../../..

CXXFLAGS += -g
COMPILE.c = $(COMPILE.cpp)

CPPFLAGS += -I.
CPPFLAGS += -I$(BASE)/sse-pkg/include
CPPFLAGS += -I$(BASE)/pdm-pkg/pdm/include

VPATH += $(BASE)/pdm-pkg/pdm/src
CPPFLAGS += -I$(BASE)/pdm-pkg/pdm/src

#VPATH += ../Projects/edglib/libsrc
#CPPFLAGS += -I../../edglib/libsrc

default: test

OBJS1 = testCluster.o PdmCWDClusterer.o PdmChildClusterer.o stubs.o \
	PdmSuperClusterer.o PdmPDClusterer.o
EXEC1 = testCluster

OBJS2 = doCluster.o PdmCWDClusterer.o PdmChildClusterer.o stubs.o \
	PdmSuperClusterer.o PdmPDClusterer.o
EXEC2 = doCluster

ALL_EXECS = $(EXEC1) $(EXEC2)
ALL_OBJS = $(OBJS1) $(OBJS2)
OTHER_TMPS = core config.h

prepare: config.h

config.h:
	touch $@

test: prepare $(EXEC1) $(EXEC2)
	$(EXEC1)
	$(EXEC2) <cwpaths.25779 | diff good.25779 -
	@echo TEST PASSED

$(EXEC1): $(OBJS1)
	g++ -o $@ $(OBJS1)

$(EXEC2): $(OBJS2)
	g++ -o $@ $(OBJS2)

clean:
	-rm $(ALL_EXECS) $(ALL_OBJS) $(OTHER_TMPS)


# XXX manually maintained dependencies for now
testCluster.o: PdmCWDClusterer.h PdmPDClusterer.h PdmChildClusterer.h \
	PdmSuperClusterer.h
doCluster.o: PdmCWDClusterer.h PdmPDClusterer.h PdmChildClusterer.h \
	PdmSuperClusterer.h
PdmCWDClusterer.o: PdmCWDClusterer.h PdmChildClusterer.h PdmSuperClusterer.h
PdmChildClusterer.o: PdmChildClusterer.h PdmSuperClusterer.h
PdmSuperClusterer.o: PdmSuperClusterer.h PdmChildClusterer.h
PdmPDClusterer.o: PdmPDClusterer.h PdmChildClusterer.h PdmSuperClusterer.h
 