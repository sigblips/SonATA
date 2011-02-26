/*******************************************************************************

 File:    Print.cpp
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

/*
 * Print operators.
 */
#include <math.h>
#include "Dadd.h"

using std::endl;

namespace dadd {

ostream& operator << (ostream& s, const DaddPath& daddPath)
{
	s << "DaddPath: bin: " << daddPath.bin << ", drift: " << daddPath.drift;
	s << ", power: " << daddPath.power << endl;
	return (s);
}

ostream& operator << (ostream& s, const DaddBand& daddBand)
{
	s << "DaddBand: bad: " << daddBand.bad << ", bin: " << daddBand.bin;
	s << ", width: " << daddBand.width << ", hits: " << daddBand.hits << endl;
	s << daddBand.maxPath;
	return (s);
}

ostream& operator << (ostream& s, const DaddBinStatistics& binStats)
{
	s << "DaddBinStatistics: bins[0]: " << binStats.bins[0];
	s << ", bins[1]: " << binStats.bins[1] << ", bins[2]: " << binStats.bins[2];
	s << ", bins[3]: " << binStats.bins[3] << endl;
	// print mean and standard deviation
	int32_t bins = 0;
	bins = binStats.bins[0] + binStats.bins[1] + binStats.bins[2] +
			binStats.bins[3];
	if (bins > 1) {
		float32_t mean = 0, sdev = 0;
		mean = (float32_t ) (binStats.bins[1] + 2 *
				binStats.bins[2] + 3 * binStats.bins[3]) / bins;
		float32_t s0 = powf(mean, 2) * binStats.bins[0];
		float32_t s1 = powf(1 - mean, 2) * binStats.bins[1];
		float32_t s2 = powf(2 - mean, 2) * binStats.bins[2];
		float32_t s3 = powf(3 - mean, 2) * binStats.bins[3];
		sdev = sqrt((s0 + s1 + s2 + s3) / (bins - 1));
		s << " mean = " << mean << ", sd = " << sdev << endl;
		s << " 0/1: " << (float32_t) binStats.bins[0] / binStats.bins[1];
		s << ", 1/2: " << (float32_t) binStats.bins[1] / binStats.bins[2];
		s << ", 2/3: " << (float32_t) binStats.bins[2] / binStats.bins[3];
		s << endl;
	}
	return (s);
}

ostream& operator << (ostream& s, const DaddHitStatistics& hitStats)
{
	s << "DaddHitStatistics: hits: " << hitStats.hits << ", bad bands: ";
	s << hitStats.badBands << endl << "max path " << hitStats.maxPath;
	return (s);
}

ostream& operator << (ostream& s, const DaddStatistics& stats)
{
	s << "DaddHitStatistics: pol: " << stats.pol << endl;
	s << stats.binStats << stats.hitStats;
	return (s);
}

ostream& operator << (ostream& s, const DaddTiming::d& dadd)
{
	s << "dadd timing: dadds: " << dadd.dadds << endl << " computeBinStats: ";
	s << dadd.computeBinStats << endl << " topDown: " << dadd.topDown << endl;
	s << " threshold: " << dadd.threshold << endl << " reportHits: ";
	s << dadd.reportHits << endl << " total: " << dadd.total << endl;
	return (s);
}

ostream& operator << (ostream& s, const DaddTiming::b& badBand)
{
	s << "bad band timing: reports: " << badBand.reports << ", total: ";
	s << badBand.total << endl;
	return (s);
}

ostream& operator << (ostream& s, const DaddTiming::p& sum)
{
	s << "pathsum timing: pairSums: " << sum.pairSums << ", total: ";
	s << sum.total << endl;
	return (s);
}

ostream& operator << (ostream& s, const DaddTiming& timing)
{
	s << "DADD timing:" << endl << timing.dadd << timing.badBand << timing.sum;
	return (s);
}

}