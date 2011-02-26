#!/bin/sh
################################################################################
#
# File:    sonata-seeker-command-cron-wrapper.sh
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

# sse-seeker-command-cron-wrapper

# send the seeker commands on its tcp command port
# all arguments are passed through 

GCC_ROOT=/usr/local/gcc3.3
ACE_ROOT=/usr/local/ACE_wrappers
MYSQL_ROOT=/usr/local/mysql
SSE_ROOT=${HOME}/sonata_install

# print commands as they are executed
#set -x

PATH=${GCC_ROOT}/bin:/usr/local/sbin:/usr/local/java/bin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/ccs/bin:/usr/ucb:${SSE_ROOT}/bin
export PATH

# Set up the shell environment:
# -u    Treat unset variables as an error when substituting.
set -u

LD_LIBRARY_PATH=${MYSQL_ROOT}/lib:/usr/local/lib:${ACE_ROOT}/ace
export LD_LIBRARY_PATH

send-seeker-command-via-telnet.expect "$*"  > /dev/null 2>&1

exit 0
