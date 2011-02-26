/*******************************************************************************

 File:    CwBadBandList.cpp
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

//
// DX CWD bad band list
//
#include "CwBadBandList.h"
#include "Err.h"

namespace dx {

CwBadBandList::CwBadBandList()
{
}

CwBadBandList::~CwBadBandList()
{
	clear();
}

void
CwBadBandList::clear()
{
	lock();
	rawList.clear();
	finalList.clear();
	BadBandList::clear();
	unlock();
}

//
// done: all bad bands have been loaded; merge the bands into a minimal
//		list
//
// Notes:
//		The bands are sorted by pol:frequency.  Adjacent bands of the
//		same polarization are merged.
//		The final output is a list (vector) of CwBadBand structures
//		which represent the final bad band list.
//
void
CwBadBandList::done()
{
	bool first = true;
	CwBadBand band, nextBand;
	CwMap::iterator i;

	lock();
	finalList.clear();
	for (i = rawList.begin(); i != rawList.end(); ++i) {
		nextBand = i->second;
		if (first) {
			band = nextBand;
			first = false;
		}
		// new band if different pol or not contiguous
		else if (band.band.pol != nextBand.band.pol || !merge(band, nextBand)) {
			add(band);
			band = nextBand;
		}
	}
	if (!first)
		add(band);
//	rawList.clear();
	BadBandList::done();
	unlock();
}

int32_t
CwBadBandList::getSize()
{
	return (isComplete() ? finalList.size() : 0);
}

void
CwBadBandList::record(CwBadBand& band)
{
	int32_t key = CW_KEY(band);
	rawList.insert(make_pair(key, band));
	BadBandList::clear();
}

//
// getNth: get the Nth bad band from the final list
//
::CwBadBand&
CwBadBandList::getNth(int32_t index)
{
	if (getSize() < index)
		Fatal(777);

	return (finalList[index]);
}

//
// merge: merge two bad bands if possible
//
// Notes:
//		Two bands can be merged only if they overlap.  The merged
//		band will contain the total number of paths in the two
//		bands and the strongest path in either band.
//		The result is stored in the first argument.
//		Because the bands are sorted by center frequency, the
//		new band must be higher in frequency than the current band.
//
bool
CwBadBandList::merge(CwBadBand& band, CwBadBand& nextBand)
{
	bool merged = false;

	lock();
	if (band.band.bin + band.band.width >= nextBand.band.bin) {
		band.band.width = nextBand.band.bin
				+ nextBand.band.width - band.band.bin;
		band.paths += nextBand.paths;
		if (band.maxPath.power < nextBand.maxPath.power)
			band.maxPath = nextBand.maxPath;
		merged = true;
	}
	unlock();
	return (merged);
}

//
// add: add a CwBadBand to the final list
//
// Notes:
//		Converts the internal bad band format to the format
//		expected by the SSE
//
void
CwBadBandList::add(CwBadBand& band)
{
	::CwBadBand cwBand;

	lock();
	cwBand.band.centerFreq
			= binsToAbsoluteMHz(band.band.bin + band.band.width / 2);
	cwBand.band.bandwidth = binsToMHz(band.band.width);
	cwBand.pol = band.band.pol;
	cwBand.paths = band.paths;
	cwBand.maxPathCount = band.maxPaths;

	// build the maximum strength path
	cwBand.maxPath.rfFreq =  binsToAbsoluteMHz(band.maxPath.bin);
	float64_t driftHz = binsToHz(band.maxPath.drift);
	cwBand.maxPath.drift = driftHz / getObsLength();
	cwBand.maxPath.width = getBinWidth();
	cwBand.maxPath.power = band.maxPath.power;

	finalList.push_back(cwBand);
	unlock();
}

//
// print the entire raw bad band list
//
void
CwBadBandList::print()
{
	cout << "CwBadBandList: " << rawList.size() << " raw bad bands"
			<< endl;
	CwMap::iterator i;
	for (i = rawList.begin(); i != rawList.end(); ++i) {
		CwBadBand cwBand = i->second;
		cout << cwBand << endl;
	}
}

}