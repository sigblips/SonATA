#!/bin/sh
################################################################################
#
# File:    compare-dir-checksums.sh
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


# Compare dirs on two hosts by doing a checksum on all files.

host1=$1
dir1=$2

host2=$3
dir2=$4

if [ $# -ne 4 ]
then
   echo "usage: $0 <host1> <dir1> <host2> <dir2>"
   exit
fi

getChecksums()
{
   host=$1
   dir=$2
   outfile=$3
 
   # Use sort to get the files in a consistent order across hosts.
   # Use the -i option on xargs to handle filenames with 
   # embedded blanks.

   ssh $host "cd $dir; find . -type f | sort | \
         xargs -i{} sum '{}'" > $outfile
}

outfile1=/tmp/${USER}-$$-checksums-dir-one.txt
getChecksums $host1 $dir1 $outfile1

outfile2=/tmp/${USER}-$$-checksums-dir-two.txt
getChecksums $host2 $dir2 $outfile2

echo "compare-dir-checksums: diff $host1 $dir1 $host2 $dir2"
diff $outfile1 $outfile2

rm $outfile1 $outfile2

