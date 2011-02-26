#!/bin/tcsh

# runpacketgen-dx-xonly.tcsh

# For DX:
# Uing default Bandwidth .4096 MHz, 1 Channel
#Using default Multicast Port 51000 and Multicast Address 227.1.1.1
#Using default channel 0
# CW signal at 210 Hz above center, drift 0.2 Hz/sec, SNR 30, X pol

packetgen -x -a 227.1.1.1 -p 51000 -n 40000 -P 200 -R 1620 -S 30 -D 0.20 -X -F 0.000210 -D 0.10 -S 40  -l 

