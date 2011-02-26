/*******************************************************************************

 File:    PulseConfirmationTask.cpp
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
// $Header: /home/cvs/nss/sonata-pkg/dx/src/PulseConfirmationTask.cpp,v 1.2 2009/02/13 03:06:31 kes Exp $
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
#if ASSIGN_CPUS
	// assign the task processor affinity in multiprocessor systems
	int32_t nCpus = sysconf(_SC_NPROCESSORS_CONF);
	cpu_set_t affinity;
	CPU_ZERO(&affinity);
	int32_t n = 0;
	if (nCpus > 2) {
		// remove affinity for cpu 1
		++n;
	}
	if (nCpus > 3) {
		// remove affinity for cpu 2
		++n;
	}
	// assign affinity
	for (int32_t i = n; i < nCpus; ++i)
		CPU_SET(i, &affinity);
	pid_t tid = gettid();
	int rval = sched_setaffinity(tid, sizeof(cpu_set_t), &affinity);
	Assert(rval >= 0);
#endif

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
	Activity *act = state->findActivity(msg->getActivityId());
	if (!act)
		return;

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
		err = handleSecondaryMsg(msg);
		if (err) {
			FatalStr((Error) msg->getCode(), "msg code");
			LogFatal(ERR_IMT, msg->getActivityId(), "code %d", msg->getCode());
		}
		break;
	}
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
	// find the activity
	Activity *act = state->findActivity(msg->getActivityId());
	if (!act) {
		LogError(ERR_NSA, msg->getActivityId(), "message = %d",
				msg->getCode());
		return (ERR_NSA);
	}

	// make sure we are processing secondary candidates
	if (!act->allowSecondaryMsg())
		return (ERR_IMT);

	// if the activity is not in signal detection ignore the message

	Error err = 0;
	switch (msg->getCode()) {
	case SEND_CANDIDATE_PULSE_SIGNAL:
		if (act->getState() == DX_ACT_RUN_SD)
			err = doSecondaryConfirmation(msg, act);
		break;
	default:
		err = ERR_IMT;
		break;
	}
	return (err);
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
	Activity *act = state->findActivity(msg->getActivityId());
	if (!act || act->getState() != DX_ACT_RUN_SD)
		return;
	channel = act->getChannel();
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
//	Debug(DEBUG_NEVER, msg->getActivityId(), 0);

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
	Activity *act = state->findActivity(msg->getActivityId());
	if (!act || act->getState() != DX_ACT_RUN_SD)
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
	PulseSignalHeader *hdr = static_cast<PulseSignalHeader *> (msg->getData());
	SignalId id = hdr->sig.origSignalId;
	Debug(DEBUG_NEVER, id.number, "id.number");

	Signal *pulseCand = act->findCandidateUsingOrigId(id);
	if (!pulseCand) {
		LogError(ERR_SNC, msg->getActivityId(), "activity %d, number %d",
				id.activityId, id.number);
		return (ERR_SNC);
	}
	sig = pulseCand->getSignalDescription();
	createArchiveChannel(msg, pulseCand, sig, act);

	PulseSignal *pulseSig = pulseCand->getPulse();
	Assert(pulseSig);
	hdr = pulseSig->getSignal();
	::Pulse *p = (::Pulse *)(hdr + 1);

	int32_t pulses = hdr->train.numberOfPulses;
	Resolution res = hdr->train.res;

	// calculate PFA for the power found in the pulse bins seen by the primary
	float32_t totalPower = 0;
	int32_t totalPulses = 0;
	for (int i = 0; i < pulses; ++i) {
		::Pulse pulse = p[i];
		ComplexFloat32 bin;
		float32_t binPower = 0;
		if (pulse.pol != POL_RIGHTCIRCULAR) {
			bin = left.ac->extractBin(pulse.rfFreq, act->getBinWidthHz(res),
					pulse.spectrumNumber, true);
			binPower = norm(bin);
			++totalPulses;
		}
		if (pulse.pol != POL_LEFTCIRCULAR) {
			bin = right.ac->extractBin(pulse.rfFreq, act->getBinWidthHz(res),
					pulse.spectrumNumber, true);
			binPower += norm(bin);
			++totalPulses;
		}
		totalPower += binPower;
		p[i].power = binPower;
	}

	float64_t pfa = ChiSquare(2 * totalPulses, 2 * totalPower);
	hdr->sig.path.power = totalPower;
	hdr->cfm.snr = ((totalPower - totalPulses) / totalPulses) *
			act->getBinWidthHz(res);
	hdr->cfm.pfa = pfa;
	const DxActivityParameters& params = act->getActivityParams();
	Debug(DEBUG_CONFIRM, pfa, "pfa");
	Debug(DEBUG_CONFIRM, params.secondaryPulseTrainSignifThresh,
			"secondaryPulseTrainSignifThresh");
	Debug(DEBUG_CONFIRM, totalPower, "totalPower");

	// was the power distribution unusual enough to claim we saw something?
	if (pfa <= params.secondaryPulseTrainSignifThresh) {
		pulseCand->setClass(CLASS_CAND);
		pulseCand->setReason(PASSED_POWER_THRESH);
	}
	else {
		pulseCand->setClass(CLASS_RFI);
		pulseCand->setReason(FAILED_POWER_THRESH);
	}

	// report to the SSE
	hdr->sig.containsBadBands = SignalClassifier::applyBadBands(act, hdr->sig);
	sendSecondaryReport(pulseCand, act);

	// send original message back to confirmation task to report
	Msg *cMsg = msgList->alloc();
	msg->forward(cMsg);
	cMsg->setCode(SEND_PULSE_CANDIDATE_RESULT);
	confirmationQ->send(cMsg);

	return (0);
}

/**
 * Create the archive channel.
 *
 * Description:\n
 * 	Creates an archive channel consisting of a number of subchannels (typically
 * 	16) subchannels surrounding the nominal signal description.  An archive
 * 	channel is created for each polarization.
 */
