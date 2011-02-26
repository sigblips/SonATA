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

lib_LIBRARIES = libsseDxInterface.a

PUBLIC_HEADER_FILES = \
	SseDxMsg.h

libsseDxInterface_a_SOURCES = \
	sseDxInterfaceLib.h \
	SseDxMsg.cpp \
	CwBadBand.cpp \
	PulseBadBand.cpp \
	SignalPath.cpp \
	SignalDescription.cpp \
	ConfirmationStats.cpp \
	Baseline.cpp \
	BaselineStatistics.cpp \
	ComplexAmplitudes.cpp \
	Count.cpp \
	CwCoherentSegment.cpp \
	CwCoherentSignal.cpp \
	CwPowerSignal.cpp \
	FollowUpSignal.cpp \
	DetectionStatistics.cpp \
	FrequencyBand.cpp \
	FrequencyMaskHeader.cpp \
	HereIAm.cpp \
	ThereYouAre.cpp \
	DxActivityParameters.cpp \
	DxBaseAddr.cpp \
	DxConfiguration.cpp \
	DxIntrinsics.cpp \
	DxScienceDataRequest.cpp \
	DxActivityStatus.cpp \
	DxStatus.cpp \
	DxTuned.cpp \
	PulseSignal.cpp \
	PulseParameters.cpp \
	RecentRfiMaskHeader.cpp \
	SignalId.cpp \
	StartActivity.cpp \
	ArchiveRequest.cpp \
	ArchiveDataHeader.cpp \
	$(PUBLIC_HEADER_FILES)


# public headers to include in 'make install' target
include_HEADERS = $(PUBLIC_HEADER_FILES)


check_PROGRAMS = testUnit checkSseDxMsgSizes
TESTS =  testUnit

testUnit_SOURCES = \
  testUnit.cpp \
  TestSseDxInterfaceLib.h \
  TestSseDxInterfaceLib.cpp

EXTRA_PROGRAMS = 

checkSseDxMsgSizes_SOURCES = \
  checkSseDxMsgSizes.cpp

# disable warnings for now (gcc -w option) when building checkSseDxMsgSizes.
# gcc3.3 warns about the use of the offsetof macro
# for structs that have a constructor defined (but seems to work
# correctly)
# eg,  
# checkSseDxMsgSizes.cpp:252: warning: invalid access to non-static data member 
#   `DxStatus::timestamp' of NULL object
#checkSseDxMsgSizes.cpp:252: warning: (perhaps the `offsetof' macro was used 
#   incorrectly)

checkSseDxMsgSizes_CXXFLAGS = @CXXFLAGS@ -w

# build checksizes by itself for easier marshalling development
checksizes:
	g++ -w -I. -I.. -I../include checkSseDxMsgSizes.cpp -o checkSseDxMsgSizes


LIB_DEPENDS = $(SSE_INTERFACE_LIB) $(SSEUTIL_LIB) 
libsseDxInterface_a_DEPENDENCIES = $(LIB_DEPENDS)

INCLUDES=  -I$(SSE_INC_DIR) -I$(SSEUTIL_INC_DIR) \
	 -I$(SSE_INTERFACE_INC_DIR) -I$(CPPUNIT_ROOT)/include 

LIBLIST = -L.  $(lib_LIBRARIES) $(LIB_DEPENDS)

LDADD = $(LIBLIST)

testUnit_LDFLAGS =  -L$(CPPUNIT_ROOT)/lib 
testUnit_LDADD = $(LIBLIST) -lcutextui -lcu 
testUnit_DEPENDENCIES = $(lib_LIBRARIES)



