/*******************************************************************************

 File:    CwFollowupSignal.h
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
// $Header: /home/cvs/nss/sonata-pkg/dx/include/CwFollowupSignal.h,v 1.2 2009/02/22 04:37:44 kes Exp $
//
#ifndef _FollowUpCwSignalH
#define _FollowUpCwSignalH

#include "Signal.h"

namespace dx {

class CwFollowupSignal: public Signal {
public:
	CwFollowupSignal(FollowUpCwSignal *sig_);
	~CwFollowupSignal();

	CwFollowupSignal *getCwFollowup();

	FollowUpCwSignal& getSignal();
	SignalId& getSignalId();
	SignalId& getOrigSignalId();

	virtual void setClass(SignalClass sigClass_);
	virtual void setReason(SignalClassReason reason_);
	virtual SignalClass getClass();
	virtual SignalClassReason getReason();

	float64_t getRfFreq();
	float32_t getDrift();

private:
	FollowUpCwSignal signal;
	SignalClass sigClass;
	SignalClassReason reason;

	// disallow
	CwFollowupSignal(const CwFollowupSignal&);
	Signal& operator=(const Signal&);
};

}

#endif