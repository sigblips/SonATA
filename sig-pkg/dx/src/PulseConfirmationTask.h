/*******************************************************************************

 File:    PulseConfirmationTask.h
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
//  Pulse candidate signal confirmation task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/PulseConfirmationTask.h,v 1.3 2009/02/22 04:48:37 kes Exp $
//
#ifndef _PulseConfirmationTaskH
#define _PulseConfirmationTaskH

#include "ArchiveChannel.h"
#include "Channel.h"
#include "DxStruct.h"
#include "Msg.h"
#include "Partition.h"
#include "QTask.h"
#include "Signal.h"
#include "State.h"

using namespace sonata_lib;

namespace dx {

//
// startup arguments
//
struct PulseConfirmationArgs {
	Queue *confirmationQ;
	Queue *respQ;

	PulseConfirmationArgs(): confirmationQ(0), respQ(0) {}
	PulseConfirmationArgs(Queue *confirmationQ_, Queue *respQ_):
			confirmationQ(confirmationQ_), respQ(respQ_) {}
};

/**
 * Confirmation data structure for a single polarization.
 */
struct PulseData {
	ArchiveChannel *ac;					// archive channel data
	float32_t basePower;				// baseline power

	PulseData(): ac(0), basePower(0)
	{
		ac = new ArchiveChannel;
		Assert(ac);
	}
};

class PulseConfirmationTask: public QTask {
public:
	static PulseConfirmationTask *getInstance();
	~PulseConfirmationTask();

private:
	static PulseConfirmationTask *instance;

	Channel *channel;
	Queue *confirmationQ;				// confirmation task queue
	Queue *respQ;						// SSE response queue
	SignalDescription sig;				// signal description
	PulseData right, left;;

	Msg *msg;
	PartitionSet *partitionSet;
	State *state;

	// methods
	void extractArgs();
	void handleMsg(Msg *msg);
	Error handleSecondaryMsg(Msg *msg);

	void startActivity(Msg *msg);
	void stopActivity(Msg *msg);

	void doPrimaryConfirmation(Msg *msg);
	Error doSecondaryConfirmation(Msg *msg, Activity *act);

	void createArchiveChannel(Msg *msg, Signal *cand,
			const SignalDescription& sig, Activity *act);
	void extractSignalData();

	// secondary methods
	void sendSecondaryReport(Signal *sig, Activity *act);

	// hidden
	PulseConfirmationTask(string name_);

	// forbidden
	PulseConfirmationTask(const PulseConfirmationTask&);
	PulseConfirmationTask& operator=(const PulseConfirmationTask&);
};

}

#endif