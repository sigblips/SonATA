/*******************************************************************************

 File:    MinMaxDxSkyFreqMhz.cpp
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



#include <ace/OS.h>
#include "MinMaxDxSkyFreqMhz.h"
#include "DxProxy.h"
#include <algorithm>

using namespace std;

static list<double> extractDxSkyFreqs(DxList &dxList)
{
    list<double> skyFreqs;

    // pull out the sky freq from each dx
    DxList::iterator it;
    for (it = dxList.begin(); it != dxList.end(); ++it)
    {
	DxProxy *proxy = *it;

	// skip dxs to be ignored
	if (proxy->getDxSkyFreq() > 0.0)
	{
	    skyFreqs.push_back(proxy->getDxSkyFreq());
	}
    }

    return skyFreqs;
}


// find the minimum sky frequency of the DxProxy's in the list
double MinDxSkyFreqMhz(DxList &dxList)
{
    double minFreq = -1;
    if (dxList.size() > 0)
    {
	list<double> dxFreqList = extractDxSkyFreqs(dxList);
	if (! dxFreqList.empty())
	{
	    minFreq = *min_element(dxFreqList.begin(), dxFreqList.end());
	}
    }
    return minFreq;
}

// find the max sky frequency of the DxProxy's in the list
double MaxDxSkyFreqMhz(DxList &dxList)
{
    double maxFreq = -1;
    if (dxList.size() > 0)
    {
	list<double> dxFreqList = extractDxSkyFreqs(dxList);
	if (! dxFreqList.empty())
	{
	    maxFreq = *max_element(dxFreqList.begin(), dxFreqList.end());
	}
    }
    return maxFreq;
}