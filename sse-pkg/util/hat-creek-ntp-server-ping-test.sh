#!/bin/sh
################################################################################
#
# File:    hat-creek-ntp-server-ping-test.sh
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


# ping the ntp server machines at hat creek to
# see if they are accessible

runPing()
{
    host=$1
    #host $host

    # -c <count request packets>
    # -w <timeout in secs>
    result=`ping -c1 -w2 $host`

    if [ $? -eq 0 ]
    then
       echo "$host (ntp server) is alive"
    else
       echo "$host (ntp server) can't be reached"
    fi
}

#echo "NTP servers"

date
echo ""

runPing ntp1-1.hcro.org