#!/bin/tcsh

setenv thisDate `date  +'%Y-%m-%d-%H:%M'`
sudo packetrelay -f data-collect-beam2-${thisDate}.pktdata \
	-I 229.2.1.1 -o 52100 -n 1000000 
