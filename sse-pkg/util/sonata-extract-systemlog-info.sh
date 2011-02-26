#!/bin/sh
################################################################################
#
# File:    sonata-extract-systemlog-info.sh
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


# extract the essential activity information
# from the sonata system log

#   print activity separator line
#   Act number, type (from Activity Started line)
#   dx min/max freq
#   total number of confirmed candidates (from a regular star observation)
#   Data requests for candidates sent to dxs for followups
#   Total number of signals sent to dxs for followups

# assume input comes from stdin

gawk '
BEGIN {
	actid = 0	
	dxName = ""
	actName = ""
}
/Activity Started/ { print "======================================================================="; print $0 }
/TargetId/
/Follow up of act/
/Dx Min\/Max Freq/
/Data request/
/total candidates sent to dxs for followup/
/Pending Data Collection/ { print $0;  print "=======================================================================" }

# pull out activity id & dx name from the beginning
# of the signal report summary


/# Activity Id/ { actid = $5 }
/Dx Name/ { dxName = $5 }


# look for confirmed signals & their resolution class reasons
# 1=freq, 2=sigtype, 5=drift, 8=pol, 9=BB 10=class, 11=reason, 13=snr
# reason = MConfrm or  MReCfrm


NF > 10 && ($11 == "Confrm" || $11 == "RConfrm" || $11 == "NtSnOff" || $11 == "NoSignl" || $11 == "SeenOff" || $11 == "SnMulBm" ) {

 print "Act:", actid, dxName, $0

}

# resolution for followups only 

/# Activity Name:/ { actName = $4 }

NF > 10 && ($11 == "RctRFI" || $11 == "ZeroDft" || $11 == "FdCohD") {

   if (actName == "OnStar" || actName == "OffStar") {
	print "Act:", actid, dxName, $0
   }
}




/Star#/ { print "======================================================================="; print $0 }
' 