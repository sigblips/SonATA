#!/bin/tcsh

# runpacketgen-dx.tcsh

# For DX:
# Uing default Bandwidth .5463333333 MHz, 1 Channel
#Using default Multicast Port 51000 and Multicast Address 227.0.0.1
#Using default channel 0
# CW signal at 210 Hz above center, drift 0.2 Hz/sec, SNR 30, X pol
# CW signal at -210 Hz below center, drift  -.1 Hz/sec, SNR 40 Y pol

packetgen  -n 40000 -P 200 -R 1620 -S 30 -D 0.20 -X -F 0.000210 -D 0.10 -S 40 -Y -F -0.000210 -l -i 5000 

