/*******************************************************************************

 File:    SignalMask.cpp
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



#include "DebugLog.h" // include this first so ACE header is defined first
#include "SignalMask.h" 
#include "readMask.h"
#include "SseSystem.h"
#include "SseArchive.h"
#include "SseMessage.h"
#include "DxProxy.h"
#include "MsgSender.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

SignalMask::SignalMask(const string &maskFilename, const string &maskType,
		       int verboseLevel):
    maskType_(maskType),
    verboseLevel_(verboseLevel)
{
    // assume file is in the setup directory
    string fullPath(SseSystem::getSetupDir());
    fullPath += maskFilename;

    // load the mask into the vector

    maskHeader_.alignPad = 0;  // to appease Purify
    int32_t code = readMask(maskList_, fullPath, 
	   &maskHeader_.numberOfFreqBands, 
	   &maskHeader_.maskVersionDate, 
	   &maskHeader_.bandCovered );

    if ( code == 1 )
    {
	// loaded ok
    }
    else {

	stringstream strm;
	strm << "SignalMask: Error reading " << maskType_ << " " <<
		fullPath << " code " << code << endl;
	SseMessage::log(MsgSender,
                        NSS_NO_ACTIVITY_ID, SSE_MSG_FILE_ERROR,
                        SEVERITY_ERROR, strm.str().c_str(),
                        __FILE__, __LINE__);

	// further error handling tbd
    }
}


// For testing:
SignalMask::SignalMask(FrequencyMaskHeader & maskHeader,
		       vector <FrequencyBand> & maskList,
		       const string &maskType,
		       int verboseLevel)
   : 
   maskHeader_(maskHeader),
   maskList_(maskList),
   maskType_(maskType),
   verboseLevel_(verboseLevel)
{
   
}

vector <FrequencyBand> SignalMask::getFreqBands()
{
   return maskList_;
}



SignalMask::~SignalMask()
{
}

DxSignalMask::DxSignalMask(const string &maskFilename,
			     const string &maskType, 
			     int verboseLevel):
  SignalMask(maskFilename,
	     maskType, 
	     verboseLevel)
{
}

void DxSignalMask::sendMaskToDx(DxProxy *proxy)
{
    VERBOSE2(verboseLevel_,
		 "SignalMask: sending " << maskType_ << endl;);

    if (! maskList_.empty())
    {

	// convert vector into an array
	FrequencyBand freqBandArray[maskHeader_.numberOfFreqBands];
	int32_t i = 0;
	
	for (vector<FrequencyBand>::iterator index = maskList_.begin();
	     index != maskList_.end(); ++index)
	{
	    freqBandArray[i] = *index;
	    i++;
	}
	
	forwardMaskToDx(proxy, maskHeader_, freqBandArray);
    }
    else
    {

	stringstream strm;
	strm << "SignalMask: Error trying to send empty mask of type "
	    << maskType_ << endl;
	SseMessage::log(MsgSender,
                        NSS_NO_ACTIVITY_ID, SSE_MSG_FILE_ERROR,
                        SEVERITY_ERROR, strm.str().c_str(),
                        __FILE__, __LINE__);
    }
}

PermRfiMask::PermRfiMask(const string &maskFilename, const string &maskType, 
			 int verboseLevel) :
  DxSignalMask(maskFilename, maskType, verboseLevel)
{
}

void PermRfiMask::forwardMaskToDx(DxProxy *proxy, 
			      const FrequencyMaskHeader &maskHeader,
			      const FrequencyBand freqBandArray[])
{
  proxy->sendPermRfiMask(maskHeader, freqBandArray);
}