/*******************************************************************************

 File:    checkSseTscopeMsgSizes.cpp
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


// checks all the struct sizes & field offsets
// in the sseIfcInterface messages, as a
// marshalling aid.

#include "sseTscopeInterface.h"

#include <iostream>
#include <cstddef>

using namespace std;

#define SIZEOF(x) cout << "\nsizeof " << #x << ": " << sizeof(x) << endl

#define OFFSETOF(x,field) cout << "offsetof " << #field << ": " << offsetof(x,field) << endl

int main() {

  SIZEOF(TscopeIntrinsics);
  OFFSETOF(TscopeIntrinsics, interfaceVersionNumber);
  OFFSETOF(TscopeIntrinsics, name);

  SIZEOF(TscopeAssignSubarray);
  OFFSETOF(TscopeAssignSubarray, beam);
  OFFSETOF(TscopeAssignSubarray, subarray);

  SIZEOF(TscopeCalRequest);
  OFFSETOF(TscopeCalRequest, calType);
  OFFSETOF(TscopeCalRequest, integrateSecs);
  OFFSETOF(TscopeCalRequest, numCycles);

  SIZEOF(TscopeNullType);
  OFFSETOF(TscopeNullType, nullType);

  SIZEOF(TscopeStopRequest);
  OFFSETOF(TscopeStopRequest, subarray);

  SIZEOF(TscopeStowRequest);
  OFFSETOF(TscopeStowRequest, subarray);

  SIZEOF(TscopePointing);
  OFFSETOF(TscopePointing, coordSys);
  OFFSETOF(TscopePointing, azDeg);
  OFFSETOF(TscopePointing, elDeg);
  OFFSETOF(TscopePointing, raHours);
  OFFSETOF(TscopePointing, decDeg);
  OFFSETOF(TscopePointing, galLongDeg);
  OFFSETOF(TscopePointing, galLatDeg);

  SIZEOF(TscopeTuningStatus);
  OFFSETOF(TscopeTuningStatus, skyFreqMhz);

  SIZEOF(TscopeIfChainStatus);
  OFFSETOF(TscopeIfChainStatus, skyFreqMhz);

  SIZEOF(TscopeSubarrayStatus);
  OFFSETOF(TscopeSubarrayStatus, numTotal);
  OFFSETOF(TscopeSubarrayStatus, numSharedPointing);
  OFFSETOF(TscopeSubarrayStatus, numTrack);
  OFFSETOF(TscopeSubarrayStatus, numSlew);
  OFFSETOF(TscopeSubarrayStatus, numStop);
  OFFSETOF(TscopeSubarrayStatus, numOffline);
  OFFSETOF(TscopeSubarrayStatus, numDriveError);
  OFFSETOF(TscopeSubarrayStatus, wrap);
  OFFSETOF(TscopeSubarrayStatus, zfocusMhz);
  OFFSETOF(TscopeSubarrayStatus, gcErrorDeg);

  SIZEOF(TscopeStatusMultibeam);
  OFFSETOF(TscopeStatusMultibeam, time);
  OFFSETOF(TscopeStatusMultibeam, subarray);
  OFFSETOF(TscopeStatusMultibeam, primaryPointing);
  OFFSETOF(TscopeStatusMultibeam, synthPointing);
  OFFSETOF(TscopeStatusMultibeam, tuning);
  OFFSETOF(TscopeStatusMultibeam, ataBackendHost);
  OFFSETOF(TscopeStatusMultibeam, simulated);

  SIZEOF(TscopeBeamCoords);
  OFFSETOF(TscopeBeamCoords, beam);
  OFFSETOF(TscopeBeamCoords, pointing);

  SIZEOF(TscopeSubarrayCoords);
  OFFSETOF(TscopeSubarrayCoords, subarray);
  OFFSETOF(TscopeSubarrayCoords, pointing);

  SIZEOF(TscopeTuneRequest);
  OFFSETOF(TscopeTuneRequest, tuning);
  OFFSETOF(TscopeTuneRequest, skyFreqMhz);

  SIZEOF(TscopeZfocusRequest);
  OFFSETOF(TscopeZfocusRequest, skyFreqMhz);
  OFFSETOF(TscopeZfocusRequest, subarray);

  SIZEOF(TscopeWrapRequest);
  OFFSETOF(TscopeWrapRequest, wrapNumber);
  OFFSETOF(TscopeWrapRequest, subarray);

  SIZEOF(TscopeMonitorRequest);
  OFFSETOF(TscopeMonitorRequest, periodSecs);

  SIZEOF(TscopeAntgroupAutoselect);
}