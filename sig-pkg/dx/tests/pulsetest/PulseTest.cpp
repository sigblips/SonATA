/*******************************************************************************

 File:    PulseTest.cpp
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

/**
 * PulseTest.cpp
 *
 *  Created on: Mar 16, 2010
 *      Author: kes
 */
#include <math.h>
#include <map>
#include <set>
#include <iostream>
#include <vector>

using namespace std;

// forward declaration
struct Pls;

typedef int int32_t;
typedef float float32_t;
typedef double float64_t;

const int32_t SPECTRA = 16;
const int32_t BINS = 32;

struct Pulse {
	int32_t spectrum;
	int32_t bin;
	float32_t power;

//	Pulse(): spectrum(0), bin(0), power(0.0) {}
//	Pulse(int32_t s, int32_t b, float32_t p): spectrum(s), bin(b), power(p) {}
	friend ostream& operator << (ostream& s, const Pulse& pulse);
};

struct Train {
	float32_t bin;
	float32_t drift;
	set<Pls *> pulses;

	Train(): bin(0.0), drift(0.0) {}
	Train(Pls& p0, Pls& p1);

	friend ostream& operator << (ostream& s, const Train& train);
};

typedef set<Train *> TrainSet;

struct Pls {
	Pulse pulse;
	TrainSet trains;

	Pls() {}
	Pls(const Pulse& p): pulse(p) {}

	friend ostream& operator << (ostream& s, const Pls& pls);
};

typedef vector<Pulse> PulseVector;
typedef vector<Pls> PlsVector;
typedef vector<Train *> TrainVector;

// canned pulse list
Pulse p[] = {
	{ 0, 3, 12 },
	{ 1, 30, 10 },
	{ 2, 5, 15 },
	{ 2, 11, 10 },
	{ 5, 4, 9 },
	{ 5, 28, 5 },
	{ 6, 9, 9 },
	{ 10, 3, 7 },
	{ 11, 13, 10 },
	{ 13, 25, 5 },
	{ 15, 5, 5 },
	{ 15, 17, 8 }
};
	
const int32_t PULSES = sizeof(p) / sizeof(Pulse);

// function declarations
void createPulses(PulseVector& pulses);
void createPls(PulseVector& pulses, PlsVector& pls);
void findTrains(PlsVector& pulses, TrainVector& trains);
void checkPulse(Pls& p0, Pls& p1, TrainVector& trains);
bool commonTrain(Pls& p0, Pls& p1);
bool containedInTrain(Pls& p0, Pls& p1, TrainVector& trains);
bool insideCone(Pls& p0, Pls& p1);
void createTrain(Pls& p0, Pls& p1, TrainVector& trains);

int
main(int argc, char **argv)
{
	PulseVector pulses;
	createPulses(pulses);
	cout << "Initial pulses:" << endl;
	for (PulseVector::iterator p = pulses.begin(); p != pulses.end(); ++p)
		cout << *p;

	PlsVector pls;
	createPls(pulses, pls);
	cout << "Initial plses:" << endl;
	for (PlsVector::iterator p = pls.begin(); p != pls.end(); ++p)
		cout << *p;
	TrainVector trains;
	findTrains(pls, trains);
	cout << trains.size() << " raw train(s):" << endl;
	int32_t i = 0;
	for (TrainVector::iterator p = trains.begin(); p != trains.end(); ++p)
		cout << i++ << endl << **p;
	
	// now eliminate all trains which do not contain at least 3 pulses
	for (TrainVector::iterator p = trains.begin(); p != trains.end(); ) {
		if ((*p)->pulses.size() < 3) {
			delete *p;
			p = trains.erase(p);
		}
		else
			++p;
	}
	cout << trains.size() << " final train(s):" << endl;
	i = 0;
	for (TrainVector::iterator p = trains.begin(); p != trains.end(); ++p)
		cout << i++ << endl << **p;
}

/**
 * Create a set of pulses for pulse detection
 */
void
createPulses(PulseVector& pulses)
{
	for (int32_t i = 0; i < PULSES; ++i)
		pulses.push_back(p[i]);
}

/**
 * Create the Pls vector from the set of pulses
*/
void
createPls(PulseVector& pulses, PlsVector& pls)
{
	for (int32_t i = 0; i < pulses.size(); ++i) {
		Pls pl(pulses[i]);
		pls.push_back(pl);
	}
}

