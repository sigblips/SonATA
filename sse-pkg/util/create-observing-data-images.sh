#!/bin/sh
################################################################################
#
# File:    create-observing-data-images.sh
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
# and create baseline and waterfall images for
# some of the dxs.
# Output images are named baselineX.jpeg
# and waterfallX.jpeg, where X runs from 1 up to
# the max number of dxs requested.

# debug
#set -x

# command line arg values
dbHost=""
dbName=""
outDir="."
maxDxs="3"
timeoutMinutes="30"

# command line arg names
dbHostArgName="-dbhost"
dbNameArgName="-dbname"
outDirArgName="-outdir"
maxDxsArgName="-maxdxs"
timeoutMinutesArgName="-timeoutminutes"
helpArgName="-help"

usage()
{
   echo "usage: $0 $dbHostArgName <dbhost> $dbNameArgName <dbname> \
[${outDirArgName} <output directory, default is '.'>] \
[${maxDxsArgName} <max # dxs, default=3>] \
[${timeoutMinutesArgName} <timeout in minutes, default=${timeoutMinutes}>] \
[$helpArgName]"
   echo "Create baseline and waterfall images for some of the dxs listed in the most recent observation in the database."
}

# process command line args
parseArgs()
{
   while [ "$1" ]
   do
      if [ "$1" = $dbHostArgName ]
      then

         if [ $# -lt 2 ]
         then
            echo "Missing argument for $dbHostArgName"
            exit 1
         fi
         shift
         dbHost="$1"

      elif [ "$1" = $dbNameArgName ]
      then

         if [ $# -lt 2 ]
         then
            echo "Missing argument for $dbNameArgName"
            exit 1
         fi
         shift
         dbName="$1"

      elif [ "$1" = $outDirArgName ]
      then
         if [ $# -lt 2 ]
         then
            echo "Missing argument for $outDirArgName"
            exit 1
         fi
         shift
         outDir="$1"

      elif [ "$1" = $maxDxsArgName ]
      then
         if [ $# -lt 2 ]
         then
            echo "Missing argument for $maxDxsArgName"
            exit 1
         fi
         shift
         maxDxs="$1"

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

   # validate args

   if [ "$dbHost" = "" ]
   then
       echo "Must supply $dbHostArgName argument"
       usage
       exit
   fi 

   if [ "$dbName" = "" ]
   then
       echo "Must supply $dbNameArgName argument"
       usage
       exit
   fi 

}

createBaselinesAndWaterfalls()
{
   # Create a waterfall image using data from a recently
   # completed activity.

   # Query the database for a date and act number.
   # Use that to get the path to one of the .compamp
   # or .archive-compamp confirmation data files in the
   # SSE_ARCHIVE directory, and turn it into a gif image.

   # If the latest obs info is too old, then 
   # create an empty image and label it as such.  

   obsInfo=`mysql -h $dbHost --skip-column-names \
     -e "select ts, activityId from ActivityUnits \
     where validObservation = 'Yes' and \
     ts > date_add(now(), interval -${timeoutMinutes} minute) \
     order by id desc limit 1;" $dbName`

   if [ "$?" -ne "0" ]
   then
      echo "$0: mysql query failed"
      exit 1
   fi

   #echo $obsInfo

   if [ "$obsInfo" != "" ]
   then

      # create baselines & waterfalls

      date=`echo $obsInfo | cut -d" " -f1`
      time=`echo $obsInfo | cut -d" " -f2`
      actId=`echo $obsInfo | cut -d" " -f3`

      # get dx numbers for this activity
      dxNumbers=`mysql -h $dbHost --skip-column-names \
        -e "select dxNumber from ActivityUnits \
        where activityId = $actId and validObservation = 'Yes'\
        order by rand() limit $maxDxs;" $dbName`

     if [ "$?" -ne "0" ]
     then
        echo "$0: mysql query failed"
        exit 1
     fi


     #echo "dxNumbers: $dxNumbers"

     archiveSubdir="$HOME/sonata_archive/$date/act$actId"
     #echo $archiveDir

     pol="L"
     maxRealDxId=999
     count=1
     for dx in $dxNumbers 
     do
        if [ $dx -gt $maxRealDxId ] 
        then
           dxPrefix="dxsim"
        else
           dxPrefix="dx"
        fi

        #TBD: check that files exist

        title="$date $time UTC ActId:$actId DX:$dx Pol:${pol}"  

	# might be more than 1 file with same act number and dx,
	# so grab the latest

	dxPol="${dxPrefix}${dx}.${pol}"

	waterFile=`ls -rt $archiveSubdir/*.${dxPol}.compamp | tail -1`

        waterfallDisplayHeadless -file $waterFile \
           -batch -res 2 -outfile waterfall${count}.jpeg \
           -title "${title}"

	baseFile=`ls -rt $archiveSubdir/*.${dxPol}.baseline | tail -1`

        baselineImage -infile $baseFile \
           -outfile baseline${count}.jpeg \
           -title "${title}"

        count=`expr $count + 1`

      done

   else

      # no recent target info available
      # create 'no data available' images

      #echo "no recent data available"

      date=`date`

      count=1
      while [ $count -le $maxDxs ]
      do

         convert -background black -fill white -size 539x250 \
            -gravity center label:"$date\nNot Currently Observing" \
            baseline${count}.jpeg

         convert -background black -fill white -size 539x170 \
            -gravity center label:"$date\nNot Currently Observing" \
            waterfall${count}.jpeg

         count=`expr $count + 1`

      done

   fi

}

#-------------------------

parseArgs $@

cd $outDir

createBaselinesAndWaterfalls
