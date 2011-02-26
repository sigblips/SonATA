/*******************************************************************************

 File:    CwSignal.cpp
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
// DX CW signal class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/CwSignal.cpp,v 1.3 2009/02/22 04:41:41 kes Exp $
//
#include "CwSignal.h"

namespace dx {

CwSignal::CwSignal(CwPowerSignal *sig_): signal(*sig_)
{
}

CwSignal::~CwSignal()
{
}

CwSignal *
CwSignal::getCw()
{
	return (this);
}

CwPowerSignal&
CwSignal::getSignal()
{
	return (signal);
}

SignalDescription&
CwSignal::getSignalDescription()
{
	return (signal.sig);
}

SignalId&
CwSignal::getSignalId()
{
	return (signal.sig.signalId);
}

SignalId&
CwSignal::getOrigSignalId()
{
	return (signal.sig.origSignalId);
}

void
CwSignal::setClass(SignalClass sigClass_)
{
	signal.sig.sigClass = sigClass_;
}

void
CwSignal::setReason(SignalClassReason reason_)
{
	signal.sig.reason = reason_;
}

SignalClass
CwSignal::getClass()
{
	return (signal.sig.sigClass);
}

SignalClassReason
CwSignal::getReason()
{
	return (signal.sig.reason);
}

float64_t
CwSignal::getRfFreq()
{
	return (signal.sig.path.rfFreq);
}

float32_t
CwSignal::getDrift()
{
	return (signal.sig.path.drift);
}

float32_t
CwSignal::getWidth()
{
	return (signal.sig.path.width);
}

float32_t
CwSignal::getPower()
{
	return (signal.sig.path.power);
}

}
