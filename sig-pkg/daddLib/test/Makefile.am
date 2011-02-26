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

## $Id: Makefile.am,v 1.4 2009/06/30 18:08:07 kes Exp $

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

DADD_LIBDIR = ../src
DADD_LIB = $(DADD_LIBDIR)/libDadd.a

LIB_DEPENDS = $(DADD_LIB)

test_DEPENDENCIES = $(LIB_DEPENDS)

DADDINCLUDE = ../include

SIGPROC_DIR = $(top_srcdir)/..
SIGPROC_INCDIR = $(SIGPROC_DIR)/include

SSE_INCDIR = $(top_srcdir)/../../sse-pkg/include

INCLUDES= -I . -I$(DADDINCLUDE) \
	-I$(top_srcdir)/include -I$(SIGPROC_INCDIR) \
	-I$(top_srcdir)/lib -I$(SSE_INCDIR)
	

test_SOURCES = \
	test.cpp

DADD_LIBS = \
  -lpthread -lnsl \
  ../src/libDadd.a \
  -lfftw3f

LDADD = $(DADD_LIBS)
