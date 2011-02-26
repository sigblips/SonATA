/*******************************************************************************

 File:    PulseBadBandList.cpp
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
// Pulse bad band list
//
#include <iostream>
#include "PulseBadBandList.h"
#include "Err.h"

using std::cout;
using std::endl;

namespace dx {

PulseBadBandList::PulseBadBandList()
{
}

PulseBadBandList::~PulseBadBandList()
{
}

void
PulseBadBandList::clear()
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
//		The bands are sorted by res:frequency.  Adjacent bands of the
//		same resolution are merged.
//		The final output is a vector of PulseBadBand structures
//		which represent the final bad band list.
//
void
PulseBadBandList::done()
{
	bool first = true;
	PulseBadBand band, nextBand;
	PulseBadBandMap::iterator i;

	lock();
	finalList.clear();
	for (i = rawList.begin(); i != rawList.end(); ++i) {
		nextBand = i->second;
		if (first) {
			band = nextBand;
			first = false;
		}
		// see if we're finished with this resolution
		else if (band.res != nextBand.res || !merge(band, nextBand)) {
			add(band);
			band = nextBand;
		}
	}
	if (!first)
		add(band);
	rawList.clear();
	BadBandList::done();
	unlock();
}

int32_t
PulseBadBandList::getSize()
{
	return (isComplete() ? finalList.size() : 0);
}

void
PulseBadBandList::record(PulseBadBand& band)
{
	int32_t key = PULSE_KEY_BADBAND(band);

	lock();
	rawList.insert(make_pair(key, band));
	BadBandList::clear();
	unlock();
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
PulseBadBandList::merge(PulseBadBand& band, PulseBadBand& nextBand)
{
	bool merged = false;

	lock();
	if (band.band.bin + band.band.width >= nextBand.band.bin) {
		band.band.width = nextBand.band.bin
				+ nextBand.band.width - band.band.bin;
		band.pulses += nextBand.pulses;
		if (band.band.pol != nextBand.band.pol)
			band.band.pol = POL_MIXED;
		band.tooManyTriplets = (bool_t)
				(band.tooManyTriplets || nextBand.tooManyTriplets);
		merged = true;
	}
	unlock();
	return (merged);
}


//
// add: add a PulseBadBand to the final list
//
// Notes:
//		Converts the internal bad band format to the format
//		expected by the SSE
//
void
PulseBadBandList::add(PulseBadBand& band)
{
	::PulseBadBand pulseBand;

	lock();
	pulseBand.band.centerFreq
			= binsToAbsoluteMHz(band.band.bin + band.band.width / 2);
	pulseBand.band.bandwidth = binsToMHz(band.band.width);
	pulseBand.res = band.res;
	pulseBand.pol = band.band.pol;
	pulseBand.pulses = band.pulses;
	pulseBand.triplets = band.triplets;
	pulseBand.maxPulseCount = band.maxPulses;
	pulseBand.maxTripletCount = band.maxTriplets;
	pulseBand.tooManyTriplets = (bool_t) band.tooManyTriplets;

	finalList.push_back(pulseBand);
	unlock();
}

//
// getNth: get the Nth bad band from the final list
//
::PulseBadBand&
PulseBadBandList::getNth(int32_t index)
{
	if (getSize() < index)
		Fatal(777);

	return (finalList[index]);
}

//
// print the entire raw bad band list
//
void
PulseBadBandList::print()
{
	cout << "DxPDBadBandList: " << rawList.size() << " raw bad bands"
			<< endl;
	PulseBadBandMap::iterator i;
	for (i = rawList.begin(); i != rawList.end(); ++i) {
		PulseBadBand pdBand = i->second;
		cout << pdBand << endl;
	}
}

}
