/*******************************************************************************

 File:    CwConfirmationTask.h
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
// Cw candidate signal confirmation task
//
// This task performs confirmation for a single CW signal
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CwConfirmationTask.h,v 1.7 2009/06/26 20:49:59 kes Exp $
//
#ifndef _CwConfirmationTaskH
#define _CwConfirmationTaskH

#include <iostream>
#include <fftw3.h>
#include "Activity.h"
#include "ArchiveChannel.h"
#include "DxStruct.h"
#include "Msg.h"
#include "Partition.h"
#include "QTask.h"
#include "State.h"
#include "TransformWidth.h"

using std::cout;
using std::endl;
using std::dec;
using namespace sonata_lib;

namespace dx {

const int32_t WIDTH_FACTOR = 2;

//
// startup arguments
//
struct CwConfirmationArgs {
	Queue *confirmationQ;			// confirmation task queue
	Queue *respQ;					// SSE response queue

	CwConfirmationArgs(): confirmationQ(0), respQ(0) {}
	CwConfirmationArgs(Queue *confirmationQ_, Queue *respQ_):
			confirmationQ(confirmationQ_), respQ(respQ_) {}
};

struct PowerPath {
	float64_t bin;						// start bin of path
	float64_t drift;					// drift of path (bins)
	float32_t power;					// path power

	PowerPath(): bin(0), drift(0), power(0) {}

	friend ostream& operator << (ostream &strm, const PowerPath& ppp)
	{
		cout << "PowerPath: "
			<< "bin: " << ppp.bin
			<< ", drift: " << ppp.drift
			<< ", power: " << ppp.power << endl;
		return (strm);
	}
};

struct ChiReport {
	int32_t bins;						// # of bins per spectrum
	float64_t power;					// total power
	float64_t chiSq;
	int32_t clusterWidth;
	int32_t clusterIndex;

	ChiReport(): bins(0), power(0), chiSq(0), clusterWidth(0),
			clusterIndex(0) {}

	friend ostream& operator << (ostream &strm, const ChiReport& pchir)
	{
		strm << "ChiReport: "
			<< "bins: " << pchir.bins
			<< ", power: " << pchir.power
			<< ", chiSq: " << pchir.chiSq
			<< ", clusterWidth: " << pchir.clusterWidth
			<< ", clusterIndex: " << pchir.clusterIndex << endl;
		return (strm);
	}

};

struct CoherentReport {
	int32_t microDrift;					// # of microbins of drift
	float64_t signif;					// significance
	ChiReport chi;					// chi-squared report

	CoherentReport(): microDrift(0), signif(0) {}

	friend ostream& operator << (ostream &strm, const CoherentReport &pcohr)
	{
		strm << "CoherentReport: "
			<< "microdrift: " << pcohr.microDrift
			<< ", signif: " << pcohr.signif << endl
			<< pcohr.chi;
		return (strm);
	}
};

/**
 * Confirmation data structure for a single polarization.
 */
struct CwData {
	ArchiveChannel *ac;					// archive channel data
//	int32_t sigSamples;					// # of signal channel samples
//	int32_t sigSpectra;					// # of power spectra
//	int32_t sigBins;					// # of power bins per spectrum
//	int32_t cohSamples;					// # of coherent samples
	float32_t basePower;				// baseline power
//	ComplexPair *cdData;				// raw CD data
//	ComplexFloat32 *sigTd;				// signal channel TD data
//	ComplexFloat32 *sigFd;				// signal channel FD data
//	ComplexFloat32 *tdData;				// signal time domain data
//	ComplexFloat32 *fdData;				// signal frequency domain data
//	float32_t *sigPower;				// signal power data
//	ComplexFloat32 *cohTd;				// coherent time domain data
	PowerPath bestPath;					// best power path
	ChiReport chiReport;				// chi-square report
	CoherentReport coherentReport;		// coherent report
	SignalDescription cfmSig;			// signal to confirm
	ConfirmationStats cfm;				// confirmation stats

	CwData(): ac(0), basePower(0)
	{
		ac = new ArchiveChannel;
		Assert(ac);
	}
};

//
// This task controls confirmation of signals selected as
// candidates
//
class CwConfirmationTask: public QTask {
public:
	static CwConfirmationTask *getInstance();
	~CwConfirmationTask();

private:
	static CwConfirmationTask *instance;

//	int32_t coherentBins;				// # of bins in a coherent spectrum
//	int32_t coherentSamples;			// # of samples in coherent data
//	int32_t coherentSpectra;			// # of coherent spectra
//	int32_t coherentFftLen;				// length of current coherent fft
//	float64_t coherentBinWidth;

