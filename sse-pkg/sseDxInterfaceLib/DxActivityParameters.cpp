/*******************************************************************************

 File:    DxActivityParameters.cpp
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
#include "DxOpsBitset.h"
#include <iomanip>

static const int PrintPrecision(9);  // MilliHz

// DX operations bitmask
static const char *DxOpsMaskNames[] =
{
    "DATA_COLLECTION", 
    "BASELINING",
    "FREQ_INVERSION",
    "PULSE_DETECTION", 
    "POWER_CWD", 
    "COHERENT_CWD",
    "APPLY_BIRDIE_MASK", 
    "APPLY_RCVR_BIRDIE_MASK", 
    "APPLY_PERMANENT_RFI_MASK", 
    "APPLY_RECENT_RFI_MASK",
    "APPLY_TEST_SIGNAL_MASK",
    "APPLY_DOPPLER", 
    "REJECT_ZERO_DRIFT_SIGNALS",
    "CANDIDATE_SELECTION", 
    "PROCESS_SECONDARY_CANDIDATES",
    "FOLLOW_UP_CANDIDATES",
    "SEND_RAW_SIGNAL_DETECTION_PRODUCTS", 
};

void SseDxMsg::printDxOpsMaskNames(
    ostream &strm, 
    const DxOpsBitset &dxOpsBitset)
{
    if (dxOpsBitset.any())
    {
	for (unsigned int i=0; i<dxOpsBitset.size(); ++i)
	{
	    if (dxOpsBitset.test(i))
	    {
		AssertMsg(i < ARRAY_LENGTH(DxOpsMaskNames), 
			  "Invalid Dx Ops Mask bit is set");
		strm << "    "  
		     << DxOpsMaskNames[i] << endl;
	    }
	}
    }
} 

static void printOperationsMaskNames(ostream &strm, uint32_t opsMask)
{
    // print out the names of the operations set in mask 
    DxOpsBitset dxOpsBitset(opsMask);
    SseDxMsg::printDxOpsMaskNames(strm, dxOpsBitset);
}

ostream& operator << (ostream &strm, const DxActivityParameters &ap)
{
    strm.precision(PrintPrecision);  // show N places after the decimal
    strm.setf(std::ios::fixed);  // show all decimal places up to precision

    strm << "Dx Activity Parameters:" << endl;
    strm << "-------------------" << endl;

    strm << "  activityId:                " 
	 << ap.activityId << endl;
    strm << "  dataCollectionLength:      "
	 << ap.dataCollectionLength << " Secs" << endl;
    strm << "  rcvrSkyFreq:           " 
	 << ap.rcvrSkyFreq << " MHz" << endl;
    strm << "  ifcSkyFreq:           " 
	 << ap.ifcSkyFreq << " MHz" << endl;
    strm << "  dxSkyFreq:           "
	 << ap.dxSkyFreq << " MHz" << endl;
    strm << "  channelNumber:         "
	 << ap.channelNumber << endl;

    // print scienceDataRequest at the end

    //operations
    strm << hex; 
    strm << "  operations:                0x" << ap.operations << endl;
    strm << dec;  // restore format

    printOperationsMaskNames(strm, ap.operations);

    strm << "  sensitivityRatio:          " 
	 << ap.sensitivityRatio << endl;

    strm << "  maxNumberOfCandidates:     " 
	 << ap.maxNumberOfCandidates << endl;

    strm << "  clusteringFreqTolerance:   "
	 << ap.clusteringFreqTolerance << " Hz" << endl; 

    strm << "  zeroDriftTolerance:        "
	 << ap.zeroDriftTolerance << " Hz/Sec" << endl;

    strm << "  maxDriftRateTolerance:        "
	 << ap.maxDriftRateTolerance << " Hz/Sec" << endl;

    // CW
    strm << "  badBandCwPathLimit:        "
	 << ap.badBandCwPathLimit << endl;

    strm << "  cwClusteringDeltaFreq:     "
	 << ap.cwClusteringDeltaFreq << " bins" << endl;

    strm << "  daddResolution:            "
	 <<  SseDxMsg::resolutionToString(ap.daddResolution) << endl;

    strm << "  daddThreshold:             "  
	 << ap.daddThreshold << " Sigma" << endl;  

    strm << "  cwCoherentThresh:          "
	 << ap.cwCoherentThreshold << " Sigma" << endl;

    strm << "  secondaryCwCoherentThresh:          "
	 << ap.secondaryCwCoherentThreshold << " Sigma" << endl;

    strm << "  secondaryPfaMargin:          "
	 << ap.secondaryPfaMargin << " Sigma" << endl;

    strm << "  limitsForCoherentDetect:   "
	 << ap.limitsForCoherentDetection << " Sigma" << endl;
    

    // Pulse
    strm << "  badBandPulseTripletLimit:  "; 
    strm << ap.badBandPulseTripletLimit << endl;

    strm << "  badBandPulseLimit:        "
	 << ap.badBandPulseLimit << endl;

    strm << "  pulseClusteringDeltaFreq:  ";
    strm << ap.pulseClusteringDeltaFreq << " bins" << endl;

    strm << "  pulseTrainSignifThresh:    ";
    strm << ap.pulseTrainSignifThresh << endl;

    strm << "  secondaryPulseTrainSignifThresh:    ";
    strm << ap.secondaryPulseTrainSignifThresh << endl;

    strm << "  maxPulsesPerHalfFrame:        "
	 << ap.maxPulsesPerHalfFrame << endl;

    strm << "  maxPulsesPerSubchannelPerHalfFrame:        "
	 << ap.maxPulsesPerSubchannelPerHalfFrame << endl;

    strm << "  Pulse parameters:" << endl;
    for (int i=0; i< MAX_RESOLUTIONS; ++i)
    {	
	if (ap.requestPulseResolution[i])
	{
	    strm << "  __pd[" << i << "]:__";
	    strm << "(" << static_cast<Resolution>(i) << ")" << endl;
	    strm <<  ap.pd[i];
	}
    }

    strm << "  baselineSubchannelAverage:      "
	 << ap.baselineSubchannelAverage << endl;

    strm << "  baselineInitAccumHalfFrames: " <<
	ap.baselineInitAccumHalfFrames << endl;

    strm << "  baselineDecay: " << 
	ap.baselineDecay << endl;

    strm << "  baselineWarningLimits:" << endl
	 << ap.baselineWarningLimits;
    strm << "  baselineErrorLimits:" << endl
	 << ap.baselineErrorLimits;

    strm << endl;
    strm << ap.scienceDataRequest;

    return strm;

}

DxActivityParameters::DxActivityParameters()
    :
    activityId(0),
    dataCollectionLength(0),
    rcvrSkyFreq(0),
    ifcSkyFreq(0),
    dxSkyFreq(0), 
    channelNumber(0),
    operations(0),
    sensitivityRatio(0),
    maxNumberOfCandidates(0),
    clusteringFreqTolerance(0),
    zeroDriftTolerance(0),
    maxDriftRateTolerance(0),
    alignPad1(0),

    // Cw
    badBandCwPathLimit(0.0),
    cwClusteringDeltaFreq(0),
    daddResolution(RES_1HZ),
    daddThreshold(0),
    cwCoherentThreshold(0),
    secondaryCwCoherentThreshold(0),
    secondaryPfaMargin(0),
    limitsForCoherentDetection(0),
    
    // Pulse
    badBandPulseTripletLimit(0),
    badBandPulseLimit(0),
    pulseClusteringDeltaFreq(0),
    pulseTrainSignifThresh(0),
    secondaryPulseTrainSignifThresh(0),
    maxPulsesPerHalfFrame(0),
    maxPulsesPerSubchannelPerHalfFrame(0),

    baselineSubchannelAverage(0),
    baselineInitAccumHalfFrames(0),
    baselineDecay(0.0),
    alignPad2(0)
{
    for (int i=0; i<MAX_RESOLUTIONS; ++i)
    {
	requestPulseResolution[i] = SSE_FALSE;
    }

    // use default constructor for
    // PulseParameters
    // DxScienceDataRequest
    // BaselineLimits
}

void DxActivityParameters::marshall()
{
    HTONL(activityId);
    HTONL(dataCollectionLength);
    HTOND(rcvrSkyFreq);
    HTOND(ifcSkyFreq);
    HTOND(dxSkyFreq);
    HTONL(channelNumber);

    HTONL(operations);
    HTONF(sensitivityRatio);
    HTONL(maxNumberOfCandidates);
    HTONF(clusteringFreqTolerance);
    HTONF(zeroDriftTolerance);
    HTONF(maxDriftRateTolerance);

    // CW
    HTOND(badBandCwPathLimit);
    HTONL(cwClusteringDeltaFreq);
    SseDxMsg::marshall(daddResolution); // Resolution
    HTOND(daddThreshold);              
    HTOND(cwCoherentThreshold);
    HTOND(secondaryCwCoherentThreshold);
    HTOND(secondaryPfaMargin);
    HTOND(limitsForCoherentDetection);

    // Pulse
    HTOND(badBandPulseTripletLimit); 
    HTOND(badBandPulseLimit);
    HTONL(pulseClusteringDeltaFreq); 
    HTONF(pulseTrainSignifThresh);
    HTONF(secondaryPulseTrainSignifThresh);
    HTONL(maxPulsesPerHalfFrame);
    HTONL(maxPulsesPerSubchannelPerHalfFrame);

    // marshall pulse parameters
    for (int i=0; i< MAX_RESOLUTIONS; ++i)
    {
	SseMsg::marshall(requestPulseResolution[i]);  // bool_t
	pd[i].marshall();
    }
    
    scienceDataRequest.marshall();

    HTONL(baselineSubchannelAverage);
    HTONL(baselineInitAccumHalfFrames);

    HTONF(baselineDecay);
    baselineWarningLimits.marshall();
    baselineErrorLimits.marshall();
}

void DxActivityParameters::demarshall()
{
    marshall();
}