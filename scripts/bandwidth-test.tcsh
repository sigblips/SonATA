#!/bin/tcsh

# bandwidth-test.tcsh

packetread -a 229.1.1.1 -P 51100 -s $1 > channel0 &
packetread -a 229.1.1.1 -P 51101 -s $1 > channel1 &
packetread -a 229.1.1.1 -P 51102 -s $1 > channel2 &
packetread -a 229.1.1.1 -P 51103 -s $1 > channel3 &
packetread -a 229.1.1.1 -P 51104 -s $1 > channel4 &
packetread -a 229.1.1.1 -P 51105 -s $1 > channel5 &
packetread -a 229.1.1.1 -P 51106 -s $1 > channel6 &
packetread -a 229.1.1.1 -P 51107 -s $1 > channel7 &
sleep 60
echo "Done"
