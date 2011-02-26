/*******************************************************************************

 File:    Intrinsics.cpp
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
 * (Channelizer) Intrinsics.cpp
 *
 *  Created on: Nov 18, 2009
 *      Authors: kes, tksonata
 */
#include "sseChannelizerInterfaceLib.h"
#include "SseMsg.h"

Intrinsics::Intrinsics():
   beamId(-1), pol(POL_UNINIT),
   totalChannels(0), outputChannels(0), 
   mhzPerChannel(0), foldings(0), oversampling(0)
{
   interfaceVersion[0] = '\0';
   name[0] = '\0';
   host[0] = '\0';
   codeVersion[0] = '\0';
   filterName[0] = '\0';
}

int32_t Intrinsics::getOutputChannels()
{
    return(outputChannels);
}

float64_t Intrinsics::getMhzPerChannel()
{
    return(mhzPerChannel);
}
void
Intrinsics::marshall()
{
   beamBase.marshall();
   HTONL(beamId);
   SseMsg::marshall(pol);

   channelBase.marshall();
   HTONL(totalChannels);
   HTONL(outputChannels);
   HTOND(mhzPerChannel);

   HTONL(foldings);
   HTONF(oversampling);
}

void
Intrinsics::demarshall()
{
   marshall();
}

ostream& ssechan::operator << (ostream& strm, const Intrinsics& intrinsics)
{
   strm
      << "Channelizer Intrinsics:" << endl
      << "------------------------" << endl
      << "interfaceVersion: "
      << intrinsics.interfaceVersion << endl
      << "Name: " << intrinsics.name << endl
      << "Host: " << intrinsics.host << endl
      << "Code version: " << intrinsics.codeVersion << endl
      << "Input: " << endl
      << "  Beam base address: " << intrinsics.beamBase
      << "  Beam ID: " << intrinsics.beamId << endl
      << "  Pol: " << SseMsg::polarizationToString(intrinsics.pol) << endl
      << "Output: " << endl
      << "  Channel base address: " << intrinsics.channelBase
      << "  Total channels: " << intrinsics.totalChannels << endl
      << "  Output channels: " << intrinsics.outputChannels << endl
      << "  MHz per channel: " << intrinsics.mhzPerChannel << endl
      << "DFB: " << endl
      << "  foldings: " << intrinsics.foldings << endl
      << "  oversampling: " << intrinsics.oversampling << endl
      << "  filter: " << intrinsics.filterName << endl
      << endl;

   return (strm);
}