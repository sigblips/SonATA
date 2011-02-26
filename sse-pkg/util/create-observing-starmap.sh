#!/bin/sh
################################################################################
#
# File:    create-observing-starmap.sh
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



# Look up the current observing information
# and create a starmap showing the ATA primary
# beam position. 

# debug
#set -x

# command line arg values
dbHost=""
dbName=""
outFile=""
timeoutMinutes="30"

# command line arg names
dbHostArgName="-dbhost"
dbNameArgName="-dbname"
outFileArgName="-outfile"
timeoutMinutesArgName="-timeoutminutes"
helpArgName="-help"

usage()
{
   echo "usage: $0 $dbHostArgName <dbhost> $dbNameArgName <dbname> \
$outFileArgName <outfile.gif> \
[${timeoutMinutesArgName} <timeout in minutes, default=${timeoutMinutes}>] \
[$helpArgName]"
   echo "Create starmap based on most recent observing information in database"
}

# process command line args
parseArgs()
{
   minArgs=6

    # check for minim number of args
   if [ $# -lt $minArgs ]
   then
       usage
       exit 1
   fi

   while [ "$1" ]
   do
      if [ "$1" = $dbHostArgName ]
      then

         if [ $# -lt 2 ]
         then
            echo "missing argument for $dbHostArgName"
            exit 1
         fi
         shift
         dbHost="$1"

      elif [ "$1" = $dbNameArgName ]
      then

         if [ $# -lt 2 ]
         then
            echo "missing argument for $dbNameArgName"
            exit 1
         fi
         shift
         dbName="$1"

      elif [ "$1" = $outFileArgName ]
      then
         if [ $# -lt 2 ]
         then
            echo "missing argument for $outFileArgName"
            exit 1
         fi
         shift
         outFile="$1"

      elif [ "$1" = $timeoutMinutesArgName ]
      then
         if [ $# -lt 2 ]
         then
            echo "Missing argument for $timeoutMinutesArgName"
            exit 1
         fi
         shift
         timeoutMinutes="$1"

      elif [ "$1" = $helpArgName ]
      then

         usage
         exit

      else

         echo "Invalid argument: $1"
         usage
         exit
      fi
    shift
done

}

createStarmap()
{
   # Get the current primary beam pointing position
   # and observing info from the observing database.
   # If the latest obs info is too old, then 
   # choose some arbitrary sky position.

   coordPrecision=3

   obsInfo=`mysql -h $dbHost --skip-column-names \
      -e "select ts, actId, \
      format(raHours,$coordPrecision), \
      format(decDeg,$coordPrecision) from TscopePointReq \
      where ataBeam = 'primary' and \
      ts > date_add(now(), interval -${timeoutMinutes} minute) \
      order by id desc limit 1;" $dbName`

   if [ "$?" -ne "0" ]
   then
      echo "$0: mysql query failed"
      exit 1
   fi

   #echo $obsInfo

   isTimedOut=0

   if [ "$obsInfo" != "" ]
   then
      date=`echo $obsInfo | cut -d" " -f1`
      time=`echo $obsInfo | cut -d" " -f2`
      actId=`echo $obsInfo | cut -d" " -f3`
      raHours=`echo $obsInfo | cut -d" " -f4`
      decDeg=`echo $obsInfo | cut -d" " -f5`

      #echo "raHours: $raHours"
      #echo "decDeg: $decDeg"

   else

      # no recent target info available
      # create a default map and label 'no active observations'
      isTimedOut=1

      actId=-1

      # big dipper
      raHours=12.000
      decDeg=65.000

   fi


   # create a starmap centered at the primary beam location
   create-starmap -rahours $raHours -decdeg $decDeg -crosshair -outfile $outFile

   # add title
   convert ${outFile} -fill white -undercolor black -gravity north \
      -pointsize 18 -annotate +0+0 \
      "ATA Primary Beam Pointing\n RA: $raHours hr  Dec: $decDeg deg" \
      ${outFile}

   # add date/time info
   updateTime=`date`

   convert ${outFile} -fill white -undercolor black -gravity south \
       -pointsize 14 -annotate +0+0 \
     "Last updated:  $updateTime   Act: $actId" \
     ${outFile}

   if [ $isTimedOut = 1 ]
   then
      convert ${outFile} -fill white -undercolor black -gravity center \
        -pointsize 36 -annotate +0+0 "Not Currently Observing" \
        ${outFile}
   fi

}

#-------------------------

parseArgs $@

createStarmap