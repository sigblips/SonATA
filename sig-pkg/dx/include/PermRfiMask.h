/*******************************************************************************

 File:    PermRfiMask.h
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
// Permanent RFI mask class definition; derived from FrequencyMask
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/PermRfiMask.h,v 1.3 2009/02/22 04:37:44 kes Exp $
//
#ifndef _PermRfiMaskH
#define _PermRfiMaskH

#include "FrequencyMask.h"

namespace dx {

class PermRfiMask: public FrequencyMask {
public:
	PermRfiMask(const FrequencyMaskHeader& hdr_, const FrequencyBand *mask_);
	~PermRfiMask();

private:
	// forbidden
	PermRfiMask(const PermRfiMask&);
	PermRfiMask& operator=(const PermRfiMask&);
};

}

#endif