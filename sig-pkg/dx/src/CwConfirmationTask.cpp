/*******************************************************************************

 File:    CwConfirmationTask.cpp
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
// CWD confirmation class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CwConfirmationTask.cpp,v 1.8 2009/06/26 20:50:30 kes Exp $
//
#include <math.h>
#include <DxOpsBitset.h>
//#include "dedrift.h"
//#include "Buffer.h"
#include "ConfirmationTask.h"
#include "CwConfirmationTask.h"
#include "CwSignal.h"
#include "Statistics.h"
#include "SignalClassifier.h"
#include "Signal.h"
#include "SseOutputTask.h"

namespace dx {

CwConfirmationTask *CwConfirmationTask::instance = 0;

CwConfirmationTask *
CwConfirmationTask::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new CwConfirmationTask("CwConfirmationTask");
	l.unlock();
	return (instance);
}

CwConfirmationTask::CwConfirmationTask(string name_):
		QTask(name_, CWD_CONFIRMATION_PRIO), confirmationQ(0), respQ(0),
		resolution(RES_UNINIT), msgList(0), partitionSet(0), state(0),
		transform(0)
{
}

CwConfirmationTask::~CwConfirmationTask()
{
}

void
CwConfirmationTask::extractArgs()
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
	CwConfirmationArgs *cwArgs = static_cast<CwConfirmationArgs *> (args);
	Assert(args);
	confirmationQ = cwArgs->confirmationQ;
	Assert(confirmationQ);
	respQ = cwArgs->respQ;
	Assert(respQ);

	msgList = MsgList::getInstance();
	Assert(msgList);
	partitionSet = PartitionSet::getInstance();
	Assert(partitionSet);
	state = State::getInstance();
	Assert(state);
	transform = TransformWidth::getInstance();
	Assert(transform);
}

void
CwConfirmationTask::handleMsg(Msg *msg)
{
	// find the activity
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId())))
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
// handleSecondaryMsg: handle additional messages for the secondary detection
//
// Notes:
//		These messages are allowed only when the activity specifies the
//		DX will handle secondary candidates.
//		The DX must handle signal descriptions from a primary detector;
//		these signal descriptions specify the candidate signals to be
//		confirmed.
//
Error
CwConfirmationTask::handleSecondaryMsg(Msg *msg)
{
	// find the activity
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId())))
		return (ERR_NSA);

	// make sure we're processing secondary candidates
	if (!act->allowSecondaryMsg())
		return (ERR_IMT);

	Error err = 0;
	switch (msg->getCode()) {
	case SEND_CW_COHERENT_SIGNAL:
		if (act->getState() == DX_ACT_RUN_SD)
			err = doSecondaryConfirmation(msg, act);
		break;
	default:
		err = ERR_IMT;
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
CwConfirmationTask::startActivity(Msg *msg)
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
//		The control task is in charge of changing the
//		activity state.
//
void
CwConfirmationTask::stopActivity(Msg *msg)
{
	//Debug(DEBUG_CONFIRM, msg->getActivityId(), "Activity Stopped");

	Msg *cMsg = msgList->alloc((DxMessageCode) ActivityStopped,
			msg->getActivityId());
	cMsg->setUnit((sonata_lib::Unit) UnitCwConfirmation);
	confirmationQ->send(cMsg);
}

/**
 * Test coherence of a primary signal (one detected by this detector).
 *
 * Description:\n
 * 	Determines the coherence width of the candidate CW power signal.  This
 * 	is done by\n
 * 		(1) Retrieving the CD data for a set of subchannels centered on
 * 			the signal.\n
 * 		(2) Synthesizing a wide archive channel from those subchannels.  This
 * 			is done by selecting a sample from each of the subchannels,
 * 			converting them to floating point and arranging them in FFT
 * 			frequency order, and performing an inverse FFT to recreate
 * 			the original time samples.\n
 * 		(3) Extracting a narrow signal channel around the signal (typically
 * 			~20Hz wide by dedrifting and heterodyning the archive channel,
 * 			then summing blocks of the result to extract a narrower DC
 * 			channel, which contains the signal.\n
 * 		(4) The signal channel is then transformed to create a set of ~32
 * 			1Hz bins.
 */
//
// doPrimaryConfirmation: perform primary confirmation processing on the
//		signal
//
// Description:
//		Confirmation consists of
//		(1) retrieval of the appropriate CD data from disk,
//			consisting of 16 subchannels surrounding the signal
//		(2) rearranging the subchannel data so that the 16 subchannels
//			are contiguous, at the same time converting from 4-bit
//			integer complex to floating point complex
//		(3) performing an inverse transform to combine the 16
//			subchannels into a single wider channel
//		(4) performing the equivalent of a FUDD detection on the
//			resulting channel
// Notes:
//		A pointer to the candidate signal is passed in the
//		message.
//
void
CwConfirmationTask::doPrimaryConfirmation(Msg *msg)
{
	Debug(DEBUG_CONFIRM, msg->getActivityId(), "confirm candidate");

	// activity must exist and be in correct state
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId())))
		return;

	// if we're not running signal detection, don't process the
	// candidate
	if (act->getState() != DX_ACT_RUN_SD)
		return;

	Signal *cwCand = static_cast<Signal *> (msg->getData());

	LogInfo(ERR_NE, act->getActivityId(), "sig rf %lf, d %f, p %f",
			cwCand->getRfFreq(), cwCand->getDrift(), cwCand->getPower());

	// extract the signal information
	sig = cwCand->getSignalDescription();
	LogInfo(ERR_NE, act->getActivityId(),
			"sig rf %lf, s %d, d %f, p %f",
			sig.path.rfFreq, channel->getSubchannel(sig.path.rfFreq),
			sig.path.drift, sig.path.power);

