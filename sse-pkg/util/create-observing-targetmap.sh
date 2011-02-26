#!/bin/sh
################################################################################
#
# File:    create-observing-targetmap.sh
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



# Look up the current observing information (or for a specific activity Id)
# and create a skymap showing the ATA primary
# beam and the associated targets (synthesized beam pointings).

# debug
#set -x

# command line arg values
dbHost=""
dbName=""
outFile=""
timeoutMinutes=30

# command line arg names
dbHostArgName="-dbhost"
dbNameArgName="-dbname"
outFileArgName="-outfile"
timeoutMinutesArgName="-timeoutminutes"
actIdArgName="-actid"
helpArgName="-help"

primaryRaHours=0.0
primaryDecDeg=0.0

target1RaHours=0.0
target1DecDeg=999  

target2RaHours=0.0
target2DecDeg=999  

target3RaHours=0.0
target3DecDeg=999  

userGaveActId="false"
requestedActId=-1
actIdUsed=-1
defaultSkyFreqMhz=1420
skyFreqMhz=${defaultSkyFreqMhz}

usage()
{
   echo "usage: $0 $dbHostArgName <dbhost> $dbNameArgName <dbname> \
$outFileArgName <outfile.gif> \
[${timeoutMinutesArgName} <timeout in minutes, default=${timeoutMinutes}>] \
[${actIdArgName} <id>] [$helpArgName]"
   echo "Create a skymap showing target selections for the most recent activity"
   echo "in the database (subject to timeout) or by a selected activity ID."
}