void
PulseConfirmationTask::createArchiveChannel(Msg *msg, Signal *cand,
		const SignalDescription& sig, Activity *act)
{
	Debug(DEBUG_CWD_CONFIRM, (void *) cand, "candidate");

	// get the subchannel containing the signal; if it is near the band
	// edge, adjust it
	int32_t subchannel = sig.subchannelNumber;
	int32_t nSubchan = channel->getSubchannelsPerArchiveChannel();
	if (subchannel - nSubchan / 2 < 0)
		subchannel = nSubchan / 2;
	else if (subchannel + nSubchan / 2 >= channel->getUsableSubchannels())
		subchannel = channel->getUsableSubchannels() - nSubchan / 2;

	ArchiveChannel *rP = right.ac;
	ArchiveChannel *lP = left.ac;

	// initialize the archive channels
	rP->setup(channel->getSubchannelsPerArchiveChannel(),
			channel->getHalfFrames(),
			channel->getSamplesPerSubchannelHalfFrame(),
			channel->getCdStridePerSubchannel(),
			channel->getSubchannelOversampling(),
			channel->getSubchannelCenterFreq(subchannel),
			channel->getSubchannelRateMHz());

	lP->setup(channel->getSubchannelsPerArchiveChannel(),
			channel->getHalfFrames(),
			channel->getSamplesPerSubchannelHalfFrame(),
			channel->getCdStridePerSubchannel(),
			channel->getSubchannelOversampling(),
			channel->getSubchannelCenterFreq(subchannel),
			channel->getSubchannelRateMHz());

	ComplexPair *rCd = static_cast<ComplexPair *> (channel->getCdData(
			POL_RIGHTCIRCULAR, subchannel - nSubchan / 2, 0));
	ComplexPair *lCd = static_cast<ComplexPair *> (channel->getCdData(
			POL_LEFTCIRCULAR, subchannel - nSubchan / 2, 0));

	// create the archive channel data
	right.basePower = rP->create(rCd);
	left.basePower = lP->create(lCd);
}

void
PulseConfirmationTask::extractSignalData()
{
}

void
PulseConfirmationTask::sendSecondaryReport(Signal *cand, Activity *act)
{
	PulseSignal *pulseSig = cand->getPulse();
	Assert(pulseSig);
	PulseSignalHeader *hdr = pulseSig->getSignal();
	Assert(hdr);

	size_t size = sizeof(PulseSignalHeader)
			+ hdr->train.numberOfPulses * sizeof(::Pulse);
	MemBlk *blk = partitionSet->alloc(size);
	Assert(blk);
	PulseSignalHeader *data = static_cast<PulseSignalHeader *> (blk->getData());
	*data = *hdr;
	::Pulse *pulseSrc = (::Pulse *)(hdr + 1);
	::Pulse *pulseDest = (::Pulse *)(data + 1);
	for (int i=0; i<hdr->train.numberOfPulses; i++)
		pulseDest[i] = pulseSrc[i];

	Msg *msg = msgList->alloc(SEND_PULSE_CANDIDATE_RESULT,
			act->getActivityId(), data, size, blk);
	msg->setUnit((sonata_lib::Unit) UnitPulseConfirmation);
	respQ->send(msg);
}

}
