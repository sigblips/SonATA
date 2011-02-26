/*******************************************************************************

 File:    PulseSignal.h
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
// PD signal clas
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/PulseSignal.h,v 1.3 2009/02/22 04:37:45 kes Exp $
//
#ifndef _PulseSignalH
#define _PulseSignalH

#include "Partition.h"
#include "Signal.h"

namespace dx {

class PulseSignal: public Signal {
public:
	PulseSignal(PulseSignalHeader *sig_);
	~PulseSignal();

	PulseSignal *getPulse();

	void setConfirmationStats(ConfirmationStats& cfm_);

	PulseSignalHeader *getSignal();
	SignalDescription& getSignalDescription();
	SignalId& getSignalId();
	SignalId& getOrigSignalId();

	virtual void setClass(SignalClass sigClass_);
	virtual void setReason(SignalClassReason reason_);
	virtual SignalClass getClass();
	virtual SignalClassReason getReason();

	ConfirmationStats *getConfirmationStats();

	float64_t getRfFreq();
	float32_t getDrift();
	float32_t getWidth();
	float32_t getPower();
	Resolution getResolution();

private:
	MemBlk *blk;
	PulseSignalHeader *signal;

	// disallow
	PulseSignal(const PulseSignal&);
	Signal& operator=(const Signal&);
};

}

#endif