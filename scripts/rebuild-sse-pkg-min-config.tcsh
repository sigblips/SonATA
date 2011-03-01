#!/bin/tcsh

cd ${HOME}/SonATA/sse-pkg

./reconfig

make -j3 install
