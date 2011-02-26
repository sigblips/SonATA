/*******************************************************************************

 File:    PulseFollowupSignal.cpp
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
// Followup PD signal class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/PulseFollowupSignal.cpp,v 1.2 2009/02/22 04:41:42 kes Exp $
//
#include "PulseFollowupSignal.h"

namespace dx {

PulseFollowupSignal::PulseFollowupSignal(FollowUpPulseSignal *sig_):
		signal(*sig_), sigClass(CLASS_UNINIT), reason(CLASS_REASON_UNINIT)
{
}

PulseFollowupSignal::~PulseFollowupSignal()
{
}

PulseFollowupSignal *
PulseFollowupSignal::getPulseFollowup()
{
	return (this);
}

FollowUpPulseSignal&
PulseFollowupSignal::getSignal()
{
	return (signal);
}

#ifdef notdef
SignalDescription&
PulseFollowupSignal::getSignalDescription()
{
	return (signal.sig);
}
#endif

SignalId&
PulseFollowupSignal::getSignalId()
{
	return (signal.sig.origSignalId);
}

SignalId&
PulseFollowupSignal::getOrigSignalId()
{
	return (signal.sig.origSignalId);
}

void
PulseFollowupSignal::setClass(SignalClass sigClass_)
{
	sigClass = sigClass_;
}

void
PulseFollowupSignal::setReason(SignalClassReason reason_)
{
	reason = reason_;
}

SignalClass
PulseFollowupSignal::getClass()
{
	return (sigClass);
}

SignalClassReason
PulseFollowupSignal::getReason()
{
	return (reason);
}

float64_t
PulseFollowupSignal::getRfFreq()
{
	return (signal.sig.rfFreq);
}

float32_t
PulseFollowupSignal::getDrift()
{
	return (signal.sig.drift);
}

}
