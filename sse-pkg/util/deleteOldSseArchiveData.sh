#!/bin/sh
################################################################################
#
# File:    deleteOldSseArchiveData.sh
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


#*************************
# Update this for SonATA
#*************************

# Delete archive data from the sonata_archive
# directory that is older than a given number of
# days.
# Only deletes the data that is stored in the
# sonata_archive YYYY-MM-DD subdirectories.

# Options:  -delete  really delete the data
# (Default is just to report the data that would be deleted).


# Find the directory this script is in, so that we 
# can look there for the auxiliary programs that
# do the time checks.
dir=`dirname $0` 
whereIam=`(cd $dir;pwd)`

# set the paths so this can be run as a cron job
# and export them so they can be found.

PATH="/usr/bin:/usr/local/bin:${whereIam}"
export PATH

LD_LIBRARY_PATH="/usr/local/gcc3.3/lib:/usr/local/lib"

# configure adds a run time dependency on the existence
# of these libraries, even though this script does not use them:

LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/NI_GPIB_ENET:/usr/local/STLport-4.5.3/lib"
export LD_LIBRARY_PATH

# Anything older than this ageoff time will be deleted
ageOffTimeInDays=4


archiveDir="${HOME}/sonata_archive"

reallyDelete="false"

# process command line args
while [ "$1" ]
do
   if [ "$1" = "-delete" ]
   then
      reallyDelete="true"
   else
      echo "Invalid argument"
      echo "Usage: $0 [-delete]"
      echo "-delete: really delete old archive files"
      echo "(otherwise just report which ones would be deleted)"
      exit
   fi
   shift

done


# TBD check for SSE_ARCHIVE env var

if [ ! -d ${archiveDir} ]
then
    echo "Archive directory ${archiveDir} does not exist."
    exit
fi

cd ${archiveDir}


# convert age off time in days to seconds
secsPerDay=86400
ageOffTimeInSecs=`expr ${ageOffTimeInDays} \* ${secsPerDay}`

#echo "ageOffTimeInSecs = $ageOffTimeInSecs"

# get current time in seconds since the UNIX epoch
currentTime=`currentTimeInSecs`

#echo "currentTimeInSecs=$currentTime"

# get the list of archive subdirectories.
# filename is of the form YYYY-MM-DD

dirlist=`ls -d [0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9] 2>/dev/null`

if [ "${dirlist}" = "" ]
then
    #echo "No archive files found in ${archiveDir} "
    exit
fi

if [ ${reallyDelete} = "false" ]
then
    echo "Delete function is disabled, checking files only"
fi

for dir in ${dirlist} 
do

    # see if the last file modification time on this 
    # directory is older than the current time
    # by the ageOffTimeInSecs.  If so, recursively 
    # delete it

    #echo ${dir}

    fileModTime=`fileModTimeInSecs $dir`
    timeDiffSecs=`expr ${currentTime} - ${fileModTime}`

    if [ ${timeDiffSecs} -gt ${ageOffTimeInSecs} ]
    then
        if [ ${reallyDelete} = "true" ]
	then
	   rm -fr ${dir}
	   #echo "remove disabled, would have deleted ${dir}"
	else
	   echo "$dir is $timeDiffSecs secs old, would be deleted"
	fi
    else

        if [ ${reallyDelete} = "false" ]
	then
	   echo "$dir is $timeDiffSecs secs old, would be saved"
	fi
    fi

done
