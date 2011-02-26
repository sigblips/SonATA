/*******************************************************************************

 File:    ExpandedSignalReport.h
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


#ifndef ExpandedSignalReport_H
#define ExpandedSignalReport_H

#include <iosfwd>
#include <string>
#include <sstream>
#include <vector>
#include "sseDxInterface.h"
#include "SignalReport.h"

class ExpandedSignalReport : public SignalReport
{
 public:
    ExpandedSignalReport(const string &activityName,
			 int activityId,
			 const string &dxName,
			 const string &reportType,
			 const string &dxTuningInfo);
    virtual ~ExpandedSignalReport();

    // methods to add signals to the report
    void addSignal(const PulseSignalHeader &pulseHdr,
		   Pulse pulses[]);

 private:

    void printPulseTrainDescription(
	ostream &strm, const PulseTrainDescription &ptd);

    void printPulses(
	ostream &strm, const PulseSignalHeader &hdr, Pulse pulses[]);

    // Disable copy construction & assignment.
    // Don't define these.
    ExpandedSignalReport(const ExpandedSignalReport& rhs);
    ExpandedSignalReport& operator=(const ExpandedSignalReport& rhs);

};

#endif // ExpandedSignalReport_H