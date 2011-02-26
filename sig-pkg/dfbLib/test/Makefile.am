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

## $Id: Makefile.am,v 1.10 2009/06/30 19:06:37 kes Exp $

## Process this file with automake to produce Makefile.in

top_srcdir = ..
top_builddir = ..

AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = test

check_PROGRAMS = test

TESTS = test

EXTRA_PROGRAMS =

EXTRA_DIST =

BUILT_SOURCES =

DFB_LIB_DIR = ../src
DFB_LIB = $(DFB_LIB_DIR)/libDfb.a

SIGPROC_DIR = $(top_srcdir)/..
SIGPROC_INCDIR = $(SIGPROC_INCDIR)/include

PKT_DIR = $(top_srcdir)/../ATApackets
PKT_INCDIR = $(PKT_DIR)/include
PKT_LIBDIR = $(PKT_DIR)/src

LIB_DEPENDS = $(DFB_LIB)

test_DEPENDENCIES = $(LIB_DEPENDS)

DFBINCLUDE = ../include

INCLUDES= -I . -I$(DFBINCLUDE) -I$(PKT_INCDIR) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib

test_SOURCES = \
	test.cpp \
	DfbTest.cpp \
	DfbTest.h \
	TestFilter.cpp \
	testVal.h

DFB_LIBS = \
  -lpthread -lnsl \
  ../src/libDfb.a \
  -lfftw3f \
  -L$(PKT_LIBDIR) \
  -lSup

LDADD = $(DFB_LIBS)
