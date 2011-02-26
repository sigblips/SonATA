/*******************************************************************************

 File:    testCluster.cpp
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
#include "PdmPDClusterer.h"
#include "PdmSuperClusterer.h"
#include <stdio.h>

void
assertEqual(int line, int want, int got)
{
	if (want == got)
		return;
	printf("at line %d wanted %d got %d\n", line, want, got);
	abort();
}

#define ASSERT_EQ(w,g) assertEqual(__LINE__, w, g)

void
assertEqualFloat(int line, double want, double got, double tolerance)
{
	if (fabs(want - got) < tolerance)
		return;
	printf("at line %d wanted %f got %f\n", line, want, got);
	abort();
}

#define ASSERT_EQ_F(w,g,t) assertEqualFloat(__LINE__, w, g, t)

// floating point tolerances
// frequencies accurate to 1/10 of a Hz
#define D_FREQ 0.0000001
// pulse periods accurate to 1/10 of a second
#define D_PERIOD 0.1

void
testCWDCluster()
{
	PdmSuperClusterer super;
	PdmCWDClusterer clust(&super, POL_LEFTCIRCULAR);

	// with nothing added, should return empty vector
	clust.allHitsLoaded();
	super.compute();
	int count = super.getCount();
	ASSERT_EQ(0, count);

	// with one hit added, should return 1 supercluster describing that hit
	super.clear();
	clust.recordHit(100,5,50);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// after clear, should return empty vector
	super.clear();
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(0, count);

	// with two hits added far apart,
	// should return 2 superclusters describing each hit
	super.clear();
	clust.recordHit(100,5,50);
	clust.recordHit(2100,5,50);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(2, count);

	// verify description
	PdmClusterTag tag;
	tag = super.getNthMainSignal(0);
	CwPowerSignal cw = tag.holder->getCWD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(100), cw.sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(5)/clust.getSecondsPerObs(),
				cw.sig.drift, D_FREQ);
	ASSERT_EQ_F(50, cw.sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1), cw.sig.width, D_FREQ);

	tag = super.getNthMainSignal(1);
	cw = tag.holder->getCWD()->getNth(tag.index);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(2100), cw.sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(5)/clust.getSecondsPerObs(),
				cw.sig.drift, D_FREQ);
	ASSERT_EQ_F(50, cw.sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1), cw.sig.width, D_FREQ);

	// with two equally powered hits added within the lev1 range
	// should return 1 supercluster describing first hit, with 
	// width set to number of bins from lowest to highest hit, inclusive
	super.clear();
	clust.recordHit(100,5,50);
	clust.recordHit(102,5,50);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// verify description
	tag = super.getNthMainSignal(0);
	cw = tag.holder->getCWD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(100), cw.sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(5)/clust.getSecondsPerObs(),
				cw.sig.drift, D_FREQ);
	ASSERT_EQ_F(50, cw.sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(3), cw.sig.width, D_FREQ);

	// with three unequally powered hits added within the lev1 range
	// should return 1 supercluster describing highest power hit, with 
	// width set to number of bins from lowest to highest hit, inclusive
	super.clear();
	clust.recordHit(100,5,50);
	clust.recordHit(102,5,52);
	clust.recordHit(104,5,51);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// verify description
	tag = super.getNthMainSignal(0);
	cw = tag.holder->getCWD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(102), cw.sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(5)/clust.getSecondsPerObs(),
				cw.sig.drift, D_FREQ);
	ASSERT_EQ_F(52, cw.sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(5), cw.sig.width, D_FREQ);

	// with two equally powered hits added outside the lev1 range but
	// within the lev2 range
	// should return 1 supercluster describing first hit
	super.clear();
	clust.recordHit(100,5,50);
	clust.recordHit(200,5,50);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// verify description
	tag = super.getNthMainSignal(0);
	cw = tag.holder->getCWD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(100), cw.sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(5)/clust.getSecondsPerObs(),
				cw.sig.drift, D_FREQ);
	ASSERT_EQ_F(50, cw.sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1), cw.sig.width, D_FREQ);

	// with unequally powered hits added outside the lev1 range but
	// within the lev2 range
	// should return 1 supercluster describing highest power hit
	super.clear();
	clust.recordHit(100,5,50);
	clust.recordHit(200,5,52);
	clust.recordHit(300,5,51);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// verify description
	tag = super.getNthMainSignal(0);
	cw = tag.holder->getCWD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(200), cw.sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(5)/clust.getSecondsPerObs(),
				cw.sig.drift, D_FREQ);
	ASSERT_EQ_F(52, cw.sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1), cw.sig.width, D_FREQ);

	super.clear();
}

void
testPDCluster()
{
	PdmSuperClusterer super;
	PdmPDClusterer clust(&super, RES_1HZ);

	// with nothing added, should return empty vector
	clust.allHitsLoaded();
	super.compute();
	int count = super.getCount();
	ASSERT_EQ(0, count);

	// with one hit added, should return 1 supercluster describing that hit
	// "bogus" triplets should get stripped
	super.clear();
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 101, 50,
						4, 102, 50,
						6, 103, 50);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 101, 0,
						4, 102, 50,
						6, 103, 50);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 101, 50,
						4, 102, 0,
						6, 103, 50);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 101, 50,
						4, 102, 50,
						6, 103, 0);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						4, 101, 50,
						4, 102, 50,
						4, 103, 50);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// after clear, should return empty vector
	super.clear();
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(0, count);

	// with two hits added far apart,
	// should return 2 superclusters describing each hit
	super.clear();
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 101, 50,
						4, 102, 50,
						6, 103, 50);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 2101, 50,
						4, 2102, 50,
						6, 2103, 50);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(2, count);

	// verify description
	PdmClusterTag tag;
	tag = super.getNthMainSignal(0);
	const PulseSignalHeader *pd = &tag.holder->getPD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(100), pd->sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(32)/clust.getSecondsPerObs(),
				pd->sig.drift, D_FREQ);
	ASSERT_EQ_F(3*50, pd->sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1), pd->sig.width, D_FREQ);

	tag = super.getNthMainSignal(1);
	pd = &tag.holder->getPD()->getNth(tag.index);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(2100), pd->sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(32)/clust.getSecondsPerObs(),
				pd->sig.drift, D_FREQ);
	ASSERT_EQ_F(3*50, pd->sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1), pd->sig.width, D_FREQ);

	// with three triplets in the lev1 range, should calculate a path
	// through all pulses with unique spectra.  Only the strongest 
	// pulse for a given spectrum number is retained.  Power should
	// sum over all included pulses
	super.clear();
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 101, 8,
						4, 102, 2,
						6, 103, 4);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 111, 1,
						4, 102, 16,
						6, 113, 32);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						6, 103, 64,
						8, 104, 128,
						10, 105, 256);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// verify description
	tag = super.getNthMainSignal(0);
	pd = &tag.holder->getPD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(100), pd->sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(32)/clust.getSecondsPerObs(),
				pd->sig.drift, D_FREQ);
	ASSERT_EQ_F(8+16+64+128+256, pd->sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1+10*((float)2/3)),
				pd->sig.width, D_FREQ);

	// check pulse period
	ASSERT_EQ_F(2.0 * clust.getSecondsPerObs() / clust.getSpectraPerObs(),
				pd->train.pulsePeriod, D_PERIOD);

	// check individual pulses
	ASSERT_EQ(5, pd->train.numberOfPulses);
	Pulse *p = (Pulse *)(pd+1);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(101), p[0].frequency, D_FREQ);
	ASSERT_EQ(8, (int)p[0].power);
	ASSERT_EQ(2, p[0].spectrumNumber);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(102), p[1].frequency, D_FREQ);
	ASSERT_EQ(16, (int)p[1].power);
	ASSERT_EQ(4, p[1].spectrumNumber);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(103), p[2].frequency, D_FREQ);
	ASSERT_EQ(64, (int)p[2].power);
	ASSERT_EQ(6, p[2].spectrumNumber);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(104), p[3].frequency, D_FREQ);
	ASSERT_EQ(128, (int)p[3].power);
	ASSERT_EQ(8, p[3].spectrumNumber);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(105), p[4].frequency, D_FREQ);
	ASSERT_EQ(256, (int)p[4].power);
	ASSERT_EQ(10, p[4].spectrumNumber);

	// with two equally powered hits added outside the lev1 range but
	// within the lev2 range
	// should return 1 supercluster describing first hit
	super.clear();
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 101, 50,
						4, 102, 50,
						6, 103, 50);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 201, 50,
						4, 202, 50,
						6, 203, 50);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// verify description
	tag = super.getNthMainSignal(0);
	pd = &tag.holder->getPD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(100), pd->sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(32)/clust.getSecondsPerObs(),
				pd->sig.drift, D_FREQ);
	ASSERT_EQ_F(3*50, pd->sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1), pd->sig.width, D_FREQ);

	// with unequally powered hits added outside the lev1 range but
	// within the lev2 range
	// should return 1 supercluster describing highest power hit
	super.clear();
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 101, 50,
						4, 102, 50,
						6, 103, 50);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						2, 201, 60,
						4, 202, 60,
						6, 203, 60);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// verify description
	tag = super.getNthMainSignal(0);
	pd = &tag.holder->getPD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(200), pd->sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(32)/clust.getSecondsPerObs(),
				pd->sig.drift, D_FREQ);
	ASSERT_EQ_F(3*60, pd->sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1), pd->sig.width, D_FREQ);

	// check pulse period
	ASSERT_EQ_F(2.0 * clust.getSecondsPerObs() / clust.getSpectraPerObs(),
				pd->train.pulsePeriod, D_PERIOD);

	// check individual pulses
	ASSERT_EQ(3, pd->train.numberOfPulses);
	p = (Pulse *)(pd+1);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(201), p[0].frequency, D_FREQ);
	ASSERT_EQ(60, (int)p[0].power);
	ASSERT_EQ(2, p[0].spectrumNumber);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(202), p[1].frequency, D_FREQ);
	ASSERT_EQ(60, (int)p[1].power);
	ASSERT_EQ(4, p[1].spectrumNumber);
	ASSERT_EQ_F(clust.binsToAbsoluteMHz(203), p[2].frequency, D_FREQ);
	ASSERT_EQ(60, (int)p[2].power);
	ASSERT_EQ(6, p[2].spectrumNumber);

	// pulse period test -- 3 triplets; two with one period and one with
	// another;  should take most common period 
	super.clear();
	clust.recordTriplet(POL_LEFTCIRCULAR,
						8, 104, 10,
						10, 105, 10,
						12, 106, 10);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						14, 107, 10,
						18, 109, 10,
						22, 111, 10);
	clust.recordTriplet(POL_LEFTCIRCULAR,
						26, 113, 10,
						30, 115, 10,
						34, 117, 10);
	clust.allHitsLoaded();
	super.compute();
	count = super.getCount();
	ASSERT_EQ(1, count);

	// verify description
	tag = super.getNthMainSignal(0);
	pd = &tag.holder->getPD()->getNth(tag.index);

	ASSERT_EQ_F(clust.binsToAbsoluteMHz(100), pd->sig.rfFreq, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(32)/clust.getSecondsPerObs(),
				pd->sig.drift, D_FREQ);
	ASSERT_EQ_F(9*10, pd->sig.power, D_FREQ);
	ASSERT_EQ_F(clust.binsToRelativeHz(1), pd->sig.width, D_FREQ);

	// check pulse period
	ASSERT_EQ_F(4.0 * clust.getSecondsPerObs() / clust.getSpectraPerObs(),
				pd->train.pulsePeriod, D_PERIOD);

	super.clear();
}

int
main()
{
	//putenv("TR_Clusterer_detail=yes");
	testCWDCluster();
	testPDCluster();
	return 0;
}