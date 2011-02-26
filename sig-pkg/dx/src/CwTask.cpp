/*******************************************************************************

 File:    CwTask.cpp
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
// CW detection class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CwTask.cpp,v 1.8 2009/05/24 22:46:42 kes Exp $
//
#include <iostream>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <DxOpsBitset.h>
#include "System.h"
#include "CwTask.h"
#include "BandReport.h"
#include "ClusterHit.h"

using std::cout;
using std::endl;

namespace dx {

CwTask *CwTask::instance = 0;

CwTask *
CwTask::getInstance()
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new CwTask("CwTask");
	l.unlock();
	return (instance);
}

CwTask::CwTask(string name_): QTask(name_, CWD_PRIO), resolution(RES_UNINIT),
		badBandLimit(0), dualPolThreshold(0),
		nPols(0), pol(0), singlePolThreshold(0), spectra(0), spectrumBins(0),
		totalBins(0), activity(0), detectionBuf(0), channel(0),
		badBandList(0), detectionQ(0), superClusterer(0),
		rightClusterer(0), leftClusterer(0),
		cmdArgs(0), msgList(0), state(0)
{
}

CwTask::~CwTask()
{
}

void
CwTask::extractArgs()
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
	CwArgs *cwArgs = static_cast<CwArgs *> (args);
	Assert(cwArgs);
	detectionQ = cwArgs->detectionQ;
	Assert(detectionQ);

	cmdArgs = Args::getInstance();
	Assert(cmdArgs);
	msgList = MsgList::getInstance();
	Assert(msgList);
	state = State::getInstance();
	Assert(state);
}

void
CwTask::handleMsg(Msg *msg)
{
	switch (msg->getCode()) {
	case StartDetection:
		startDetection(msg);
		break;
	case STOP_DX_ACTIVITY:
		stopDetection(msg);
		break;
	default:
		break;
	}
}

/**
 * start detection
 *
 * Description:\n
 * 	Sets up for CW power detection.
*/
void
CwTask::startDetection(Msg *msg)
{
	// activity must exist and be in correct state
	Activity *act;
	if (!(act = state->findActivity(msg->getActivityId())))
		return;

	if (act->getState() == DX_ACT_PEND_SD)
		act->setState(DX_ACT_RUN_SD);

	if (act->getState() != DX_ACT_RUN_SD) {
		LogError(ERR_IAS, act->getActivityId(), "activity %d, state %d",
				act->getActivityId(), act->getState());
		return;
	}

	// record the activity and get its parameters
	activity = act;
	channel = activity->getChannel();
	Assert(channel);
	if (!detectionBuf)
		detectionBuf = channel->allocDetectionBuf();
	Assert(detectionBuf);

	const DxActivityParameters& params = activity->getActivityParams();

	resolution = params.daddResolution;
	spectra = channel->getSpectra(resolution);
	spectrumBins = channel->getUsableBinsPerSpectrum(resolution);
	totalBins = spectrumBins + spectra;
	singlePolThreshold = computeThreshold(params.daddThreshold);

	activity->resetDetectionStatistics();

	badBandList = activity->getCwBadBandList();
	Assert(badBandList);
	badBandList->clear();
	badBandList->setObsParams(channel->getLowFreq(),
			activity->getBinWidthHz(resolution),
			activity->getSpectra(resolution));

	// compute the bad band limit

	badBandLimit = (int32_t) (params.badBandCwPathLimit
			* MHZ_TO_KHZ(channel->getChannelWidthMHz()));
//	cout << "Cw bad band limit " << badBandLimit << endl;

Debug(DEBUG_DADD, 1, "# 1");

	// set up clustering
	superClusterer = activity->getSuperClusterer();
	Assert(superClusterer);
	Assert(!superClusterer->getCount());

	rightClusterer = new CwClusterer(superClusterer, POL_RIGHTCIRCULAR);
	Assert(rightClusterer);
	rightClusterer->setClusterRange(params.cwClusteringDeltaFreq);

	leftClusterer = new CwClusterer(superClusterer, POL_LEFTCIRCULAR);
	Assert(leftClusterer);
	leftClusterer->setClusterRange(params.cwClusteringDeltaFreq);

	// if there is no CW detection, just send a detection
	// complete message after logging all hits done
	DxOpsBitset operations(params.operations);
	if (operations.test(POWER_CWD))
		doDetection(msg);
	sendDetectionComplete(msg);
}

/**
 * Perform DADD CW detection.
 *
 * Descripton:\n
 * 	Perform DADD on all data, both positive and negative slopes for
 * 	left and right.
 */
