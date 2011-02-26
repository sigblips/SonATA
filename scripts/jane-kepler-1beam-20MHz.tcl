# 1 beams
# jane-kepler-1beam-20MHz.tcl
#antenna selection done elsewhere

act set targetbeam1 173
act set targetprimary 173
act set type target
act set candarch all
act set multitargetnulls off
act set delay 15

sched set beginfreq 8420 
sched set endfreq 8440
sched set dxtune range
sched set rftune auto
sched set target user
sched set beam1 on
sched set beam2 off
sched set beam3 off
sched set followup on


dx set length 48
dx set datareqsubchan 384
dx set baseinitaccum 20
dx set basewarn off
dx set baseerror off
dx set datareqsubband 1535 max
dx set pulsethresh 12.0
dx set tripletthresh 30
dx set maxcand 20

db set name sonatadb
db set usedb on
db set user sonata
db set host sse100

