#!/bin/sh
################################################################################
#
# File:    opensonata-build-test.sh
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


# run an automated OpenSonata build test

# print commands as they are executed
#set -x

# Set up the shell environment:
# -u    Treat unset variables as an error when substituting.
set -u

branchDefault="master"

: ${mailTo="${USER}"}
: ${gitRepos="git@github.com:SETI/OpenSonATA.git"}
: ${gitBranch="${branchDefault}"}
: ${workDir="${HOME}/tmp/opensonata-build-test-workdir"}

ACE_ROOT=/usr/local/ACE_wrappers
export ACE_ROOT
GCC_ROOT="/usr/local/gcc3.3"

PATH=${GCC_ROOT}/bin:/usr/local/sbin:/usr/lib:/usr/local/java/bin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/ccs/bin:/usr/ucb:.:${HOME}/git/install/bin
export PATH

LD_LIBRARY_PATH=${GCC_ROOT}/lib:/usr/local/lib:${ACE_ROOT}/ace
export LD_LIBRARY_PATH
PATH=$PATH:${ACE_ROOT}/bin

# check out & build the OpenSonata code

# get the os type from the first field in the uname results
ostype=`uname -a | cut -d' ' -f1`

buildTest()
{
    echo "ACE_ROOT is ${ACE_ROOT}"
    echo "LD_LIBRARY_PATH is ${LD_LIBRARY_PATH}"
    echo "PATH is ${PATH}"

    echo "work dir: ${workDir}"

    mkdir -p ${workDir}
    cd ${workDir}

    topDir="OpenSonATA"
    chmod -fR u+w $topDir
    rm -rf $topDir

    echo "Testing git repos: ${gitRepos} ${gitBranch}"

    if git clone ${gitRepos}; then
       echo "source update OK"
    else
       echo "source update FAILED"
       return 1
    fi

    # see if testing a remote branch
    if [ "${gitBranch}" != "${branchDefault}" ]
    then
        cd $workDir/$topDir
        git checkout --track -b ${gitBranch} origin/${gitBranch}
    fi

    # *** sse-pkg ***
    cd $workDir/$topDir/sse-pkg

    ./reconfig

# 'make distcheck' is more thorough, but breaks when
# nonstandard configure options are needed. 
# Note : When automake is updated it should be possible
# to use the 'DISTCHECK_CONFIGURE_FLAGS' option to
# override.

#    if make RUNTESTFLAGS=--debug distcheck; then

    if make RUNTESTFLAGS=--debug check; then
	echo "make succeeded"
	# return 0
    else
	echo "make failed"
	return 1
    fi

    # don't build sigproc-pkg on sun/solaris
    if [ "$ostype" = "SunOS" ]
    then
        echo "ostype is $ostype, skipping sigproc-pkg build"
        return 0
    fi


    # *** sigproc-pkg ***
    cd $workDir/$topDir/sigproc-pkg

    ./reconfig

    if make leastsquares && make check; then
	echo make succeeded;
	return 0
    else
	echo make failed
	return 1
    fi
}

# build the package and email the results

tmpFilePrefix=/tmp/opensonata-check.$$
buildResultsFile=${tmpFilePrefix}.buildResults.txt
mailMsgFile=${tmpFilePrefix}.mailMsg.txt


subjectPrefix="OpenSonata auto build test: ${ostype} (`hostname`) ${gitRepos} branch: ${gitBranch}"

if buildTest > $buildResultsFile 2>&1 ; then
    subjectText="Success: ${subjectPrefix}"
    echo "build succeeded" > $buildResultsFile
else
    subjectText="FAILURE: ${subjectPrefix}"
fi

# Send the results.  To avoid problems with long
# mail messages, trim out the middle of the build log.

echo "To: ${mailTo}" > $mailMsgFile
echo "Subject: $subjectText" >> $mailMsgFile
head -50 $buildResultsFile >> $mailMsgFile
echo "........." >> $mailMsgFile
tail -200 $buildResultsFile >> $mailMsgFile

# sendmail options:
# -t = Extract recipients from message headers
# -oi = don't treat line with only a '.' as EOF

sendmail -t -oi < $mailMsgFile

# clean up
rm -f $buildResultsFile
rm -f $mailMsgFile

exit 0
