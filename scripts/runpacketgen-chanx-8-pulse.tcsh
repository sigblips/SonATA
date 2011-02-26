#!/bin/tcsh

# runpacketgen-chanx-8-pulse.tcsh
 
# For channelizer:
# Bandwidth 3.2768 MHz

 packetgen -b -x  -n 4000000 -P 200 -R 1420.4096 -B 3.2768  -l -D -0.2 -S 1000 -X -O 2 -o 20 -F -0.500100 -a 228.1.50.1 -p 50100