/**
 * Find all trains.
*/
void
findTrains(PlsVector& pulses, TrainVector& trains)
{
	PlsVector::iterator ps, pe;
	TrainVector::iterator t;
	// start at the beginning and end of the pulse list, creating trains of
	// maximum length as we go
	// outer loop is 
	for (int32_t i = 0; i < pulses.size() - 1; ++i) {
		for (int32_t j = pulses.size() - 1; j > i; --j) {
			checkPulse(pulses[i], pulses[j], trains);
		}
	}
}

void
checkPulse(Pls& p0, Pls& p1, TrainVector& trains)
{
	if (commonTrain(p0, p1))
		return;
	if (containedInTrain(p0, p1, trains))
		return;
	if (insideCone(p0, p1))
		createTrain(p0, p1, trains);
}

/**
 * Check whether the two pulses are part of a common train
 *
 * Description:\n
 *	Compares the train sets of the two pulses to see if they are both
 *	contained in the same train.  Returns true if they.
*/

bool
commonTrain(Pls& p0, Pls& p1)
{
	if (!p0.trains.size() || !p1.trains.size())
		return (false);
	for (TrainSet::iterator p = p0.trains.begin(); p != p0.trains.end(); ++p) {
		if (p1.trains.find(*p) != p1.trains.end())
			return (true);
	}
	return (false);
}

/**
 * Check whether the second pulse can be addede to a train which already
 *	contains the first pulse.
 *
 * Description:\n
 *	Looks at each train to which the first pulse belongs, then sees whether
 *	the second pulse fits into that train (i.e., is within one bin of the
 *	line describing the train).  Returns true if the second pulse has been
 *	added to the train.
*/
bool
containedInTrain(Pls& p0, Pls& p1, TrainVector& trains)
{
	for (TrainSet::iterator p = p0.trains.begin(); p != p0.trains.end(); ++p) {
		float32_t bin = (*p)->bin + p1.pulse.spectrum * (*p)->drift;
		if (fabs(p1.pulse.bin - bin) < 1.0) {
			// part of the train, add it
			(*p)->pulses.insert(&p1);
			// and add the train to the pulse
			p1.trains.insert(*p);
			return (true);
		}
	}
	return (false);
}

/**
 * Test whether the pair of pulses lies within the maximum drift range.
 */
bool
insideCone(Pls& p0, Pls& p1)
{
	float32_t db = p1.pulse.bin - p0.pulse.bin;
	float32_t ds = p1.pulse.spectrum - p0.pulse.spectrum;
	float32_t d = 100;
	if (ds)
		d = db / ds;
	return (fabs(d) <= 1.0);
}

/**
 * Create a new train.
 */
void
createTrain(Pls& p0, Pls& p1, TrainVector& trains)
{
	// create a new train containing the pulses
	Train *t = new Train(p0, p1);
	trains.push_back(t);

	// now add the train to the pulses
	p0.trains.insert(t);
	p1.trains.insert(t);
}

Train::Train(Pls& p0, Pls& p1)
{
	float32_t db = p1.pulse.bin - p0.pulse.bin;
	float32_t ds = p1.pulse.spectrum - p0.pulse.spectrum;
	drift = db / ds;
	bin = p0.pulse.bin - p0.pulse.spectrum * drift;
	pulses.insert(&p0);
	pulses.insert(&p1);
}

ostream& operator << (ostream& s, const Pulse& pulse)
{
	s << "Pulse: spectrum: " << pulse.spectrum << ", bin: " << pulse.bin;
	s << ", power: " << pulse.power << endl;
	return (s);
}

ostream& operator << (ostream& s, const Train& train)
{
	s << "Train: bin: " << train.bin << ", drift: " << train.drift;
	s << ", pulses: " << train.pulses.size() << endl;
	set <Pls *>::iterator p;
	for (p = train.pulses.begin(); p != train.pulses.end(); ++p)
		s << **p;
	s << endl;
	return (s);
}

ostream& operator << (ostream& s, const Pls& pls)
{
	s << "Pls: " << pls.pulse;
#ifdef notdef
	TrainSet::iterator p;
	for (p = pls.trains.begin(); p != pls.trains.end(); ++p)
		s << "Train: :" << *p;
	s << endl;
#endif	
	return (s);
}