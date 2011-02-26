/*******************************************************************************

 File:    CwBadBandList.h
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
// CWD bad band list
//
#ifndef _CwBadBandListH
#define _CwBadBandListH

#include <iostream>
#include <map>
#include <vector>
#include <sseDxInterface.h>
#include "System.h"
//#include "BadBand.h"
#include "BadBandList.h"

using std::multimap;
using std::pair;
using std::make_pair;
using std::vector;
using std::cout;
using std::endl;

namespace dx {

#define CW_KEY(band)		(band.band.pol << 24 | band.band.bin)
typedef multimap<int32_t, CwBadBand> CwMap;
typedef vector< ::CwBadBand> CwList;

class CwBadBandList: public BadBandList {
public:
	CwBadBandList();
	~CwBadBandList();

	virtual void clear();
	virtual void done();
	virtual int32_t getSize();

	void record(CwBadBand& band);
	::CwBadBand& getNth(int32_t index);

	virtual void print();

private:
	CwMap rawList;
	CwList finalList;

	// internal functions
	bool merge(CwBadBand& band, CwBadBand& nextBand);
	void add(CwBadBand& band);

	// forbidden
	CwBadBandList(const CwBadBandList&);
	CwBadBandList& operator=(const CwBadBandList&);
};

}

#endif