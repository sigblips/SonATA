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

bin_PROGRAMS = chansim

chansim_MOST_SOURCES = \
  Channelizer.cpp \
  Channelizer.h \
  SseProxy.cpp \
  SseProxy.h

chansim_SOURCES = \
  $(chansim_MOST_SOURCES) \
  chansim.cpp

check_PROGRAMS = testUnitChan

TESTS = $(check_PROGRAMS)

testUnitChan_SOURCES = \
  testUnitChan.cpp \
  TestChan.cpp \
  TestChan.h \
  $(chansim_MOST_SOURCES)

chansim_DEPENDENCIES = \
	 $(SSECOMMUTIL_LIB) \
	 $(SSE_CHANNELIZER_INTERFACE_LIB) \
	 $(SSE_INTERFACE_LIB) \
	 $(SSEUTIL_LIB)

testUnitChan_DEPENDENCIES = $(chansim_DEPENDENCIES) 

SSEINCLUDE = $(top_srcdir)/include

INCLUDES= -I$(ACE_ROOT) \
	-I$(SSECOMMUTIL_INC_DIR) -I$(SSEUTIL_INC_DIR) \
	-I$(SSE_CHANNELIZER_INTERFACE_INC_DIR) \
	-I$(SSE_INTERFACE_INC_DIR) -I$(SSE_INC_DIR) \
	 -I$(CPPUNIT_ROOT)/include

LDADD = -L$(ACE_ROOT)/ace -lACE \
	$(chansim_DEPENDENCIES) \
	-lpthread -lnsl

testUnitChan_LDFLAGS = -L$(CPPUNIT_ROOT)/lib 
testUnitChan_LDADD = -lcutextui -lcu $(LDADD)
