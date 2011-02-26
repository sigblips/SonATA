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

## $Id: Makefile.am,v 1.1.1.1 2006/08/02 05:28:57 kes Exp $

## Process this file with automake to produce Makefile.in

# DEBUGFLAGS is a hook for easily adding flags on the make command line.

AS = nasm

CXXFLAGS = @CXXFLAGS@ -Woverloaded-virtual -D_REENTRANT $(DEBUGFLAGS)

# Use this for testing w/gprof
#CXXFLAGS = @CXXFLAGS@ -Woverloaded-virtual -D_REENTRANT -pg $(DEBUGFLAGS)

AUTOMAKE_OPTIONS = foreign

# define rule for assembling files
%.o:	%.s
	$(AS) -f elf -o $@ $<

bin_PROGRAMS = testBadBands

EXTRA_PROGRAMS =

EXTRA_DIST =

BUILT_SOURCES =

DX_LIB_DIR = $(top_srcdir)/../../lib
DX_LIB = $(DX_LIB_DIR)/libDx.a

DSP_INCDIR = $(top_srcdir)/../../../bw/include

BWS_INCDIR = $(top_srcdir)/../../../drivers/BlueWave/cddi

SSE_INCDIR = $(top_srcdir)/../../../../sse-pkg/include

SSE_INTERFACE_DIR = $(top_builddir)/../../../../sse-pkg/sseInterfaceLib
SSE_INTERFACE_INCDIR = $(top_srcdir)/../../../../sse-pkg/sseInterfaceLib
SSE_INTERFACE_LIB = $(SSE_INTERFACE_DIR)/libsseInterface.a

SSE_DX_INTERFACE_DIR = $(top_builddir)/../../../../sse-pkg//sseDxInterfaceLib
SSE_DX_INTERFACE_INCDIR = $(top_srcdir)/../../../../sse-pkg/sseDxInterfaceLib
SSE_DX_INTERFACE_LIB = $(SSE_DX_INTERFACE_DIR)/libsseDxInterface.a

SSE_FREELIB_DIR = $(top_builddir)/../../../../sse-pkg/freelib
SSE_FREELIB_INCDIR = $(top_srcdir)/../../../../sse-pkg/freelib
SSE_FREELIB_LIB = $(SSE_FREELIB_DIR)/libfree.la

SSE_UTIL_DIR = $(top_builddir)/../../../../sse-pkg/sseutil
SSE_UTIL_INCDIR = $(top_srcdir)/../../../../sse-pkg/sseutil
SSE_UTIL_LIB = $(SSE_UTIL_DIR)/libsseutil.a

# the following are third-party software packages
THIRD_PARTY_DIR = $(top_srcdir)/../../third-party

FFTW_DIR = $(THIRD_PARTY_DIR)/fftw
FFTW_INCDIR = $(FFTW_DIR)/include
FFTW_LIB_DIR = $(FFTW_DIR)/lib
FFTW_LIB = $(FFTW_LIB_DIR)/libfftw.a
#
# INTEL FFT library
#
# NOTE:
#	The intel fft library is now specified in
#	nss/pdm-pkg/pdm/configure.in
#
#	The library which will be used is specified
#	by the AC_CHECK_LIB macro. 
#	mkl_def is the generic library, mkl_p4 is for p4
#	machines, mkl_p3 is for p3 machines.
#
# As root execute: 
#	 export LD_LIBRARY_PATH=/opt/intel/mkl/lib/32
#

LIB_DEPENDS = $(DX_LIB) $(SSE_INTERFACE_LIB) $(SSE_DX_INTERFACE_LIB) \
		$(SSE_FREE_LIB) $(SSE_UTIL_LIB) 

testBadBands_DEPENDENCIES = $(LIB_DEPENDS)

DXINCLUDE = $(top_srcdir)/../../include
CPPUNIT_ROOT=/usr/local/CppUnit

INCLUDES= -I$(DXINCLUDE) -I$(DSP_INCDIR) -I$(BWS_INCDIR) \
	-I$(FFTW_INCDIR) -I$(SSE_INCDIR) -I$(SSE_INTERFACE_INCDIR) \
	-I$(SSE_DX_INTERFACE_INCDIR) -I$(SSE_FREELIB_INCDIR) \
	-I$(SSE_UTIL_DIR) \
	-I$(CPPUNIT_ROOT)/include -I$(top_srcdir)/include \
	-I/usr/include/tcl8.3 -I/usr/include/tcl8.2 \
	-I$(top_srcdir)/lib

DX_LIBS = \
  $(IFFT_LINK_OPTIONS) \
  -lpthread -lnsl \
  ../../lib/libDx.a \
  -L$(SSE_DX_INTERFACE_DIR) \
  -L$(SSE_INTERFACE_DIR) \
  -L$(SSE_FREELIB_DIR) \
  -L$(SSE_UTIL_DIR) \
  -L$(FFTW_LIB_DIR) \
  -lsseDxInterface \
  -lsseInterface \
  -lsseutil \
  -lfftw

LDADD = $(DX_LIBS)

testBadBands_SOURCES = testBadBands.cpp