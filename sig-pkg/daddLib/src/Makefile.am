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

## $Id: Makefile.am,v 1.5 2009/06/30 18:07:43 kes Exp $

## Process this file with automake to produce Makefile.in

top_srcdir = ..
top_builddir = ..

AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES =	libDadd.a

PUBLIC_HEADER_FILES =

SIGPROC_DIR = $(top_srcdir)/..
SIGPROC_INCDIR = $(SIGPROC_DIR)/include

SSE_DIR = $(top_srcdir)/../../sse-pkg
SSE_INCDIR = $(SSE_DIR)/include

SSE_INTERFACE_DIR = $(top_builddir)/../../sse-pkg/sseInterfaceLib
SSE_INTERFACE_INCDIR = $(SSE_INTERFACE_DIR)

libDadd_a_SOURCES = \
		Dadd.cpp \
		DaddSum.cpp \
		Print.cpp \
		Threshold.cpp \
		TopDown.cpp

# public headers to include in "make install" target
include_HEADERS =

check_programs =
TESTS =

EXTRA_PROGRAMS =

DADD_INCLUDE = ../include

INCLUDES = -I$(DADD_INCLUDE) -I$(SIGPROC_INCDIR) \
		-I$(SSE_INCDIR) -I$(SSE_INTERFACE_INCDIR)
LIBLIST = -L.

LDADD = $(LIBLIST)