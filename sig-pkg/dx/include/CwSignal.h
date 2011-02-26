/*******************************************************************************

 File:    CwSignal.h
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
// CWD signal class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/CwSignal.h,v 1.3 2009/02/22 04:37:44 kes Exp $
//
#ifndef _CwSignalH
#define _CwSignalH

#include "Signal.h"

namespace dx {

class CwSignal: public Signal {
public:
	CwSignal(CwPowerSignal *sig_);
	~CwSignal();

	CwSignal *getCw();

	CwPowerSignal& getSignal();
	SignalDescription& getSignalDescription();
	SignalId& getSignalId();
	SignalId& getOrigSignalId();

	virtual void setClass(SignalClass sigClass_);
	virtual void setReason(SignalClassReason reason_);
	virtual SignalClass getClass();
	virtual SignalClassReason getReason();

	float64_t getRfFreq();
	float32_t getDrift();
	float32_t getWidth();
	float32_t getPower();

private:
	CwPowerSignal signal;

	// disallow
	CwSignal(const CwSignal&);
	Signal& operator=(const Signal&);
};

}

#endif