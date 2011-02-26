################################################################################
#
# File:    tssMask.tcl
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

# TSS Mask
# Any star which has only these frequencies left will be skipped
# This is meant to include the mask used in TSS which is expected
# to include frequencies which are not skipped with the NSS
# TSS Permanent RFI Mask
set bandcovered  { 2100.00 1810.0 }
set masks  {
 1230	10.0
 1240	10.0
 1250	10.0
 1260	10.0
 1330	10.0
 1350	10.0
 1380   10.0
 1530	10.0
 1540	10.0
 1550	10.0
 1570	10.0
 1580	10.0
 1600   10.0
 1610 	10.0
 1690  	10.0
 1720 	10.0
 1730 	10.0
 1740   10.0
 1750	10.0
 2161.0 2.0
 2163.0 2.0
 2682   6.0
 2721.0  2.0
 2854.  6.0
}