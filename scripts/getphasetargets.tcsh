#!/bin/tcsh

# getphasetargets

findtargets -dbhost sse100 -dbname sonatadb -tzoffset -7 -type calphase
