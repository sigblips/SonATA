/*******************************************************************************

 File:    ExpandedSignalReport.cpp
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



#include "ExpandedSignalReport.h" 
#include <iostream>
#include <fstream>
#include "SseUtil.h"
#include "SseDxMsg.h"
#include "SseMsg.h"
#include "Assert.h"
#include <iomanip>
#include <ctype.h>

using namespace std;

ExpandedSignalReport::ExpandedSignalReport(
    const string &activityName,
    int activityId,
    const string &dxName,
    const string &reportType,
    const string &dxTuningInfo)
    :
    SignalReport(activityName, activityId, dxName, reportType, dxTuningInfo)
{
    printPreamble(getSignalStrm());

}

ExpandedSignalReport::~ExpandedSignalReport()
{
}

void ExpandedSignalReport::addSignal(const PulseSignalHeader &pulseHdr,
				     Pulse pulses[])
{
    ostream &strm = getSignalStrm();

    strm << "# =============== Pulse Train =========================" << endl;
    strm << endl;

    printCondensedSigDescripHeader(strm);
    printCondensedSigDescrip(strm, "Pul", pulseHdr.sig);
    printCondensedConfirmStats(strm, pulseHdr.cfm);
    strm << endl;
    strm << endl;

    printPulseTrainDescription(strm, pulseHdr.train);
    strm << endl;

    printPulses(strm, pulseHdr, pulses);
    strm << endl;
}

void ExpandedSignalReport::printPulseTrainDescription(
    ostream &strm, const PulseTrainDescription &ptd)
{
    strm << "#Period Pulse Resolution" << endl;
    strm << "#Secs   Count Hz        " << endl;
    strm << "#------ ----- ----------" << endl; 

    strm.precision(2);
    strm << setw(7) << ptd.pulsePeriod << " ";

    strm << setw(5) << ptd.numberOfPulses << " ";

    strm << setw(10) << SseDxMsg::resolutionToString(ptd.res); 
    strm << endl;

}

void ExpandedSignalReport::printPulses(
    ostream &strm, const PulseSignalHeader &hdr, Pulse pulses[])
{
    strm << "#Freq           Power Spectrum Bin     Pol\n" 
	 << "#MHz                  Number   Number\n"
	 << "#-------------- ----- -------- ------- ---" << endl;

    // print the pulses.
    for (int i=0; i < hdr.train.numberOfPulses; ++i)
    {
	strm.precision(9); 
	strm << setw(15) << pulses[i].rfFreq << " ";

	strm.precision(0); 
	strm << setw(5) << pulses[i].power << " ";

	strm << setw(8) << pulses[i].spectrumNumber << " ";

	strm << setw(7) << pulses[i].binNumber << " ";

	// polarization (first letter only)
	char pol = SseMsg::polarizationToSingleUpperChar(pulses[i].pol);
	strm << setw(3) << pol;

	strm << endl;
    }
}
