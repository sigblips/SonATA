#!/bin/sh
################################################################################
#
# File:    backup-prelude-files-to-tape.sh
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


#************************
#  Update for SonATA
#************************

# Backup prelude observing files to tape
# using the Netbackup software

# list of files & directories, all on one line
files=`list-prelude-files-for-backup | tr "\n" " "`

#echo "Backing up these files & dirs to tape: $files"

netbackupPolicy="prelude-backup"

# key phrase associated with backup for aid in retrieval
# appended with: YYYY-MM-DD HH:MM:SS
keyphrase="prelude obs `date +'%Y-%m-%d %k:%M:%S UTC'`"

/usr/openv/netbackup/bin/bpbackup -k "${keyphrase}" -p ${netbackupPolicy} $files



