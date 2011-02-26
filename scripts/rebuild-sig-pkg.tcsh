#!/bin/tcsh

cd ${HOME}/OpenSonATA/sse-pkg

./reconfig --dxlibsonly

make -j25 install

cd ${HOME}/OpenSonATA/sig-pkg

./reconfig

make -j25 install
