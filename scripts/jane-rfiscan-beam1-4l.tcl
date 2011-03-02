# jane-rfiscan-beam1-4l.tcl

sched set beam1 on
sched set beam2 off
sched set beam3 off

sched set beginfreq 1410.2
sched set endfreq 1450.2
sched set dxtune range
sched set rftune auto
sched set target user

db set host sse100
db set name sonatadb
db set usedb on

act set type rfiscan

dx set length 94

