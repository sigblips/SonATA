#!/bin/tcsh

# runpacketsend-dx-rosetta-xpol.tcsh

sudo packetsend -c -f ${HOME}/sonata_install/data/rosetta-xpol-2010-07-13-312.pktdata -J 229.1.1.1 -j 51100 -n 1 -i 1000 -b 1
