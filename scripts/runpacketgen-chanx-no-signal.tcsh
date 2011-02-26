#!/bin/tcsh

# runpacketgen-chanx-no-signal.tcsh

# For channelizer:
# Bandwidth 3.2768 MHz
#  X pol only, no signal

 packetgen -b -x -n 40000000 -P 200 -R 1420.6144 -B 3.276800 -l  -a 226.1.50.1 -p 50000

