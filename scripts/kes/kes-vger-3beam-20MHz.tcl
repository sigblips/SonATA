# 3 beams
# kes-vger-3beam-20MHz.tcl
#antenna selection done elsewhere

act set targetbeam1 131
act set targetbeam2 131
act set targetbeam3 131
act set targetprimary 131
act set type target
act set candarch confirmed
act set multitargetnulls off
act set delay 10

sched set beginfreq 8410
sched set endfreq 8430
sched set dxtune range
sched set rftune auto
sched set target user
sched set multitarget on
sched set followup on
sched set beam1 on
sched set beam2 on
sched set beam3 on


dx set length 94
dx set datareqsubchan 384
dx set baseinitaccum 5
dx set basewarn off
dx set baseerror off
dx set datareqsubchan 1535 max

db set name sonatadb
db set usedb on

