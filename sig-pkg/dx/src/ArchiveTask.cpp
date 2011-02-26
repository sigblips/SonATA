/*******************************************************************************

 File:    ArchiveTask.cpp
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
// Archive task
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ArchiveTask.cpp,v 1.8 2009/06/30 18:47:22 kes Exp $
//
#include <sys/time.h>
#include "ArchiveTask.h"
#include "DxErr.h"
#include "Log.h"

namespace dx {

ArchiveTask *ArchiveTask::instance = 0;

ArchiveTask *
ArchiveTask::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new ArchiveTask("ArchiveTask");
	l.unlock();
	return (instance);
}

ArchiveTask::ArchiveTask(string tname_):
		QTask(tname_, ARCHIVE_PRIO), activity(0), channel(0),
		controlQ(0), archiverOutQ(0), msgList(0), partitionSet(0), state(0),
		archiverCmdTask(0), archiverConnectionTask(0), archiverInputTask(0),
		archiverOutputTask(0)
{
}

ArchiveTask::~ArchiveTask()
{
}

void
ArchiveTask::extractArgs()
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

	// extract the startup arguments
	ArchiveArgs *archiveArgs = static_cast<ArchiveArgs *> (args);
	Assert(archiveArgs);
	controlQ = archiveArgs->controlQ;
	Assert(controlQ);

	msgList = MsgList::getInstance();
	Assert(msgList);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
	state = State::getInstance();
	Assert(state);

	archiveList.clear();
	discardList.clear();

	// create and dependent tasks
	createTasks();
	startTasks();
}

/**
 * Create the dependent archiver tasks.
 */
void
ArchiveTask::createTasks()
{
	archiverConnectionTask = ArchiverConnectionTask::getInstance();
	Assert(archiverConnectionTask);
	archiverInputTask = ArchiverInputTask::getInstance();

	Assert(archiverInputTask);
	archiverOutputTask = ArchiverOutputTask::getInstance();
	Assert(archiverOutputTask);
	archiverOutQ = archiverOutputTask->getInputQueue();

	archiverCmdTask = ArchiverCmdTask::getInstance();
	Assert(archiverCmdTask);
}

/**
 * Start the dependent archiver tasks.
 */
void
ArchiveTask::startTasks()
{
	// create the archiver connection
	Connection *archiver = new Tcp("Archiver", (sonata_lib::Unit) UnitArchiver,
			ActiveTcpConnection);
	Assert(archiver);

	// create the tasks
	archiverConnectionArgs = ArchiverConnectionArgs(archiver);
	archiverConnectionTask->start(&archiverConnectionArgs);

	Queue *archiverCmdQ = archiverCmdTask->getInputQueue();
	Assert(archiverCmdQ);
	Queue *archiverConnectionQ = archiverConnectionTask->getInputQueue();
	Assert(archiverConnectionQ);
	archiverInputArgs = ArchiverInputArgs(archiver, archiverCmdQ,
			archiverConnectionQ);
	archiverInputTask->start(&archiverInputArgs);

	archiverOutputArgs = ArchiverOutputArgs(archiver);
	archiverOutputTask->start(&archiverOutputArgs);

	archiverCmdArgs = ArchiverCmdArgs(archiverOutQ);
	archiverCmdTask->start(&archiverCmdArgs);
}

void
ArchiveTask::handleMsg(Msg *msg)
{
	uint32_t code = msg->getCode();
	switch (code) {
	case StartArchive:
		startActivity(msg);
		break;
	case STOP_DX_ACTIVITY:
		stopActivity(msg);
		break;
	case CwConfirmationComplete:
	case PulseConfirmationComplete:
		resolveCandidate(msg);
		break;
	case ConfirmationComplete:
		processConfirmationComplete(msg);
		break;
	case REQUEST_ARCHIVE_DATA:
		// queue an archive request
		requestArchive(msg);
		break;
	case DISCARD_ARCHIVE_DATA:
		// discard the archive data for this candidate
		discardArchive(msg);
		break;
	default:
		LogFatal(ERR_IMT, msg->getActivityId(), "type %d", code);
		Fatal(ERR_IMT);
		break;
	}
}

