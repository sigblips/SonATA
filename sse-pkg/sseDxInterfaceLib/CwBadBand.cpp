/*******************************************************************************

 File:    CwBadBand.cpp
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


#include "sseDxInterfaceLib.h"

// print message bodies
ostream& operator << (ostream &strm, const CwBadBand & cwBadBand)
{
    strm << "CwBadBand:" << endl
	 << "--------------" << endl
	 << "band: " << cwBadBand.band
	 << "pol: " << SseMsg::polarizationToString(cwBadBand.pol) << endl
	 << "paths: " << cwBadBand.paths << endl
	 << "maxPathCount: " << cwBadBand.maxPathCount << endl
	 << "maxPath: " << cwBadBand.maxPath << endl;
    
    return strm;
}

CwBadBand::CwBadBand()
    :
    pol(POL_UNINIT),
    paths(0),
    maxPathCount(0),
    alignPad(0)
{
}

void CwBadBand::marshall()
{
    band.marshall();    // FrequencyBand
    SseMsg::marshall(pol);
    HTONL(paths);
    HTONL(maxPathCount);
    maxPath.marshall();  // SignalPath
}

void CwBadBand::demarshall()
{
    marshall();
}