	CwCoherentSignal primaryCoherentSignal;// main coherent signal
	fftwf_plan coherentFftPlan;			// FFT plan for current length
//	BufPair *sigTDBuf;					// time-domain signal buffers
//	BufPair *sigFDBuf;					// frequency-domain signal buffers
//	BufPair *sigPowerBuf;				// signal power buffers
	Channel *channel;					// channel for this activity
	CwData left;						// left pol description
	CwData right;						// right pol description
//	ArchiveChannel cwChannel;			// archive/confirmation channel
	Queue *confirmationQ;				// control task queue
	Queue *respQ;						// SSE response queue
	Resolution resolution;				// signal resolution
	SignalDescription sig;				// signal description
	struct p {
		int32_t samples;				// # of samples
		int32_t spectra;				// # of spectra
		int32_t bins;					// # of bins per spectrum
		int32_t totalBins;				// total # of bins in buffer
		ComplexFloat32 *td;				// time-domain samples
		ComplexFloat32 *fd;				// frequency-time plane
		float32_t *pd;					// frequency-time plane power

		p(): samples(0), spectra(0), bins(0), totalBins(0), td(0), fd(0), pd(0)
				{}
	} power;							// power signal
	struct c {
		int32_t samples;				// # of samples
//		int32_t spectra;				// # of spectra
//		int32_t bins;					// # of bins per spectrum
		float64_t binWidthHz;			// microbin width in Hz
		ComplexFloat32 *td;				// time-domain samples
		ComplexFloat32 *dd;				// dedrifted time-domain samples
		ComplexFloat32 *fd;				// frequency-time plane
//		float32_t *pd;					// frequency-time plane power
		fftwf_plan plan;				// fftw plan

		c(): samples(0), td(0), dd(0), fd(0), plan(0) {}
	} coherent;							// coherent signal

	MsgList *msgList;
	PartitionSet *partitionSet;
	State *state;
	TransformWidth *transform;

	// methods
	void extractArgs();
	void createTasks();
	void startTasks();
	void handleMsg(Msg *msg);
	Error handleSecondaryMsg(Msg *msg);

	void startActivity(Msg *msg);
	void stopActivity(Msg *msg);

	void doPrimaryConfirmation(Msg *msg);
	Error doSecondaryConfirmation(Msg *msg, Activity *act);

	void createArchiveChannel(Msg *msg, Signal *cand,
			const SignalDescription& sig, Activity *act);
	void createSignalChannel(Msg *msg, Activity *act,
			Signal *cand, SignalDescription *sig);
	void createPowerSpectra(CwData& polData);
	void doPowerDetection(CwData& polData, Activity *act);
	void doCoherentDetection(CwData& polData, Activity *act);

	void doPowerSearch(CwData& polData);
	CoherentReport doCoherentSearch(CwData& polData, int32_t samples);
	void extractCoherentSignal(ComplexFloat32 *sigTDData,
			ComplexFloat32 *coherentTDData, int32_t spectra, int32_t bins,
			PowerPath bestPath);
	float32_t computePathSum(int32_t spectra, int32_t binsPerSpectrum,
			int32_t drift, int32_t sign, float32_t *pathBase);
	int32_t computeTruePath(float32_t spectrum, float32_t spectra,
			float32_t drift);
	void dedrift(ComplexFloat32 *sigTDData, ComplexFloat32 *coherentTDData,
			int32_t spectra, int32_t bins, PowerPath path);
	ChiReport checkCoherence(ComplexFloat32 *data, float64_t avgPower,
			int32_t bins, int32_t drift, int32_t fSearch, int32_t wSearch);
	void dedriftFTPlane(ComplexFloat32 *ftp, int32_t spectra,
			int32_t binsPerSpectrum, int32_t firstSample, float64_t shift,
			float64_t drift);
	void computeSignalData(SignalDescription& cfmSig,
			CoherentReport& coherentReport);
	void computeData(CwData& data, Signal *cwCand, Activity *act);
	CwData& selectStrongerPath();
	void classifySignal(SystemType origin, CwData& left,
			CwData& right, CwData& summary, Activity *act);
	void sendPrimaryReport(CwData& summary, Activity *act);
	void sendSecondaryReport(CwData& summary, Activity *act);

	// hidden
	CwConfirmationTask(string name_);

	// forbidden
	CwConfirmationTask(const CwConfirmationTask&);
	CwConfirmationTask& operator=(const CwConfirmationTask&);
};

}

#endif