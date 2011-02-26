/*******************************************************************************

 File:    PulseConfirmationChannel.h
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
// PulseConfirmationChannel: Pulse confirmation channel class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/PulseConfirmationChannel.h,v 1.4 2009/02/22 04:48:37 kes Exp $
//
#ifndef _PulseConfirmationChannelH
#define _PulseConfirmationchannelH

#include <fftw3.h>
#include "ConfirmationChannel.h"
#include "TransformWidth.h"

using namespace sonata_lib;

namespace dx {

//
// This class is derived from the ConfirmationChannel class,
// and is used to create and extract pulse bins used in confirmation
// pulse detection
//
class PulseConfirmationChannel: public ConfirmationChannel {
public:
	PulseConfirmationChannel();
	~PulseConfirmationChannel();

	Error extractBin(ComplexPair *cdData, ComplexFloat32 *binVal,
			int32_t spectrum, float64_t freq, Resolution res);

private:
	fftwf_plan fftPulsePlan;
	int32_t fftPulsePlanSize;
};

}

#endif

