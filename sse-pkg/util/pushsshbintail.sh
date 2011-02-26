#!/bin/sh
################################################################################
#
# File:    pushsshbintail.sh
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



# push a 'tailed' version of the given file
# onto the remote host.

# repeatedly run bintail on the file,
# restarting it every time it exits (ie, when the
# input file is truncated).  ssh the output to
# the remote host.

# -host <remotehost>
# -file <file to remote tail>

if [ $# -ne 4 ]
then
    echo "usage: $0 -host <remotehost> -file <filename>"
    exit
fi

while [ "$1" ]
do
    if [ "$1" = "-host" ]
    then
	shift
	remotehost="$1"
    elif [ "$1" = "-file" ]
    then
	shift
	filename="$1"
    else
	echo "error: invalid arg: " $1
	exit
    fi
    
    shift
done

#echo "host is $remotehost"
#echo "file is $filename"

while [ 1 ]
do
   # echo "restarting bintail"
   bintail $filename | ssh $remotehost "cat > $filename"
done