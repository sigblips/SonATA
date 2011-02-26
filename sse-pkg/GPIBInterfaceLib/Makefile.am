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

## Process this file with automake to produce Makefile.in

CXXFLAGS = @CXXFLAGS@ -Wall -Woverloaded-virtual
LDFLAGS = @GPIB_LDFLAGS@
LIBS = @GPIB_LIBS@ 
AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libGPIBInterface.a

PUBLIC_HEADER_FILES = \
        GPIBDevice.h \
        SCU.h \
	SignalGenerator.h \
	AttnBank.h

libGPIBInterface_a_SOURCES = \
        GPIBDevice.cpp \
        SCU.cpp \
        SignalGenerator.cpp \
	AttnBank.cpp

# public headers to include in 'make install' target
include_HEADERS = $(PUBLIC_HEADER_FILES)

INCLUDES= -I$(SSE_INC_DIR) -I$(GPIB_INTERFACE_INC_DIR) @GPIB_INCLUDES@
