#!/bin/tcsh

# kes-switchConfigFile-2beam-2dx.tcsh

cd ~/OpenSonATA/sse-pkg/setup
cp kes-expectedSonATAComponents-2beam-2dx.cfg expectedSonATAComponents.cfg
cp expectedSonATAComponents.cfg ${HOME}/sonata_install/setup
echo "expectedComponents = kes-expectedSonATAComponents-2beam-2dx.cfg" 
