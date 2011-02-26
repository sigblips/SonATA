/*******************************************************************************

 File:    CwConfirmationTaskStub.cpp
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
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CwConfirmationTaskStub.cpp,v 1.4 2009/06/30 19:16:02 kes Exp $
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
		QTask(name_, CWD_CONFIRMATION_PRIO), fftCoherentLen(0),
		fftCoherentPlan(0)
{
}

CwConfirmationTask::~CwConfirmationTask()
{
}

void
CwConfirmationTask::extractArgs()
{
	return;
}

void
CwConfirmationTask::handleMsg(Msg *msg)
{
	return;
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
	return (0);
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
	return;
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
	return;
}

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
	return;
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
	return (0);
}

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
	return;
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
CwConfirmationTask::doPowerSearch(float32_t *powerData, int32_t spectra,
		int32_t binsPerSpectrum, PowerPath& bestPath)
{
	return;
}

void
CwConfirmationTask::doCoherentDetection(CwData& polData,
		Activity *act)
{
	return;
}

CoherentReport
CwConfirmationTask::doCoherentSearch(ComplexFloat32 *coherentTDData,
		int32_t samples, float32_t basePower)
{
	CoherentReport bestCoherence;
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
	return;
}

float32_t
CwConfirmationTask::computePathSum(int32_t spectra, int32_t binsPerSpectrum,
		int32_t drift, int32_t sign, float32_t *pathBase)
{
	return (0);
}

int32_t
CwConfirmationTask::computeTruePath(float32_t spectrum, float32_t spectra,
		float32_t drift)
{
	return (0);
}

void
CwConfirmationTask::dedrift(ComplexFloat32 *sigTDData,
		ComplexFloat32 *coherentTDData, int32_t spectra, int32_t bins,
		PowerPath path)
{
	return;
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
	ChiReport best;
	return (best);
}

void
CwConfirmationTask::dedriftFTPlane(ComplexFloat32 *ftp, int32_t spectra,
		int32_t binsPerSpectrum, int32_t firstSample, float64_t shift,
		float64_t drift)
{
	return;
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
	return;
}


//
// computeData: compute an improved description of the signal based
//		on the results of the power detection
//
void
CwConfirmationTask::computeData(CwData& data, Signal *cwCand,
		Activity *act)
{
	return;
}

//
// computeStrongerPath: compute the stronger of the best power paths
//		for the left and right polarizations
//
CwData&
CwConfirmationTask::selectStrongerPath()
{
	return (left);
}

//
// classifySignal: classify the signal based on the results of the
//		confirmation
//
void
CwConfirmationTask::classifySignal(SystemType origin, CwData &left,
		CwData& right, CwData& summary, Activity *act)
{
	return;
}

void
CwConfirmationTask::sendPrimaryReport(CwData& summary, Activity *act)
{
	return;
}

void
CwConfirmationTask::sendSecondaryReport(CwData& summary,
		Activity *act)
{
	return;
}

}
