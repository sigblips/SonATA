/*******************************************************************************

 File:    CwCoherentSignal.cpp
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

ostream& operator << (ostream &strm, const CwCoherentSignal &cwcs)
{
    strm << "*Cw Coherent*" << endl;
    strm << cwcs.sig;      // signal description

    strm << cwcs.cfm; // confirmation stats
    strm << "nSegments: " << cwcs.nSegments << endl; 

    strm << "Segments: " << endl
	 << "=========" << endl;

    Assert(cwcs.nSegments >= 0 && cwcs.nSegments < MAX_CW_COHERENT_SEGMENTS);
    for (int i = 0; i < cwcs.nSegments; i++)
    {
        strm << i << ") " << cwcs.segment[i];
    }

    return strm;
}

CwCoherentSignal::CwCoherentSignal()
    :
    nSegments(0),
    alignPad(0)
{
    // use default constructors on all other fields
}

void CwCoherentSignal::marshall()
{
    sig.marshall();  // signal description
    cfm.marshall();  // confirmation stats

    Assert(nSegments >= 0 && nSegments < MAX_CW_COHERENT_SEGMENTS);
    for (int i = 0; i < nSegments; i++)
    {
        segment[i].marshall();
    }
    HTONL(nSegments);

}

void CwCoherentSignal::demarshall()
{
    sig.demarshall();  // signal description
    cfm.demarshall();  // confirmation stats

    NTOHL(nSegments);

    Assert(nSegments >= 0 && nSegments < MAX_CW_COHERENT_SEGMENTS);
    for (int i = 0; i < nSegments; i++)
    {
        segment[i].demarshall();
    }
}