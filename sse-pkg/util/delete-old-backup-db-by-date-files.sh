#!/bin/sh
################################################################################
#
# File:    delete-old-backup-db-by-date-files.sh
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



# delete files created by the backup-db-by-date script
# that are older than the cutoff age (default 7 days)

# command line arg names
backupDirArgName="-dir"
ageLimitDaysArgName="-agedays"
deleteFilesArgName="-delete"

# defaults

ageLimitDays=7
backupDir=""
reallyDelete="false"

usage()
{
   echo "delete db backup files older than N days"
   echo "usage: $0 $backupDirArgName <dirname> [$ageLimitDaysArgName <days>] [$deleteFilesArgName]"
   echo "Note: if $deleteFilesArgName is not set, then just shows files that would be deleted"
}


# process command line args
while [ "$1" ]
do
   if [ "$1" = $backupDirArgName ]
   then
      if [ $# -lt 2 ]
      then
         echo "missing argument for $backupDirArgName"
         exit 1
      fi

      shift
      backupDir="$1"

   elif [ "$1" = $ageLimitDaysArgName ]
   then
      if [ $# -lt 2 ]
      then
         echo "missing argument for $ageLimitDaysArgName"
         exit 1
      fi

      shift
      ageLimitDays="$1"
   elif [ "$1" = $deleteFilesArgName ]
   then
      reallyDelete="true"      
   else
      echo "Invalid argument: $1"
      usage
      exit
   fi
   shift
done

#echo "ageLimit: $ageLimitDays"
#echo "backupDir: $backupDir"
#echo "reallyDelete: $reallyDelete"

if [ "$backupDir" = "" ]
then
   echo "$0: must specify $backupDirArgName argument"
   exit 
fi

if [ ! -d $backupDir ]
then
   echo "$0: can't access directory $backupDir"
   exit
fi

action="ls"
if [ $reallyDelete = "true" ]
then
   action="rm"
fi

#  allow time limit to go to zero
timeLimit="-mtime +${ageLimitDays}"
if [ "$ageLimitDays" -eq 0 ]
then
    timeLimit=""
fi

backupFilenameIdString="mysql-db-backup.tar.gz"

find $backupDir $timeLimit -name \*${backupFilenameIdString} -exec $action {} \;







