#!/bin/sh
################################################################################
#
# File:    startssedd.sh
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


# Start up the waterfall & baseline display programs.

# Each xterm spawns a shell, and each executed
# program is followed by a shell, so that all
# the windows stick around until this script
# exits.  That way the windows remain up for
# debugging purposes in case any of the programs
# exit prematurely.

# process command line args

dxlist="$*"

if [ "$dxlist" = "" ]
then 
   echo " must give a list of dx names"
   exit
fi


# Determine the archive dir based on the SSE_ARCHIVE env var.
if [ "${SSE_ARCHIVE}" ]
then
    ARCHIVE_DIR="${SSE_ARCHIVE}"
else
    ARCHIVE_DIR="${HOME}/sonata_archive"
fi


baselineYpos=0
waterfallYpos=0

for dxName in ${dxlist} 
do

  echo "Starting displays for $dxName"


  # waterfalls

  # Start the waterfall Left display.
  # Put in in an xterm so it's easy to kill later on when we exit
  xterm -iconic -title "Waterfall"  -e sh -c "waterfallDisplay -file ${ARCHIVE_DIR}/confirmdata/${dxName}-L.compamp -xpos 0 -ypos ${waterfallYpos} -res 1; sh" &
  joblist="$joblist $!"

  waterfallYpos=`expr $waterfallYpos + 100`

  # Start the waterfall right display.
  # Put in in an xterm so it's easy to kill later on when we exit
    xterm -iconic -title "Waterfall"  -e sh -c "waterfallDisplay -file ${ARCHIVE_DIR}/confirmdata/${dxName}-R.compamp -xpos 0 -ypos ${waterfallYpos} -res 1; sh" &
  joblist="$joblist $!"

  waterfallYpos=`expr $waterfallYpos + 100`

  # baselines

  # Start the baseline Left display.
  # Put in in an xterm so it's easy to kill later on when we exit
  xterm -iconic -title "Baseline" -e sh -c "baselineDisplay -file ${ARCHIVE_DIR}/confirmdata/${dxName}-L.baseline -xpos 700 -ypos ${baselineYpos}; sh" &
  joblist="$joblist $!"

  baselineYpos=`expr $baselineYpos + 100`

  # Start the baseline right display.
  # Put in in an xterm so it's easy to kill later on when we exit
  xterm -iconic -title "Baseline" -e sh -c "baselineDisplay -file ${ARCHIVE_DIR}/confirmdata/${dxName}-R.baseline -xpos 700 -ypos ${baselineYpos}; sh" &
  joblist="$joblist $!"

  baselineYpos=`expr $baselineYpos + 100`


done


# stick around until user indicates we're done
echo "Starting data displays -- this may take a few seconds..."
echo "Press <return> to exit..."
read cmd

# kill all the backgrounded jobs 
kill -9 $joblist > /dev/null

