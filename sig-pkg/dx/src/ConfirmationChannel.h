/*******************************************************************************

 File:    ConfirmationChannel.h
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
// ConfirmationChannel: confirmation channel class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/ConfirmationChannel.h,v 1.5 2009/02/22 04:48:37 kes Exp $
//
#ifndef _ConfirmationChannelH
#define _ConfirmationChannelH

#include <complex>
#include <fftw3.h>
#include "Activity.h"
//#include "CDLayout.h"
#include "DxStruct.h"

using namespace sonata_lib;

namespace dx {

//
// This class handles processing of subchannel (also known as
// Complex Amplitudes) data: retrieving it from disk, rearranging
// it and converting it to floating point, synthesis of a wider
// confirmation channel, and extraction of the signal path from
// the wider channel.
//

class ConfirmationChannel {
public:
	ConfirmationChannel();
	~ConfirmationChannel();

	void computeChannelParams(Activity *act_, SignalDescription *sig_,
			int32_t subchannels_);
//	CDLayout& getCDLayout() { return (bufLayout); }
//	dxXferSpec& getRetrievalSpec();
//	Error retrieveCDData(DiskIORequest *req);
	void assembleConfData(ComplexPair *cdData, ComplexFloat32 *confData,
			int32_t spectrum);
	void synthesizeConfChannel(ComplexFloat32 *confData,
			ComplexFloat32 *confTDData, float32_t *basePower);
	Error dumpSubchannelData(Buffer *buf, const string *diskFile = 0);

protected:
	int32_t stride;						// stride between subchannels
	int32_t subchannels;				// # of subchannels
	int32_t subchannelSpectra;			// # of subchannel spectra
	int32_t startSubchannel;			// starting subchannel
	uint32_t halfFrames;				// # of half frames in activity
	int32_t samples;					// total # of wide channel samples
	uint32_t subchannelBinsPerBlk;		// # of subchannel bins in a host buf block
	uint32_t subchannelBlkLen;			// len of a subchannel in a host buf block
	float64_t centerFreq;				// center frequency of the channel
	fftwf_plan fftPlan;					// plan
	Activity *activity;					// activity for this candidate
	Channel *channel;					// channel
//	CDLayout bufLayout;					// buffer layout
//	XferSpec xfer;						// disk xfer spec
	SignalDescription *sig;				// signal description

	// forbidden
	ConfirmationChannel(const ConfirmationChannel&);
	ConfirmationChannel& operator=(const ConfirmationChannel&);
};

}

#endif