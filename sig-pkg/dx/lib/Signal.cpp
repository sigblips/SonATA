/*******************************************************************************

 File:    Signal.cpp
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
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/Signal.cpp,v 1.5 2009/05/24 23:42:44 kes Exp $
//
#include "Err.h"
#include "Signal.h"

using namespace sonata_lib;

namespace dx {

Signal::Signal(SignalState state_): state(state_), origin(PRIMARY), bufPair(0)
{
	Debug(DEBUG_SIGNAL, (void *) this, "this");
	Debug(DEBUG_SIGNAL, (int32_t) state, "state");
}

Signal::~Signal()
{
	Debug(DEBUG_SIGNAL, (void *) this, "this");
	releaseBufPair();
}

CwSignal *
Signal::getCw()
{
	return (0);
}

#ifdef notdef
CwCoherentSignal *
Signal::getCwCoherent()
{
	return (0);
}
#endif

PulseSignal *
Signal::getPulse()
{
	return (0);
}

CwFollowupSignal *
Signal::getCwFollowup()
{
	return (0);
}

PulseFollowupSignal *
Signal::getPulseFollowup()
{
	return (0);
}

BadBand *
Signal::getBadBand()
{
	return (0);
}

void
Signal::setConfirmationStats(ConfirmationStats& cfm_)
{
}

void
Signal::setState(SignalState state_)
{
	state = state_;
}

void
Signal::setOrigin(SystemType origin_)
{
	origin = origin_;
}

SignalState
Signal::getState()
{
	return (state);
}

SystemType
Signal::getOrigin()
{
	return (origin);
}

bool
Signal::isPrimary()
{
	return (origin == PRIMARY);
}

SignalDescription&
Signal::getSignalDescription()
{
	return (sig);
}

SignalId&
Signal::getSignalId()
{
	return (sig.signalId);
}

SignalId&
Signal::getOrigSignalId()
{
	return (sig.origSignalId);
}

void
Signal::setClass(SignalClass sigClass_)
{
	sig.sigClass = sigClass_;
}

void
Signal::setReason(SignalClassReason reason_)
{
	sig.reason = reason_;
}

SignalClass
Signal::getClass()
{
	return (sig.sigClass);
}

SignalClassReason
Signal::getReason()
{
	return (sig.reason);
}

ConfirmationStats *
Signal::getConfirmationStats()
{
	return (0);
}

float64_t
Signal::getRfFreq()
{
	return (0);
}

float32_t
Signal::getDrift()
{
	return (0);
}

float32_t
Signal::getWidth()
{
	return (0);
}

float32_t
Signal::getPower()
{
	return (0);
}

Resolution
Signal::getResolution()
{
	return (RES_UNINIT);
}

#ifdef notdef
void
Signal::setCdLayout(const BufLayout& layout_)
{
	layout = layout_;
}

BufLayout&
Signal::getCdLayout()
{
	return (layout);
}
#endif

//
// retrieval buffer methods
//
void
Signal::setBufPair(BufPair *bufPair_)
{
	if (bufPair)
		FatalStr(ERR_BAA, "Signal::setBufPair");

	bufPair = bufPair_;
}

void
Signal::releaseBufPair()
{
	if (bufPair)
		bufPair->release();
	bufPair = 0;
}

#ifdef notdef
BufPair *
Signal::getBufPair()
{
	return (bufPair);
}

Buffer *
Signal::getLBuf()
{
	return (bufPair ? bufPair->getLBuf() : 0);
}

Buffer *
Signal::getRBuf()
{
	return (bufPair ? bufPair->getRBuf() : 0);
}
#endif

SignalList::SignalList(): llock("slLock"), signalList()
{
}

SignalList::~SignalList()
{
	clear();
}

void
SignalList::clear()
{
	Signal *sig;
	SigList::iterator pos;

	lock();
	for (pos = signalList.begin(); pos != signalList.end(); ++pos) {
		sig = *pos;
		delete sig;
	}
	signalList.clear();
	unlock();
}

void
SignalList::add(Signal *sig_)
{
	lock();
	signalList.push_back(sig_);
	unlock();
}

void
SignalList::remove(Signal *sig_)
{
	lock();
	if (sig_) {
		signalList.remove(sig_);
		delete sig_;
	}
	unlock();
}

//
// remove: remove all signals with state "state_"
//
void
SignalList::remove(SignalState state_)
{
	Signal *sig;
	SigList::iterator pos;

	lock();
	for (pos = signalList.begin(); pos != signalList.end(); ) {
		sig = *pos;
		if (state_ == ANY_STATE || sig->getState() == state_) {
			pos = signalList.erase(pos);
			delete sig;
		}
		else
			++pos;
	}
	unlock();
}

void
SignalList::remove(SignalId& signalId_)
{
	Signal *sig;
	SigList::iterator pos;

	lock();

	Debug(DEBUG_SIGNAL, (int32_t) signalId_.number, "signalID.number");

	pos = getPos(signalId_);
	Debug(DEBUG_SIGNAL, 1, 0);
	if (pos != signalList.end()) {
		Debug(DEBUG_SIGNAL, 2, 0);
		sig = *pos;
		signalList.erase(pos);
		delete sig;
	}
	Debug(DEBUG_SIGNAL, 3, 0);
	unlock();
}

//
// getCount: get count of signals of a specified type
//
int32_t
SignalList::getCount(SignalType type_)
{
	int32_t count = 0;
	SigList::iterator pos;

	if (type_ == ANY_TYPE)
		return (size());

	lock();
	for (pos = signalList.begin(); pos != signalList.end(); ++pos) {
		if (type_ == CW_POWER && (*pos)->getCw())
			++count;
		else if (type_ == PULSE && (*pos)->getPulse())
			++count;
	}
	unlock();
	return (count);
}

//
// getCount: get count of signals in a specified state
//
int32_t
SignalList::getCount(SignalState state_)
{
	int32_t count = 0;
	SigList::iterator pos;

	if (state_ == ANY_STATE)
		return (size());

	lock();
	for (pos = signalList.begin(); pos != signalList.end(); ++pos) {
		if ((*pos)->getState() == state_)
			++count;
	}
	unlock();
	return (count);
}

//
// getFirst: get the first signal in the list
//
// Notes:
//		The signal is not removed from the list
//
Signal *
SignalList::getFirst()
{
	Signal *sig = 0;

	lock();
	if (!signalList.empty())
		sig = signalList.front();
	unlock();
	return (sig);
}

//
// getFirst: get the first signal with a given state
//
Signal *
SignalList::getFirst(SignalState state_)
{
	Signal *sig = 0;
	SigList::iterator pos;

	if (state_ == ANY_STATE)
		return (getFirst());

	lock();
	for (pos = signalList.begin(); pos != signalList.end(); ++pos) {
		if ((*pos)->getState() == state_) {
			sig = *pos;
			break;
		}
	}
	unlock();
	return (sig);
}

//
// find: find the specified signal, return it and remove it from the list
//
// Notes:
//		Finds the signal by its id
//
Signal *
SignalList::find(SignalId& signalId_)
{
	Signal *sig = 0;
	SigList::iterator pos;

	lock();
	pos = getPos(signalId_);
	if (pos != signalList.end()) {
		sig = *pos;
		Debug(DEBUG_SIGNAL, (void *) sig, "sig");
	}
	unlock();
	return (sig);
}
//
// findUsingOrigId: find the specified signal, return it and remove it from the list
//
// Notes:
//		Finds the signal by its id
//
Signal *
SignalList::findUsingOrigId(SignalId& signalId_)
{
	Signal *sig = 0;
	SigList::iterator pos;

	lock();
	pos = getPosUsingOrigId(signalId_);
	if (pos != signalList.end()) {
		sig = *pos;
		Debug(DEBUG_SIGNAL, (void *) sig, "sig");
	}
	unlock();
	return (sig);
}
//
// getPos: find the position of the specified signal in the list
//
//
SigList::iterator
SignalList::getPos(SignalId& signalId_)
{
	SigList::iterator pos;

	lock();
	Debug(DEBUG_SIGNAL, (int32_t) signalId_.dxNumber, "_dx");
	Debug(DEBUG_SIGNAL, (int32_t) signalId_.activityId, "_activity");
	Debug(DEBUG_SIGNAL, (int32_t) signalId_.number, "_number");
	Debug(DEBUG_SIGNAL, (int32_t) signalList.size(), "size");
	for (pos = signalList.begin(); pos != signalList.end(); ++pos) {
		Debug(DEBUG_SIGNAL, (void *) *pos, "*pos");
		SignalId signalId = (*pos)->getSignalId();
		Debug(DEBUG_SIGNAL, (int32_t) signalId.dxNumber, "dx");
		Debug(DEBUG_SIGNAL, (int32_t) signalId.activityId, "activity");
		Debug(DEBUG_SIGNAL, (int32_t) signalId.number, "number");
		if (signalId.dxNumber == signalId_.dxNumber
				&& signalId.activityId == signalId_.activityId
				&& signalId.number == signalId_.number) {
			break;
		}
	}
	unlock();
	return (pos);
}
//
// getPosUsingOrigId: as above, but match original ID instead of current
//
//
SigList::iterator
SignalList::getPosUsingOrigId(SignalId& signalId_)
{
	SigList::iterator pos;

	lock();
	Debug(DEBUG_SIGNAL, (int32_t) signalId_.dxNumber, "_dx");
	Debug(DEBUG_SIGNAL, (int32_t) signalId_.activityId, "_activity");
	Debug(DEBUG_SIGNAL, (int32_t) signalId_.number, "_number");
	Debug(DEBUG_SIGNAL, (int32_t) signalList.size(), "size");
	for (pos = signalList.begin(); pos != signalList.end(); ++pos) {
		Debug(DEBUG_SIGNAL, (void *) *pos, "*pos");
		SignalId signalId = (*pos)->getOrigSignalId();
		Debug(DEBUG_SIGNAL, (int32_t) signalId.dxNumber, "dx");
		Debug(DEBUG_SIGNAL, (int32_t) signalId.activityId, "activity");
		Debug(DEBUG_SIGNAL, (int32_t) signalId.number, "number");
		if (signalId.dxNumber == signalId_.dxNumber
				&& signalId.activityId == signalId_.activityId
				&& signalId.number == signalId_.number) {
			break;
		}
	}
	unlock();
	return (pos);
}

}