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

lib_LIBRARIES = libsseInterface.a

PUBLIC_HEADER_FILES = \
	HostNetByteorder.h \
	SseMsg.h \
	Signals.h

libsseInterface_a_SOURCES = \
	sseInterfaceLib.h \
	SseMsg.cpp \
	NssDate.cpp \
	NssMessage.cpp \
	SseInterfaceHeader.cpp \
	SseMsgDateUtil.cpp \
	Signals.cpp \
	$(PUBLIC_HEADER_FILES)

# public headers to include in 'make install' target
include_HEADERS = $(PUBLIC_HEADER_FILES)

check_PROGRAMS = testUnit

TESTS =  testUnit

testUnit_SOURCES = \
  testUnit.cpp \
  TestSseInterfaceLib.h \
  TestSseInterfaceLib.cpp \
  $(libsseInterface_a_SOURCES)

LIB_DEPENDS = $(SSEUTIL_LIB) 
libsseInterface_a_DEPENDENCIES = $(LIB_DEPENDS)

INCLUDES= -I$(SSE_INC_DIR) -I$(SSEUTIL_INC_DIR) \
	-I$(CPPUNIT_ROOT)/include

LIBLIST = -L. $(lib_LIBRARIES) $(SSEUTIL_LIB)

LDADD = $(LIBLIST)

testUnit_LDFLAGS =  -L$(CPPUNIT_ROOT)/lib 
testUnit_LDADD = $(LIBLIST) -lcutextui -lcu 
testUnit_DEPENDENCIES = $(lib_LIBRARIES)

