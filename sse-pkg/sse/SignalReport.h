/*******************************************************************************

 File:    SignalReport.h
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


#ifndef SignalReport_H
#define SignalReport_H

#include <iosfwd>
#include <string>
#include "sseDxInterface.h"
#include <sstream>
#include <vector>

using std::string;
using std::vector;
using std::stringstream;

class SignalReport
{
 public:
    SignalReport(const string &activityName,
		 int activityId,
		 const string &dxName,
		 const string &reportType,
		 const string &dxTuningInfo);
    virtual ~SignalReport() = 0;  // make this an abstract base class

    virtual void addText(const string &text);

    // misc utils
    void setPageSize(size_t pageSize);
    size_t getPageSize();
    int getActivityId() const;

    bool saveToFile(const string &filename);
    friend ostream& operator << (ostream &strm, 
				 const SignalReport &report);

 protected:
    virtual void printPreamble(ostream &strm) const;
    virtual void printCondensedSigDescripHeader(ostream &strm) const;
    virtual void printCondensedSigDescrip(ostream &strm, const string &sigType,
	const SignalDescription &sig);
    virtual void printCondensedConfirmStats(ostream &strm,
				    const ConfirmationStats &stats);
    virtual void printPageHeader(ostream &strm) const;   // subclasses override as desired

    stringstream & getSignalStrm();

 private:

    string activityName_;
    int activityId_;
    string dxName_;
    string reportType_;
    string dxTuningInfo_;
    string creationTime_; 

    size_t pageSize_;
    vector<string> reportText_;
    stringstream signalStrm_;  // stores info for each signal

    // Disable copy construction & assignment.
    // Don't define these.
    SignalReport(const SignalReport& rhs);
    SignalReport& operator=(const SignalReport& rhs);

};

#endif // SignalReport_H