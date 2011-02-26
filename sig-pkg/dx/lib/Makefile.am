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

## $Id: Makefile.am,v 1.13 2009/06/30 22:51:16 kes Exp $

## Process this file with automake to produce Makefile.in

top_srcdir = ..
top_builddir = ..

AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES =	libDx.a

PUBLIC_HEADER_FILES =

SIGPROC_DIR = $(top_srcdir)/..
SIGPROC_INCDIR = $(SIGPROC_DIR)/include

SSE_INCDIR = $(top_srcdir)/../../sse-pkg/include

SSE_INTERFACE_DIR = $(top_builddir)/../../sse-pkg/sseInterfaceLib
SSE_INTERFACE_INCDIR = $(top_srcdir)/../../sse-pkg/sseInterfaceLib

SSE_DX_INTERFACE_DIR = $(top_builddir)/../../sse-pkg/sseDxInterfaceLib
SSE_DX_INTERFACE_INCDIR = $(top_srcdir)/../../sse-pkg/sseDxInterfaceLib

# DFB library directories
DFB_DIR = $(top_srcdir)/../dfbLib
DFB_INCDIR = $(DFB_DIR)/include

# the following are packet headers
PKT_HDR_DIR = $(top_srcdir)/../ATApackets
PKT_HDR_INCDIR = $(PKT_HDR_DIR)/include

# SonATA library directories
SONATA_DIR = $(top_srcdir)/../sonataLib
SONATA_INCDIR = $(SONATA_DIR)/include

# DADD library directories
DADD_DIR = $(top_srcdir)/../daddLib
DADD_INCDIR = $(DADD_DIR)/include

# Spectra library directories
SPECTRA_DIR = $(top_srcdir)/../spectraLib
SPECTRA_INCDIR = $(SPECTRA_DIR)/include

libDx_a_SOURCES = \
	Activity.cpp \
	ArchiveChannel.cpp \
	Args.cpp \
	BadBand.cpp \
	BadBandList.cpp \
	BirdieMask.cpp \
	Channel.cpp \
	ChildClusterer.cpp \
	CwBadBandList.cpp \
	CwClusterer.cpp \
	CwFollowupSignal.cpp \
	CwSignal.cpp \
	DxErrMsg.cpp \
	DxUtil.cpp \
	FrequencyMask.cpp \
	PermRfiMask.cpp \
	PulseBadBandList.cpp \
	PulseClusterer.cpp \
	PulseFollowupSignal.cpp \
	PulseSignal.cpp \
	RecentRfiMask.cpp \
	Signal.cpp \
	SignalIdGenerator.cpp \
	State.cpp \
	Statistics.cpp \
	SuperClusterer.cpp \
	SubchannelMask.cpp \
	TestSignalMask.cpp \
	TransformWidth.cpp \
	WorkQ.cpp

# public headers to include in "make install" target
include_HEADERS =

check_programs =
TESTS =

EXTRA_PROGRAMS =

DX_INCDIR = $(top_srcdir)/include
#CCPUNIT_ROOT = /usr/local/CppUnit

INCLUDES = -I$(DX_INCDIR) -I$(SONATA_INCDIR) -I$(PKT_HDR_INCDIR) \
		-I$(DFB_INCDIR) -I$(SPECTRA_INCDIR) -I$(DADD_INCDIR)\
		-I$(SIGPROC_INCDIR) -I$(SSE_INCDIR) -I$(SSE_DX_INTERFACE_INCDIR) \
		-I$(SSE_INTERFACE_INCDIR)
LIBLIST = -L.

LDADD = $(LIBLIST)
