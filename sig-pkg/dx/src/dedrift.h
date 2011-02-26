/*******************************************************************************

 File:    dedrift.h
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
// dedrift: functions to handle dedrifting of time domain data
//
// $Header: /home/cvs/nss/sonata-pkg/dx/src/dedrift.h,v 1.6 2009/02/22 04:48:38 kes Exp $
//
#ifndef _DedriftH
#define _DedriftH

#include <sseDxInterface.h>
#include "System.h"
#include "DxTypes.h"

using namespace sonata_lib;

namespace dx {

// function prototypes
void DedriftFTPlane(ComplexFloat32 *cftp, ComplexFloat32 *sftp,
		int32_t blks, int32_t samplesPerBlk, int32_t firstSample,
		float64_t shiftConst, float64_t driftConst, float32_t *power = 0);

}
#endif
