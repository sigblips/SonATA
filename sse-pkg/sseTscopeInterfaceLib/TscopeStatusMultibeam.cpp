/*******************************************************************************

 File:    TscopeStatusMultibeam.cpp
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



#include "sseTscopeInterfaceLib.h"
#include "SseUtil.h"
#include <sstream>

// --------------------------------

TscopeTuningStatus::TscopeTuningStatus() :
   skyFreqMhz(0.0)
{
}

void TscopeTuningStatus::marshall() 
{
   HTOND(skyFreqMhz);
}

void TscopeTuningStatus::demarshall()
{
   marshall();
}

ostream& operator << (ostream& strm, const TscopeTuningStatus& status)
{
   strm 
      << " skyfreq: " << status.skyFreqMhz << " MHz" 
      << endl;

   return strm;
}
// --------------------------------

TscopeIfChainStatus::TscopeIfChainStatus() :
   skyFreqMhz(0.0)
{
}

void TscopeIfChainStatus::marshall() 
{
   HTOND(skyFreqMhz);
}

void TscopeIfChainStatus::demarshall()
{
   marshall();
}

ostream& operator << (ostream& strm, const TscopeIfChainStatus& status)
{
   strm 
      << " skyfreq: " << status.skyFreqMhz << " MHz" 
      << endl;

   return strm;
}

// --------------------------------
TscopeSubarrayStatus::TscopeSubarrayStatus() :
   numTotal(0),
   numSharedPointing(0),
   numTrack(0),
   numSlew(0),
   numStop(0),
   numOffline(0),
   numDriveError(0),
   wrap(0),
   zfocusMhz(0),
   gcErrorDeg(0)
{
}

void TscopeSubarrayStatus::marshall()
{
   HTONL(numTotal);
   HTONL(numSharedPointing);
   HTONL(numTrack);
   HTONL(numSlew);
   HTONL(numStop);
   HTONL(numOffline);
   HTONL(numDriveError);
   HTONL(wrap);
   HTOND(zfocusMhz);
   HTOND(gcErrorDeg);
}

void TscopeSubarrayStatus::demarshall()
{
   marshall();
}

ostream& operator << (ostream& strm, const TscopeSubarrayStatus& status)
{
   stringstream localstrm;

   const int HzPrecision(6);
   localstrm.precision(HzPrecision); 
   localstrm.setf(std::ios::fixed);  // show all decimal places up to precision
   localstrm 
      << "  numTotal: " << status.numTotal 
      << "  numSharedPointing: " << status.numSharedPointing 
      << "  numTrack: " << status.numTrack 
      << "  numSlew: " << status.numSlew 
      << "  numStop: " << status.numStop 
      << "  numOffline: " << status.numOffline 
      << "  numDriveError: " << status.numDriveError
      << "  wrap: " << status.wrap 
      << "  zfocus: " << status.zfocusMhz << " MHz" 
      << "  gcErr: " << status.gcErrorDeg << " deg  " 
      << endl;
   
   strm << localstrm.str();
   
   return strm;
}


// --------------------------------

TscopeStatusMultibeam::TscopeStatusMultibeam() :
    simulated(SSE_FALSE),
    alignPad(0)
{
   SseUtil::strMaxCpy(time, "--:--:-- UTC", MAX_TEXT_STRING);
   SseUtil::strMaxCpy(ataBackendHost, "", MAX_TEXT_STRING);
}

void TscopeStatusMultibeam::marshall()
{
   //SseMsg::marshall(allocated);  // bool_t

   // no marshalling needed for time char array

   for (int beamIndex = 0; beamIndex < TSCOPE_N_BEAMS; beamIndex++)
   {
      subarray[beamIndex].marshall();
      primaryPointing[beamIndex].marshall();
      synthPointing[beamIndex].marshall();
      ifChain[beamIndex].marshall();
   }

   for (int tuningIndex = 0; tuningIndex < TSCOPE_N_TUNINGS; tuningIndex++)
   {
      tuning[tuningIndex].marshall();
   }

   // no marshalling needed for ataBackendHost char array

   SseMsg::marshall(simulated);  // bool_t
}

void TscopeStatusMultibeam::demarshall()
{
   marshall();
}

ostream& operator << (ostream& strm, const TscopeStatusMultibeam& status)
{
   stringstream localstrm;

   const int HzPrecision(6);
   localstrm.precision(HzPrecision); 
   localstrm.setf(std::ios::fixed);  // show all decimal places up to precision

/*
  string allocatedState("no");
  if (status.allocated)
  {
  allocatedState = "yes";
  }
*/

   localstrm 
      << "tscope status:\n"
      << "time: " << status.time 
      << endl;

   // beam subarray 
   for (int beamIndex = 0; beamIndex < TSCOPE_N_BEAMS; beamIndex++)
   {
      string beamName(SseTscopeMsg::beamToName(static_cast<TscopeBeam>(beamIndex)));

      localstrm << "BEAM[" << beamIndex << "]: " << beamName 
                << ": SUBARRAY: " << status.subarray[beamIndex];

      localstrm << "BEAM[" << beamIndex << "]: " << beamName 
                << ": PRIMARY: " <<  status.primaryPointing[beamIndex];

      localstrm << "BEAM[" << beamIndex << "]: " << beamName 
                << ": SYNTH: " <<  status.synthPointing[beamIndex];

      localstrm << "BEAM[" << beamIndex << "]: " << beamName 
                << ": IF: " <<  status.ifChain[beamIndex];

   }
      
   // Tunings
   for (int tuningIndex = 0; tuningIndex < TSCOPE_N_TUNINGS; tuningIndex++)
   {
      localstrm << "TUNING[" << tuningIndex << "]: "
		<< SseTscopeMsg::tuningToName(static_cast<TscopeTuning>(tuningIndex))
		<< status.tuning[tuningIndex];
   }

   localstrm << "ATA Backend Host: " << status.ataBackendHost << endl;

   localstrm << "Simulated: " << status.simulated << endl;
   
   strm << localstrm.str();
   
   return strm;
}
  