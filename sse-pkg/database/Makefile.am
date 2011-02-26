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

bin_PROGRAMS = \
	createAllskyStarcat \
	createRaDecRegionTargetCat \
	createEclipticSurveyTargetCat \
	createGalSurveyTargetCat \
	createGalSurveyGridOutline \
	convertTychoToTargetCat \
	convertGalGridToTargetCat \
	convertSimbadToTargetCat \
	convertReconsToTargetCat \
	targetgrid

dist_bin_SCRIPTS = \
	createSpacecraftCatalog

# Reports is obsolete
#SUBDIRS = Reports

EXTRA_DIST = \
	seeker.schema \
	seeker.spacecraft.data \
	seeker.ao-starlist.data 

# If desired, override the default LIBS setting so that
# programs don't pick up any dependencies on tcl etc.
# that are not needed.

#LDADD=
#LIBS=
#AM_CXXFLAGS = -Wall

createAllskyStarcat_SOURCES = createAllskyStarcat.cpp
createRaDecRegionTargetCat_SOURCES = createRaDecRegionTargetCat.cpp
createGalSurveyTargetCat_SOURCES = createGalSurveyTargetCat.cpp
createGalSurveyGridOutline_SOURCES = createGalSurveyGridOutline.cpp
createEclipticSurveyTargetCat_SOURCES = createEclipticSurveyTargetCat.cpp
convertTychoToTargetCat_SOURCES = convertTychoToTargetCat.cpp
convertGalGridToTargetCat_SOURCES = convertGalGridToTargetCat.cpp
convertSimbadToTargetCat_SOURCES = convertSimbadToTargetCat.cpp
convertReconsToTargetCat_SOURCES = convertReconsToTargetCat.cpp
targetgrid_SOURCES = targetgrid.cpp

targetgrid_DEPENDENCIES = $(SSEUTIL_LIB)
createEclipticSurveyTargetCat_DEPENDENCIES = $(SSECOMMUTIL_LIB)
convertSimbadToTargetCat_DEPENDENCIES = $(SSEUTIL_LIB)
convertReconsToTargetCat_DEPENDENCIES = $(SSEUTIL_LIB)
convertTychoToTargetCat_DEPENDENCIES = $(SSEUTIL_LIB)
convertGalGridToTargetCat_DEPENDENCIES = $(SSEUTIL_LIB)
createGalSurveyTargetCat_DEPENDENCIES = $(AAPLUS_LIB) $(WCS_LIB) $(NOVAS_LIB)
createGalSurveyGridOutline_DEPENDENCIES = $(AAPLUS_LIB) $(WCS_LIB) $(NOVAS_LIB)

INCLUDES=-I$(SSEUTIL_INC_DIR) -I$(SSECOMMUTIL_INC_DIR) -I$(SSE_INC_DIR) 
LDADD = $(SSEUTIL_LIB) $(SSECOMMUTIL_LIB) $(AAPLUS_LIB) $(WCS_LIB) $(NOVAS_LIB)
