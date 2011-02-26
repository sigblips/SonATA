################################################################################
#
# File:    README.txt
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

Documentation notes for SonATA Web Interface

1. Global variables used by pages:

  (session) autoRefresh: 'on' or 'off'

2. Protected page access:
  - Any files in the "protected" directory require login to use.
  - User role is set to 'observer'
  - user name is 'sonata'

3. Connections between sse-pkg/seeker & sonata web interface:

   a. status file in sonata_archive/templogs/system-status-summary.txt

       contains currently running activities
       tbd: describe file format

   b. config file in sonata_archive/templogs/system-config.txt
       contains: db host, db name
       tbd: describe file format

   c. runsse process test (overall system up/down)

   d. seeker running a telnet-like server on port 2555
      for commands & command responses

   Note: $HOME/.sserc-auto file must be installed for
      seeker to start up in server mode.