void
ArchiveTask::startActivity(Msg *msg)
{
	if (activity) {
		LogError(ERR_AIA, msg->getActivityId(),
				"activity %d, can't start activity %d",
				activity->getActivityId(), msg->getActivityId());
		return;
	}

	// clear the internal holding lists
	allCandidatesReceived = false;
	archiveList.clear();
	discardList.clear();

	// the activity must exist and be DX_ACT_RUN_SD
	int32_t activityId = msg->getActivityId();
	Activity *act = state->findActivity(activityId);

	if (!(act = state->findActivity(activityId))) {
		LogError(ERR_NSA, activityId, "activity %d", activityId);
		return;
	}
	else if (act->getState() != DX_ACT_RUN_SD
			&& act->getState() != DX_ACT_PEND_SD) {
		LogError(ERR_ANS, act->getActivityId(), "activity %d, state %d",
				act->getActivityId(), act->getState());
		return;
	}

	activity = act;
	channel = activity->getChannel();
	Assert(channel);
}

void
ArchiveTask::stopActivity(Msg *msg)
{
	int32_t activityId = msg->getActivityId();

	//Debug(DEBUG_ARCHIVE, (int32_t) activityId, "msg act");
	// if we're not processing this activity, just send acknowledgement
	// that it is stopped
	if (!activity || activity->getActivityId() != activityId) {
		sendActivityStopped(msg);
		return;
	}

	//Debug(DEBUG_ARCHIVE, (int32_t) activity->getActivityId(), "act act");

	// just release all the candidates and notify that we are done
	activity->releaseCandidates(ANY_STATE);
	sendActivityStopped(msg);
	sendArchiveComplete(msg);

	//Debug(DEBUG_ARCHIVE, 0, "stopActivity");
}

//
// resolveCandidate: resolve an archive candidate
//
// Notes:
//		This method is required only because an archive request
//		can be received from the archiver before we have finished
//		the confirmation process (in the case of pulse detection).
//		It checks the archive and discard lists to see whether
//		an archive or discard request has already been received
//		for the candidate - if it has, then the candidate is
//		archived or discarded now.
//
void
ArchiveTask::resolveCandidate(Msg *msg)
{
	ArchiveRequest archiveRequest;
	Signal *candidate;
	SignalId signalId;
	ArchiveList::iterator pos;

	candidate = static_cast<Signal *> (msg->getData());
	signalId = candidate->getSignalId();
#if 0
	Debug(DEBUG_ARCHIVE, signalId.number, "signalId.number");
	Debug(DEBUG_ARCHIVE, (int32_t)candidate, "candidate");
	CDLayout tmpBufLayout = candidate->getCDLayout();
	Debug(DEBUG_ARCHIVE, tmpBufLayout.centerFreq, "tmpBufLayout.centerFreq");
	Debug(DEBUG_ARCHIVE, tmpBufLayout.subchannels, "tmpBufLayout.subchannels");
#endif
	// check the archive list first, then the discard list
	if ((pos = archiveList.find(signalId.number)) != archiveList.end()) {
		//cout << "pending archive " << endl << signalId;
		requestArchive(msg, pos->second);
		archiveList.erase(pos);
	}
	else if ((pos = discardList.find(signalId.number)) != discardList.end()) {
		//cout << "pending discard " << endl << signalId;
		//Debug(DEBUG_ARCHIVE, 0, "found in discardList");
		discardArchive(msg, pos->second);
		discardList.erase(pos);
	}
}

void
ArchiveTask::processConfirmationComplete(Msg *msg)
{
	// we may have already aborted the activity
	if (!activity)
		return;

	// all candidates have been received; now we just wait for archive
	allCandidatesReceived = true;

	//Debug(DEBUG_ARCHIVE, activity->getCombinedCount(AnyState),
			//"cand count");
	if (!activity->getCombinedCount(ANY_STATE)) {
		archiveList.clear();
		discardList.clear();
		sendArchiveComplete(msg);
		//Debug(DEBUG_ARCHIVE, msg->getActivityId(),
			//	"sent archive complete, act");
	}
}

//
// requestArchive: create a dummy request to archive a candidate
//
void
ArchiveTask::requestArchive(Msg *msg, ArchiveRequest& archiveRequest)
{
	ArchiveRequest aRequest = archiveRequest;

	Msg *aMsg = msgList->alloc(REQUEST_ARCHIVE_DATA, msg->getActivityId(),
			&aRequest, sizeof(ArchiveRequest), 0, STATIC);
	requestArchive(aMsg);
	msgList->free(aMsg);
}

