#!/bin/tcsh

cd ${HOME}/OpenSonATA/sse-pkg

./reconfig

make -j25 install
