# jane-test-pulse.tcl
# packetgen -> chan1x -> dx1000,dx2000,dx3000

act set type iftest
act set watchdogs off

dx set datareqsubchan 406
dx set length 94

sched set dxtune range
sched set beam1 on
sched set beam2 on
sched set beam3 on
sched set beginfreq 1320
sched set endfreq 1330
sched set multitarget on

dx set basewarn off
dx set baseerror off

dx set baseinitaccum 5
dx set baserep 20
dx set daddthresh 6.0
db set name jane_test
db set host mozart
db set usedb on