void
ArchiveTask::requestArchive(Msg *msg)
{
	ArchiveRequest *archiveRequest =
			static_cast<ArchiveRequest *> (msg->getData());

	// activity must exist and be in correct state
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId())))
		return;

	if (!activity) {
		LogError(ERR_NAIA, msg->getActivityId(), "archive request");
		return;
	}
	else if (activity != act) {
		LogError(ERR_ANIA, act->getActivityId(), "archive activity = %d",
				activity->getActivityId());
		return;
	}

	//Debug(DEBUG_ARCHIVE, msg->getActivityId(), "activity");
	//Debug(DEBUG_ARCHIVE, (int32_t) act->getState(), "state");

	// if we're stopping,
	if (act->getState() == DX_ACT_STOPPING) {
		//Debug(DEBUG_ARCHIVE, msg->getActivityId(), "stopping");
		return;
	}

	// must be running signal detection
	if (act->getState() != DX_ACT_RUN_SD) {
		LogError(ERR_IAS, act->getActivityId(), "activity %d, state = %d",
				act->getActivityId(), act->getState());
		return;
	}

	// get the archive request and verify that the signal is a valid
	// candidate
	archiveRequest = static_cast<ArchiveRequest *> (msg->getData());

//	Debug(DEBUG_ALWAYS, (int32_t) archiveRequest, "archive request");

	Signal *candidate
			= act->findCandidateInEitherList(archiveRequest->signalId);
	//Debug(DEBUG_NEVER, (int32_t) candidate, "candidate");
//	if (candidate)
//		Debug(DEBUG_ALWAYS, (int32_t) candidate->getState(), "state");
	// it must be a candidate, but it may not yet be confirmed
	if (!candidate) {
		//Debug(DEBUG_ARCHIVE, archiveRequest->signalId.number,
		 //           "Signal not a Candidate id = ");
		LogError(ERR_SNC, act->getActivityId(), "signal Id %d",
				archiveRequest->signalId.number);
		return;
	}
//	Debug(DEBUG_ARCHIVE, (int32_t) candidate->getState(),
			//"candidate state");

	// if the candidate has not yet been processed by confirmation,
	// put it in a holding queue
	if (candidate->getState() != CONFIRMED) {
		// add the candidate id to the archive list
		archiveList.insert(pair<int32_t, ArchiveRequest>
				(archiveRequest->signalId.number, *archiveRequest));
		return;
	}

#if 0
	Debug(DEBUG_ARCHIVE, (int32_t)candidate, "candidate");
	CDLayout tmpBufLayout = candidate->getCDLayout();
	Debug(DEBUG_ARCHIVE, tmpBufLayout.centerFreq, "tmpBufLayout.centerFreq");
	Debug(DEBUG_ARCHIVE, tmpBufLayout.subchannels, "tmpBufLayout.subchannels");
#endif

	sendArchive(candidate);

	// release the buffer pair
//	Debug(DEBUG_ARCHIVE, 0, "releaseBufPair");
//	candidate->releaseBufPair();

	// release the candidate
	//Debug(DEBUG_ARCHIVE, (int32_t) act->getCombinedCount(AnyState),
	//		"candidateCount before remove");
	act->removeCandidateFromEitherList(candidate);
	// see whether we have archived all candidates
//	Debug(DEBUG_ARCHIVE, (int32_t) allCandidatesReceived,
			//"allCandidatesRec");
	//Debug(DEBUG_ARCHIVE, (int32_t) act->getCombinedCount(AnyState),
	//		"candidateCount after remove");
	if (!act->getCombinedCount(ANY_STATE) && allCandidatesReceived)
		sendArchiveComplete(msg);
}

#ifdef notdef
/**
 * Assemble a buffer of confirmation data for the archived signal.
 *
 * Description:\n
 * 	Left and right buffers are allocated to hold the data.\n\n
 *
 * Notes:\n
 * 	All CD data is buffered in memory, corner-turned so that each
 * 	subchannel's data is contiguous and adjacent subchannels are
 * 	contiguous.  We want to send one half frame from each subchannel
 * 	being archived.
 */
void
ArchiveTask::retrieveCdData(Activity *act, Signal *candidate)
{
	// we have the signal, make sure it has a buffer pair attached
	BufPair *bufPair;

	if (!(bufPair = candidate->getBufPair())) {
		bufPair = state->getBufPair(true);
		candidate-><(bufPair);
	}


	// now retrieve the data for the candidate
	DiskIORequest lRetrieval, rRetrieval;

	lRetrieval.buffer = candidate->getLBuf();
	rRetrieval.buffer = candidate->getRBuf();

	bufLayout = candidate->getCDLayout();
	bufLayout.computeXferSpec(lRetrieval.xfer);
	bufLayout.computeXferSpec(rRetrieval.xfer);

	lRetrieval.xfer.dest.base = reinterpret_cast<uint64_t>
			(lRetrieval.buffer->getBuf());
	rRetrieval.xfer.dest.base = reinterpret_cast<uint64_t>
			(rRetrieval.buffer->getBuf());

	lRetrieval.file = act->getFile(LCdFile);
	rRetrieval.file = act->getFile(RCdFile);

	channel->retrieveCDData(&lRetrieval);
	channel->retrieveCDData(&rRetrieval);
}
#endif

