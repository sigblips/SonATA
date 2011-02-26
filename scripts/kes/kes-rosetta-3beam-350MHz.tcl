# 3 beams
# kes-rosetta-3beam-350MHz.tcl
# antenna selection done elsewhere

act set targetbeam1 160
act set targetbeam2 160
act set targetbeam3 160
act set targetprimary 160
act set type target
act set candarch confirmed
act set multitargetnulls off
act set delay 15

sched set beginfreq 1400
sched set endfreq 1750
sched set dxtune range
sched set rftune auto
sched set target user
sched set multitarget on
sched set beam1 on
sched set beam2 on
sched set beam3 on


dx set length 94
dx set datareqsubchan 384
dx set baseinitaccum 5
dx set basewarn off
dx set baseerror off
dx set datareqsubchan 1535 max
dx set pulsethresh 9.21
dx set tripletthresh 30
dx set maxcand 20

db set name sonatadb
db set usedb on
db set user sonata

