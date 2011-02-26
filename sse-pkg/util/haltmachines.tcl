#!/usr/local/bin/tclsh
################################################################################
#
# File:    haltmachines.tcl
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



# halt (shutdown) the machines specified in the 
# command line args.

# assumes that the user has sudo permission
# to run 'halt' on the machine(s), and that
# remote login without password is set up.

# get confirmation:

if { $argv == "" } {
    puts "halt one or more machines"
    puts "usage: haltmachines <machine name> \[... <machine name>\]"
    exit 1
}

puts "Warning: about to halt all of the following machines:"
puts ""
puts "$argv"
puts ""

puts "Are you sure you want to halt them?  (yes/no/quit)"

while { 1 } {

    gets stdin answer

    switch -exact -- $answer {

	"yes" { break }
	"no" { exit 1 }
	"quit" { exit 1 }
	default { puts "please answer yes, no, or quit" }

    }
}

# parse command line args
foreach host $argv {

    #set cmd "sudo halt";
    set cmd "date";

     if [ catch { set foo [ exec ssh $host $cmd ] } result ] {

	puts "Error: could not halt $host"
	puts $result

     } else {

	puts "sent $cmd command to $host"
     }

}