# process command line args
parseArgs()
{
   if [ $# -eq 0 ]
   then
      usage
      exit
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

      elif [ "$1" = $actIdArgName ]
      then

         if [ $# -lt 2 ]
         then
            echo "Missing argument for $actIdArgName"
            exit 1
         fi
         shift
         userGaveActId="true"
         requestedActId="$1"

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

checkForRequiredArgs()
{

   if [ "$dbHost" = "" ]
   then
      echo "$0: must specify $dbHostArgName"
      usage
      exit 1
   fi

   if [ "$dbName" = "" ]
   then
      echo "$0: must specify $dbNameArgName"
      usage
      exit 1
   fi

   if [ "$outFile" = "" ]
   then
      echo "$0: must specify $outFileArgName"
      usage
      exit 1
   fi

}

extractBeamPointings()
{
   nArgsPerLine=3
   targetCount=0

   while [ $# -gt 0 ]
   do
      ataBeam=$1
      raHours=$2
      decDeg=$3

      #echo "ataBeam: $ataBeam"
      #echo "raHours: $raHours"
      #echo "decDeg: $decDeg"

      if [ $ataBeam = 'primary' ]
      then
         primaryRaHours=$raHours
         primaryDecDeg=$decDeg
      else
         targetCount=`expr ${targetCount} + 1`

         if [ $targetCount -eq 1 ]
         then
            target1RaHours=$raHours
            target1DecDeg=$decDeg
         elif [ $targetCount -eq 2 ]
         then
            target2RaHours=$raHours
            target2DecDeg=$decDeg
         elif [ $targetCount -eq 3 ]
         then
            target3RaHours=$raHours
            target3DecDeg=$decDeg
         fi
      fi

      shift $nArgsPerLine
   done

}

# Use bc to perform a calculation, returning the result
calculate()
{
   formula=$*
   numDecimalDigits=5

   echo "scale = ${numDecimalDigits}; ${formula}" | bc
}

getActIdAndSkyFreq()
{
   if [ ${userGaveActId} = "true" ]
   then
       actSelection="id = ${requestedActId}"
   else
       # select by time
       actSelection="ts > date_add(now(), interval -${timeoutMinutes} minute)" 
   fi

   query="select id, minDxSkyFreqMhz from Activities \
      where ${actSelection} and minDxSkyFreqMhz is not null \
      order by id desc limit 1"

   actInfo=`mysql -h $dbHost --skip-column-names -e"$query" $dbName`

   if [ "$?" -ne "0" ]
   then
      echo "$0: mysql query failed: $query"
      exit 1
   fi

   if [ "$actInfo" != "" ]
   then
      actIdUsed=`echo $actInfo | cut -d" " -f1`
      skyFreqMhz=`echo $actInfo | cut -d" " -f2`

      #echo "actIdUsed: $actIdUsed"
      #echo "skyFreqMhz: $skyFreqMhz"
      
      if [ ${skyFreqMhz} -lt 1 ]
      then
          # not a valid observing activity
          actIdUsed=-1
          skyFreqMhz=${defaultSkyFreqMhz}
      fi

   else

      if [ ${userGaveActId} = "true" ]
      then
         echo "$0: target information for requested activity Id \
${requestedActId} not found in database"
         exit 1
      fi

      # timed out, use dummy info
      actIdUsed=-1
      skyFreqMhz=${defaultSkyFreqMhz}
   fi

}


createTargetmap()
{
   # Get primary beam pointing position,
   # target (synth beam) pointings,
   # and observing info from the observing database.
   # If a specific activity ID is requested, then 
   # use that to select the pointing information.
   # Otherwise, try to get the most recent activity.
   # If the latest obs info is too old, then 
   # choose some arbitrary sky position
   # and label the map appropriately.

   # For synth beam targets, assume x & y beams
   # are pointing in the same place, i.e., only
   # plot the coordinates of one pol of the pair.

   # determine act Id and sky freq  
   getActIdAndSkyFreq

   coordPrecision=3

   obsInfo=`mysql -h $dbHost --skip-column-names \
      -e "select ataBeam, \
      format(raHours,$coordPrecision), \
      format(decDeg,$coordPrecision) from TscopePointReq \
      where (ataBeam = 'primary' or atabeam like 'beamx%') and \
      actId = ${actIdUsed}" $dbName`

   if [ "$?" -ne "0" ]
   then
      echo "$0: mysql query failed"
      exit 1
   fi

   #echo "obsInfo: $obsInfo"

   noTargetInfo=0

   if [ "$obsInfo" != "" ]
   then

      extractBeamPointings $obsInfo

   else

      # no recent target info available
      # create a default map and label accordingly

      noTargetInfo=1
      actIdUsed=-1

      # big dipper
      primaryRaHours=12.000
      primaryDecDeg=65.000

   fi

   # find the primary beamsize
   ataBeamsizeAt1GhzDeg=3.5
   skyFreqGhz=`calculate $skyFreqMhz/1000`
   beamsizeDeg=`calculate $ataBeamsizeAt1GhzDeg/$skyFreqGhz`
   #echo "beamsizeDeg=$beamsizeDeg"

   # set the map scale so that the primary beam fills most of it
   beamsizeRatio=`calculate $beamsizeDeg/$ataBeamsizeAt1GhzDeg`
   mapDegPerCmAt1Ghz=0.4
   mapDegPerCm=`calculate $mapDegPerCmAt1Ghz*$beamsizeRatio`
   #echo "mapDegPerCm=$mapDegPerCm"

   # create a starmap centered at the primary beam location
   create-starmap -rahours $primaryRaHours -decdeg $primaryDecDeg \
        -target1rahours $target1RaHours -target1decdeg $target1DecDeg \
        -target2rahours $target2RaHours -target2decdeg $target2DecDeg \
        -target3rahours $target3RaHours -target3decdeg $target3DecDeg \
        -primarybeamsizedeg $beamsizeDeg -degpercm $mapDegPerCm \
        ${scale} -showprimarybeam -outfile $outFile 

   # add title
   convert ${outFile} -fill white -undercolor black -gravity north \
      -pointsize 18 -annotate +0+0 \
      "ATA Primary Beam FOV\n RA: $primaryRaHours hr  Dec: $primaryDecDeg deg" \
      ${outFile}

   # add date/time info
   updateTime=`date`

   convert ${outFile} -fill white -undercolor black -gravity south \
       -pointsize 14 -annotate +0+0 \
     "Last updated:  $updateTime   Act: $actIdUsed  Freq: $skyFreqMhz MHz" \
     ${outFile}

   if [ $noTargetInfo = 1 ]
   then
      convert ${outFile} -fill white -undercolor black -gravity center \
        -pointsize 36 -annotate +0+0 "Not Currently Observing" \
        ${outFile}
   fi

}

#-------------------------

parseArgs $@

checkForRequiredArgs

createTargetmap