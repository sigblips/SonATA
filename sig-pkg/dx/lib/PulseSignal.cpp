/*******************************************************************************

 File:    PulseSignal.cpp
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
// Pulse signal class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/PulseSignal.cpp,v 1.3 2009/02/22 04:41:42 kes Exp $
//
#include <fftw3.h>
#include "System.h"
#include "PulseSignal.h"

using std::cout;
using namespace sonata_lib;

namespace dx {

PulseSignal::PulseSignal(PulseSignalHeader *sig_)
{
	int32_t pulses = sig_->train.numberOfPulses;
	size_t len = sizeof(PulseSignalHeader) + pulses * sizeof(Pulse);
	PartitionSet *partitionSet = PartitionSet::getInstance();
	blk = partitionSet->alloc(len);
	Assert(blk);
	signal = static_cast<PulseSignalHeader *> (blk->getData());
	*signal = *sig_;
//	cout << "PulseSignal, signal " << *signal;
	Pulse *dp = (Pulse *) (signal + 1);
	Pulse *sp = (Pulse *) (sig_ + 1);
	for (int32_t i = 0; i < pulses; ++i)
		dp[i] = sp[i];
}

PulseSignal::~PulseSignal()
{
	blk->free();
}

PulseSignal *
PulseSignal::getPulse()
{
	return (this);
}

void
PulseSignal::setConfirmationStats(ConfirmationStats& cfm_)
{
	signal->cfm = cfm_;
}

PulseSignalHeader *
PulseSignal::getSignal()
{
//	Debug(DEBUG_NEVER, (void *) signal, "signal");
	return (signal);
}

SignalDescription&
PulseSignal::getSignalDescription()
{
	return (signal->sig);
}

SignalId&
PulseSignal::getSignalId()
{
	return (signal->sig.signalId);
}

SignalId&
PulseSignal::getOrigSignalId()
{
	return (signal->sig.origSignalId);
}

void
PulseSignal::setClass(SignalClass sigClass_)
{
	signal->sig.sigClass = sigClass_;
}

void
PulseSignal::setReason(SignalClassReason reason_)
{
	signal->sig.reason = reason_;
}

SignalClass
PulseSignal::getClass()
{
	return (signal->sig.sigClass);
}

SignalClassReason
PulseSignal::getReason()
{
	return (signal->sig.reason);
}

ConfirmationStats *
PulseSignal::getConfirmationStats()
{
	return (&signal->cfm);
}

float64_t
PulseSignal::getRfFreq()
{
	return (signal->sig.path.rfFreq);
}

float32_t
PulseSignal::getDrift()
{
	return (signal->sig.path.drift);
}

float32_t
PulseSignal::getWidth()
{
	return (signal->sig.path.width);
}

float32_t
PulseSignal::getPower()
{
	return (signal->sig.path.power);
}


Resolution
PulseSignal::getResolution()
{
	return (signal->train.res);
}

}
	