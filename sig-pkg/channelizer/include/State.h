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

/**
 * State class (singleton)
 */
//
#ifndef _StateH
#define _StateH

#include "Beam.h"
#include "Buffer.h"
#include "ChErr.h"
#include "ChTypes.h"
#include "InputBuffer.h"
#include "Lock.h"
#include "Log.h"
#include "System.h"
#include <sseChannelizerInterface.h>

using namespace ssechan;
using namespace sonata_lib;

namespace chan {

class State {
public:
	static State *getInstance();

	~State();

	void lock() { sLock.lock(); }
	void unlock() { sLock.unlock(); }

#ifdef notdef
	/**
	 * Perform system configuration.
	 */
	void configure(ChannelizerConfiguration *configuration_);
	const ChannelizerConfiguration& getConfiguration();
#endif

	// state functions
	void setState(ChannelizerState state_) { beam->setState(state_); }
	ChannelizerState getState() { return (beam->getState()); }

	/**
	 * Channelizer configuration.
	 */
	const Intrinsics& getIntrinsics();
//	int32_t getNumber() { return (number); }
//	int32_t getSerialNumber() { return (intrinsics.serialNumber); }
	Status getStatus();
	/**
	 * Channel-specific values.
	 */
	float64_t getBeamWidthMHz() { return (beam->getBandwidth()); }
//	float64_t getBeamOversampling() { return (beam->getOversampling()); }

	/**
	 * Channel-specific values.
	 */
	int32_t getTotalChannels() { return (beam->getTotalChannels()); }
	int32_t getUsableChannels() { return (beam->getUsableChannels()); }
#ifdef notdef
	float64_t getChannelWidthMHz() { return (channelSpec.widthMHz); }
	float64_t getChannelRateMHz() { return (channelSpec.effectiveWidthMHz); }
#endif
	int32_t getFoldings() { return (beam->getFoldings()); }
	float64_t getOversampling() { return (beam->getOversampling()); }

#ifdef notdef
	/**
	 * Buffer functions.
	 */
	InputBuffer *allocInputBuf(bool wait_ = false);
	void freeInputBuf(InputBuffer *buf);

	/**
	 * Functions to convert between polarization representations
	 */
	ATADataPacketHeader::PolarizationCode getPolCode(Polarization pol);
#endif
	Polarization getPol(ATADataPacketHeader::PolarizationCode pol);

private:
	static State *instance;

//	ChannelizerState state;				// state of the channelizer
	Error err;							// last error
	Lock sLock;							// access lock
	Intrinsics intrinsics;
//	Status status;

	Beam *beam;							// beam data

#ifdef notdef
	/**
	 * Channel specification
	 */
	struct cSpec {
		float64_t widthMHz;				// nominal width of the channel (MHz)
		float64_t oversampling;			// oversampling percentage
		float64_t effectiveWidthMHz;	// effective width of the channel (MHz)

		cSpec() {
			init(DEFAULT_CHANNEL_WIDTH_MHZ, DEFAULT_CHANNEL_OVERSAMPLING);
		}
		void init(float64_t w, float64_t o) {
			widthMHz = w;
			oversampling = o;
			effectiveWidthMHz = widthMHz / (1.0 - oversampling);
		}
	} channelSpec;
#endif
	void init();

	// hidden
	State();

	// forbidden
	State(const State&);
	State& operator=(const State&);
};

}

#endif