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


# *****************************************************
# Please do NOT add any ACE dependencies to this library.
# It is required to be usable without ACE!
# *****************************************************

AUTOMAKE_OPTIONS = foreign

# Make this src directory available to the test code so that it can 
# be used to find local data files needed by the test suites.  This is
# especially important for making the 'distcheck' target since
# those executables are built into a separate directory from
# the source files.

DEFS = -DSRCDIR=\"$(srcdir)\" @DEFS@


lib_LIBRARIES = libdoppler.a

PUBLIC_HEADER_FILES = \
	astro.h \
	doppler.h

libdoppler_a_SOURCES = \
	astro.c \
	doppler.c \
	doppler_work.c \
	precession.c \
	readephem.c \
	ast_const.h \
	doppler_work.h \
	precession.h \
	readephem.h \
	$(PUBLIC_HEADER_FILES)


# Force the ephemeris test datafiles to be included in the distribution
EXTRA_DIST = earth-ephem-2002.xyz \
	pioneer10-ephem-2002.xyz

# dummy targets to keep make happy
earth-ephem-2002.xyz:

pioneer10-ephem-2002.xyz:


# public headers to include in 'make install' target
include_HEADERS = $(PUBLIC_HEADER_FILES)

check_PROGRAMS = testUnit

TESTS =  testUnit 

testUnit_SOURCES = \
  testUnit.cpp \
  Testlib.cpp \
  Testlib.h

# Earth & spacecraft ephemeris generating code, originally from 
# Dreher.
# This code relies on the NAIF spice libraries which are
# not required to build the rest of the sse package,
# so don't make this target as part of the reqular build.
# The spice toolkit (which includes the libraries)
# can be obtained from:
# ftp://naif.jpl.nasa.gov/pub/naif/toolkit/

SPICELIB=/usr/local/cspice/lib/cspice.a

scephm: scephm.f
	g77 scephm.f -o scephm $(SPICELIB) 


# *****************************************************
# Please do NOT add any ACE dependencies to this library.
# It is required to be usable without ACE!
# *****************************************************

INCLUDES= -I$(CPPUNIT_ROOT)/include 

LIBLIST = -L. -ldoppler

LDADD = $(LIBLIST)

testUnit_LDFLAGS =  -L$(CPPUNIT_ROOT)/lib 
testUnit_LDADD = $(LIBLIST) -lcutextui -lcu 
testUnit_DEPENDENCIES = $(lib_LIBRARIES)
