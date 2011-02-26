/*******************************************************************************

 File:    BadBandList.cpp
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
// Bad band list base class
//
#include "BadBandList.h"

namespace dx {

BadBandList::BadBandList():  llock("badBandListLock"), complete(false),
		spectra(256), baseFreq(6.0), binWidth(1.0),
		obsLength((spectra / 2) / binWidth)
{
}

BadBandList::~BadBandList()
{
}

float64_t
BadBandList::binsToMHz(int32_t bins)
{
	return (bins * binWidth / 1e6);
}

float64_t
BadBandList::binsToHz(int32_t bins)
{
	return (bins * binWidth);
}

float64_t
BadBandList::binsToAbsoluteMHz(int32_t bins)
{
	return (baseFreq + binsToMHz(bins));
}

void
BadBandList::clear()
{
	complete = false;
}

void
BadBandList::done()
{
	complete = true;
}

}

