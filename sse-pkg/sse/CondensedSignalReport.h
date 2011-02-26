/*******************************************************************************

 File:    CondensedSignalReport.h
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


#ifndef CondensedSignalReport_H
#define CondensedSignalReport_H

#include <iosfwd>
#include <string>
#include "sseDxInterface.h"
#include "SignalReport.h"

class CondensedSignalReport : public SignalReport
{
 public:
    CondensedSignalReport(const string &activityName,
			  int activityId,
			  const string &dxName,
			  const string &reportType,
			  const string &dxTuningInfo);
    virtual ~CondensedSignalReport();

    // methods to add signals to the report
    void addSignal(const CwPowerSignal &cwp);
    void addSignal(const CwCoherentSignal &cwc);
    void addSignal(const PulseSignalHeader &pulseHdr);

    void addSignal(const string & sigType,
		   const SignalDescription & descrip,
		   const ConfirmationStats & cfm);
	
 private:
    void printPageHeader(ostream &strm) const;   // override base class

    // Disable copy construction & assignment.
    // Don't define these.
    CondensedSignalReport(const CondensedSignalReport& rhs);
    CondensedSignalReport& operator=(const CondensedSignalReport& rhs);

};

#endif // CondensedSignalReport_H