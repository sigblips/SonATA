/*******************************************************************************

 File:    DetectionStatistics.cpp
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
#include <sstream>

using std::stringstream;

DetectionStatistics::DetectionStatistics()
    :
    totalCandidates(0),
    cwCandidates(0),
    pulseCandidates(0),
    candidatesOverMax(0),
    totalSignals(0),
    cwSignals(0),
    pulseSignals(0),
    leftCwHits(0),        
    rightCwHits(0),        
    leftCwClusters(0),    
    rightCwClusters(0),    
    totalPulses(0),   
    leftPulses(0),    
    rightPulses(0),   
    triplets(0),      
    pulseTrains(0),   
    pulseClusters(0)
{
}

void DetectionStatistics::marshall()
{
    HTONL(totalCandidates),
    HTONL(cwCandidates);
    HTONL(pulseCandidates);
    HTONL(candidatesOverMax);
    HTONL(totalSignals);
    HTONL(cwSignals);
    HTONL(pulseSignals);
    HTONL(leftCwHits);        
    HTONL(rightCwHits);        
    HTONL(leftCwClusters);    
    HTONL(rightCwClusters);    
    HTONL(totalPulses);   
    HTONL(leftPulses);    
    HTONL(rightPulses);   
    HTONL(triplets);      
    HTONL(pulseTrains);   
    HTONL(pulseClusters); 
}

void DetectionStatistics::demarshall()
{
    marshall();
}

ostream& operator << (ostream &strm, const DetectionStatistics &detStats)
{
    strm << "DetectionStatistics" << endl
	 << "-------------------" << endl
	 << "totalCandidates...: " << detStats.totalCandidates << endl
	 << "cwCandidates......: " << detStats.cwCandidates << endl
	 << "pulseCandidates...: " << detStats.pulseCandidates << endl
	 << "candidatesOverMax.: " << detStats.candidatesOverMax << endl

	 << "totalSignals......: " << detStats.totalSignals << endl
	 << "cwSignals.........: " << detStats.cwSignals << endl
	 << "pulseSignals......: " << detStats.pulseSignals << endl

	 << "leftCwHits........: " << detStats.leftCwHits << endl        
	 << "rightCwHits.......: " << detStats.rightCwHits << endl        

	 << "leftCwClusters....: " << detStats.leftCwClusters << endl    
	 << "rightCwClusters...: " << detStats.rightCwClusters << endl    

	 << "totalPulses.......: " << detStats.totalPulses << endl   
	 << "leftPulses........: " << detStats.leftPulses << endl    
	 << "rightPulses.......: " << detStats.rightPulses << endl   

	 << "triplets..........: " << detStats.triplets << endl      
	 << "pulseTrains.......: " << detStats.pulseTrains << endl   
	 << "pulseClusters.....: " << detStats.pulseClusters << endl 
	;

    return strm;
}
