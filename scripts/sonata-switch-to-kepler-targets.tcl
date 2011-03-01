# sonata-switch-to-kepler-targets.tcl

# change database to cygnusx3 where other Kepler targets were observed
db set name cygnusx3

# change catalog priorities
sched set catshigh exoplanets,habcat
sched set targetmerit catalog,completelyobs,timeleft,meridian

# restart observing
start obs
