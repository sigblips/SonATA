/*******************************************************************************

 File:    State.h
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

// State class (singleton)
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/State.h,v 1.10 2009/06/26 20:47:20 kes Exp $
//
#ifndef _StateH
#define _StateH

//#include "Activity.h"
#include "BirdieMask.h"
#include "Buffer.h"
#include "Dadd.h"
#include "DxErr.h"
#include "DxTypes.h"
#include "InputBuffer.h"
#include "Lock.h"
#include "Log.h"
#include "PermRfiMask.h"
#include "RecentRfiMask.h"
#include "TestSignalMask.h"
#include "System.h"
#include <sseDxInterface.h>

using namespace dadd;
using namespace sonata_lib;

namespace dx {

// forward declaration
class Activity;
class ActivityList;

class State {
public:
	static State *getInstance();

	~State();

	void lock() { sLock.lock(); }
	void unlock() { sLock.unlock(); }

	/**
	 * Perform system configuration.
	 */
	void configure(DxConfiguration *configuration_);
	const DxConfiguration& getConfiguration();

	/**
	 * DX configuration.
	 */
	const DxIntrinsics& getIntrinsics();
	int32_t getNumber() { return (number); }
	int32_t getSerialNumber() { return (intrinsics.serialNumber); }
	DxStatus getStatus();
	const char8_t *getArchiverHost();
	int32_t getArchiverPort();

	int32_t getMaxFrames() { return (maxFrames); }
	int32_t getMaxHalfFrames() { return (2 * getMaxFrames() + 1); }
	int32_t getSubchannelsPerArchiveChannel() { return (ARCHIVE_SUBCHANNELS); }
	float64_t getFrameTime() { return (1.0 / getBinWidthHz(RES_1HZ)); }

	float64_t getCdBytesPerSample() { return (CD_BYTES_PER_BIN); }
	float64_t getCwBytesPerBin() { return (CWD_BYTES_PER_BIN); }

	/**
	 * Half frame-oriented values.
	 */
	int32_t getSamplesPerSubchannelHalfFrame() {
		return (SUBCHANNEL_SAMPLES_PER_HALF_FRAME);
	}
	int32_t getBytesPerSubchannelHalfFrame() {
		return (getSamplesPerSubchannelHalfFrame() * sizeof(ComplexFloat32));
	}
	int32_t getSamplesPerSubchannelFrame() {
		return (2 * getSamplesPerSubchannelHalfFrame());
	}
	int32_t getSamplesPerHalfFrame() {
		return (getSamplesPerSubchannelHalfFrame() * getUsableSubchannels());
	}
	int32_t getSamplesPerFrame() {
		return (getSamplesPerSubchannelFrame() * getUsableSubchannels());
	}

	/**
	 * Channel-specific values.
	 */
	float64_t getChannelWidthMHz() { return (channelSpec.widthMHz); }
	float64_t getChannelRateMHz() { return (channelSpec.effectiveWidthMHz); }
	float64_t getChannelOversampling() { return (channelSpec.oversampling); }

	/**
	 * Subchannel-specific values.
	 */
	int32_t getMaxSubchannels() { return (MAX_SUBCHANNELS); }
	int32_t getTotalSubchannels() { return (subchannelSpec.total); }
	int32_t getUsableSubchannels() { return (subchannelSpec.usable); }
	float64_t getSubchannelWidthMHz() { return (subchannelSpec.widthMHz); }
	float64_t getSubchannelRateMHz() { return (subchannelSpec.effectiveWidthMHz); }
	int32_t getFoldings() { return (subchannelSpec.foldings); }
	float64_t getSubchannelOversampling() {
		return (subchannelSpec.oversampling);
	}
	/**
	 * Bin values.
	 */
	int32_t getTotalBinsPerSubchannel1Hz() { return (binsPerSubchannel1Hz); }
	int32_t getTotalBinsPerSubchannel(Resolution res);
	int32_t getUsableBinsPerSubchannel(Resolution res);
	int32_t getUsableBinsPerSpectrum(Resolution res);
	float64_t getBinWidthMHz(Resolution res);
	float64_t getBinWidthHz(Resolution res);

	/**
	 * Activity functions.
	 */
	Activity *allocActivity(const DxActivityParameters *params_);
	void freeActivity(Activity *activity);
	Activity *getFirstActivity();
	Activity *getNextActivity();
	Activity *findActivity(int32_t activityId);
	Activity *findActivity(DxActivityState activityState);

	/**
	 * Mask functions.
	 */
	void setPermRfiMask(const FrequencyMaskHeader *hdr_);
	PermRfiMask *getPermRfiMask();
	void setBirdieMask(const FrequencyMaskHeader *hdr_);
	BirdieMask *getBirdieMask();
	void setRcvrBirdieMask(const FrequencyMaskHeader *hdr_);
	BirdieMask *getRcvrBirdieMask();
	void setRecentRfiMask(const RecentRfiMaskHeader *hdr_);
	RecentRfiMask *getRecentRfiMask();
	void setTestSignalMask(const FrequencyMaskHeader *hdr_);
	TestSignalMask *getTestSignalMask();

	/**
	 * Buffer functions.
	 */
	InputBuffer *allocInputBuf(bool wait_ = false);
	void freeInputBuf(InputBuffer *buf);
	BufPair *allocHfBuf(bool wait_ = false);
	void freeHfBuf(BufPair *bufPair);
	Buffer *allocDetectionBuf();
	void freeDetectionBuf(Buffer *buf_);
	BufPair *allocArchiveBuf(bool wait_ = false);
	void freeArchiveBuf(BufPair *bufPair);
	BufPair *allocCandBuf(bool wait_ = false);
	void freeCandBuf(BufPair *bufPair);

	/**
	 * Functions to convert between polarization representations.
	 */
	Polarization getPol(ATADataPacketHeader::PolarizationCode pol);
	ATADataPacketHeader::PolarizationCode getPolCode(Polarization pol);

