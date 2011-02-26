/*******************************************************************************

 File:    CwFollowupSignal.cpp
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
// Follow up CWD signal class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/CwFollowupSignal.cpp,v 1.2 2009/02/22 04:41:41 kes Exp $
//
#include "CwFollowupSignal.h"

namespace dx {

CwFollowupSignal::CwFollowupSignal(FollowUpCwSignal *sig_):
		signal(*sig_), sigClass(CLASS_UNINIT), reason(CLASS_REASON_UNINIT)
{
}

CwFollowupSignal::~CwFollowupSignal()
{
}

CwFollowupSignal *
CwFollowupSignal::getCwFollowup()
{
	return (this);
}

FollowUpCwSignal&
CwFollowupSignal::getSignal()
{
	return (signal);
}

#ifdef notdef
SignalDescription&
CwFollowupSignal::getSignalDescription()
{
	return (signal.sig);
}
#endif

SignalId&
CwFollowupSignal::getSignalId()
{
	return (signal.sig.origSignalId);
}

SignalId&
CwFollowupSignal::getOrigSignalId()
{
	return (signal.sig.origSignalId);
}

void
CwFollowupSignal::setClass(SignalClass sigClass_)
{
	sigClass = sigClass_;
}

void
CwFollowupSignal::setReason(SignalClassReason reason_)
{
	reason = reason_;
}

SignalClass
CwFollowupSignal::getClass()
{
	return (sigClass);
}

SignalClassReason
CwFollowupSignal::getReason()
{
	return (reason);
}

float64_t
CwFollowupSignal::getRfFreq()
{
	return (signal.sig.rfFreq);
}

float32_t
CwFollowupSignal::getDrift()
{
	return (signal.sig.drift);
}

}
