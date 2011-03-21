#!/bin/tcsh

# runpacketsend-data-capture-replay.tcsh

sudo packetsend -J 228.1.50.1 -j 50100 -f $1 -n 1 -i 1000 -b 10 
