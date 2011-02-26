/*******************************************************************************

 File:    testBadBands.cpp
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

#include <iostream>
#include "CwBadBandList.h"
#include "PulseBadBandList.h"

#define TOL_MHZ			(.000000001)
#define TOL_HZ			(.0001)

using std::cout;
using std::endl;

void testCWDBadBands();
void testPDBadBands();
void printCwBadBands(CwBadBandList& bands);
void printPulseBadBands(PulseBadBandList& bands);
void compareCwBadBands(CwBadBandList& a, CwBadBand& b, CwBadBand& c);
void comparePulseBadBands(PulseBadBandList& a, PulseBadBand& b,
		PulseBadBand& c);
void assertf(float64_t f1, float64_t f2, float64_t tol);

int
main()
{
	testCWDBadBands();
	testPDBadBands();
}

void
testCWDBadBands()
{
	CwBadBandList bands;
	DaddPath maxPath;

	bands.setObsParams(5.0, HZ_PER_1HZ_BIN, 128);

	bands.done();
	cout << "count = " << bands.getSize() << endl;

	bands.clear();

	CwBadBand band1 = {
		{ 0, 32768, POL_LEFTCIRCULAR },
		225,
		{ 1000, 5, 100 }
	};	
	CwBadBand band2 = {
		{ 491520, 32768, POL_LEFTCIRCULAR },
		325,
		{ 492520, 10, 200 }
	};	
	CwBadBand band3 = {
		{ 524288, 32768, POL_LEFTCIRCULAR },
		425,
		{ 524788, 11, 230 }
	};	
	CwBadBand band4 = {
		{ 557056, 32768, POL_LEFTCIRCULAR },
		525,
		{ 557856, 12, 220 }
	};

	// comparison bands
	// this is the sum of band2 and band3 above
	CwBadBand band23 = {
		{ 491520, 65536, POL_LEFTCIRCULAR },
		750,
		{ 524788, 11, 230 }
	};

	// this is the sum of band2, band3 and band4 above
	CwBadBand band234 = {
		{ 491520, 98304, POL_LEFTCIRCULAR },
		1275,
		{ 524788, 11, 230 }
	};

	bands.record(band1);
	cout << bands.getSize() << " bands" << endl;
//	bands.print();
	bands.done();
//	printCwBadBands(bands);

	// check for a valid bad band
	
	assert(bands.getSize() == 1);
	CwBadBand b1 = bands.getNth(0);
	compareCwBadBands(bands, band1, b1);
	bands.clear();
	bands.record(band1);
	bands.record(band2);
	bands.done();
	assert(bands.getSize() == 2);
	b1 = bands.getNth(0);
	CwBadBand b2 = bands.getNth(1);
	compareCwBadBands(bands, band1, b1);
	compareCwBadBands(bands, band2, b2);
//	printCwBadBands(bands);

	bands.clear();
	bands.record(band1);
	bands.record(band2);
	bands.record(band3);
	bands.done();
	assert(bands.getSize() == 2);
	b1 = bands.getNth(0);
	b2 = bands.getNth(1);
	compareCwBadBands(bands, band1, b1);
	compareCwBadBands(bands, band23, b2);
//	printCwBadBands(bands);

	bands.clear();
	bands.record(band1);
	bands.record(band2);
	bands.record(band3);
	bands.record(band4);
//	bands.print();
	bands.done();
	assert(bands.getSize() == 2);
	b1 = bands.getNth(0);
	b2 = bands.getNth(1);
	compareCwBadBands(bands, band1, b1);
	compareCwBadBands(bands, band234, b2);
//	printCwBadBands(bands);

	// test two polarizations
	CwBadBand band5 = {
		{ 0, 32768, POL_LEFTCIRCULAR },
		225,
		{ 1000, 5, 100 }
	};	
	CwBadBand band6 = {
		{ 491520, 32768, POL_LEFTCIRCULAR },
		325,
		{ 492520, 10, 200 }
	};	
	CwBadBand band7 = {
		{ 524288, 32768, POL_RIGHTCIRCULAR },
		425,
		{ 524788, 11, 230 }
	};	
	CwBadBand band8 = {
		{ 557056, 32768, POL_LEFTCIRCULAR },
		525,
		{ 557856, 12, 220 }
	};

	CwBadBand band9 = {
		{ 589824, 32768, POL_LEFTCIRCULAR },
		325,
		{ 590012, 13, 225 }
	};

	CwBadBand band89 = {
		{ 557056, 65536, POL_LEFTCIRCULAR },
		850,
		{ 590012, 13, 225 }
	};

	bands.clear();
	bands.record(band5);
	bands.record(band6);
	bands.record(band7);
	bands.record(band8);
	bands.record(band9);
	bands.print();
	bands.done();
	cout << "count = " << bands.getSize() << endl;
	assert(bands.getSize() == 4);
	b1 = bands.getNth(0);
	b2 = bands.getNth(1);
	CwBadBand b3 = bands.getNth(2);
	CwBadBand b4 = bands.getNth(3);
	compareCwBadBands(bands, band5, b2);
	compareCwBadBands(bands, band6, b3);
	compareCwBadBands(bands, band7, b1);
	compareCwBadBands(bands, band89, b4);
//	printCwBadBands(bands);
}
	
void
testPDBadBands()
{
	PulseBadBandList bands;

	bands.setObsParams(5.0, HZ_PER_1HZ_BIN, 128);

	bands.done();
	cout << "count = " << bands.getSize() << endl;

	bands.clear();

	PulseBadBand band1 = {
		{ 0, 4096, POL_LEFTCIRCULAR },
		RES_1HZ,
		100,
		false
	};	

	PulseBadBand band2 = {
		{ 266240, 4096, POL_LEFTCIRCULAR },
		RES_1HZ,
		100,
		false
	};

	PulseBadBand band3 = {
		{ 270336, 4096, POL_LEFTCIRCULAR },
		RES_1HZ,
		110,
		true
	};

	PulseBadBand band4 = {
		{ 274432, 4096, POL_LEFTCIRCULAR },
		RES_1HZ,
		300,
		false
	};

	PulseBadBand band23 = {
		{ 266240, 8192, POL_LEFTCIRCULAR },
		RES_1HZ,
		210,
		true,
	};
	
	PulseBadBand band234 = {
		{ 266240, 12288, POL_LEFTCIRCULAR },
		RES_1HZ,
		510,
		true,
	};
	
	bands.record(band1);
	bands.print();
	bands.done();
	cout << "count = " << bands.getSize();
	assert(bands.getSize() == 1);
	PulseBadBand b1 = bands.getNth(0);
	comparePulseBadBands(bands, band1, b1);
	printPulseBadBands(bands);

	bands.clear();
	bands.record(band1);
	bands.record(band2);
	bands.done();
	assert(bands.getSize() == 2);
	b1 = bands.getNth(0);
	PulseBadBand b2 = bands.getNth(1);	
	comparePulseBadBands(bands, band1, b1);
	comparePulseBadBands(bands, band2, b2);
	printPulseBadBands(bands);

	bands.clear();
	bands.record(band1);
	bands.record(band2);
	bands.record(band3);
	bands.done();
	cout << "count = " << bands.getSize();
	assert(bands.getSize() == 2);
	b1 = bands.getNth(0);
	b2 = bands.getNth(1);
	comparePulseBadBands(bands, band1, b1);
	comparePulseBadBands(bands, band23, b2);
	printPulseBadBands(bands);

	bands.clear();
	bands.record(band1);
	bands.record(band2);
	bands.record(band3);
	bands.record(band4);
	bands.done();
	cout << "count = " << bands.getSize();
	assert(bands.getSize() == 2);
	b1 = bands.getNth(0);
	b2 = bands.getNth(1);
	comparePulseBadBands(bands, band1, b1);
	comparePulseBadBands(bands, band234, b2);
	printPulseBadBands(bands);
}

void
printCwBadBands(CwBadBandList& bands)
{
	cout << "count = " << bands.getSize() << endl;
	for (int i = 0; i < bands.getSize(); ++i) {
		CwBadBand band = bands.getNth(i);
		cout << band << endl;
	}
}

void
printPulseBadBands(PulseBadBandList& bands)
{
	cout << "count = " << bands.getSize() << endl;
	for (int i = 0; i < bands.getSize(); ++i) {
		PulseBadBand band = bands.getNth(i);
		cout << band << endl;
	}
}

void
compareCwBadBands(CwBadBandList& a, CwBadBand& b, CwBadBand& c)
{
	CwBadBand c1;

	c1.band.centerFreq = a.binsToAbsoluteMHz(b.band.bin + b.band.width / 2);
	c1.band.bandwidth = a.binsToMHz(b.band.width);
	c1.pol = b.band.pol;
	c1.paths = b.paths;
	c1.maxPath.rfFreq = a.binsToAbsoluteMHz(b.maxPath.bin);
	c1.maxPath.drift = a.binsToHz(b.maxPath.drift) / a.getObsLength();
	c1.maxPath.width = a.binsToHz(1);
	c1.maxPath.power = b.maxPath.power;

	assertf(c.band.centerFreq, c1.band.centerFreq, TOL_MHZ);
	assertf(c.band.bandwidth, c1.band.bandwidth, TOL_MHZ);
	assert(c.pol == c1.pol);
	assert(c.paths == c1.paths);
	assertf(c.maxPath.rfFreq, c1.maxPath.rfFreq, TOL_MHZ);
	assertf(c.maxPath.drift, c1.maxPath.drift, TOL_HZ);
	assertf(c.maxPath.width, c1.maxPath.width, TOL_HZ);
	assert(c.maxPath.power == c1.maxPath.power);
}

void
comparePulseBadBands(PulseBadBandList& a, PulseBadBand& b, PulseBadBand& c)
{
	PulseBadBand c1;

	c1.band.centerFreq = a.binsToAbsoluteMHz(b.band.bin + b.band.width / 2);
	c1.band.bandwidth = a.binsToMHz(b.band.width);
	c1.pol = b.band.pol;
	c1.pulses = b.pulses;
	c1.tooManyTriplets = (bool_t) ((bool_t) b.tooManyTriplets || c.tooManyTriplets);

	assertf(c.band.centerFreq, c1.band.centerFreq, TOL_MHZ);
	assertf(c.band.bandwidth, c1.band.bandwidth, TOL_MHZ);
	assert(c.pol == c1.pol);
	assert(c.pulses == c1.pulses);
	assert(c.tooManyTriplets == c1.tooManyTriplets);
}

void
assertf(float64_t f1, float64_t f2, float64_t tol)
{
	assert(fabs(f1 - f2) < tol);
}