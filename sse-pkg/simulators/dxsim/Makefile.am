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

bin_PROGRAMS = dxsim

dxsim_MOST_SOURCES = \
  Dx.cpp \
  Dx.h \
  DxStateMachine.cpp \
  DxStateMachine.h \
  DxUserCmds.h \
  SseDatagramHandler.h \
  SseDatagramHandler.cpp \
  SseProxy.cpp \
  SseProxy.h \
  DxArchiverProxy.h \
  DxArchiverProxy.cpp \
  DxCmdLineArgs.cpp \
  DxCmdLineArgs.h

dxsim_SOURCES = \
  $(dxsim_MOST_SOURCES) \
  dxsim.cpp
  

check_PROGRAMS = testUnitDx

TESTS = $(check_PROGRAMS)

testUnitDx_SOURCES = \
  $(dxsim_MOST_SOURCES) \
  testUnitDx.cpp \
  TestDx.cpp \
  TestDx.h \
  TestDxStateMachine.cpp \
  TestDxStateMachine.h


dxsim_DEPENDENCIES = $(SSECOMMUTIL_LIB) $(SSE_DX_INTERFACE_LIB) $(SSE_INTERFACE_LIB) $(SSEUTIL_LIB)
testUnitDx_DEPENDENCIES = $(dxsim_DEPENDENCIES)

INCLUDES= -I$(ACE_ROOT) -I$(SSECOMMUTIL_INC_DIR) -I$(SSEUTIL_INC_DIR) \
	-I$(SSE_DX_INTERFACE_INC_DIR) \
	-I$(SSE_INTERFACE_INC_DIR) -I$(SSE_INC_DIR) \
	-I$(CPPUNIT_ROOT)/include -fno-strict-aliasing

LDADD = -L$(ACE_ROOT)/ace -lACE \
	$(dxsim_DEPENDENCIES) \
	-lpthread -lnsl 

testUnitDx_LDFLAGS =  -L$(CPPUNIT_ROOT)/lib 
testUnitDx_LDADD = -lcutextui -lcu $(LDADD)
