/*******************************************************************************

 File:    TestSignalMask.h
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
// TestSignalMask class definition
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/TestSignalMask.h,v 1.3 2009/02/22 04:37:45 kes Exp $
//
#ifndef _TestSignalMaskH
#define _TestSignalMaskH

#include "FrequencyMask.h"

namespace dx {

//
// Notes:
//		Because a test signal mask may be shared by more than one
//		activity, each activity must keep track of where it
//		is in processing the mask.
//
class TestSignalMask: public FrequencyMask {
public:
	TestSignalMask(const FrequencyMaskHeader& hdr_, const FrequencyBand *band_);
	~TestSignalMask();

private:
	NssDate maskDate;

	// forbidden
	TestSignalMask(const TestSignalMask&);
	TestSignalMask& operator=(const TestSignalMask&);
};

}

#endif