void
CwTask::doDetection(Msg *msg)
{
	dadd.setup(spectra, spectrumBins, totalBins, singlePolThreshold,
			DADD_BAND_BINS, badBandLimit, TDDadd, true);
	if (channel->rightPolActive())
		processPol(POL_RIGHTCIRCULAR);
	DaddStatistics stats = dadd.getStatistics();
//	cout << "right pol" << endl << stats;
	if (channel->leftPolActive())
		processPol(POL_LEFTCIRCULAR);
	stats = dadd.getStatistics();
//	cout << "left pol" << endl << stats;

//	sendDetectionComplete(msg, false);
}

/**
 * Stop the signal detection.
 *
 * Description:\n
 * 	Stop the signal detection.
 */
void
CwTask::stopDetection(Msg *msg)
{
	// if no activity is in detection state, ignore the message
	if (!activity)
		return;

	// if the activity ID doesn't match, ignore the message
	if (activity->getActivityId() != msg->getActivityId())
		return;

	// we're either stopping any collecting activity or the one
	// which is currently collecting
	activity->setState(DX_ACT_STOPPING);
	sendDetectionComplete(msg, true);
}

//
// computeThreshold
//
uint32_t
CwTask::computeThreshold(float64_t sigma)
{
	float64_t thresh = spectra * CWD_MEAN_BIN_POWER
			+ sqrt(spectra) * CWD_STDEV_BIN_POWER * sigma;
	Debug(DEBUG_CWD, thresh, "DADD threshold");
	return ((uint32_t) thresh);
}

/**
 * Process a single polarization.
 *
 * Description:\n
 * 	Performs detection on the full bandwidth for a single polarization.  The
 * 	packed power data is in a memory buffer as a contiguous block of data.
 */
void
CwTask::processPol(Polarization pol)
{
	unpack(Positive, pol);
	detect(Positive, pol);
	unpack(Negative, pol);
	detect(Negative, pol);
	BandReport reportBadBand(pol, badBandList);
	dadd.reportBadBands(&reportBadBand);
}

//
// sendDetectionComplete: issue a CW detection complete message
//
void
CwTask::sendDetectionComplete(Msg *msg, bool stopped)
{
	Msg *dMsg = msgList->alloc((DxMessageCode) CwComplete,
			activity->getActivityId());
	dMsg->setUnit((sonata_lib::Unit) UnitCw);
	if (!stopped) {
		rightClusterer->allHitsLoaded();
		statistics.rightCwHits = rightClusterer->getHits();
		statistics.rightCwClusters = rightClusterer->getCount();
		leftClusterer->allHitsLoaded();
		statistics.leftCwHits = leftClusterer->getHits();
		statistics.leftCwClusters = leftClusterer->getCount();
		badBandList->done();

		// record the total # of hits and clusters
		activity->setDetectionStatistics(statistics);
#ifdef notdef
		LogWarning(ERR_NE, msg->getActivityId(), "total clusters = %d",
				clusters);
#endif
	}
	else
		dMsg->setCode(ActivityStopped);
	detectionQ->send(dMsg);
	activity = 0;
}

/**
 * Unpack the packed CW data into the detection buffer.
 *
 * Description:\n
 * 	The packed CW data is in a memory buffer; the data has already been
 * 	corner turned.
 */
void
CwTask::unpack(DaddSlope slope, Polarization pol)
{
	uint8_t *pData =
			static_cast<uint8_t *> (channel->getCwData(pol, resolution));
	uint64_t *upData = static_cast<uint64_t *> (detectionBuf->getData());

	int32_t pOfs = 0;
	if (slope == Positive) {
		// we are doing the positive slopes;
		int32_t upOfs = 0;
		unpacker.unpack(Positive, pData, upData, pOfs, upOfs,
				spectra, spectrumBins, spectrumBins, totalBins);

	}
	else {
		// negative slopes; unpack the data in reverse order
		int32_t upOfs = totalBins;
		unpacker.unpack(Negative, pData, upData, pOfs, upOfs,
				spectra, spectrumBins, spectrumBins, totalBins);
	}
}

//
// detect: perform Cw detection
//
// Synopsis:
//		void detect(slope);
//		DaddSlope slope;			direction of slope (pos or neg)
// Description:
//
// Notes:
//		None.
//
void
CwTask::detect(DaddSlope slope, Polarization pol)
{
	CwClusterer *clusterer;
	switch (pol) {
	case POL_RIGHTCIRCULAR:
	case POL_XLINEAR:
		clusterer = rightClusterer;
		break;
	case POL_LEFTCIRCULAR:
	case POL_YLINEAR:
		clusterer = leftClusterer;
		break;
	default:
		Assert(0);
	}
	// do the adds
	ClusterHit clusterHit(clusterer);
	dadd.execute(pol, slope,
			static_cast<DaddAccum *> (detectionBuf->getData()), &clusterHit);
}

}
