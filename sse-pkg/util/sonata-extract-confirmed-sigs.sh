#!/bin/sh
################################################################################
#
# File:    sonata-extract-confirmed-sigs.sh
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


# gawk the sonata system log for confirmed signals
# input comes from stdin

gawk '

BEGIN {
	actid = 0	
	dxname = ""
}

# pull out activity id & dx name from the beginning
# of the signal report summary


/# Activity Id/ { actid = $5 }
/Dx Name/ { dxname = $5 }


# look for confirmed signals 
# 1=freq, 2=sigtype, 5=drift, 8=pol, 9=BB 10=class, 11=reason, 13=snr
# reason = MConfrm or  MReCfrm

NF > 10 && ($11 == "Confrm" || $11 == "RConfrm") {

 print "act", actid, dxname, $0

}


# signal report looks like this:
# (except for the added pound symbols in front of the signals)

# ==================
# SonATA Signal Reports
# ==================
# Activity Name: OneSiteIfTest
# Activity Id  : 5508
# Creation Time: 2003-09-26 00:03:17 UTC
# Report Type  : Candidates & Candidate Results (condensed format)
# Dx Name     : dx33
# Dx SkyFreq: 1420.000000 MHz
# Ifc SkyFreq: 1420.000000 MHz
# Dx Assigned Bandwidth: 1.736111 MHz (2560 subchannels) 
# Dx Band Edges: 1419.131605 MHz -> 1420.867716 MHz


#Freq           Sig Sub  Signal Drift    Width Power Pol BB Clss Reason  PFA    SNR
#MHz            Typ band Id     Hz/S     Hz
#-------------- --- ---- ------ -------- ----- ----- --- -- ---- ------- ------ -----

#1419.358336_131 CwP  334      0  0.09937 6.623   728   L N Cand PsMPwrT 
#1420.400002_797 CwP 1870      1  0.09937 6.623   728   R N Cand PsMPwrT 
#1419.358335_970 CwC  334      0  0.09984 0.005  3604   L Y Cand MConfrm -3604.34  9.32 
#1420.400002_642 CwC 1870      1  0.09984 0.010  3908   R Y Cand MConfrm -3899.61 10

'
