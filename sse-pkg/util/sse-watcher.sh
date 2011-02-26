#!/bin/sh 
################################################################################
#
# File:    sse-watcher.sh
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


# sse-watcher

# Monitor/control a running sse system as started by the "runsse" script",
# ie:
# - Use the 'screen' program to watch & control the seeker control window
# - Use tail & xtail to monitor the various system status windows.

# Each xterm spawns a shell, and each executed
# program is followed by a shell, so that all
# the windows stick around until this script
# exits.  That way the windows remain up for
# debugging purposes in case any of the programs
# exit prematurely.


# set some shared xterm options
# -sb = save lines and add scrollbar
# -sl = number of lines to save
nScrollLines=2000
XTERM_OPTIONS="-sb -sl ${nScrollLines}"


# Determine the archive dir based on the SSE_ARCHIVE env var.
if [ "${SSE_ARCHIVE}" ]
then
    ARCHIVE_DIR="${SSE_ARCHIVE}"
else
    ARCHIVE_DIR="${HOME}/sonata_archive"
fi

# define the location of the logs
TEMPLOG_DIR="${ARCHIVE_DIR}/templogs"
PERMLOG_DIR="${ARCHIVE_DIR}/permlogs"

# system status logs

ACTLOG="${TEMPLOG_DIR}/sse-activity-log.txt"
SYSTEM_STATUS_LOG="${TEMPLOG_DIR}/sse-system-status.txt"
VERBOSE_DEBUG_LOG="${TEMPLOG_DIR}/sse-debug-log.txt"

DEFAULT_FONT="9x15"
SELECTED_SYSTEMLOG_FONT="8x13"

# monitor the seeker using the 'screen' program.
# screen will attach to the already running shell that
# contains the seeker.

SCREEN_SESSION_NAME="screen-runsse"
 
# start seeker monitor
xterm  ${XTERM_OPTIONS} -geometry 106x24-8-91 \
    -font "9x15" -title "$0 Monitor/control of SSE seeker" -e sh \
    -c "screen -x -S ${SCREEN_SESSION_NAME} ; sh"   &
joblist="$joblist $!"


# start monitoring the logs, N lines from the end

TAIL_START_NLINES="-1000"

# tail the system status log
xterm ${XTERM_OPTIONS}  -geometry 123x36-0+4 \
-font ${DEFAULT_FONT} -title "System Status" \
-e sh -c "tail ${TAIL_START_NLINES}f ${SYSTEM_STATUS_LOG} ; sh"  &
joblist="$joblist $!"

# tail the activity log
xterm ${XTERM_OPTIONS}  -geometry 123x6-1+582 \
-font ${DEFAULT_FONT} -title "Activity Log" \
-e sh -c "tail ${TAIL_START_NLINES}f ${ACTLOG} ; sh"  &
joblist="$joblist $!"

# tail the verbose debug log
xterm -iconic ${XTERM_OPTIONS} -geometry 78x6+2+669 \
-font ${DEFAULT_FONT} -title "Debug Log" \
-e sh -c "tail ${TAIL_START_NLINES}f ${VERBOSE_DEBUG_LOG} ; sh"  &
joblist="$joblist $!"


# tail the systemlogs
xterm ${XTERM_OPTIONS} -geometry  83x45+27+169 \
-font ${DEFAULT_FONT} -title "System Log" \
-e sh -c "xtail ${PERMLOG_DIR}/systemlogs ; sh"  &
joblist="$joblist $!"


# tail the systemlogs, extracting only the essential information
xterm ${XTERM_OPTIONS} -geometry  109x15+27-83 \
-font ${SELECTED_SYSTEMLOG_FONT} -title "Selected System Log Info (Tuning & Candidate Information)" \
-e sh -c "xtail ${PERMLOG_DIR}/systemlogs | sonata-extract-systemlog-info.sh; sh"  &
joblist="$joblist $!"


# tail the errorlogs
xterm ${XTERM_OPTIONS} -geometry 83x7+26+25 \
-font ${DEFAULT_FONT} -title "Message / Error Log" \
-e sh -c "xtail ${PERMLOG_DIR}/errorlogs ; sh"  &
joblist="$joblist $!"


# tail the systemlogs, extracting only the info needed for the 
# observers log
xterm ${XTERM_OPTIONS} -geometry 86x15+61-20 \
-font ${SELECTED_SYSTEMLOG_FONT} -title "Observer's log" \
-e sh -c "xtail ${PERMLOG_DIR}/systemlogs | sonata-extract-observers-log-info.sh; sh" &
joblist="$joblist $!"



# stick around until user indicates we're done
echo "Press <return> to exit the $0 script..."
read cmd

# kill all the backgrounded jobs 
kill -9 $joblist > /dev/null

