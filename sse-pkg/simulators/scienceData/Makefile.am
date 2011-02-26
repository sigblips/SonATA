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

bin_PROGRAMS = dumpSonATABaselines dumpSonATACompAmps diffSonATABaselines \
	extractSonATACompampsSubchannel

dumpSonATABaselines_SOURCES = \
  dumpSonATABaselines.cpp 

dumpSonATACompAmps_SOURCES = \
  dumpSonATACompAmps.cpp 

diffSonATABaselines_SOURCES = \
  diffSonATABaselines.cpp 

extractSonATACompampsSubchannel_SOURCES = \
  extractSonATACompampsSubchannel.cpp

LIB_DEPENDS = $(SSE_DX_INTERFACE_LIB) $(SSE_INTERFACE_LIB) $(SSEUTIL_LIB)
dumpSonATABaselines_DEPENDENCIES = $(LIB_DEPENDS)
dumpSonATACompAmps_DEPENDENCIES =  $(LIB_DEPENDS)
diffSonATABaselines_DEPENDENCIES = $(LIB_DEPENDS)
extractSonATACompampsSubchannel_DEPENDENCIES = $(LIB_DEPENDS)

SSEINCLUDE = $(top_srcdir)/include

INCLUDES= -I$(ACE_ROOT) -I$(SSEUTIL_INC_DIR) \
  -I$(SSE_INC_DIR) -I$(CPPUNIT_ROOT)/include

LDADD = -L$(ACE_ROOT)/ace -lACE \
	 $(LIB_DEPENDS)
	 -lpthread

EXTRA_DIST = nss.p6.baseline.L \
	nss.p6.baseline.R \
	nss.p6.compamp.L \
	nss.p6.compamp.R \
	nss.DriftCwThenPulseTestSig.L.compamp \
	nss.DriftPulseTestSig.L.compamp \
	nss.DriftTestSig.L.compamp \
	nss.TestSig.compamp.L \
	nss.TestSig.compamp.R \
	nss.DriftTestSig.L.baseline \
	nss.TestSig.baseline.L \
	nss.TestSig.baseline.R


