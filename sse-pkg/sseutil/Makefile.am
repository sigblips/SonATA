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

# *****************************************************
# Please do NOT add any ACE dependencies to this library.
# It is required to be usable without ACE!
# *****************************************************

bin_PROGRAMS = printIsoDateTime dopplerShift 

lib_LIBRARIES = libsseutil.a

PUBLIC_HEADER_FILES = \
  CmdPattern.h \
  IoPlug.h \
  SseUtil.h \
  SseTimer.h \
  ProcessInputCmd.h \
  RedirectStreamToFile.h \
  Publisher.h \
  Subscriber.h \
  Parameter.h \
  RangeParameter.h \
  AnyValueParameter.h \
  ChoiceParameter.h \
  MultiChoiceParameter.h \
  ParameterImpl.h \
  ParameterGroup.h \
  Angle.h \
  ArrayLength.h \
  Interpolate.h \
  CmdLineParser.h

libsseutil_a_SOURCES = \
  IoPlug.cpp \
  SseUtil.cpp \
  SseTimer.cpp \
  getEnvString.cpp \
  RedirectStreamToFile.cpp \
  Publisher.cpp \
  Subscriber.cpp \
  splitByDelimiter.cpp \
  strCase.cpp \
  tokenize.cpp \
  ParameterImpl.cpp \
  ParameterGroup.cpp \
  Angle.cpp \
  Interpolate.cpp \
  CmdLineParser.cpp \
  $(PUBLIC_HEADER_FILES)

# public headers to include in 'make install' target
include_HEADERS = $(PUBLIC_HEADER_FILES)

check_PROGRAMS = testUnit testMailMsg testAngle

TESTS =  testUnit testAngle

printIsoDateTime_SOURCES = \
  printIsoDateTime.cpp \
  $(libsseutil_a_SOURCES)

dopplerShift_SOURCES = \
  dopplerShift.cpp \
  $(libsseutil_a_SOURCES)

testUnit_SOURCES = \
  testUnit.cpp \
  Testlib.cpp \
  TestCmdLineParser.cpp \
  TestCmdLineParser.h \
  Testlib.h

testMailMsg_SOURCES =\
  testMailMsg.cpp \
  $(libsseutil_a_SOURCES)

testAngle_SOURCES = testAngle.cpp \
                Angle.cpp \
                Angle.h
testAngle_LDADD = -L$(CPPUNIT_ROOT)/lib -lcutextui -lcu


# *****************************************************
# Please do NOT add any ACE dependencies to this library.
# It is required to be usable without ACE!
# *****************************************************

INCLUDES=  -I$(SSE_INC_DIR) -I$(CPPUNIT_ROOT)/include
LIBLIST = -L. $(lib_LIBRARIES)  -lnsl -lpthread

LDADD = $(LIBLIST)

testUnit_LDFLAGS =  -L$(CPPUNIT_ROOT)/lib 
testUnit_LDADD = $(LIBLIST) -lcutextui -lcu 
testUnit_DEPENDENCIES = $(lib_LIBRARIES)

