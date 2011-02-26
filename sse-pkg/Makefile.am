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


AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

if DX_LIBS_ONLY
SUBDIRS = \
	include \
	sseutil \
	sseInterfaceLib \
	sseDxInterfaceLib \
	sseChannelizerInterfaceLib 
else	

SUBDIRS = \
	include \
	aaplusLib \
	wcsLib \
	novasLib \
	doppler \
	sseutil \
	GPIBInterfaceLib \
	sseInterfaceLib \
	sseTscopeInterfaceLib \
	sseDxInterfaceLib \
	sseChannelizerInterfaceLib \
	sseDxArchiverInterfaceLib \
	ssecommutil \
	simulators \
	tscope \
	tsig \
	ifc \
	dxArchiver \
	sse \
	setup \
	util \
	m4 \
	database \
	jseeker


endif

noinst_SCRIPTS = reconfig
EXTRA_DIST = reconfig


# Force the creation of the setup directory
setupdir = $(prefix)/setup/
setup_DATA =

localinstall:
	make install prefix=`pwd`/install

# create source code documentation
doxygen:
	doxygen doxygen.config


# empty targets to keep the build happy

reconfig:
