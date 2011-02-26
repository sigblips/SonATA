/*******************************************************************************

 File:    RecentRfiMask.h
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
// Recent RFI mask class
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/RecentRfiMask.h,v 1.3 2009/02/22 04:37:45 kes Exp $
//
#ifndef _RecentRfiMaskH
#define _RecentRfiMaskH

#include "FrequencyMask.h"

namespace dx {

class RecentRfiMask: public FrequencyMask {
public:
	RecentRfiMask(const RecentRfiMaskHeader& hdr_, const FrequencyBand *mask_);
	~RecentRfiMask();

	int32_t getActivityId() { return (activityId); }

private:
	int32_t activityId;					// activity this mask applies to

	// forbidden
	RecentRfiMask(const RecentRfiMask&);
	RecentRfiMask& operator=(const RecentRfiMask&);
};

}

#endif