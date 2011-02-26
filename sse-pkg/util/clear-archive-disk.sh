#!/bin/sh
################################################################################
#
# File:    clear-archive-disk.sh
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


#********************************
#  Save for historical reference
#********************************


# Clean up nss archive disk by umount, newfs, and remount.
# Argument is mount point.
# Mount point path must start with expected prefix (see below).

#set -x

mountPathPrefix="/usr/local/prelude_archive/disks/mount-points"

if [ $# -ne 1 ]
then
   echo "Clear disk by doing umount, newfs, and remount."
   echo "Disk mount point path must begin with $mountPathPrefix"
   echo "usage: $0 <mount point>"
   exit 1
fi

mountPoint=$1

# Safety check against accidental erasure of incorrect disk:
# Check mountPoint path

if [ `dirname ${mountPoint}` != ${mountPathPrefix} ]
then
  echo "$0: Error: mount point base dir must be ${mountPathPrefix}"
  exit 1
fi

# Figure out device name by lookup in the mount table
device=`grep $mountPoint /etc/mnttab | awk '{print $1}'`
if [ "$device" = "" ]
then
   echo "$0: Error: mount point '$mountPoint' not found in mount table"
   exit
fi

# make sure only one entry matches
deviceCount=`echo $device | wc -w`
if [ $deviceCount -ne 1 ]
then 
   echo "$0: Error: multiple devices found for mount point \
$mountPoint in mount table"
   exit
fi

# Unmount the disk.  Use -f to force unmount even if processes
# are using the disk.
umount -f $device
if [ "$?" -ne "0" ]
then
   echo "$0: umount failed for $device"
   exit
fi

# ignore the verbose newfs output
newfs $device < /dev/null  > /dev/null 2>&1

mount $device $mountPoint

# set permissions so that nss user can access the new filesystem
chown -R nss:software $mountPoint
chmod -R 750 $mountPoint