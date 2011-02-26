#!/bin/sh
################################################################################
#
# File:    backup-slave-db.sh
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


#***********************
# Update for SonATA
#***********************

# Full binary backup of databases running under mysql slave db

mysqlPort=3308
mysqlHost="127.0.0.1"
mysqlAdminCmdPrefix="mysqladmin -h $mysqlHost --port=$mysqlPort --user=root"

# stop slave so that tables are idle while the backup runs
$mysqlAdminCmdPrefix stop-slave 
$mysqlAdminCmdPrefix flush-tables

# do the backup, ignoring slave related housekeeping files
srcDir="/usr/local/data/mysql-slave"
destDir="/usr/local/backup/mysql/obs-db-rdiff-backup"

date

echo "Source dir: $srcDir"
echo "Backup dir: $destDir"

rdiff-backup  \
  --print-statistics \
  --exclude $srcDir/relay-log.info \
  --exclude $srcDir/slave.pid \
  --exclude "$srcDir/slave-relay-bin*" \
  --exclude $srcDir/errorlog.err \
  $srcDir $destDir

rdiff-backup --compare $srcDir $destDir

# restart the slave mirror
$mysqlAdminCmdPrefix start-slave


