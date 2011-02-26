################################################################################
#
# File:    seekerlib.tcl
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

# seeker Tcl routines


# Define wrappers for all of the global UI objects in the seeker
# so that they don't have to be dereferenced to run their methods

proc act { args } {
    global actGlobal
    eval $actGlobal $args
}

# dx archiver
proc arch { args } {
    global archGlobal
    eval $archGlobal $args
}

# channelizer
proc channel { args } {
    global chanGlobal
    eval $chanGlobal $args
}

# database
proc db { args } {
    global dbGlobal
    eval $dbGlobal $args
}

proc ifc { args } {
    global ifcGlobal
    eval $ifcGlobal $args
}

proc ifc1 { args } {
    global ifc1Global
    eval $ifc1Global $args
}

proc ifc2 { args } {
    global ifc2Global
    eval $ifc2Global $args
}

proc ifc3 { args } {
    global ifc3Global
    eval $ifc3Global $args
}

# parent of all global parameter objects
proc para { args } {
    global paraGlobal
    eval $paraGlobal $args
}

proc dx { args } {
    global dxGlobal
    eval $dxGlobal $args
}

proc sched { args } {
    global schedGlobal
    eval $schedGlobal $args
}

proc tscope { args } {
    global tscopeGlobal
    eval $tscopeGlobal $args
}

proc tsig { args } {
    global tsigGlobal
    eval $tsigGlobal $args
}

proc tsig1 { args } {
    global tsig1Global
    eval $tsig1Global $args
}

proc tsig2 { args } {
    global tsig2Global
    eval $tsig2Global $args
}

proc tsig3 { args } {
    global tsig3Global
    eval $tsig3Global $args
}

proc control { args } {
    global componentControlImmedCmdsGlobal
    eval $componentControlImmedCmdsGlobal $args
}

proc verbose { args } {
    global verboseGlobal
    eval $verboseGlobal $args
}

# some useful aliases

# run the freeants task with a single command
proc freeants { args } {
    start freeants
}

# assign the same target id to all beams
proc targetidallbeams { args } {

    foreach beam {1 2 3 4 5 6} {
       act set targetbeam${beam} $args
    }

    act set targetprimary $args
}


# Print the tcl error information.
# When an error occurs in Tcl, the global variable 
# errorInfo will contain a stack-trace of the active 
# procedures when the error occured.

proc tclerror {args} {
   global errorInfo
   puts "$errorInfo"
}

# source a tcl script, echoing any error
proc restore { args } {
   if [ catch {source $args} result ] {
	tclerror
   }
}

# Replace the core history command with
# one that logs the user's input instead.
# The tcl command format is
# 'history add { 'user's input text'}
# so extract the last part and log it.

proc history { args } {
    
    if { $args == "" } {
	puts "history command bypassed to provide logging"
    } else {

      # pull out the history sub command
      
      set subcmd [lindex $args 0]
      if { $subcmd == "add" } { 

	# get the user's input text
        set userinput [lindex $args 1]

        #puts "userinput = $userinput"

	# write the user text to the SseArchive::SystemLog
	systemlog "user cmd: $userinput"

     }
   }
}

# Let the user add text to the log file.
# This tcl wrapper lets us send a variable
# number of args (words) to the systemlog
# function.

proc log { args } {

    systemlog "user log: $args"

    puts "message logged: $args"

}


# execute the args inside a shell
# to get wildcard expansion, etc.

proc sh { args } {
    exec sh -c "$args"
}

# make quit an alias for exit 
proc quit {} {
    exit;
}


# clean system shutdown
proc shutdown { args } {

    puts "shutting down system..."

    # stop any activities
    stop

    # allow some time for op to complete
    sh sleep 5

    # disconnect all components
    tscope cleanup all
    tscope shutdown all

    dx shutdown all
    tsig shutdown all
    ifc shutdown all
    arch shutdown all
    channel shutdown all
    
    # tell control to shutdown any subcomponents
    # it still has running, then itself
    control shutdown all
    control selfshutdown all

    # allow some time for op to complete
    sh sleep 5

    quit
}


# Run a grid pointing sequence, based on
# the specified previous activity id.
# User must pretune the dxs to the same
# freqs used in the prevActId before
# calling this routine.
# TBD: Get this information from the
# previous activity. 

proc grid { prevActId } {

    # clear any pending followups
    act clearfollowuplist

    # automatically follow the grid sequence
    sched set followup on
    sched set followupmode auto

    # don't let the scheduler try to pick a new target
    sched set target user

    # set the first activity type in the
    # grid sequence
    act set type gridwest

    act set prevactid $prevActId

    start

}


# Perform a followup observation of
# the specified previous activity id.
# Dxs will be automatically tuned to the
# freqs used in the prevActId.

proc followup { prevActId } {

    # clear any pending followups
    act clearfollowuplist

    # turn on followups
    sched set followup on
    sched set followupmode auto

    # set previous act id
    act addfollowupactid $prevActId
    act set prevactid $prevActId

    start
}


# Run the seeker as a server, talking to it via a tcp port.
# Based on code from
# http://www.tcl.tk/scripting/netserver.html
# (Echo_Server, Echo_Accept, etc.)


# Open the server listening socket
# and enter the Tcl event loop
#
# Arguments:
#	port	The server's port number

proc SeekerServer {port} {
    set s [socket -server SeekerAccept $port]
    vwait forever
}

# Accept a connection from a new client.
# This is called after a new socket connection
# has been created by Tcl.
#
# Arguments:
#	sock	The new socket connection to the client
#	addr	The client's IP address
#	port	The client's port number
	
proc SeekerAccept {sock addr port} {
    global echo

    # Record the client's information

    puts "Accept $sock from $addr port $port"
    set echo(addr,$sock) [list $addr $port]

    # Ensure that each "puts" by the server
    # results in a network transmission

    fconfigure $sock -buffering line

    # Set up a callback for when the client sends data

    fileevent $sock readable [list SeekerRunCommand $sock]
}


# This procedure is called when the server
# can read data from the client
#
# Arguments:
#	sock	The socket connection to the client

proc SeekerRunCommand {sock} {
    global echo

    # Check end of file or abnormal connection drop,
    # then run command, sending result back to the client.

    if {[eof $sock] || [catch {gets $sock line}]} {
	close $sock
	puts "Close $echo(addr,$sock)"
	unset echo(addr,$sock)
    } else {
	# echo command in sse system log
	log $line
	if [ catch {set resultText [eval $line] } errorText ] {
	    puts $sock $errorText
	} else {
	   puts $sock $resultText
	}
    }
}