//
// discardArchive: create a dummy request to discard a candidate
//
void
ArchiveTask::discardArchive(Msg *msg, ArchiveRequest& discardRequest)
{
	ArchiveRequest dRequest = discardRequest;
	Msg *dMsg;

	dMsg = msgList->alloc(REQUEST_ARCHIVE_DATA, msg->getActivityId(),
			&dRequest, sizeof(ArchiveRequest), 0, STATIC);
	discardArchive(dMsg);
	msgList->free(dMsg);
}

void
ArchiveTask::discardArchive(Msg *msg)
{
	ArchiveRequest *archiveRequest
			= static_cast<ArchiveRequest *> (msg->getData());

	// activity must exist and be in correct state
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId())))
		return;

	if (!activity) {
		LogError(ERR_NAIA, msg->getActivityId(), "archive request");
		return;
	}
	else if (activity != act) {
		LogError(ERR_ANIA, act->getActivityId(), "archive activity = %d",
				activity->getActivityId());
		return;
	}

	if (act->getState() == DX_ACT_STOPPING) {
		//Debug(DEBUG_ARCHIVE, msg->getActivityId(), "stopping");
		return;
	}

	// must be running signal detection
	if (act->getState() != DX_ACT_RUN_SD) {
		LogError(ERR_IAS, act->getActivityId(), "activity %d, state = %d",
				act->getActivityId(), act->getState());
		return;
	}

	if (activity != act) {
		activity = act;
		allCandidatesReceived = false;
	}

	Assert(activity);

	// get the archive request and verify that the signal is a valid
	// candidate
	archiveRequest = static_cast<ArchiveRequest *> (msg->getData());

//	Debug(DEBUG_ALWAYS, (int32_t) archiveRequest, "archive request");
//	cout << "act " << act->getActivityId() << endl << archiveRequest->signalId.number;

	Signal *candidate
			= act->findCandidateInEitherList(archiveRequest->signalId);
	if (!candidate) {
		LogError(ERR_SNC, act->getActivityId(), "signal Id %d",
				archiveRequest->signalId.number);
		return;
	}

	// it must be a candidate, but it may not yet be confirmed, so we may
	// have to put it into the holding queue
	if (candidate->getState() != CONFIRMED) {
		// add the candidate to the discard list
		discardList.insert(pair<int32_t, ArchiveRequest>
				(archiveRequest->signalId.number, *archiveRequest));
		return;
	}

	// found the signal; now just release the buffer pair
	candidate->releaseBufPair();

	//Debug(DEBUG_ARCHIVE, (int32_t) act->getCombinedCount(AnyState),
	//		"getCandidateCount before discard");
	act->removeCandidateFromEitherList(candidate);

	//Debug(DEBUG_ARCHIVE, (int32_t) act->getCombinedCount(AnyState),
			//"getCandidateCount after discard");
	//Debug(DEBUG_ARCHIVE, (int32_t) allCandidatesReceived,
			//"allCandidatesReceived");

	// see whether we have archived all candidates
	if (!act->getCombinedCount(ANY_STATE) && allCandidatesReceived)
		sendArchiveComplete(msg);
}

//
// sendArchiveComplete: notify the control task that archiving has been
//		completed
//
void
ArchiveTask::sendArchiveComplete(Msg *msg)
{
	//Debug(DEBUG_ARCHIVE, msg->getActivityId(), "sending");
	Msg *cMsg = msgList->alloc((DxMessageCode) ArchiveComplete,
			msg->getActivityId());
	controlQ->send(cMsg);

	activity = 0;
	archiveList.clear();
	discardList.clear();
//	Debug(DEBUG_ARCHIVE, msg->getActivityId(), "sent");
}

//
// sendActivityStopped: notify the control task that the activity has
//		been stopped
//
void
ArchiveTask::sendActivityStopped(Msg *msg)
{
	Msg *cMsg = msgList->alloc((DxMessageCode) ActivityStopped,
			msg->getActivityId());
	cMsg->setUnit((sonata_lib::Unit) UnitArchive);
	controlQ->send(cMsg);
}

