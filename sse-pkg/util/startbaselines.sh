#!/bin/sh
################################################################################
#
# File:    startbaselines.sh
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


# Start SSE baseline display programs
# for the specified pol, and the specified dxs

# Each xterm spawns a shell, and each executed
# program is followed by a shell, so that all
# the windows stick around until this script
# exits.  That way the windows remain up for
# debugging purposes in case any of the programs
# exit prematurely.

# process command line args

usage()
{
   echo "Usage: $0 <-left|-right> <dxName> <dxName>..."
}

if [ "$1" = "-left" ]
then
    pol="L"
elif [ "$1" = "-right" ]
then
    pol="R"
else
    echo "$0: first arg must be '-left' or '-right'"
    usage
    exit
fi

shift

dxlist="$*"

if [ "$dxlist" = "" ]
then 
   echo "$0: Must give a list of dx names"
   usage
   exit
fi

# Determine the archive dir based on the SSE_ARCHIVE env var.
if [ "${SSE_ARCHIVE}" ]
then
    ARCHIVE_DIR="${SSE_ARCHIVE}"
else
    ARCHIVE_DIR="${HOME}/sonata_archive"
fi

# baseline window dimensions
windowWidth=648
windowHeight=220

# widescreen width
screenWidth=1920

# try to tile the baseline displays

baselineYpos=0
baselineXpos=0
for dxName in ${dxlist} 
do



     # Start the baseline display for the requested pol
     # Put in in an xterm so it's easy to kill later on when we exit
     xterm -iconic -title "Baseline" -e sh -c "baselineDisplay -file ${ARCHIVE_DIR}/confirmdata/${dxName}-${pol}.baseline -xpos ${baselineXpos} -ypos ${baselineYpos} -height ${windowHeight} -width ${windowWidth}; sh" &

     joblist="$joblist $!"

     baselineXpos=`expr ${baselineXpos} + ${windowWidth}`
     if [ ${baselineXpos} -ge ${screenWidth} ]
     then
	    # wrap to new row
	    baselineYpos=`expr ${baselineYpos} + ${windowHeight}`
	    baselineXpos=0
     fi

done


# stick around until user indicates we're done
echo "Starting ${pol} pol baseline data displays -- this may take a few seconds..."
echo "Press <return> to exit..."
read cmd

# kill all the backgrounded jobs 
kill -9 $joblist > /dev/null

