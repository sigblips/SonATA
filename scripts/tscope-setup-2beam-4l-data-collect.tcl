#  antenna 4L
# tscope-setup-2beam-4l-data-collect.tcl


sched set beginfreq 8420.0 
sched set endfreq 8430.0
sched set dxtune range
sched set rftune auto
sched set target user
sched set multitarget on
sched set beam1 on
sched set beam2 on
sched set beam3 off
sched set tasks prepants,bfreset,bfautoatten,bfinit,caldelay


dx set length 48
dx set datareqsubchan 384
dx set baseinitaccum 5
dx set basewarn off
dx set baseerror off

db set name sonatadb
db set usedb on

tscope set antlistsource param
tscope set antsprimary  4l
tscope set antsxpol 4l
tscope set antsypol 4l
# both tunings must be the same
tscope set tuningc 8425.0
tscope set tuningd 8425.0
 tscope assign beamxc1 4l
 tscope assign beamyc1 4l
 tscope assign beamxd1 4l
 tscope assign beamyd1 4l
 tscope assign beamxd2 4l
 tscope assign beamyd2 4l