/**
 * Send archive data for a candidate.
 *
 * Description:\n
 * 	Sends archive data for the candidate to the archiver.  The data is
 * 	sent in packets of a half frame, with all the confirmation subchannels
 * 	in a packet.  Left is sent first, then right,
 */
void
ArchiveTask::sendArchive(Signal *signal)
{
	timeval start;
	gettimeofday(&start, 0);
	float64_t t = (float64_t) start.tv_sec + start.tv_usec / 1e6;

	// send the archive header and half frame count
	sendArchiveHeader(signal);
	sendArchiveData(signal);
	sendArchiveDone();
	timeval end;
	gettimeofday(&end, 0);
	t = (float64_t) end.tv_sec + end.tv_usec / 1e6;
	float64_t sec = end.tv_sec - start.tv_sec;
	float64_t usec;
	if ((usec = (end.tv_usec - start.tv_usec)) < 0) {
		usec += 1e6;
		--sec;
	}
}

/**
 * Send the archive header.
 *
 * Description:\n
 * 	Formats and sends the archive header for the archived candidate.  One
 * 	archive header is sent per candidate.
 */
void
ArchiveTask::sendArchiveHeader(Signal *signal)
{
	// send a message indicating that the signal is now being archived
	MemBlk *blk = partitionSet->alloc(sizeof(ArchiveDataHeader));
	Assert(blk);
	ArchiveDataHeader *archiveHdr = static_cast<ArchiveDataHeader *>
			(blk->getData());
	archiveHdr->signalId = signal->getSignalId();
	Msg *msg = msgList->alloc(ARCHIVE_SIGNAL, activity->getActivityId(),
			archiveHdr, sizeof(ArchiveDataHeader), blk);
	archiverOutQ->send(msg);
}

/**
 * Send all the archive data for the signal.
 *
 * Description:\n
 * 	Sends all the data (both polarizations) to the archiver.  Data is
 * 	sent one half frame x confirmation subchannels.
 */
void
ArchiveTask::sendArchiveData(Signal *signal)
{
	int32_t halfFrames = activity->getHalfFrames();

	// compute the starting subchannel
	SignalDescription sig = signal->getSignalDescription();
	int32_t subchannel = sig.subchannelNumber;
	int32_t subchannels = channel->getSubchannelsPerArchiveChannel();
	if (subchannel - subchannels / 2 < 0)
		subchannel = subchannels / 2;
	else if (subchannel + subchannels / 2 > channel->getUsableSubchannels())
		subchannel = channel->getUsableSubchannels() - subchannels / 2;
	int32_t startSubchannel = subchannel - subchannels / 2;

	// first, send the half frame count
	MemBlk *blk = partitionSet->alloc(sizeof(Count));
	Assert(blk);
	Count *count = static_cast<Count *> (blk->getData());
	count->count = halfFrames;
	Msg *msg = msgList->alloc(BEGIN_SENDING_ARCHIVE_COMPLEX_AMPLITUDES,
			activity->getActivityId(), count, sizeof(Count), blk);
	archiverOutQ->send(msg);

	// now send the data for each half frame, first left pol, then right pol
	for (int32_t i = 0; i < halfFrames; ++i)
		sendHalfFrame(POL_LEFTCIRCULAR, startSubchannel, i);
	for (int32_t i = 0; i < halfFrames; ++i)
		sendHalfFrame(POL_RIGHTCIRCULAR, startSubchannel, i);
}

//
// sendArchiveDone: send a message indicating that all the archive
//		data for this signal has been sent
//
void
ArchiveTask::sendArchiveDone()
{
	Msg *msg = msgList->alloc(DONE_SENDING_ARCHIVE_COMPLEX_AMPLITUDES,
			activity->getActivityId());
	archiverOutQ->send(msg);
}

/**
 * Send a half frame of data for a single polarization.
 *
 * Description:\n
 * 	Sends a message to the archiver containing a half frame of data for
 * 	a single polarization.  The data is organized so that each subchannel's
 * 	data (512 samples) are contiguous, i.e., subchannel 0 is followed by
 * 	subchannel 1 which is followed by subchannel 2, etc.  The number of
 * 	subchannels may be variable.\m\n
 *
 * Notes:\n
 * 	The subchannel is absolute, since data is extracted directly from the
 * 	CD buffer.
 *
 * @param		pol polarization.
 * @param		subchannel.
 * @param		hf half frame.
 */
