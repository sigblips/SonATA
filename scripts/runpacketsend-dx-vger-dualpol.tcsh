#!/bin/tcsh

# runpacketsend-dx-vger-dualpol.tcsh

sudo packetsend -c -f ${HOME}/sonata_install/data/vger-dualpol-2010-07-14-406.pktdata -J 229.1.1.1 -j 51100 -n 1 -i 500 -b 1
