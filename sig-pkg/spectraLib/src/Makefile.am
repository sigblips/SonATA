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

## $Id: Makefile.am,v 1.5 2009/06/30 22:53:56 kes Exp $

## Process this file with automake to produce Makefile.in

top_srcdir = ..
top_builddir = ..

AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES =	libSpectra.a

PUBLIC_HEADER_FILES =

SIGPROC_DIR = $(top_srcdir)/..
SIGPROC_INCDIR = $(SIGPROC_DIR)/include

SSE_INCDIR = $(top_srcdir)/../../sse-pkg/include

SSE_INTERFACE_DIR = $(top_builddir)/../../sse-pkg/sseInterfaceLib
SSE_INTERFACE_INCDIR = $(top_srcdir)/../../sse-pkg/sseInterfaceLib
SSE_INTERFACE_LIB = $(SSE_INTERFACE_DIR)/libsseInterface.a

libSpectra_a_SOURCES = \
		Print.cpp \
		Spectra.cpp

# public headers to include in "make install" target
include_HEADERS =

check_programs =
TESTS =

EXTRA_PROGRAMS =

SPECTRA_INCLUDE = ../include

INCLUDES = -I$(SPECTRA_INCLUDE) -I$(SIGPROC_INCDIR) -I$(SSE_INCDIR)
LIBLIST = -L.

LDADD = $(LIBLIST)