void
ArchiveTask::sendHalfFrame(Polarization pol, int32_t startSubchannel,
		int32_t hf)
{
	int32_t subchannels = channel->getSubchannelsPerArchiveChannel();

	// compute the size of the data buffer required
	size_t hfSize = channel->getCdBytesPerSubchannelHalfFrame();
	size_t bufSize = sizeof(ComplexAmplitudeHeader)
			+ channel->getSubchannelsPerArchiveChannel() * hfSize;

	// allocate a buffer
	MemBlk *blk = partitionSet->alloc(bufSize);
	Assert(blk);
	ComplexAmplitudeHeader *hdr = static_cast<ComplexAmplitudeHeader *>
			(blk->getData());
	SubchannelCoef1KHz *data = reinterpret_cast<SubchannelCoef1KHz *> (hdr + 1);

	// build the header
	hdr->rfCenterFreq = channel->getSubchannelCenterFreq(startSubchannel);
	hdr->halfFrameNumber = hf;
	hdr->activityId = activity->getActivityId();
	hdr->hzPerSubchannel = MHZ_TO_HZ(channel->getSubchannelWidthMHz());
	hdr->startSubchannelId = startSubchannel;
	hdr->numberOfSubchannels = channel->getSubchannelsPerArchiveChannel();
	hdr->overSampling = channel->getSubchannelOversampling();
//	hdr->res = RES_1KHZ;
	hdr->pol = pol;

	// extract data from the CD buffer and put it in the message buffer, for
	// each subchannel being sent.
	for (int32_t i = 0; i < subchannels; ++i) {
		// now we need to extract the data from the buffer
		ComplexPair *cdData = static_cast<ComplexPair *>
				(channel->getCdData(pol,  startSubchannel + i, hf));
		SubchannelCoef1KHz *sData = data + i;
		memcpy(sData, cdData, hfSize);
	}

	// send the data to the archiver
	Msg *msg = msgList->alloc(SEND_ARCHIVE_COMPLEX_AMPLITUDES,
			activity->getActivityId(), hdr, bufSize, blk);
	archiverOutQ->send(msg);
}

#ifdef notdef
//
// extractHalfFrame: extract a half frame of data from the confirmation
//		data buffer and store it in a complex amplitudes buffer for
//		archiving
//
// Notes:
//		None.
//
void
ArchiveTask::extractSubchannelHalfFrame(int32_t halfFrame, int32_t subchannel,
		ComplexPair *cdData, SubchannelCoef1KHz *data)
{
	// compute the block number in which this half frame lies
	int32_t blk = halfFrame / bufLayout.halfFramesPerBlk;
	int32_t blkHalfFrame = halfFrame % bufLayout.halfFramesPerBlk;

	// compute the length of a block in the buffer
	int32_t blkLen = bufLayout.subchannels * bufLayout.subchannelBinsPerHalfFrame
			* bufLayout.halfFramesPerBlk;

	// compute the address of the data in the buffer
	int32_t ofs = blk * blkLen + (subchannel * bufLayout.halfFramesPerBlk
			+ blkHalfFrame) * bufLayout.subchannelBinsPerHalfFrame;
#ifdef notdef
	Debug(DEBUG_ARCHIVE, halfFrame, "halfFrame");
	Debug(DEBUG_ARCHIVE, subchannel, "subchannel");
	Debug(DEBUG_ARCHIVE, blk, "blk");
	Debug(DEBUG_ARCHIVE, blkHalfFrame, "blkHalfFrame");
	Debug(DEBUG_ARCHIVE, ofs, "ofs");
	Debug(DEBUG_ARCHIVE, bufLayout.subchannels, "subchannels");
	Debug(DEBUG_ARCHIVE, bufLayout.halfFramesPerBlk, "halfFramesPerBlk");
	Debug(DEBUG_ARCHIVE, blkLen, "blkLen");
	Debug(DEBUG_ARCHIVE, bufLayout.centerFreq, "centerFreq");
	Debug(DEBUG_ARCHIVE, bufLayout.subchannelBinsPerHalfFrame,
			"subchannelBinsPerHalfFrame");
#endif

	cdData += ofs;

	// copy the data from the CD buffer to the output buffer
	memcpy(data, cdData,
			(int32_t) (bufLayout.subchannelBinsPerHalfFrame * CD_BYTES_PER_BIN));
}
#endif

}