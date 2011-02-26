/*******************************************************************************

 File:    Status.cpp
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

/*
 * (Channelizer) Status.cpp
 *
 *  Created on: Nov 18, 2009
 *      Authors: kes, tksonata
 */
#include "sseChannelizerInterfaceLib.h"
#include "SseChanMsg.h"
#include <string>

using std::string;

Status::Status(): 
   centerSkyFreqMhz(-1),
   state(STATE_IDLE),
   alignPad(0)
{
}

void
Status::marshall()
{
   timestamp.marshall();
   startTime.marshall();
   HTOND(centerSkyFreqMhz);
   state = static_cast<ChannelizerState> (htonl(state));
}

void
Status::demarshall()
{
   marshall();
}

ostream& ssechan::operator << (ostream& strm, const Status& status)
{
   strm
      << "Channelizer Status:" << endl
      << "-------------------" << endl
      << "Timestamp: " << status.timestamp << endl
      << "Start Time: " << status.startTime << endl
      << "Center sky freq MHz: " << status.centerSkyFreqMhz << endl 
      << "State: " << SseChanMsg::stateToString(status.state)
      << endl;

   return (strm);
}