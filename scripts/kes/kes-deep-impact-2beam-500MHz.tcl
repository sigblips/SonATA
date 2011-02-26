# kes-deep-impact-2beam-500MHz.tcl
# Deep Impact transmits at 8435.370 Doppler 8435.609
# Full Array 
act set targetbeam1 166
act set targetbeam2 166
act set targetbeam3 166
act set targetprimary 166
act set type target
act set watchdogs on
dx set basewarn off
dx set baseerror off
act set candarch none
sched set beginfreq 8435.6 
sched set endfreq 8436.0
sched set dxtune range
sched set rftune auto
sched set target user
dx set length 48
dx set datareqsubchan 384
db set name sonatadb
db set usedb on
sched set beam1 on
sched set beam2 off
sched set beam3 on
sched set multitarget on
act set multitargetnulls off