#ifdef notdef
	// if no retrieval buffer has been allocated, do it now.  We have to
	// block waiting for the buffer pair, because archiving may still
	// be in progress
	if (!cwCand->getBufPair()) {
		BufPair *bufPair = state->allocArchiveBuf(true);
		Assert(bufPair);
		cwCand->setBufPair(bufPair);
	}
#endif
	Debug(DEBUG_CWD_CONFIRM, msg->getActivityId(), "create signal channel");

	// create the signal channel by extracting a channel around the
	// signal, then dedrifting and heterodyning the nominal
	// signal to baseband in a narrow channel
	createArchiveChannel(msg, cwCand, sig, act);

	// do the power detection to find the best path
	Debug(DEBUG_CWD_CONFIRM, (void *) act, "do power detection");
	doPowerDetection(right, act);
	doPowerDetection(left, act);
//	cout << "right " << right.bestPath;
//	cout << "left " << left.bestPath;

	// compute signal descriptions for both pols, then select the
	// stronger of the two paths as the one to use for coherent
	// detection for both polarizations
	computeData(right, cwCand, act);
	computeData(left, cwCand, act);
//	cout << "right power " << left.cfmSig.path;
//	cout << "left power " << right.cfmSig.path;

	CwData& stronger = selectStrongerPath();

	right.bestPath = stronger.bestPath;
	left.bestPath = stronger.bestPath;

	// finally, do the coherent detection for both pols
	doCoherentDetection(right, act);
	doCoherentDetection(left, act);
//	cout << "right coherent " << left.cfmSig.path;
//	cout << "left coherent " << right.cfmSig.path;

	// classify the signal based on the results of the
	// detection
	CwData summary;
	classifySignal(PRIMARY, right, left, summary, act);

	// send the report
	sendPrimaryReport(summary, act);

	cwCand->setState(CONFIRMED);

	// release the buffer pair; a new set of buffers will be
	// allocated by the archiver if necessary
	cwCand->releaseBufPair();

	// notify the confirmation task that we have finished this one
	MemBlk *blk = partitionSet->alloc(sizeof(Signal *));
	Assert(blk);
	Signal *signal = static_cast<Signal *> (blk->getData());
	signal = cwCand;
	SignalId signalId = signal->getSignalId();
	Debug(DEBUG_CONFIRM, signalId.number, "signalId.number");
	Msg *cMsg = msgList->alloc((DxMessageCode) CwConfirmationComplete,
			act->getActivityId(), signal, sizeof(Signal *), blk);
	confirmationQ->send(cMsg);
}

