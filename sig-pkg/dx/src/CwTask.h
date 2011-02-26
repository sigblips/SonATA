/*******************************************************************************

 File:    CwTask.h
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
// $Header: /home/cvs/nss/sonata-pkg/dx/src/CwTask.h,v 1.6 2009/03/07 19:13:41 kes Exp $
//
#ifndef _CwTaskH
#define _CwTaskH

#include "Activity.h"
#include "Args.h"
#include "CwClusterer.h"
#include "CwUnpacker.h"
#include "Dadd.h"
#include "Err.h"
#include "Msg.h"
#include "QTask.h"
#include "State.h"
#include "DxStruct.h"
#include "SuperClusterer.h"
//#include "ClusterHit.h"
//#include "BandReport.h"

using namespace dadd;
using namespace sonata_lib;

namespace dx {

struct CwArgs {
	Queue *detectionQ;					// detection task queue
#ifdef notdef
	SuperClusterer *superClusterer;		// super cluster object

	CwArgs(): detectionQ(0), superClusterer(0) {}
	CwArgs(Queue *detectionQ_, SuperClusterer *superClusterer_):
			detectionQ(detectionQ_), superClusterer(superClusterer_) {}
#else
	CwArgs(): detectionQ(0) {}
	CwArgs(Queue *detectionQ_): detectionQ(detectionQ_) {}
#endif
};

#ifdef notdef
// detection data required to process a polarization
struct CwPolarizationData {
	dx::Unit unit;					// unit for this polarization
	Polarization pol;					// this polarization
	ATADataPacketHeader::PolarizationCode polCode; // internal polarization
	void *data;							// packed data
	CwClusterer *cwClusterer;			// Cw clusters

	CwPolarizationData(): unit(dx::UnitNone), pol(POL_BOTH),
			polCode(ATADataPacketHeader::NONE), data(0), cwClusterer(0) {}

};
#endif

class CwTask: public QTask {
public:
	static CwTask *getInstance();
	~CwTask();

private:
	static CwTask *instance;

	Resolution resolution;				// DADD resolution
	int32_t badBandLimit;				// bad band path limit
	uint32_t dualPolThreshold;			// threshold for sum of polarizations
	int32_t nPols;						// # of polarizations
	int32_t pol;
	uint32_t singlePolThreshold;		// threshold for a single polarization
	int32_t spectra;					// # of spectra
	uint32_t spectrumBins;				// # of bins in a slice spectrum
	uint32_t totalBins;					// total # of bins in a buffer spectrum
										// (includes overlap bins)
	DetectionStatistics statistics;		// detection statistics
	Activity *activity;
	Buffer *detectionBuf;
	Channel *channel;
	CwBadBandList *badBandList;			// bad band list
	CwUnpacker unpacker;				// unpacker of packed data
	Dadd dadd;							// DADD processor
	Queue *detectionQ;
	SuperClusterer *superClusterer;		// super cluster
	CwClusterer *rightClusterer;
	CwClusterer *leftClusterer;

	Args *cmdArgs;
	MsgList *msgList;
	State *state;

	// methods
	void extractArgs();
	void handleMsg(Msg *msg);
	void startDetection(Msg *msg);
	void doDetection(Msg *msg);
	void stopDetection(Msg *msg);
	uint32_t computeThreshold(float64_t sigma);
	void processPol(Polarization pol);
	void sendDetectionComplete(Msg *msg, bool stopped = false);
	void unpack(DaddSlope slope, Polarization pol);
	void detect(DaddSlope slope, Polarization pol);

	// hidden
	CwTask(string name_);

	// forbidden
	CwTask(const CwTask&);
	CwTask& operator=(const CwTask&);
};

}

#endif
