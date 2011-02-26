# SSE seeker commands to end observing

# sonata-stop-obs.tcl

stop

# allow settle down time
sh sleep 10

# release ant array resources
freeants

sh sleep 2

# disconnect from telescope array
tscope cleanup

sh sleep 120

# send out "finished with array" email
sh echo "SonATA done with the array " | mailx -s 'SonATA finished with array' -r peter@seti.org atauser@seti.org

exec setAlarm DISARM,sonata,Finished

