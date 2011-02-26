/*******************************************************************************

 File:    TscopeCalRequest.cpp
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
#include <sstream>

using namespace std;

static const int PrintPrecision(6);

TscopeCalRequest::TscopeCalRequest() 
    : 
   calType(UNINIT),
   integrateSecs(0),
   numCycles(0),
   alignPad(0)
{
}

void TscopeCalRequest::marshall()
{
   SseTscopeMsg::marshall(calType);
   HTONL(integrateSecs);
   HTONL(numCycles);
}

void TscopeCalRequest::demarshall()
{
    marshall();
}

ostream& operator << (ostream& strm, const TscopeCalRequest& req)
{
    stringstream localstrm;

    localstrm.precision(PrintPrecision);  

    localstrm << "tscope cal request:" << endl
              << "  type: " << SseTscopeMsg::calTypeToString(req.calType) << endl
              << "  integrateTimeSecs: " << req.integrateSecs << endl
              << "  numCycles: "  << req.numCycles << endl
              << endl;

    strm << localstrm.str();

    return strm;
}