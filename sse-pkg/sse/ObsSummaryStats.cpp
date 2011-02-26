/*******************************************************************************

 File:    ObsSummaryStats.cpp
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



#include "ObsSummaryStats.h" 
#include <iostream>
#include <iomanip>
#include "Assert.h"
#include "SseDxMsg.h"

using namespace std;

ObsSummaryStats::ObsSummaryStats()
    :
    passCwCohDetCandidates(0),
    confirmedCwCandidates(0),
    confirmedPulseCandidates(0), 
    allCwCandidates(0),    
    allPulseCandidates(0),
    cwSignals(0),
    pulseSignals(0),      
    testSignals(0),
    zeroDriftSignals(0),
    recentRfiDatabaseMatches(0),
    unknownSignals(0)
{
    for (int i=CLASS_REASON_UNINIT+1; i<SIGNAL_CLASS_REASON_END; ++i)
    {
	classReasonCounts_[i] = 0;
    }
}

ObsSummaryStats::~ObsSummaryStats()
{
}


void ObsSummaryStats::printInExpandedFormat(ostream &strm)
{
    strm 
	<< "Observation Summary Stats" << endl
	<< "-------------------------" << endl

	<< "  CwC Detect Candidates: " << passCwCohDetCandidates << endl  
	<< "  Confirmed CW Candidates:    " << confirmedCwCandidates << endl  
	<< "  Confirmed Pulse Candidates: " << confirmedPulseCandidates << endl  
	<< "  All CW Candidates:          " << allCwCandidates << endl  
	<< "  All Pulse Candidates:       " << allPulseCandidates << endl  
	<< "  CW Signals:            " << cwSignals << endl     
	<< "  Pulse Signals:         " << pulseSignals << endl
	;

        // print the counts for each signal class reason

        for (int i=CLASS_REASON_UNINIT+1; i<SIGNAL_CLASS_REASON_END; ++i)
	{
	    strm << "  " 
		 << SseDxMsg::signalClassReasonToString(
		     static_cast<SignalClassReason>(i)) 
		 << " : "
		 << classReasonCounts_[i] << endl;
	}

}


static void printBriefFormat(ostream &strm, const ObsSummaryStats &stats)
{
  strm 
    << "Observation Summary Signal Stats:\n\n"
    << "Confirm__  Candidate  All________________________________\n"
    << "CW  Pulse  CW  Pulse  CW    Pulse / Test ZDrft RFIdb Unkn \n"
    << "--- -----  --- -----  ----- ----- / ---- ----- ----- -----"
    << endl;

  strm
    << setw(3) << stats.confirmedCwCandidates
    << " "
    << setw(5) << stats.confirmedPulseCandidates

    << "  "
    << setw(3) << stats.allCwCandidates
    << " "
    << setw(5) << stats.allPulseCandidates

    << "  "
    << setw(5) << stats.cwSignals 
    << " "		
    << setw(5) << stats.pulseSignals
    << "   "   // slash	
    << setw(4) << stats.testSignals 
    << " "		
    << setw(5) << stats.zeroDriftSignals
    << " "		
    << setw(5) << stats.recentRfiDatabaseMatches
    << " "		
    << setw(5) << stats.unknownSignals
    << endl;
}

ostream& operator << (ostream &strm, const ObsSummaryStats &stats)
{
    printBriefFormat(strm, stats);

    return strm;
}

ObsSummaryStats& ObsSummaryStats::operator+=(const ObsSummaryStats& rhs)
{
    if (this != &rhs)
    {
	passCwCohDetCandidates += rhs.passCwCohDetCandidates;
        confirmedCwCandidates += rhs.confirmedCwCandidates;           
	confirmedPulseCandidates += rhs.confirmedPulseCandidates;        
	allCwCandidates += rhs.allCwCandidates;                 
	allPulseCandidates += rhs.allPulseCandidates;              
	cwSignals += rhs.cwSignals;                   
	pulseSignals += rhs.pulseSignals;                
	testSignals += rhs.testSignals;                 
	zeroDriftSignals += rhs.zeroDriftSignals;            
	recentRfiDatabaseMatches += rhs.recentRfiDatabaseMatches;    
	unknownSignals += rhs.unknownSignals;              

    }
    return *this;
}


void ObsSummaryStats::incrementClassReasonCount(const SignalClassReason reason)
{
    Assert(reason > CLASS_REASON_UNINIT && reason < SIGNAL_CLASS_REASON_END);

    classReasonCounts_[reason]++;
}

int ObsSummaryStats::getClassReasonCount(const SignalClassReason reason)
{
    Assert(reason > CLASS_REASON_UNINIT && reason < SIGNAL_CLASS_REASON_END);

    return classReasonCounts_[reason];
}