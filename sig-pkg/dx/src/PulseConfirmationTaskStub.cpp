/*******************************************************************************

 File:    PulseConfirmationTaskStub.cpp
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
// Pulse Confirmation class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/PulseConfirmationTaskStub.cpp,v 1.2 2009/02/13 03:06:31 kes Exp $
//
#include <math.h>
#include "dedrift.h"
#include "Buffer.h"
#include "ConfirmationChannel.h"
#include "DxOpsBitset.h"
#include "PulseConfirmationTask.h"
#include "PulseSignal.h"
#include "SignalClassifier.h"
#include "Statistics.h"

namespace dx {

PulseConfirmationTask *PulseConfirmationTask::instance = 0;

PulseConfirmationTask *
PulseConfirmationTask::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new PulseConfirmationTask("PulseConfirmationTask");
	l.unlock();
	return (instance);
}

PulseConfirmationTask::PulseConfirmationTask(string name_):
		QTask(name_, PULSE_CONFIRMATION_PRIO)
{
}

PulseConfirmationTask::~PulseConfirmationTask()
{
}

void
PulseConfirmationTask::extractArgs()
{
	// extract startup args
	PulseConfirmationArgs *pulseArgs =
			static_cast<PulseConfirmationArgs *> (args);
	Assert(pulseArgs);
	confirmationQ = pulseArgs->confirmationQ;
	Assert(confirmationQ);
	respQ = pulseArgs->respQ;
	Assert(respQ);

	msgList = MsgList::getInstance();
	Assert(msgList);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
	state = State::getInstance();
	Assert(state);
}

void
PulseConfirmationTask::handleMsg(Msg *msg)
{
	Debug(DEBUG_NEVER, (Error) msg->getCode(), "code");
	// find the activity
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId()))) {
		return;
	}

	Error err;
	switch (msg->getCode()) {
	case StartConfirmation:
		startActivity(msg);
		break;
	case STOP_DX_ACTIVITY:
		stopActivity(msg);
		break;
	case ConfirmCandidate:
		doPrimaryConfirmation(msg);
		break;
	default:
		if (err = handleSecondaryMsg(msg)) {
			FatalStr((Error) msg->getCode(), "msg code");
			LogFatal(ERR_IMT, msg->getActivityId(), "code %d", msg->getCode());
		}
	}
	return;
}

//
// handleSecondaryMsg: handle additional messages for a secondary detection
//
// Notes:
//		These messages are allowed only when the activity specifies the
//		DX will be processing secondary candidates.
//		The secondary must handle signal descriptions from a primary detector;
//		these signal descriptions specify the candidate signals to be
//		confirmed.
//
Error
PulseConfirmationTask::handleSecondaryMsg(Msg *msg)
{
	return (ERR_IMT);
}

//
// startActivity: start confirmation for this activity
//
// Notes:
//		We must not currently be doing confirmation
//
void
PulseConfirmationTask::startActivity(Msg *msg)
{
	// the activity must exist and be DX_ACT_RUN_SD
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId())))
		return;
	else if (act->getState() != DX_ACT_RUN_SD)
		return;
}

//
// stopActivity: stop the current activity
//
// Notes:
//		At this point, we are between candidates, so we can stop
//		the confirmation process.  Send a message to the
//		control task indicating that we are done.
//		The confirmation task is in charge of changing the
//		activity state.
//
void
PulseConfirmationTask::stopActivity(Msg *msg)
{
	Debug(DEBUG_NEVER, msg->getActivityId(), 0);

	Msg *cMsg = msgList->alloc((DxMessageCode) ActivityStopped,
			msg->getActivityId());
	cMsg->setUnit((sonata_lib::Unit) UnitPulseConfirmation);
	confirmationQ->send(cMsg);
}

//
// doPrimaryConfirmation: perform primary detector confirmation processing on
//		the signal
//
// Notes:
//		Primary pulse confirmation is really a no-op, because we
//		already know as much about the signal as we ever will for
//		this activity.  But we need to record the extraction parameters
//		for the archiver, so this method takes care of that.
//
void
PulseConfirmationTask::doPrimaryConfirmation(Msg *msg)
{
	// activity must exist and be in correct state
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId())))
		return;

	// if we're not running signal detection, don't process the
	// candidate
	if (act->getState() != DX_ACT_RUN_SD)
		return;

	Signal *cand = static_cast<Signal *> (msg->getData());

	// we don't do any confirmation here, but we still need to
	// compute the channel and buffer params for possible archiving
	SignalId signalId = cand->getSignalId();

	// compute the confirmation data parameters for the signal
	SignalDescription sig = cand->getSignalDescription();

	// the data is now attached to the candidate, so we're done
	cand->setState(CONFIRMED);

	// notify the confirmation task that we have finished this one
	MemBlk *blk = partitionSet->alloc(sizeof(Signal *));
	Assert(blk);
	Signal *signal = static_cast<Signal *> (blk->getData());
	signal = cand;
	Msg *cMsg = msgList->alloc((DxMessageCode) PulseConfirmationComplete,
			act->getActivityId(), signal, sizeof(Signal *), blk);
	cMsg->setUnit((sonata_lib::Unit) UnitPulseConfirmation);
	confirmationQ->send(cMsg);
}

//
// doSecondaryConfirmation: perform a pulse train confirmation as a
//		secondary
//
// Notes:
//		Secondary confirmation consists of extracting the set of
//		bins corresponding to the primary pulse trains, summing
//		their powers and checking significance with a Chi-square
//		calculation.
//		Pulse-matching is done using the spectrum and bin values
//		in the individual pulses.  A better way would be to
//		use the frequency and add a time-stamp to each pulse;
//		the corresponding spectrum and bin could then be computed
//		from those values.
//
Error
PulseConfirmationTask::doSecondaryConfirmation(Msg *msg, Activity *act)
{
	return (0);
}

void
PulseConfirmationTask::computeSignalChannelParams(Activity *act,
		Signal *cand)
{
	return;
}

void
PulseConfirmationTask::extractSignalData()
{
	return;
}

void
PulseConfirmationTask::sendSecondaryReport(Signal *cand, Activity *act)
{
	return;
}

}
