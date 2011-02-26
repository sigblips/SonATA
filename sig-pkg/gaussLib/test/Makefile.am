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

## $Id: Makefile.am,v 1.5 2009/07/06 22:16:15 jane Exp $

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

SIGPROC_DIR = $(top_srcdir)/..
SIGPROC_INCDIR = $(SIGPROC_DIR)/include

SONATA_DIR = $(top_srcdir)/../sonataLib
SONATA_INCDIR = $(SONATA_DIR)/include
SONATA_LIBDIR = $(SONATA_DIR)/src
SONATA_LIB = $(SONATA_LIBDIR)/libSonata.a

SSE_DX_INTERFACE_INCDIR = $(top_srcdir)/../../sse-pkg/include
SSE_DX_INTERFACE_LIBDIR = $(top_srcdir)/../../sse-pkg/sseDxInterfaceLib
SSE_DX_INTERFACE_LIB = $(SSE_DX_INTERFACE_LIBDIR)/libsseDxInterface.a

SSE_INTERFACE_LIBDIR = $(top_srcdir)/../../sse-pkg/sseInterfaceLib
SSE_INTERFACE_LIB = $(SSE_INTERFACE_LIBDIR)/libsseInterface.a

GAUSS_LIBDIR = ../src
GAUSS_LIB = $(GAUSS_LIBDIR)/libGauss.a

LIB_DEPENDS = $(GAUSS_LIB)

test_DEPENDENCIES = $(LIB_DEPENDS)

GAUSS_INCLUDE = ../include

INCLUDES= -I. -I$(GAUSS_INCLUDE) -I$(SONATA_INCDIR) -I$(SIGPROC_INCDIR) \
		-I$(SSE_DX_INTERFACE_INCDIR)
	
test_SOURCES = \
	test.cpp \
	userSignals.cpp \
	userSignals.h

GAUSS_LIBS = \
  -lpthread -lnsl \
  $(GAUSS_LIB) \
  $(SONATA_LIB) \
  $(SSE_DX_INTERFACE_LIB) \
  $(SSE_INTERFACE_LIB)

LDADD = $(GAUSS_LIBS)
