# jane-test.tcl
# packetgen -> chan1x -> dx1000

act set type iftest
act set watchdogs off

dx set datareqsubchan 406
dx set length 94

sched set dxtune range
sched set beam1 on
sched set beam2 on
sched set beam3 on
sched set beginfreq 1420
sched set endfreq 1430
sched set multitarget off

dx set length 48
dx set basewarn off
dx set baseerror off

dx set baseinitaccum 5
dx set baserep 20

db set name jane_test
db set host mozart
db set usedb on
