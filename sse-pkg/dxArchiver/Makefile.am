################################################################################
#
# File:    Makefile.am
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

## Process this file with automake to produce Makefile.in


AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = dxArchiver

dxArchiver_MOST_SOURCES = \
  DxArchiver.cpp \
  DxArchiver.h \
  DxArchiverUserCmds.h \
  SignalArchiver.h \
  SignalArchiver.cpp \
  SseProxy.cpp \
  SseProxy.h \
  DxProxy.cpp \
  DxProxy.h \
  DxArchiverCmdLineArgs.cpp \
  DxArchiverCmdLineArgs.h

dxArchiver_SOURCES = \
  $(dxArchiver_MOST_SOURCES) \
  dxArchiverMain.cpp

check_PROGRAMS = testUnitDxArchiver

TESTS = $(check_PROGRAMS)

testUnitDxArchiver_SOURCES = \
  testUnitDxArchiver.cpp \
  TestDxArchiver.cpp \
  TestDxArchiver.h \
  $(dxArchiver_MOST_SOURCES)

dxArchiver_DEPENDENCIES = \
	 $(SSECOMMUTIL_LIB) \
	 $(SSE_DX_INTERFACE_LIB) \
	 $(SSE_DX_ARCHIVER_INTERFACE_LIB) \
	 $(SSE_INTERFACE_LIB) \
	 $(SSEUTIL_LIB)

testUnitDxArchiver_DEPENDENCIES = $(dxArchiver_DEPENDENCIES) 

SSEINCLUDE = $(top_srcdir)/include

INCLUDES= -I$(ACE_ROOT) \
	-I$(SSECOMMUTIL_INC_DIR) -I$(SSEUTIL_INC_DIR) \
	-I$(SSE_DX_INTERFACE_INC_DIR) \
	-I$(SSE_DX_ARCHIVER_INTERFACE_INC_DIR) \
	-I$(SSE_INTERFACE_INC_DIR) -I$(SSE_INC_DIR) \
	 -I$(CPPUNIT_ROOT)/include

LDADD = -L$(ACE_ROOT)/ace -lACE \
	$(dxArchiver_DEPENDENCIES) \
	-lpthread -lnsl

testUnitDxArchiver_LDFLAGS =  -L$(CPPUNIT_ROOT)/lib 
testUnitDxArchiver_LDADD = -lcutextui -lcu $(LDADD)
