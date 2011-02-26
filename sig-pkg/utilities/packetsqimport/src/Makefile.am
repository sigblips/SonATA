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

## $Id: Makefile.am,v 1.5 2009/07/01 16:56:52 jane Exp $

## Process this file with automake to produce Makefile.in

top_srcdir = ..
top_builddir = ..

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = packetsqimport

EXTRA_PROGRAMS =

EXTRA_DIST =

BUILT_SOURCES =

PACKETSQIMPORT_LIB_DIR = $(top_srcdir)/lib
PACKETSQIMPORT_LIB = $(PACKETSQIMPORT_LIB_DIR)/libDx.a

SIGPROC_DIR = $(top_srcdir)/../..
SIGPROC_INCDIR = $(SIGPROC_DIR)/include

SSE_INCDIR = $(top_srcdir)/../../../sse-pkg/include

SSE_INTERFACE_DIR = $(top_builddir)/../../../sse-pkg/sseInterfaceLib
SSE_INTERFACE_INCDIR = $(top_srcdir)/../../../sse-pkg/sseInterfaceLib

SSE_DX_INTERFACE_DIR = $(top_builddir)/../../../sse-pkg//sseDxInterfaceLib
SSE_DX_INTERFACE_INCDIR = $(top_srcdir)/../../../sse-pkg/sseDxInterfaceLib

# the following are packet headers
PKT_HDR_DIR = $(top_srcdir)/../../ATApackets
PKT_HDR_INCDIR = $(PKT_HDR_DIR)/include
PKT_HDR_LIBDIR = $(PKT_HDR_DIR)/src
PKT_LIB = $(PKT_HDR_LIBDIR)/libPkt.a

LIB_DEPENDS = $(PKT_LIB)

packetsqimport_DEPENDENCIES = $(LIB_DEPENDS)

PACKETSQIMPORT_INCLUDE = $(top_srcdir)/include

INCLUDES= -I$(PACKETSQIMPORT_INCLUDE) -I$(PKT_HDR_INCDIR) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib

PACKETSQIMPORT_LIBS = \
  -lpthread -lnsl \
  -L$(PKT_HDR_LIBDIR) -lPkt

LDADD = $(PACKETSQIMPORT_LIBS)

packetsqimport_SOURCES = packetsqimport.cpp 