/*******************************************************************************

 File:    SignalIdGenerator.cpp
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
// SignalId generator class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/lib/SignalIdGenerator.cpp,v 1.3 2009/03/06 21:56:01 kes Exp $
//
#include "SignalIdGenerator.h"

namespace dx {

SignalIdGenerator::SignalIdGenerator()
{
	NssDate startTime;

	SignalIdGenerator(0, 0, startTime);
}

SignalIdGenerator::SignalIdGenerator(int32_t serialNo,
		int32_t activityId, NssDate& startTime)
{
	signalId.dxNumber = serialNo;
	signalId.activityId = activityId;
	signalId.activityStartTime = startTime;
	// increment before, so start at -1
	signalId.number = -1;
#if 0
	// helpful for multibeam testing
	// make dxs generate signal ids in different ranges
	if (signalId.dxNumber == 41) signalId.number = 10;
#endif
}

SignalIdGenerator::~SignalIdGenerator()
{
}

}

