#!/bin/sh
################################################################################
#
# File:    list-prelude-files-for-backup.sh
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


#*****************
# Update for SonATA
#*******************

# create a list of "yesterday's" NSS/Prelude files (and directories)
# to be backed up:
# Logs: system-log, error-log
# observing database(s) snapshots
# observing data products directory (baselines, etc)
#
# The output is intended to be used with the netbackup software,
# which will not follow symbolic links, so all paths must
# be direct.
#
# Use GNU date to determine yesterdays's iso8601 date (YYYY-MM-DD)
yesterday=`date -d 'yesterday' +'%Y-%m-%d'`

# base location of system & error logs
sseArchiveDir="$HOME/sonata_archive"

# location of data products directory:
# follow symbolic link to determine disk holding yesterday's data
dataProductsRootDir="/usr/local/prelude_archive/disks/date-links"
dataProductsDisk=`ls -l $dataProductsRootDir/${yesterday} | awk '{print $NF}'`

# location of database archive snapshots
databaseBackupDir="/usr/local/prelude_archive/dbbackups"

# some of these files may not exist, that's ok

fileList=`ls -d \
$sseArchiveDir/permlogs/systemlogs/*${yesterday}* \
$sseArchiveDir/permlogs/errorlogs/*${yesterday}* \
${dataProductsDisk}/${yesterday} \
${databaseBackupDir}/*${yesterday}* \
2>/dev/null `

echo "$fileList"

