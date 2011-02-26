/*******************************************************************************

 File:    ObsSummaryStats.h
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


#ifndef ObsSummaryStats_H
#define ObsSummaryStats_H

#include <iosfwd>
#include "sseDxInterface.h"

// Observation summary information

struct ObsSummaryStats
{
    // all fields are in Counts

 public:

    // number of dx candidates that pass Cw Coherent Detection
    // (same as total number of candidates minus those that fail CwC detect)
    int passCwCohDetCandidates; 

    int confirmedCwCandidates;
    int confirmedPulseCandidates; 
    int allCwCandidates;    
    int allPulseCandidates;

    int cwSignals;
    int pulseSignals;      
    int testSignals;
    int zeroDriftSignals;
    int recentRfiDatabaseMatches;
    int unknownSignals;

    ObsSummaryStats();
    virtual ~ObsSummaryStats();

    ObsSummaryStats& operator+=(const ObsSummaryStats& rhs);

    void incrementClassReasonCount(const SignalClassReason reason);
    int getClassReasonCount(const SignalClassReason reason);

    void printInExpandedFormat(ostream &strm);

 private:
    // Disable copy construction & assignment.
    // Don't define these.
    // ObsSummaryStats(const ObsSummaryStats& rhs);
    // ObsSummaryStats& operator=(const ObsSummaryStats& rhs);

    int classReasonCounts_[SIGNAL_CLASS_REASON_END];

};

ostream& operator << (ostream &strm, const ObsSummaryStats &stats);

#endif // ObsSummaryStats_H