act set type iftest
act set watchdogs off
act set candarch none
dx set datareqsubchan 384
dx set baseinitaccum 5
dx set length 47
dx load  chan 4 dx1000
dx set pulsethresh 10
dx set tripletthresh 33
dx set daddthresh 8.5

act set delay 10

sched set beginfreq 1250
sched set endfreq 1750
sched set dxtune range
sched set pipe on
