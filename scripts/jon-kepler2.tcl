# 3 beams
#kepler-3beam.tcl
#antenna selection done elsewhere
# Transmit Freq: 8424.506 Doppler: 8424.512 

act set targetbeam1 173
act set targetbeam2 173
act set targetbeam3 173
act set targetprimary 173
act set type target
act set candarch confirmed
act set multitargetnulls off

sched set beginfreq 8420.0 
sched set endfreq 8430.0
sched set dxtune range
sched set rftune auto
sched set target user
sched set multitarget on
sched set beam1 on
sched set beam2 on
sched set beam3 on


dx set length 48
dx set datareqsubchan 384
dx set baseinitaccum 20
dx set basewarn off
dx set baseerror off
dx set datareqsubchan 1535 max

db set name sonatadb
db set usedb on


