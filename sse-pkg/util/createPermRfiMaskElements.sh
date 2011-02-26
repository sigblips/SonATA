#!/bin/sh
################################################################################
#
# File:    createPermRfiMaskElements.sh
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


# given a min, max dx tune range in mhz, 
# determine the perm rfi mask entry
# eg 
# input: 801 899
# min, max freqs would be 800 - 900
# (assuming 2 mhz wide dxs)
# so mask width is (900 - 800) = 100
# mask center is lowfreq + (width/2) = 850
# output: 850.0 100.0

gawk '
{ minFreq = $1 -1;
  maxFreq = $2 +1; 
  width = maxFreq - minFreq; 
  center = minFreq + (width/2.0);
  printf ("%.1f %.1f\n", center, width)
}
'
