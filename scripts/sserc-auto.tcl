# SSE/seeker startup file

# source the seeker's tcl library routines
source $env(HOME)/sonata_install/bin/seekerlib.tcl


# change to home dir so param tcl files can be found etc

sched set maxfailures 3

#sched set multitarget on
#act set watchdogs off
sched set beam1 on
sched set beam2 on
sched set beam3 on
sched set beam4 off
dx set length 98

act set freqinvert never

# allow for longer signal detection timeouts
# value is X times data collect time 
#act set sigdetwait 9

# single antenna beams - set primary fov (in degrees) large enough
# to allow multiple beams within the "primary FOV",
# ie, use a ratio similar to the ata32

# for star cluster observations


# for iftests, weed out the weak noise signals
dx set cwthresh -20

dx set badbandcwpathlim 250

# define a proc that forces the user to type
# all 4 letters of 'quit', to make sure it's
# not typed accidentally.
proc quiTDefinitely { args } {
	quit
}


# Prevent commands from falling through to the shell.
# Set the 'auto_noexec' flag used by the
# 'unknown' procedure.  The means the user 
# must explicitly use 'exec' to run shell commands.
#
set auto_noexec 1


if {$tcl_interactive} {

    # Use a specific version (9.9.0) of tclreadline
    # which has been modified to disable signal handling in the
    # underlying GNU readline, to avoid conflicts with signal
    # handling done elsewhere in the seeker.
    #
    # Also, an additional tclreadline::LoopOneLine routine
    # was added to tclreadlineSetup.tcl.  This is a variation
    # on the ::Loop routine that does not allow multiline input
    # (ie, no secondary input prompts), as a way to prevent
    # users from getting trapped in a tcl input mode.

    package require -exact tclreadline 9.9.0

    # uncomment the following if block, if you
    # want `ls' executed after every `cd'. (This was
    # the default up to 0.8 == tclreadline_version.)
    #
    # if {"" == [info procs cd]} {
    #     catch {rename ::tclreadline::Cd ""}
    #     rename cd ::tclreadline::Cd
    #     proc cd {args} {
    #         if {[catch {eval ::tclreadline::Cd $args} message]} {
    #             puts stderr "$message"
    #         }
    #         tclreadline::ls
    #     }
    # }

    # uncomment the following line to use
    # tclreadline's fancy ls proc.
    #
    # namespace import tclreadline::ls

    # tclreadline::Print is on (`yes') by default.
    # This mimics the command echoing like in the
    # non-readline interactive tclsh.
    # If you don't like this, uncomment the following
    # line.
    #
    # tclreadline::Print no

    # uncomment the following line, if you want
    # to change tclreadline's print behaviour
    # frequently with less typing.
    #
    # namespace import tclreadline::Print

    # store max this many lines in the history file
    #
    set tclreadline::historyLength 200

    # disable tclreadline's script completer
    #
    # ::tclreadline::readline customcompleter ""

    # alter the prompt
    namespace eval tclreadline {
       proc prompt1 {} {
          return "seeker>> "
       }

    }

    # trap ctrl-d (exit)
    ::tclreadline::readline eofchar {
	puts "type 'exit' or 'quit' to end this program"
    }

    # go to tclreadline's main loop.
    #
    #tclreadline::Loop

    # use the loop that prevents secondary input prompts
    tclreadline::LoopOneLine
} else {
	# run the seeker as a server, on a tcp port
	SeekerServer 2555
}
