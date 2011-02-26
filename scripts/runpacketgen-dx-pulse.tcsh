#!/bin/tcsh

# runpacketgen-dx-pulse.tcsh

# For DX:
# Bandwidth .54613333333 MHz, 1 Channel

packetgen  -n 40000 -P 200 -B 0.546133333 -X -S 32 -D -0.2 -O 1.0 -o 10 -F 0.000150 -Y -S 20 -D .45 -o 0.0 -F -0.000200 -l -i 5000

