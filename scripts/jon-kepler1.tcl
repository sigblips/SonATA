# Kepler Spacecraft
#transmit freq 8624.506 dopplerX1 8424.512
# RADEC 19.744583,-22.643602

act set targetbeam1 173
act set targetbeam2 173
act set targetbeam3 173
act set targetprimary 173
act set type target
act set watchdogs off
act set candarch none
sched set tasks autoselectants,prepants,bfreset,bfautoatten,bfinit,caldelay,calphase,calfreq
sched set beginfreq 8423.0 
sched set endfreq 8426.0
sched set rftune user
dx set length 94
dx set datareqsubchan 384
db set name sonatadb
db set usedb on
tscope set centertuneoffset 0.0
tscope set tuneoffset 0.0
tscope set antlistsource antgroup
# both tunings must be the same
tscope set tuningc 1420.0
tscope set tuningd 1420.0

sched set beam1 on
sched set beam2 on
sched set beam3 on

