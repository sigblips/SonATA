#!/bin/tcsh

# runpacketsend-dx-demo-test.tcsh

sudo packetsend -c -f demo-testfile.pktdata -J 227.1.1.1 -j 51000 -n 1 -i 2000 -b 1
