# 1 beams


sched set beginfreq 1410.2 
sched set endfreq 1450.2
sched set beam1 on
sched set beam2 off
sched set beam3 off
sched set tasks prepants,bfreset,bfautoatten,bfinit,caldelay

db set name sonatadb
db set usedb on
db set host sse100


tscope set antlistsource param
tscope set antsprimary  4l
tscope set antsxpol 4l
tscope set antsypol 4l
# both tunings must be the same
 tscope assign beamxc1 4l
 tscope assign beamyc1 4l
 tscope assign beamxd1 4l
 tscope assign beamyd1 4l
 tscope assign beamxd2 4l
 tscope assign beamyd2 4l

