/*******************************************************************************

 File:    CondensedSignalReport.cpp
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



#include "CondensedSignalReport.h" 
#include <iostream>
#include <fstream>
#include "SseUtil.h"
#include "SseDxMsg.h"
#include "SseMsg.h"
#include "Assert.h"
#include <iomanip>
#include <ctype.h>

using namespace std;

CondensedSignalReport::CondensedSignalReport(
    const string &activityName,
    int activityId,
    const string &dxName,
    const string &reportType,
    const string &dxTuningInfo)
    :
    SignalReport(activityName, activityId, dxName, reportType, dxTuningInfo)
{
    printPreamble(getSignalStrm());

    printPageHeader(getSignalStrm());
}

CondensedSignalReport::~CondensedSignalReport()
{
}


void CondensedSignalReport::addSignal(const CwPowerSignal &cwp)
{
    printCondensedSigDescrip(getSignalStrm(), "CwP", cwp.sig);
    getSignalStrm() << endl;
}

void CondensedSignalReport::addSignal(const CwCoherentSignal &cwc)
{
    printCondensedSigDescrip(getSignalStrm(), "CwC", cwc.sig);
    printCondensedConfirmStats(getSignalStrm(), cwc.cfm);

    getSignalStrm() << endl;
}


void CondensedSignalReport::addSignal(const PulseSignalHeader &pulseHdr)
{
    printCondensedSigDescrip(getSignalStrm(), "Pul", pulseHdr.sig);
    printCondensedConfirmStats(getSignalStrm(), pulseHdr.cfm);

    getSignalStrm() << endl;
}


void CondensedSignalReport::addSignal(const string & sigType,
				      const SignalDescription & descrip,
				      const ConfirmationStats & cfm)
{
    printCondensedSigDescrip(getSignalStrm(), sigType, descrip);
    printCondensedConfirmStats(getSignalStrm(), cfm);

    getSignalStrm() << endl;
}

// override base class
void CondensedSignalReport::printPageHeader(ostream &strm) const
{
    printCondensedSigDescripHeader(strm);
}