//
// doSecondaryConfirmation: perform coherent confirmation at the secondary
//		detector
//
// Notes:
//		The signal must be in our candidate list.
//		If the signal was not confirmed at the primary, then we just send
//		the signal description back without modification
//
Error
CwConfirmationTask::doSecondaryConfirmation(Msg *msg, Activity *act)
{
	Debug(DEBUG_CONFIRM, msg->getActivityId(), "confirm secondary candidate");

	// if we're not running signal detection, don't process the
	// candidate
	if (act->getState() != DX_ACT_RUN_SD)
		return (0);

	CwCoherentSignal *cohSig = static_cast<CwCoherentSignal *> (msg->getData());
	SignalId id = cohSig->sig.origSignalId;

	// the signal must be in the candidate list
	Signal *cwCand = act->findCandidateUsingOrigId(id);
	if (!cwCand) {
		LogError(ERR_SNC, msg->getActivityId(), "activity %d, number %d",
				id.activityId, id.number);
		return (ERR_SNC);
	}
	cohSig->sig.signalId = cwCand->getSignalId();

	// extract the signal information
	sig = cohSig->sig;
	LogInfo(ERR_NE, act->getActivityId(),
			"sig rf %lf, s %d, d %f, p %f",
			sig.path.rfFreq, channel->getSubchannel(sig.path.rfFreq),
			sig.path.drift, sig.path.power);

#ifdef notdef
	// create the signal channel by extracting a channel around the signal
	CwSignal *cwSig = cwCand->getCw();
	Assert(cwSig);
	CwPowerSignal pwrSig = cwSig->getSignal();

	float64_t deltaF = cohSig->sig.path.rfFreq - pwrSig.sig.path.rfFreq;
	float64_t deltaD = cohSig->sig.path.drift - pwrSig.sig.path.drift;
#endif

	left.cfmSig = cohSig->sig;
	right.cfmSig = cohSig->sig;
	left.cfm = cohSig->cfm;
	right.cfm = cohSig->cfm;

	// create the archive channel
	createArchiveChannel(msg, cwCand, cohSig->sig, act);

	if (cwCand && cohSig->sig.sigClass == CLASS_CAND) {
		left.bestPath.bin = 0;
		left.bestPath.drift = 0;
		left.bestPath.power = 0;
		right.bestPath.bin = 0;
		right.bestPath.drift = 0;
		right.bestPath.power = 0;

		doCoherentDetection(left, act);
		doCoherentDetection(right, act);

	}

	// classify the signal based on the results of the
	// detection
	CwData summary;
	classifySignal(SECONDARY, left, right, summary, act);
	summary.cfmSig.containsBadBands = SignalClassifier::applyBadBands(act, summary.cfmSig);

	// send the report to the SSE
	sendSecondaryReport(summary, act);

	// release the buffer pair; a new set of buffers will be
	// allocated by the archiver if necessary
	cwCand->releaseBufPair();

	// send the original message back to the confirmation task
	Msg *cMsg = msgList->alloc();
	msg->forward(cMsg);
	cMsg->setCode(SEND_CW_COHERENT_CANDIDATE_RESULT);
	Debug(DEBUG_CONFIRM, (int32_t) cohSig->sig.sigClass, "sigClass");
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
CwConfirmationTask::createArchiveChannel(Msg *msg, Signal *cand,
		const SignalDescription& sig, Activity *act)
{
	Debug(DEBUG_CWD_CONFIRM, (void *) cand, "candidate");

	const DxActivityParameters& params = act->getActivityParams();

	// get the signal resolution; this is used in determining
	// the width of the signal channel
	resolution = params.daddResolution;

	// get the subchannel containing the signal; if it is near the band
	// edge, adjust it
	int32_t subchannel = sig.subchannelNumber;
	int32_t nSubchan = channel->getSubchannelsPerArchiveChannel();
	if (subchannel - nSubchan / 2 < 0)
		subchannel = nSubchan / 2;
	else if (subchannel + nSubchan / 2 >= channel->getUsableSubchannels())
		subchannel = channel->getUsableSubchannels() - nSubchan / 2;

	ArchiveChannel *rCw = right.ac;
	ArchiveChannel *lCw = left.ac;

	// initialize the archive channels
	rCw->setup(channel->getSubchannelsPerArchiveChannel(),
			channel->getHalfFrames(),
			channel->getSamplesPerSubchannelHalfFrame(),
			channel->getCdStridePerSubchannel(),
			channel->getSubchannelOversampling(),
			channel->getSubchannelCenterFreq(subchannel),
			channel->getSubchannelRateMHz());

	lCw->setup(channel->getSubchannelsPerArchiveChannel(),
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
	right.basePower = rCw->create(rCd);
	left.basePower = lCw->create(lCd);
}

/**
 * Create the confirmation signal channel.
 *
 * Description:\n
 * 	Creates an archive channel consisting of a number of subchannels (typically
 * 	16) subchannels surrounding the nominal signal description.  This is done
 * 	by performing an inverse FFT (IFFT) using the subchannel samples.
 */
//
// createSignalChannel: create the confirmation signal channel
//
// Description:
//		Retrieves the set of 16 subchannels surrounding the nominal
//		signal description, performs an inverse transform to create
//		a 16Kbin channel containing the signal, then dedrifts and
//		heterodynes the channel to place the nominal signal at
//		baseband in a narrower channel.
// Notes:
//		The confirmation signal channel for 1 Hz resolution is
//		~21 Hz wide.
//		The process is the same for both primary and secondary detections.
//		This function does both polarizations.
//
void
CwConfirmationTask::createSignalChannel(Msg *msg, Activity *act,
		Signal *cand, SignalDescription *sig)
{
#ifdef notdef
	Debug(DEBUG_CWD_CONFIRM, (void *) cand, "candidate");

	const DxActivityParameters& params = act->getActivityParams();

	// get the signal resolution; this is used in determining
	// the width of the signal channel
	resolution = params.daddResolution;

	// initialize the archive channel
	cwChannel.setup(channel->getSubchannelsPerArchiveChannel(),
			channel->getHalfFrames(),
			channel->getSamplesPerSubchannelHalfFrame(),
			channel->getSamplesPerSubchannelHalfFrame(),
			channel->getSubchannelOversampling(),
			channel->getSubchannelCenterFreq(sig->subchannelNumber),
			channel->getSubchannelWidthMHz());

	// get the buffer for storing the subchannels to be combined into
	// the archive channel
	Buffer *subchannelBuf = state->allocSubchannelBuf(true);
	// get the address of the raw confirmation data buffers
	lRetrieval.buffer = cand->getLBuf();
	rRetrieval.buffer = cand->getRBuf();

	left.cdData = static_cast<ComplexPair *> (lRetrieval.buffer->getBuf());
	right.cdData = static_cast<ComplexPair *> (rRetrieval.buffer->getBuf());

	// get the layout for each polarization
	bufLayout.computeXferSpec(lRetrieval.xfer);
	bufLayout.computeXferSpec(rRetrieval.xfer);

//	cout << bufLayout;

	// set the base address of the buffers
	lRetrieval.xfer.dest.base = reinterpret_cast<uint64_t> (left.cdData);
	rRetrieval.xfer.dest.base = reinterpret_cast<uint64_t> (right.cdData);

	// get the files for this activity
	lRetrieval.file = act->getFile(LCdFile);
	rRetrieval.file = act->getFile(RCdFile);

	Debug(DEBUG_CWD_CONFIRM, (void *) channel, "retrieve data");

	// retrieve the data from disk
	channel->retrieveCDData(&lRetrieval);
	channel->retrieveCDData(&rRetrieval);

#ifdef notdef
	sprintf(str, "L%.6lf", sig->rfFreq);
	sigName = str;
	channel->dumpSubchannelData(lRetrieval.buffer, &sigName);
#endif

#ifdef notdef
	sprintf(str, "R%.6lf", sig->rfFreq);
	sigName = str;
	channel->dumpSubchannelData(rRetrieval.buffer, &sigName);
#endif

	// assemble the channel spectra from which the wider channel will
	// be synthesized
	Debug(DEBUG_CWD_CONFIRM, (void *) channel, "create left sigchan");

	left.basePower = 0;
	channel->createSignalChannel(left.cdData, left.tdData, resolution,
			&left.basePower);

	Debug(DEBUG_CWD_CONFIRM, (void *) channel, "create right sigchan");

	right.basePower = 0;
	channel->createSignalChannel(right.cdData, right.tdData, resolution,
			&right.basePower);

#ifdef notdef
	sprintf(str, "LS%.6lf", sig->rfFreq);
	sigName = str;
	channel->dumpSignalChannel(lSigTDBuf, &sigName);
#endif
#endif
}

//
// doPowerDetection: perform a power detection
//
// Notes:
//		This method is called only by a primary detector.
//
void
CwConfirmationTask::doPowerDetection(CwData& polData, Activity *act)
{
	ArchiveChannel *ac = polData.ac;
	int32_t bins = transform->getSigSamplesPerSpectrum(resolution);
	float64_t widthHz = act->getBinWidthHz(resolution) * bins;
	int32_t samples = ac->getSignalSamples(widthHz);
	if (samples > power.samples) {
		if (power.td)
			fftwf_free(power.td);
		size_t size = samples * sizeof(ComplexFloat32);
		power.td = static_cast<ComplexFloat32 *> (fftwf_malloc(size));
		Assert(power.td);
		power.samples = samples;
	}
	ac->extractSignalChannel(power.td, sig.path.rfFreq,
			sig.path.drift, widthHz, samples);
	int32_t spectra = ac->getSignalSpectra(bins, samples, true);
	int32_t totalBins = bins * spectra;
	if (totalBins > power.totalBins) {
		if (power.fd)
			fftwf_free(power.fd);
		size_t size = totalBins * sizeof(ComplexFloat32);
		power.fd = static_cast<ComplexFloat32 *> (fftwf_malloc(size));
		Assert(power.fd);
		if (power.pd)
			fftwf_free(power.pd);
		size = totalBins * sizeof(float32_t);
		power.pd = static_cast<float32_t *> (fftwf_malloc(size));
		Assert(power.pd);
		power.totalBins = totalBins;
	}
	power.bins = bins;
	power.spectra = spectra;
	ac->createSignalSpectra(power.td, power.fd, power.bins,
			power.samples, true);
	createPowerSpectra(polData);

	// find the strongest power path
	doPowerSearch(polData);
}

/**
 * Create power spectra from signal channel.
 */
void
CwConfirmationTask::createPowerSpectra(CwData& polData)
{
	float32_t basePower = polData.basePower / power.spectra;
	for (int32_t i = 0; i < power.spectra; ++i) {
		ComplexFloat32 *fd = &power.fd[i*power.bins];
		float32_t *pd = &power.pd[i*power.bins];
		for (int32_t j = 0; j < power.bins; ++j, ++fd, ++pd)
//			*sigPower = std::norm(*power.fd) / basePower;
			*pd = std::norm(*fd);
	}
}
//
// doPowerSearch: search for the strongest power path
//
// Synopsis:
//		void doPowerSearch(powerData, spectra, binsPerSpectrum, bin, drift);
//		float32_t *powerData;		array of bin powers
//		int32_t spectra;			# of spectra
//		int32_t binsPerSpectrum;	# of bins per spectrum
//		powerPath& bestPath;		# best path
// Description:
//		Find the strongest path in the array.
// Notes:
//		powerData is a 2D array (spectra rows and bins columns)
//		containing power values for each bin in the signal
//		channel.
//
void
CwConfirmationTask::doPowerSearch(CwData& polData)
{
	PowerPath temp;
	for (int32_t bin = 0; bin < power.bins; ++bin) {
		int32_t startDrift = -_MIN(bin, power.spectra);
		int32_t endDrift = _MIN(power.bins - bin, power.spectra);

		for (int32_t drift = startDrift; drift <= endDrift; ++drift) {
			int32_t sign = drift < 0 ? -1 : 1;
			float32_t sum = computePathSum(power.spectra,
					power.bins, drift, sign, &power.pd[bin]);
			if (sum > temp.power) {
				temp.power = sum;
				temp.bin = bin;
				temp.drift = drift;
			}
		}
	}
	polData.bestPath = temp;
	polData.bestPath.bin -= power.bins / 2;
}

/**
 * Do coherent CW detection.
 *
 * Description:\n
 * 	Dedrifts the coherent channel (~2Hz)
 */
void
CwConfirmationTask::doCoherentDetection(CwData& polData, Activity *act)
{
	ArchiveChannel *ac = polData.ac;
	float64_t widthHz = act->getBinWidthHz(resolution) * 2;
	int32_t samples = ac->getSignalSamples(widthHz);
	Assert(samples > 0);
	int32_t l = log2(samples);
	samples = 1 << l;
	if (samples > coherent.samples) {
		if (coherent.td)
			fftwf_free(coherent.td);
		size_t size = samples * sizeof(ComplexFloat32);
		coherent.td = static_cast<ComplexFloat32 *> (fftwf_malloc(size));
		Assert(coherent.td);
		if (coherent.dd)
			fftwf_free(coherent.dd);
		coherent.dd = static_cast<ComplexFloat32 *> (fftwf_malloc(size));
		Assert(coherent.dd);
		if (coherent.fd)
			fftwf_free(coherent.fd);
		coherent.fd = static_cast<ComplexFloat32 *> (fftwf_malloc(size));
		Assert(coherent.fd);
		if (samples != coherent.samples) {
			if (coherent.plan)
				fftwf_destroy_plan(coherent.plan);
			coherent.plan = fftwf_plan_dft_1d(samples,
					(fftwf_complex *) coherent.dd,
					(fftwf_complex *) coherent.fd, FFTW_FORWARD, FFTW_ESTIMATE);
			Assert(coherent.plan);
		}
		coherent.samples = samples;
	}
	ac->extractSignalChannel(coherent.td, polData.cfmSig.path.rfFreq,
			polData.cfmSig.path.drift, widthHz, samples);

	// begin coherent detection using the results of the power detection
//	int32_t coherentSpectra = 1;
//	int32_t coherentSamples = samples;
//	Debug(DEBUG_NEVER, coherentSpectra, "coherentSpectra");
//	Debug(DEBUG_NEVER, coherentSamples, "coherentBins");

	// compute the width of a coherent bin
	coherent.binWidthHz = widthHz / samples;
	Debug(DEBUG_NEVER, coherent.binWidthHz, "coherent.binWidthHz");

	#ifdef notdef
	// adjust bestPath bin and drift for wider bins
	polData.bestPath.bin /= WIDTH_FACTOR;
	polData.bestPath.drift /= WIDTH_FACTOR;

	// the length of the fft is the same as the number of coherent time samples,
	// which is the same as the number of coherent spectra, since the dedrift
	// process produces one time sample per spectrum.
	int32_t coherentSamples = coherentSpectra;


	ComplexFloat32 coherentTDData[coherentSpectra*coherentBins];
	extractCoherentSignal(polData.tdData, coherentTDData,
			coherentSpectra, coherentBins, polData.bestPath);
#endif

	polData.coherentReport = doCoherentSearch(polData, samples);
	computeSignalData(polData.cfmSig, polData.coherentReport);

	polData.cfm.pfa = polData.coherentReport.chi.chiSq;

	// compute the apparent SNR of the signal in a 1 Hz channel
	// the expected power in a 1Hz channel, assuming the nominal
	// bin power is 1.0, will be (1 / bin width).  The total
	// power in the best coherent signal must be normalized by
	// the total number of samples which were added to get the
	// power (i.e., the length of the fft).  The snr is then
	// this normalized power minus the expected power in the
	// remainder of a 1 Hz bin
	float32_t binWidth = act->getBinWidthHz(resolution);
	float32_t sigPower = polData.coherentReport.chi.power / samples;
	float32_t binPower = sigPower + (binWidth -
			polData.cfmSig.path.width) / binWidth;
	float32_t nominalBinPower1Hz = 1.0 / binWidth;
	float32_t binPower1Hz = binPower + (1 - binWidth) / binWidth -
			nominalBinPower1Hz;

	polData.cfm.snr = binPower1Hz / nominalBinPower1Hz;
}

/**
 * Perform a coherent search for the best description of the signal.
 */
CoherentReport
CwConfirmationTask::doCoherentSearch(CwData& polData, int32_t samples)
{

	int32_t middle = samples / 2;

	int32_t len = samples * sizeof(ComplexFloat32);
	int32_t hLen = len / 2;

	int32_t fSearch = samples / 2;
	int32_t dSearch = samples - 1;
	int32_t wSearch = samples;

	int32_t minDrift = -dSearch;
	int32_t maxDrift = dSearch;

#ifdef notdef
	int32_t sample;
	float64_t totalPower;

	for (sample = 0, totalPower = 0; sample < samples; sample++)
		totalPower += norm(coherentTDData[sample]);
	Debug(DEBUG_CWD_CONFIRM, totalPower, "doCoS, total TD power =");
#endif

	ComplexFloat32 temp[samples/2];
	CoherentReport bestCoherence;
	for (int32_t drift = minDrift; drift <= maxDrift; drift++) {
		memcpy(coherent.dd, coherent.td, len);
		dedriftFTPlane(coherent.dd, 1, samples, 0, 0,
				(float64_t) drift / (samples * samples));
		fftwf_execute_dft(coherent.plan, (fftwf_complex *) coherent.dd,
				(fftwf_complex *) coherent.fd);
		polData.ac->rescale(coherent.fd, coherent.samples, coherent.samples);

		// rearrange the data so DC is in the middle
		memcpy(temp, &coherent.fd[0], hLen);
		memcpy(&coherent.fd[0], &coherent.fd[middle], hLen);
		memcpy(&coherent.fd[middle], temp, hLen);

		ChiReport chi = checkCoherence(coherent.fd, 1.0, samples, drift,
				fSearch, wSearch);
		if (chi.chiSq < bestCoherence.chi.chiSq) {
			bestCoherence.chi = chi;
			bestCoherence.microDrift = drift;
		}
	}

	bestCoherence.chi.chiSq = ChiSquare(2 * bestCoherence.chi.clusterWidth,
			2 * bestCoherence.chi.power);
	bestCoherence.signif = bestCoherence.chi.chiSq;
	return (bestCoherence);
}

//
// extractCoherentSignal: dedrift the time-domain signal channel for the
//		best power signal and extract the middle two bins
//
void
CwConfirmationTask::extractCoherentSignal(ComplexFloat32 *sigTDData,
		ComplexFloat32 *coherentTDData, int32_t spectra, int32_t bins,
		PowerPath bestPath)
{
	dedrift(sigTDData, coherentTDData, spectra, bins, bestPath);
}

float32_t
CwConfirmationTask::computePathSum(int32_t spectra, int32_t binsPerSpectrum,
		int32_t drift, int32_t sign, float32_t *pathBase)
{
	float32_t sum = 0;
	for (int32_t spectrum = 0; spectrum < spectra; spectrum++) {
		int32_t x = computeTruePath(spectrum, spectra, abs(drift));
		sum += pathBase[spectrum*binsPerSpectrum+sign*x];
	}
	return (sum);
}

int32_t
CwConfirmationTask::computeTruePath(float32_t spectrum, float32_t spectra,
		float32_t drift)
{
	int32_t x = (int32_t) ((drift / spectra) * (spectrum + 0.5) + 0.5);
	return (x);
}

void
CwConfirmationTask::dedrift(ComplexFloat32 *sigTDData,
		ComplexFloat32 *coherentTDData, int32_t spectra, int32_t bins,
		PowerPath path)
{
#ifdef notdef
	float64_t shift = (float64_t) path.bin / bins;
	float64_t drift = (float64_t) path.drift / ((float64_t) bins * bins
			* spectra);

#ifdef notdef
	if (shift == 0 && drift == 0) {
		for (bin = 0; bin < spectra * bins; bin++)
			Debug(DEBUG_CWD_CONFIRM, &sigTDData[bin],
					"dedrift sigTD sample =");
	}
#endif

	// dedrift the signal channel for the best power path
	memcpy(td, sigTDData, spectra * bins * sizeof(ComplexFloat32));
	ComplexFloat32 td[spectra*bins];
	dedriftFTPlane(td, spectra, bins, 0, shift, drift);

#ifdef notdef
	if (shift == 0 && drift == 0) {
		for (bin = 0; bin < spectra * bins; bin++)
			Debug(DEBUG_CWD_CONFIRM, &td[bin], "dedrift td sample =");
	}
#endif

	for (int32_t spectrum = 0; 	spectrum < spectra; spectrum++) {
		ComplexFloat32 data(0, 0);
		int32_t delta = spectrum * bins;
		for (int32_t bin = 0; bin < bins; bin++)
			data += td[delta+bin];
		coherentTDData[spectrum] = data;
	}
#endif
}

//
// checkCoherence: find the best coherence width for this drift
//
// Notes:
//		coherenceCheck examines a complex spectrum, under the assumption
//		that the spectrum contains a coherent signal.    It estimates
//		the bandwidth of the signal. Power is summed in the bins
//		taken one at a time, then two at a time, then four at a time,
//		etc., looking for the (width, power) which is least likely
//		to have been produced by noise; i.e. the width and power
//		which are furthest in the tail of the appropriate Chi-squared
//		distribution.
//		The code here assumes binsPerSpectrum is a power of 2.
ChiReport
CwConfirmationTask::checkCoherence(ComplexFloat32 *data,
		float64_t avgPower, int32_t bins, int32_t drift, int32_t fSearch,
		int32_t wSearch)
{
	// normalize the power to average for the channel
	float64_t avgBinPower = 0;
	float64_t binPower[bins];
	for (int32_t bin = 0; bin < bins; bin++) {
		binPower[bin] = norm(data[bin]) / avgPower;
		avgBinPower += binPower[bin];
	}
	avgBinPower /= bins;

	// renormalize the power for the coherent band
	for (int32_t bin = 0; bin < bins; bin++)
		binPower[bin] /= avgBinPower;

	ChiReport best;

	// restrict the search to valid frequencies and widths for
	// the current drift.
	int32_t middle = bins / 2;
	int32_t minWidth = 1;
	int32_t maxWidth = wSearch;
	for (int32_t width = minWidth; width <= maxWidth; width *= 2) {
		int32_t halfWidth = width / 2;
		int32_t fsrch = fSearch;

		// maximum search range is from beginning to end, with
		// middle as reference
		if (fsrch > bins / 2)
			fsrch = bins / 2;

		// if the frequency search range is less than half the
		// current width, don't search this width;
		if (fsrch < halfWidth)
			break;

		// the actual frequency range can be restricted by the
		// drift and width.  For negative drifts, some lower
		// frequencies will be restricted, while for positive
		// drifts upper frequencies will be restricted.
		int32_t minFreq = middle - fsrch;
		int32_t maxFreq = middle + fsrch;

		if (drift < 0) {
			if (minFreq + drift < 0)
				minFreq = -drift;
		}
		else if (maxFreq > bins - drift)
			maxFreq = bins - drift;

		int32_t delta = halfWidth;
		if (!delta)
			delta = 1;

		for (int32_t i = minFreq; i + width <= maxFreq; i += delta) {
			float64_t power = 0;
			for (int32_t j = 0; j < width; j++)
				power += binPower[i+j];
			float64_t chiSq = ChiSquare(2 * width, 2 * power);
			if (chiSq < best.chiSq) {
				best.power = power;
				best.chiSq = chiSq;
				best.clusterWidth = width;
				best.clusterIndex = i;
			}
		}
	}

	best.power *= avgBinPower;
	best.clusterIndex = (best.clusterIndex + best.clusterWidth / 2) - middle;
	best.bins = bins;
	return (best);
}

void
CwConfirmationTask::dedriftFTPlane(ComplexFloat32 *ftp, int32_t spectra,
		int32_t binsPerSpectrum, int32_t firstSample, float64_t shift,
		float64_t drift)
{
	shift *= 2 * M_PI;
	drift *= M_PI;

	int32_t samples = spectra * binsPerSpectrum;

#ifdef notdef
	if (shift == 0 && drift == 0) {
		for (sample = 0; sample < samples; sample++)
			Debug(DEBUG_CWD_CONFIRM, &ftp[sample], "dFTP before sample =");
	}
#endif

	float64_t temp1 = 2 * drift;
	ComplexFloat32 ei2b = ComplexFloat32(cos(temp1), -sin(temp1));

	float64_t temp2 = (2 * firstSample + 1) * drift + shift;
	ComplexFloat32 ei2tbp1pa = ComplexFloat32(cos(temp2), -sin(temp2));

	float64_t temp3 = firstSample * (firstSample * drift + shift);
	ComplexFloat32 eitsq = ComplexFloat32(cos(temp3), -sin(temp3));

	for (int32_t sample = 0; sample < samples; sample++) {
		ftp[sample] *= eitsq;
		eitsq *= ei2tbp1pa;
		ei2tbp1pa *= ei2b;
	}

#ifdef notdef
	if (shift == 0 && drift == 0) {
		for (sample = 0; sample < samples; sample++)
			Debug(DEBUG_CWD_CONFIRM, &ftp[sample], "dFTP after sample =");
	}
#endif
}

//
// Notes:
//		The signal description already contains the frequency and
//		drift as found by the power detection, so now we must just
//		apply the microbin and microdrift offsets.
//
void
CwConfirmationTask::computeSignalData(SignalDescription& cfmSig,
		CoherentReport& bestCoherence)
{
	float64_t microBinWidth = coherent.binWidthHz;
	float64_t microDrift = microBinWidth / coherent.samples;

	float64_t fineDeltaF = bestCoherence.chi.clusterIndex * microBinWidth;
	float64_t fineDeltaD = bestCoherence.microDrift * microDrift;

	cfmSig.path.rfFreq += HZ_TO_MHZ((fineDeltaF));
	cfmSig.path.drift += fineDeltaD;
	cfmSig.path.width = bestCoherence.chi.clusterWidth * microBinWidth;
	cfmSig.path.power = bestCoherence.chi.power;
}


//
// computeData: compute an improved description of the signal based
//		on the results of the power detection
//
void
CwConfirmationTask::computeData(CwData& data, Signal *cwCand,
		Activity *act)
{
	float64_t deltaF = data.bestPath.bin * act->getBinWidthMHz(resolution);
	float64_t deltaD = (data.bestPath.drift * act->getBinWidthHz(resolution))
			/ act->getDataCollectionTime();
	deltaD /= 2;

	data.cfmSig = cwCand->getSignalDescription();
	data.cfmSig.path.rfFreq += deltaF;
	data.cfmSig.path.drift += deltaD;
	data.cfmSig.path.power = data.bestPath.power;
}

//
// computeStrongerPath: compute the stronger of the best power paths
//		for the left and right polarizations
//
CwData&
CwConfirmationTask::selectStrongerPath()
{
	if (left.bestPath.power >= right.bestPath.power)
		return (left);
	else
		return (right);
}

//
// classifySignal: classify the signal based on the results of the
//		confirmation
//
void
CwConfirmationTask::classifySignal(SystemType origin, CwData &right,
		CwData& left, CwData& summary, Activity *act)
{
	SignalClassReason candReason = PASSED_COHERENT_DETECT;
	SignalClassReason rfiReason = FAILED_COHERENT_DETECT;

	// classify the signal based on its unlikelyhood
	const DxActivityParameters& params = act->getActivityParams();

	// base signal is the stronger of the two
	CwData *weaker;
	if (left.cfm.pfa <= right.cfm.pfa) {
		summary = left;
		weaker = &right;
	}
	else {
		summary = right;
		weaker = &left;
	}

	float64_t threshToUse;
	if ( origin == SECONDARY){
		threshToUse = ((primaryCoherentSignal.cfm.pfa +
				params.secondaryPfaMargin) <
				params.secondaryCwCoherentThreshold ) ?
				params.secondaryCwCoherentThreshold :
				primaryCoherentSignal.cfm.pfa + params.secondaryPfaMargin;
	} else
	   threshToUse = params.cwCoherentThreshold;

	// if the stronger signal is over threshold, we have a
	// confirmed candidate
	if (summary.cfmSig.sigClass == CLASS_CAND) {
		if (summary.cfm.pfa < threshToUse)
			summary.cfmSig.reason = candReason;
		else {
			summary.cfmSig.sigClass = CLASS_RFI;
			summary.cfmSig.reason = rfiReason;
		}
	}

	// if the weaker signal is over	threshold, we have a dual-pol signal
//	cout << "left " << left.cfm << endl;
//	cout << "right " << right.cfm << endl;
//	cout << "stronger " << summary.cfm << endl;
//	cout << "weaker " << weaker.cfm << endl;

	if (weaker->cfm.pfa < threshToUse)
		summary.cfmSig.pol = POL_BOTH;

	// test for a zero-drift signal if we're filtering for that
	DxOpsBitset operations = params.operations;

	 // The secondary confirmation doesn't care if the drift is zero
        // as long as the signals was seen

	if (origin == PRIMARY ){
	   if (operations.test(REJECT_ZERO_DRIFT_SIGNALS)) {
		float32_t absDrift = fabs(summary.cfmSig.path.drift);
		if (absDrift <= params.zeroDriftTolerance) {
			summary.cfmSig.sigClass = CLASS_RFI;
			summary.cfmSig.reason = ZERO_DRIFT;
		}
		float32_t maxDrift = params.maxDriftRateTolerance
				* MHZ_TO_GHZ(params.dxSkyFreq);
		if (absDrift > maxDrift) {
                        summary.cfmSig.sigClass = CLASS_RFI;
                        summary.cfmSig.reason = DRIFT_TOO_HIGH;
                }

          }
	}

	// Debug(DEBUG_ALWAYS, origin, "origin");
	// Debug(DEBUG_ALWAYS, summary.cfmSig.sigClass, "summary.cfmSig.sigClass");
	// Debug(DEBUG_ALWAYS, summary.cfmSig.reason, "summary.cfmSig.reason");
}

void
CwConfirmationTask::sendPrimaryReport(CwData& summary, Activity *act)
{
	// send the candidate results to the SSE
	MemBlk *blk = partitionSet->alloc(sizeof(CwCoherentSignal));
	Assert(blk);
	CwCoherentSignal *cohSig =
			static_cast<CwCoherentSignal *> (blk->getData());
	cohSig->sig = summary.cfmSig;
	cohSig->cfm = summary.cfm;
	cohSig->nSegments = 1;
	cohSig->segment[0].path.rfFreq = cohSig->sig.path.rfFreq;
	cohSig->segment[0].path.drift = cohSig->sig.path.drift;
	cohSig->segment[0].path.width = cohSig->sig.path.width;
	cohSig->segment[0].path.power = cohSig->sig.path.power;
	cohSig->segment[0].pfa = cohSig->cfm.pfa;
	cohSig->segment[0].snr = cohSig->cfm.snr;

	Msg *rMsg = msgList->alloc(SEND_CW_COHERENT_SIGNAL, act->getActivityId(),
			cohSig, sizeof(::CwCoherentSignal), blk);
	respQ->send(rMsg);
}

void
CwConfirmationTask::sendSecondaryReport(CwData& summary, Activity *act)
{
	// send the candidate results to the SSE
	MemBlk *blk = partitionSet->alloc(sizeof(CwCoherentSignal));
	Assert(blk);
	CwCoherentSignal *cohSig =
			static_cast<CwCoherentSignal *> (blk->getData());
	cohSig->sig = summary.cfmSig;
	cohSig->cfm = summary.cfm;
	cohSig->nSegments = 1;
	cohSig->segment[0].path.rfFreq = cohSig->sig.path.rfFreq;
	cohSig->segment[0].path.drift = cohSig->sig.path.drift;
	cohSig->segment[0].path.width = cohSig->sig.path.width;
	cohSig->segment[0].path.power = cohSig->sig.path.power;
	cohSig->segment[0].pfa = cohSig->cfm.pfa;
	cohSig->segment[0].snr = cohSig->cfm.snr;

	Msg *rMsg = msgList->alloc(SEND_CW_COHERENT_CANDIDATE_RESULT,
			act->getActivityId(), cohSig, sizeof(CwCoherentSignal), blk);
	respQ->send(rMsg);
}

}
