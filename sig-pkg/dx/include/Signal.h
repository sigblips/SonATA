/*******************************************************************************

 File:    Signal.h
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
// Signal base class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/Signal.h,v 1.4 2009/05/15 19:08:28 kes Exp $
//
#ifndef _SignalH
#define _SignalH

#include <list>
#include <sseDxInterface.h>
//#include "ArchiveChannel.h"
#include "Buffer.h"
//#include "BufLayout.h"
#include "DxTypes.h"

using namespace sonata_lib;

namespace dx {

class CwSignal;
//class CwCoherentSignal;
class PulseSignal;
class CwFollowupSignal;
class PulseFollowupSignal;
class BadBand;
class SignalList;

class Signal {
public:
	Signal(SignalState state_ = DETECTED);
	virtual ~Signal();

	// upcasting support
	virtual CwSignal *getCw();
//	virtual CwCoherentSignal *getCwCoherent();
	virtual PulseSignal *getPulse();
	virtual CwFollowupSignal *getCwFollowup();
	virtual PulseFollowupSignal *getPulseFollowup();
	virtual BadBand *getBadBand();

	virtual void setConfirmationStats(ConfirmationStats& cfm_);

	void setState(SignalState state_);
	void setOrigin(SystemType origin_);
	SignalState getState();
	SystemType getOrigin();
	bool isPrimary();

	virtual SignalDescription& getSignalDescription();

	virtual SignalId& getSignalId();
	virtual SignalId& getOrigSignalId();

	virtual void setClass(SignalClass sigClass_);
	virtual void setReason(SignalClassReason reason_);
	virtual SignalClass getClass();
	virtual SignalClassReason getReason();

	virtual ConfirmationStats *getConfirmationStats();

	virtual float64_t getRfFreq();
	virtual float32_t getDrift();
	virtual float32_t getWidth();
	virtual float32_t getPower();
	virtual Resolution getResolution();

//	void setCdLayout(const BufLayout& bufLayout_);
//	BufLayout& getCdLayout();

	void setBufPair(BufPair *bufPair_);
	void releaseBufPair();

//	BufPair *getBufPair();
//	Buffer *getLBuf();
//	Buffer *getRBuf();

private:
	SignalState state;
	SystemType origin;
	SignalDescription sig;
//	BufLayout layout;
//	ArchiveChannel right, left;
	BufPair *bufPair;

	// disallow
	Signal(const Signal&);
	Signal& operator=(const Signal&);
};

/**
 * Signal list
*/
typedef std::list<Signal *> SigList;

class SignalList {
public:
	SignalList();
	~SignalList();

	void clear();
	void add(Signal *sig_);
	void remove(Signal *sig_);
	void remove(SignalId& signalId_);
	void remove(SignalState state_);

	bool empty() { return (signalList.empty()); }
	int32_t size() { return (signalList.size()); }
	int32_t getCount(SignalType type_ = ANY_TYPE);
	int32_t getCount(SignalState state_ = ANY_STATE);
	SigList::iterator begin() { return (signalList.begin()); }

	Signal *getFirst();
	Signal *getFirst(SignalState state_);

	Signal *find(SignalId& signalId_);
	Signal *findUsingOrigId(SignalId& signalId_);

private:
	Lock llock;
	SigList signalList;

	SigList::iterator getPos(SignalId& signalId_);
	SigList::iterator getPosUsingOrigId(SignalId& signalId_);

	void lock() { llock.lock(); }
	void unlock() { llock.unlock(); }

	// forbidden
	SignalList(const SignalList&);
	SignalList& operator=(const SignalList&);
};

}

#endif