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

## $Id: Makefile.am,v 1.1 2009/06/26 20:21:38 kes Exp $

## Process this file with automake to produce Makefile.in

top_srcdir = ..
top_builddir = ..

AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES =	libPkt.a libSup.a

PUBLIC_HEADER_FILES =

libPkt_a_SOURCES = \
		ATADataPacketHeader.cpp \
		BeamPacket.cpp \
		ChannelPacket.cpp

libSup_a_SOURCES = \
		basics.cpp

# public headers to include in "make install" target
include_HEADERS =

check_programs =
TESTS =

EXTRA_PROGRAMS =

PKT_INCLUDE = $(top_srcdir)/include

SUP_INCLUDE = $(top_srcdir)/include

INCLUDES = -I$(PKT_INCLUDE) -I$(SUP_INCLUDE)

LIBLIST = -L.

LDADD = $(LIBLIST)