private:
	static State *instance;

	int32_t number;						// DX number
	int32_t maxFrames;					// maximum number of frames
	int32_t binsPerSubchannel1Hz;		// 1 Hz bins per subchannel
//	float64_t frameTime;				// time for a frame
	Error err;							// last error
	Lock sLock;							// access lock
	ActivityList *activityList;			// activity list
	BufPairList *hfBufPairList;			// list of free half-frame buffer pairs
	Buffer *detectionBuf;				// detection buffer
	BufPairList *archiveBufPairList;	// archive channel bufs (ComplexFloat32)
	BufPairList *candBufPairList;		// archive channel bufs (ComplexPair)
	InputBufferList *inputBufList;		// list of input buffers
	DxIntrinsics intrinsics;
	DxConfiguration configuration;
	PermRfiMask *permRfiMask;
	BirdieMask *birdieMask;
	BirdieMask *rcvrBirdieMask;
	TestSignalMask *testSignalMask;
	RecentRfiMask *recentRfiMask;

	/**
	 * Channel specification
	 */
	struct cSpec {
		float64_t widthMHz;				// nominal width of the channel (MHz)
		float64_t oversampling;			// oversampling percentage
		float64_t effectiveWidthMHz;		// effective width of the channel (MHz)

		cSpec(): widthMHz(DEFAULT_CHANNEL_WIDTH_MHZ),
				oversampling(DEFAULT_CHANNEL_OVERSAMPLING) {
			effectiveWidthMHz = widthMHz / (1.0 - oversampling);
		}
		void init(float64_t w, float64_t o) {
			widthMHz = w;
			oversampling = o;
			effectiveWidthMHz = widthMHz / (1.0 - oversampling);
		}
	} channelSpec;

	/**
	 * Subchannel specification
	 */
	struct sSpec {
		int32_t total;					// total # of subchannels created
		int32_t usable;					// # of usable subchannels
		int32_t foldings;				// number of foldings
		float64_t widthMHz;				// nominal width of a subchannel (MHz)
		float64_t oversampling;			// oversampling
		float64_t effectiveWidthMHz;	// effective width (MHz)

		void init(float64_t ecw, float64_t co, int32_t t, float64_t o,
				int32_t f);
	} subchannelSpec;

	/**
	 * 1Hz bin specification
	 */
	struct bSpec {
		int32_t total;					// total # of bins in a spectrum
		int32_t usable;					// usable number of bins in a spectrum
		float64_t widthHz;				// nominal width of a bin (Hz)
		float64_t oversampling;			// oversampling
		float64_t effectiveWidthHz;		// data rate (samples/sec) (Hz)

		void init(float64_t sw, float64_t so, int32_t t, float64_t o);
	} binSpec;

	/**
	 * Compute the size in bytes of the detection buffer.  The buffer must be
	 * large enough to handle DADD data for the maximum length observation
	 * at the worst resolution (4Hz).  Assumes 2-byte accumulators for the
	 * DADD detection.\n
	 *
	 * Notes:\n
	 * 	The DADD buffering requires two extra bins per spectrum, because the
	 * 	signal is allowed to drift one bin per spectrum in either direction.
	 *
	 * @param	frames the maximum number of frames in an observation.
	 * @param	spectra the number of spectra per frame.
	 * @param	bins the width of a spectrum in bins.
	 */
	uint32_t computeDetectionBufSize(int32_t frames, int32_t spectra,
			int32_t bins) {
		return (frames * spectra * (bins + 2) * sizeof(DaddAccum));
	}

	void init();

	// hidden
	State();

	// forbidden
	State(const State&);
	State& operator=(const State&);
};

}

#endif