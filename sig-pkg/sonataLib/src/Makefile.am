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

## $Id: Makefile.am,v 1.12 2009/07/12 01:50:06 kes Exp $

## Process this file with automake to produce Makefile.in

top_srcdir = ..
top_builddir = ..

AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES =	libSonata.a

PUBLIC_HEADER_FILES =

SIGPROC_DIR = $(top_srcdir)/..
SIGPROC_INCDIR = $(SIGPROC_DIR)/include

SSE_INCDIR = $(top_srcdir)/../../sse-pkg/include

SSE_INTERFACE_DIR = $(top_builddir)/../../sse-pkg/sseInterfaceLib
SSE_INTERFACE_INCDIR = $(top_srcdir)/../../sse-pkg/sseInterfaceLib

SSE_DX_INTERFACE_DIR = $(top_builddir)/../../sse-pkg/sseDxInterfaceLib
SSE_DX_INTERFACE_INCDIR = $(top_srcdir)/../../sse-pkg/sseDxInterfaceLib

# the following are packet headers
PKT_HDR_DIR = $(top_srcdir)/../ATApackets
PKT_HDR_INCDIR = $(PKT_HDR_DIR)/include

# SonATA library directories
SONATA_DIR = $(top_srcdir)/../sonataLib
SONATA_INCDIR = $(SONATA_DIR)/include

libSonata_a_SOURCES = \
	Alarm.cpp \
	BeamPacketList.cpp \
	Buffer.cpp \
	ChannelPacketList.cpp \
	Condition.cpp \
	Display.cpp \
	Err.cpp \
	ErrMsg.cpp \
	File.cpp \
	InputBuffer.cpp \
	Keyboard.cpp \
	Lock.cpp \
	Log.cpp \
	LogTask.cpp \
	Msg.cpp \
	Partition.cpp \
	QTask.cpp \
	Queue.cpp \
	ReadFilter.cpp \
	RWLock.cpp \
	Semaphore.cpp \
	Serial.cpp \
	SmallTypes.cpp \
	Task.cpp \
	Tcp.cpp \
	Udp.cpp \
	Util.cpp

# public headers to include in "make install" target
include_HEADERS =

check_programs =
TESTS =

EXTRA_PROGRAMS =

SONATA_INCLUDE = $(top_srcdir)/include
CCPUNIT_ROOT = /usr/local/CppUnit

INCLUDES = -I$(SONATA_INCDIR) -I$(PKT_HDR_INCDIR) -I$(SIGPROC_INCDIR) \
		-I$(SSE_INCDIR) -I$(SSE_DX_INTERFACE_INCDIR) \
		-I$(SSE_INTERFACE_INCDIR)
LIBLIST = -L.

LDADD = $(LIBLIST)
