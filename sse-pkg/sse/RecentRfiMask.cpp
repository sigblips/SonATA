/*******************************************************************************

 File:    RecentRfiMask.cpp
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



#include "RecentRfiMask.h" 
#include "Assert.h"
#include "SseException.h"
#include <iostream>

using namespace std;

/*
  Create a recent RFI mask using the signals in the
  signalFreqMhz container.  The mask elements (center freq & width)
  are returned in the maskCenterFreqMhz and maskWidthMhz
  containers.  All mask elements will be at least minMaskElementWidthMhz 
  wide.  Signals that are closer than minMaskElementWidthMhz/2 will be
  merged together into a wider, combined mask element.
  All the signals covered by this combined mask element will
  be no closer than minMaskElementWidthMhz/2 from the edges of 
  the mask element.

  Throws SseException if:
     - signal freq is negative
     - signals are not sorted by increasing frequency.
  
 */

void RecentRfiMask::createMask(
   const vector<double> & signalFreqMhz, 
   double minMaskElementWidthMhz,
   vector<double> & maskCenterFreqMhz, 
   vector<double> & maskWidthMhz)
{
   const string methodName("RecentRfiMask::createMask: ");

   if (signalFreqMhz.empty())
   {
      // No signals to process, mask is empty
      return;
   }

   if (minMaskElementWidthMhz <= 0.0)
   {
      throw SseException(methodName + 
			 "Minimum mask element width is <= 0.0.\n",
			 __FILE__, __LINE__);
   }

   const double halfMinMaskWidthMhz(minMaskElementWidthMhz * 0.5);
   double currentMaskCenterFreqMhz = signalFreqMhz[0];
   double currentMaskWidthMhz = minMaskElementWidthMhz;
   double previousFreqMhz(-1.0);
   
   for (unsigned int signalIndex=0; signalIndex < signalFreqMhz.size(); 
	++signalIndex)
   {
      // Validity checks on frequency
      if (signalFreqMhz[signalIndex] < 0.0)
      {
	 throw SseException(methodName + "Negative signal frequency.\n",
			    __FILE__, __LINE__);
      }
      else if (signalFreqMhz[signalIndex] < previousFreqMhz)
      {
	 throw SseException(methodName 
			    + "Signal frequency out of sorted order.\n",
			    __FILE__, __LINE__);
      }
      previousFreqMhz = signalFreqMhz[signalIndex];


      double currentMaskUpperEdgeMhz(currentMaskCenterFreqMhz +
				     (0.5 * currentMaskWidthMhz));

      bool isSignalTooFarAway(signalFreqMhz[signalIndex] > 
			       currentMaskUpperEdgeMhz + 
			       halfMinMaskWidthMhz);

      if (isSignalTooFarAway)
      {
	 // output the current mask element
	 maskCenterFreqMhz.push_back(currentMaskCenterFreqMhz);
	 maskWidthMhz.push_back(currentMaskWidthMhz);

	 // start a new mask element
	 currentMaskCenterFreqMhz = signalFreqMhz[signalIndex];
	 currentMaskWidthMhz = minMaskElementWidthMhz;
      }

      // adjust the current mask center & width to accommodate
      // the additional signal
      
      double currentMaskLowerEdgeMhz = currentMaskCenterFreqMhz
	 - 0.5 * currentMaskWidthMhz;
      
      currentMaskUpperEdgeMhz = signalFreqMhz[signalIndex] + 
	 halfMinMaskWidthMhz;
      
      currentMaskWidthMhz = currentMaskUpperEdgeMhz - 
	 currentMaskLowerEdgeMhz;
      
      currentMaskCenterFreqMhz = 0.5 * (currentMaskUpperEdgeMhz + 
					currentMaskLowerEdgeMhz);

      bool isLastSignal(signalIndex == signalFreqMhz.size()-1);      
      if (isLastSignal)
      {
	 // output the current mask element
	 maskCenterFreqMhz.push_back(currentMaskCenterFreqMhz);
	 maskWidthMhz.push_back(currentMaskWidthMhz);
      }

   }

   Assert(maskCenterFreqMhz.size() == maskWidthMhz.size());
}




#if 0
RecentRfiMask::RecentRfiMask()
{
}

RecentRfiMask::~RecentRfiMask()
{
}
#endif