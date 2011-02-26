/*******************************************************************************

 File:    readMask.h
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


#ifndef READMASK_H
#define READMASK_H

#include <iostream>
#include <list>
#include <algorithm>
#include <memory>
#include <vector>
#include <stdlib.h>
#include <string>
#include "sseDxInterface.h"

int32_t readMask( vector<struct FrequencyBand>& freqBandList,
	  string & maskFileName, int32_t * numberOfFreqBands,
          NssDate * maskVersionDate, struct FrequencyBand * bandCovered);
#endif