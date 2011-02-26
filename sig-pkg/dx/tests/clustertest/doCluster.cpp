/*******************************************************************************

 File:    doCluster.cpp
 Project: OpenSonATA
 Authors: The OpenSonATA code is the result of many programmers
          over many years

 Copyright 2011 The SETI Institute

 OpenSonATA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OpenSonATA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
 
 Implementers of this code are requested to include the caption
 "Licensed through SETI" with a link to setiQuest.org.
 
 For alternate licensing arrangements, please contact
 The SETI Institute at www.seti.org or setiquest.org. 

*******************************************************************************/

#include "PdmCWDClusterer.h"
#include "PdmSuperClusterer.h"
#include <stdio.h>

// read a set of hit reports on stdin in TSS format
// (whitespace-separated count, bin, drift, power)
// output supercluster info

void
dumpTag(char *indent, const PdmClusterTag &tag)
{
	PdmCWDClusterer *cwdClust = tag.holder->getCWD();
	if (cwdClust)
	{
		CwPowerSignal cw = cwdClust->getNth(tag.index);

		printf("%sCW freq %.6f, drift %.4f, power %.0f, width %.2f\n",
				indent,
				//cw.sig.rfFreq + 2288 - 5.034399,
				cw.sig.rfFreq,
				cw.sig.drift,
				cw.sig.power,
				cw.sig.width);
	}
	else
	{
		printf("  non-CW signal\n");
	}
}

int
main()
{
	PdmSuperClusterer super;
	PdmCWDClusterer clust(&super, POL_LEFTCIRCULAR);

	// guestimates for TSS; match test data
	super.setObsParams(2289 - 5.034399, 64, 0.6958917);

	while(cin)
	{
		int count, bin, drift, power;
		cin >> count;
		cin >> bin;
		cin >> drift;
		cin >> power;
		clust.recordHit(bin,drift,power);
	}

	clust.allHitsLoaded();

	super.compute();

	printf("found %d superclusters:\n", super.getCount());
	for (int i = 0; i < super.getCount(); i++)
	{
		const PdmClusterTag &tag = super.getNthMainSignal(i);
		dumpTag("SUPER ",tag);
		for (int j = 0; j < super.getNthClusterCount(i); j++)
		{
			const PdmClusterTag &tag2 = super.getNthMthCluster(i,j);
			dumpTag("      ",tag2);
		}
	}

	return 0;
}