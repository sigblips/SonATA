/*******************************************************************************

 File:    SignalClassifier.h
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
// Signal classifier class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/SignalClassifier.h,v 1.4 2009/02/22 04:48:37 kes Exp $
//
#ifndef _SignalClassifierH
#define _SignalClassifierH

#include <vector>
#include "Activity.h"
#include "DxOpsBitset.h"
#include "Signal.h"
#include "SuperClusterer.h"

using namespace sonata_lib;

namespace dx {

class SignalClassifier {
public:
	SignalClassifier();
	~SignalClassifier();

	void clear();
	void classifySignals(Activity *act);
	int32_t getCount();
	int32_t getCandidate(int32_t idx);
	Signal *findFollowupSignal(Activity *act, SignalDescription& sig);
	static bool_t applyBadBands(Activity *act, SignalDescription& sig);

private:
	int32_t candidatesOverMax;
	DxActivityParameters params;
	DxOpsBitset operations;
	RecentRfiMask *recentRfi;
	TestSignalMask *testSignal;

	void classifySignal(Activity *act, SignalDescription& sig,
			SuperClusterer *superClusterer, int32_t sigNum,
			int32_t maxCandidates);
	void displayPulseSignal(PulseSignalHeader& signal);
	static bool match(Activity *act, SignalDescription& sig, FrequencyBand& band);

	// forbidden
	SignalClassifier(const SignalClassifier&);
	SignalClassifier& operator=(const SignalClassifier&);
};

}

#endif