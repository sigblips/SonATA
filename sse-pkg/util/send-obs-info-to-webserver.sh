#!/bin/sh
################################################################################
#
# File:    send-obs-info-to-webserver.sh
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


# Send observing related information to the public webserver,
# including:
# - starmap showing current ATA primary beam position
# - map zoomed in on primary beam showing seti targets
# - baselines & waterfalls

workDir="/tmp/send-obs-info-workdir.$$"

cleanup()
{
   cd

   rm -fr $workDir
}

# cleanup if ctrl-c is issued, or the script exits with an error
trap cleanup INT QUIT ABRT ILL HUP KILL ALRM TERM


# Find db name and host in use by seeker
# by parsing XML file.
# Expected format:
#
#<systemConfig>
#   <dbHost>sol</dbHost>
#    <dbName>nss_iftest</dbName>
#</systemConfig>
#
# Very cheap XML parser.
# Assumes leading tag, item, and trailing tag
# are all on the same line for a given item.

getValueForTagFromXmlFile()
{
   file=$1
   tag=$2

   awk -F'<|>' "/$tag/{print \$3}" < $file
}

# Get the observing host and database from config file
configFile="${HOME}/sonata_archive/templogs/system-config.txt"

host=`getValueForTagFromXmlFile $configFile "dbHost"`
db=`getValueForTagFromXmlFile $configFile "dbName"`

: ${webHost="publish"}
: ${webUser="${USER}"}
: ${webDir="seti-observing"}

timeoutMinutes=30

mkdir $workDir

# create current starmap
starmapFile="$workDir/ataPrimaryStarmap.gif"
create-observing-starmap -dbhost $host -dbname $db \
 -timeoutminutes $timeoutMinutes -outfile $starmapFile

# create starmap of zoomed in primary beam with targets
targetmapFile="$workDir/ataPrimaryBeamTargets.gif"
create-observing-targetmap -dbhost $host -dbname $db \
 -timeoutminutes $timeoutMinutes -outfile $targetmapFile

# create baseline and waterfall plot images

create-observing-data-images -dbhost $host -dbname $db \
  -timeoutminutes $timeoutMinutes -outdir $workDir

# send to website
# -B= batch mode (no passwd prompts), -q= no progress meter
scp -B -q -i ${HOME}/.ssh/seti-obs-web.private-key $workDir/* \
    ${webUser}@${webHost}:${webDir}

cleanup


   