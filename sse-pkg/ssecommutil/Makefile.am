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

setupdir = $(prefix)/setup/
DEFS = -DSETUPDIR=\"$(setupdir)\" \
	-I$(top_builddir) \
        @DEFS@

AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libssecommutil.a

PUBLIC_HEADER_FILES = \
  SseCommUtil.h \
  NssProxy.h \
  ComponentProxy.h \
  TclProxy.h \
  StdinHandler.h \
  UserCmdHandler.h \
  TextUI.h \
  SseSock.h \
  Log.h \
  StreamMutex.h \
  SseSystem.h \
  SseArchive.h \
  Verbose.h \
  ast_const.h \
  IfDbOffsetTable.h \
  RotatingFileLogger.h \
  SseAstro.h \
  Timeout.h

libssecommutil_a_SOURCES = \
  NssProxy.cpp \
  ComponentProxy.cpp \
  TclProxy.cpp \
  NssAcceptHandler.h \
  NssComponentManager.h \
  StdinHandler.cpp \
  TextUI.cpp \
  UserCmdHandler.cpp \
  getHostIpAddr.cpp \
  validAtaSubarray.cpp \
  Log.cpp \
  StreamMutex.cpp \
  SseSystem.cpp \
  SseSock.cpp \
  SseArchive.cpp \
  SseMessage.cpp \
  Verbose.cpp \
  IfDbOffsetTable.cpp \
  RotatingFileLogger.cpp \
  SseAstro.cpp \
  $(PUBLIC_HEADER_FILES)

# Include this in the source list above to eliminate link error
# when using stlport
#  AceLogMsgStub.cpp

coordToDecimal_SOURCES = \
  coordToDecimal.cpp \
  $(libssecommutil_a_SOURCES)

angularSep_SOURCES = \
  angularSep.cpp \
  $(libssecommutil_a_SOURCES)

beamgrid_SOURCES = \
  beamgrid.cpp \
  $(libssecommutil_a_SOURCES)


# public headers to include in 'make install' target
include_HEADERS = $(PUBLIC_HEADER_FILES)

check_PROGRAMS = testUnit TestUserCmdHandler TestTextUI

TESTS = testUnit TestUserCmdHandler 

bin_PROGRAMS = coordToDecimal angularSep beamgrid testRotatingFileLogger

testUnit_SOURCES = \
  testUnit.cpp \
  Testlib.cpp \
  Testlib.h \
  TestNssComponentManager.h \
  TestNssComponentManager.cpp \
  TestSseAstro.h \
  TestSseAstro.cpp \
  $(libssecommutil_a_SOURCES)

TestUserCmdHandler_SOURCES = \
  TestUserCmdHandler.cpp \
  $(libssecommutil_a_SOURCES)

TestTextUI_SOURCES = \
  TestTextUI.cpp \
  $(libssecommutil_a_SOURCES)

testRotatingFileLogger_SOURCES = \
  testRotatingFileLogger.cpp \
  $(libssecommutil_a_SOURCES)

LIB_DEPENDS = $(SSE_INTERFACE_LIB) \
	$(AAPLUS_LIB) $(WCS_LIB) $(NOVAS_LIB) $(SSEUTIL_LIB) 

INCLUDES= -I$(ACE_ROOT) -I$(SSE_INC_DIR) \
	-I$(AAPLUS_INC_DIR) \
	-I$(WCS_INC_DIR) \
	-I$(NOVAS_INC_DIR) \
        -I$(NOVA_ROOT)/include \
	-I$(SSEUTIL_INC_DIR) \
	-I$(SSE_INTERFACE_INC_DIR) \
	-I$(CPPUNIT_ROOT)/include

LIBLIST = -L$(ACE_ROOT)/ace -lACE -L. \
	$(LIB_DEPENDS) -lnsl -lpthread 

LDADD = $(LIBLIST)

testUnit_LDFLAGS = -L$(CPPUNIT_ROOT)/lib 
testUnit_LDADD = $(LIBLIST) -lcutextui -lcu 
testUnit_DEPENDENCIES = $(lib_LIBRARIES)

testRotatingFileLogger_LDADD = $(LIBLIST)