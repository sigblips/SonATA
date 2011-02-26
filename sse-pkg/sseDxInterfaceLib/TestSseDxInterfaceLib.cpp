/*******************************************************************************

 File:    TestSseDxInterfaceLib.cpp
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


#include "TestRunner.h"
#include "Assert.h"
#include "sseDxInterfaceLib.h"
#include "DxOpsBitset.h"
#include "TestSseDxInterfaceLib.h"
#include <iostream>
#include <cstring>

void TestSseDxInterfaceLib::setUp ()
{
}

void TestSseDxInterfaceLib::tearDown()
{
}

void TestSseDxInterfaceLib::testFoo()
{
    cout << "testFoo" << endl;
    //cu_assert(false);  // force failure  
}

void TestSseDxInterfaceLib::testDxIntrinsics()
{
    DxIntrinsics intrin;
    
    cout << intrin;

    intrin.marshall();
    intrin.demarshall();

}


static void initCwPowerSignal(CwPowerSignal &cwps)
{
    // Signal Description
    cwps.sig.path.rfFreq = 2.8;
    cwps.sig.path.drift = 3.35;
    cwps.sig.path.width = 4.45;
    cwps.sig.path.power = 5.55;

    cwps.sig.subchannelNumber = 512;
    cwps.sig.signalId.number = 5;   // value tbd
    cwps.sig.pol = POL_RIGHTCIRCULAR;
    cwps.sig.sigClass = CLASS_CAND;
    cwps.sig.reason = PASSED_POWER_THRESH;

}

static void initPulseSignal(PulseSignalHeader &ps, 
					     Pulse pulses[], int nPulses)
{
    // Signal Description
    ps.sig.path.rfFreq = 2.2;
    ps.sig.path.drift = 3.3;
    ps.sig.path.width = 4.4;
    ps.sig.path.power = 5.5;

    ps.sig.subchannelNumber = 100;   // value tbd
    ps.sig.signalId.number = 1;   // value tbd

    ps.sig.pol = POL_RIGHTCIRCULAR;
    ps.sig.sigClass = CLASS_CAND;
    ps.sig.reason = PASSED_POWER_THRESH;

    // Confirmation stats
    ps.cfm.pfa = 0.10;            
    ps.cfm.snr = 0.15;                     // signal SNR in a 1Hz channel 

    // PulseTrainDescription
    ps.train.pulsePeriod = 0.15;
    ps.train.numberOfPulses = nPulses;
    ps.train.res = RES_2HZ;

    // Pulse array
    for (int i=0; i < nPulses; ++i)
    {
	pulses[i].rfFreq = 101 + i;
	pulses[i].power = 22 + i;
	pulses[i].spectrumNumber = i;
	pulses[i].binNumber = 14 + i;
	pulses[i].pol = POL_BOTH;
    }


}

static void initCwCoherentSignal(CwCoherentSignal &cwcs)
{
    // Signal Description
    cwcs.sig.path.rfFreq = 22.8;
    cwcs.sig.path.drift = 32.35;
    cwcs.sig.path.width = 42.45;
    cwcs.sig.path.power = 52.55;

    cwcs.sig.subchannelNumber = 55;   // value tbd
    cwcs.sig.signalId.number = 5;   // value tbd
    cwcs.sig.pol = POL_LEFTCIRCULAR;
    cwcs.sig.sigClass = CLASS_CAND;
    cwcs.sig.reason = PASSED_POWER_THRESH;

    // Confirmation stats
    cwcs.cfm.pfa = 50.10;            
    cwcs.cfm.snr = 0.85;                     // signal SNR in a 1Hz channel 

    // segments
    cwcs.nSegments = 2;

}

static void initDxActivityParameters(DxActivityParameters &ap)
{
  ap.activityId = 1234;
  ap.dataCollectionLength = 10; // seconds 
  ap.ifcSkyFreq = 1234.56;
  ap.dxSkyFreq = 41234.000;
  ap.channelNumber = 0;

  ap.scienceDataRequest.sendBaselines = SSE_TRUE;
  ap.scienceDataRequest.baselineReportingHalfFrames = 5;
  ap.scienceDataRequest.sendComplexAmplitudes = SSE_TRUE;
  ap.scienceDataRequest.requestType = REQ_FREQ;
  ap.scienceDataRequest.rfFreq = 2295.123456789;
  ap.scienceDataRequest.subchannel = 0;

  //operations
  // use a bitset to set them
  DxOpsBitset opsMask;
  opsMask.set(DATA_COLLECTION);
  opsMask.set(APPLY_BIRDIE_MASK);
  opsMask.set(REJECT_ZERO_DRIFT_SIGNALS);
  opsMask.set(FOLLOW_UP_CANDIDATES);
  opsMask.set(SEND_RAW_SIGNAL_DETECTION_PRODUCTS);

  //copy the bitset value into the operations word
  ap.operations = static_cast<uint32_t>(opsMask.to_ulong());

  ap.sensitivityRatio = 1.0;

  ap.maxNumberOfCandidates = 7;
  ap.clusteringFreqTolerance = 0.5;
  ap.zeroDriftTolerance = 0.003; 
  ap.maxDriftRateTolerance = 1.000; 

  // CW
  ap.cwClusteringDeltaFreq = 5;
  ap.badBandCwPathLimit = 20; 
  ap.daddResolution = RES_1HZ;
  ap.daddThreshold = 1.0;             
  ap.cwCoherentThreshold =2.0;    
  ap.secondaryCwCoherentThreshold =-12.0;    
  ap.secondaryPfaMargin =4.0;    
  ap.limitsForCoherentDetection = 5.0;

  // Pulse
  ap.pulseClusteringDeltaFreq = 10;
  ap.badBandPulseTripletLimit = 22;
  ap.badBandPulseLimit = 100;
  ap.pulseTrainSignifThresh = 1.0;
  ap.secondaryPulseTrainSignifThresh = 2.0;

  ap.maxPulsesPerHalfFrame = 15;
  ap.maxPulsesPerSubchannelPerHalfFrame = 150;

  for (int i=0; i< MAX_RESOLUTIONS; ++i)
  {
      ap.requestPulseResolution[i] = SSE_FALSE;  
      ap.pd[i].pulseThreshold = 2.0; 
      ap.pd[i].tripletThreshold = 4.0;
      ap.pd[i].singletThreshold = 6.0;
  }
  // enable a few pulse resolutions
  ap.requestPulseResolution[RES_1HZ] = SSE_TRUE;  
  ap.requestPulseResolution[RES_2HZ] = SSE_TRUE;  
  ap.requestPulseResolution[RES_4HZ] = SSE_TRUE;  

  ap.baselineSubchannelAverage = 5;
  ap.baselineInitAccumHalfFrames = 10;
  ap.baselineDecay = 0.9;

}

static void initSseInterfaceHdr(SseInterfaceHeader &hdr,
					unsigned int code)
{
    NssDate date;
    date.tv_sec = 12345;
    date.tv_usec = 500;

    // set message hdr fields
    hdr.code = code;
    hdr.timestamp = date;  
    hdr.dataLength = 100;
    hdr.messageNumber = 12345678; 

    // TBD
    //activityId
    //sender
    //receiver

}


static void initBaseline(Baseline &baseline)
{
    BaselineHeader &hdr = baseline.header;
    
    hdr.rfCenterFreq = 102.5;
    hdr.bandwidth = 33.0;
    hdr.halfFrameNumber = 2;
    hdr.numberOfSubchannels = MAX_BASELINE_SUBCHANNELS;
    hdr.pol = POL_RIGHTCIRCULAR;

    // baseline values
    for (int i=0; i<hdr.numberOfSubchannels; ++i)
    {
	baseline.baselineValues[i] = i;
    }
}

static void initComplexAmplitudes(ComplexAmplitudes &ca)
{
    ComplexAmplitudeHeader &hdr = ca.header;
    
    hdr.rfCenterFreq = 1001.0;
    hdr.halfFrameNumber = 53;
    hdr.hzPerSubchannel = 100.12345;
    hdr.startSubchannelId = 33;
    hdr.numberOfSubchannels = 1;
	//This structure no longer has a res 
    //hdr.res = RES_1HZ;
    hdr.pol = POL_RIGHTCIRCULAR;

    for (int i=0; i < MAX_SUBCHANNEL_BINS_PER_1KHZ_HALF_FRAME; ++i)
    {
	ca.compamp.coef[i].pair = 0xAB;
    }

}

static void initFrequencyMaskHdr(FrequencyMaskHeader &maskHdr)
{
    //maskHdr.maskVersionDate = x;    // use default date
    maskHdr.bandCovered.centerFreq = 1001;
    maskHdr.bandCovered.bandwidth = 10;
    maskHdr.numberOfFreqBands = 1;

}


static void initThereYouAre(ThereYouAre &thereYouAre)
{
    strcpy(thereYouAre.sseIp,"0.1.2.3");
    thereYouAre.portId = 1001;
    strcpy(thereYouAre.interfaceVersionNumber,"Rev 1.1");
}

static void testPrintRecentRfiMask()
{
    cout << "testPrintRecentRfiMask" << endl;

    // print the default values;
    RecentRfiMaskHeader rfiMaskHdr;
    cout << rfiMaskHdr;

    // print header & array

    int numberOfFreqBands = 5;
    double centerFreq = 1420;
    rfiMaskHdr.numberOfFreqBands = numberOfFreqBands;
    rfiMaskHdr.excludedTargetId = 44;
    rfiMaskHdr.bandCovered.centerFreq = centerFreq;  // MHz
    rfiMaskHdr.bandCovered.bandwidth = 20;    // MHz

    cout << rfiMaskHdr;

    double elementBandwidthMHz = 0.001000;

    FrequencyBand freqBandArray[numberOfFreqBands];
    for (int i=0; i<numberOfFreqBands; ++i)
    {
	freqBandArray[i].centerFreq = centerFreq + i;
	freqBandArray[i].bandwidth = elementBandwidthMHz;
    }
    

    SseDxMsg::printRecentRfiMask(cout, rfiMaskHdr, freqBandArray);


}

void TestSseDxInterfaceLib::testPrint()
{
    DxConfiguration dxConfig;
    cout << dxConfig << endl;

    CwPowerSignal cwps;
    initCwPowerSignal(cwps);
    cout << cwps << endl;

    int nPulses = 2;
    PulseSignalHeader pulseHdr;
    Pulse pulses[nPulses];
    initPulseSignal(pulseHdr, pulses, nPulses);
    SseDxMsg::printPulseSignal(cout, pulseHdr, pulses);
    cout << endl;

    CwCoherentSignal cwcs;
    cout << cwcs << endl;

    initCwCoherentSignal(cwcs);
    cout << cwcs << endl;

    DxActivityParameters actParam;
    cout << actParam;
    initDxActivityParameters(actParam);
    cout << actParam;

    cout << endl;
    DxTuned dxTuned;
    cout << dxTuned << endl;

    SseInterfaceHeader hdr;
    unsigned int code = BIRDIE_MASK;
    initSseInterfaceHdr(hdr, code);
    cout << hdr;
    cout << SseDxMsg::messageCodeToString(code) << endl;

    unsigned int BAD_MESSAGE_CODE = 999999;
    initSseInterfaceHdr(hdr, BAD_MESSAGE_CODE);
    cout << "Expected bad message code error:" << endl;
    cout << hdr;
    cout << SseDxMsg::messageCodeToString(BAD_MESSAGE_CODE) << endl;

    BAD_MESSAGE_CODE = 0;
    initSseInterfaceHdr(hdr, BAD_MESSAGE_CODE);
    cout << "Expected bad message code error:" << endl;
    cout << hdr;
    cout << SseDxMsg::messageCodeToString(BAD_MESSAGE_CODE) << endl;


    // loop through all the message codes
    unsigned int firstCodeIndex = REQUEST_INTRINSICS;
    unsigned int lastCodeIndex = DX_MESSAGE_CODE_END;
    for (unsigned int i=firstCodeIndex; i< lastCodeIndex; ++i)
    {
	initSseInterfaceHdr(hdr, i);
	cout << hdr
	     << hdr.code << ": " 
	     << SseDxMsg::messageCodeToString(i) << endl;
    }
    cout << endl;

    DxIntrinsics intrinsics;
    cout << intrinsics;


    // --- deprecated -----
    ComplexAmplitudes compamps;
    initComplexAmplitudes(compamps);
    cout << "Complex Amps:" << endl;
    cout << compamps;
    cout << endl;

    // --- deprecated -----
    Baseline baseline;
    initBaseline(baseline);
    cout << "Baseline: " << endl;
    cout << baseline;
    cout << endl;

    FrequencyBand band;
    band.centerFreq = 2001;
    band.bandwidth = 201.2;
    cout << "freq band: " << endl;
    cout << band;


    FrequencyMaskHeader maskHdr;
    initFrequencyMaskHdr(maskHdr);
    cout << maskHdr;

    Count count;
    cout << count;
    count.count = 32;
    cout << count;

    HereIAm hereIAm;
    strcpy(hereIAm.interfaceVersionNumber, "Rev 1.0");
    cout << hereIAm  << endl;

    ThereYouAre thereYouAre;
    initThereYouAre(thereYouAre);
    cout << thereYouAre << endl;

    SignalDescription signalDescrip;
    cout << signalDescrip;

    cout << endl;


    SignalClass class1(CLASS_CAND);
    cout << "signal class: " << SseDxMsg::signalClassToString(class1) << endl;

    SignalClass class2(CLASS_UNKNOWN);
    cout << "signal class: " << SseDxMsg::signalClassToString(class2) << endl;

    cu_assert(SseDxMsg::signalClassToString(CLASS_CAND) == "Cand");
    cu_assert(SseDxMsg::stringToSignalClass("Cand") == CLASS_CAND);


    const int badSignalClassValue = 999999;
    SignalClass class3((SignalClass) badSignalClassValue);
    cout << "Expected bad signal class:" <<endl;
    cout << "signal class: " << SseDxMsg::signalClassToString(class3) << endl;

    SignalClassReason reason1(TEST_SIGNAL_MATCH);
    cout << "classreason: " 
	 << SseDxMsg::signalClassReasonToString(reason1) << endl;
    cout << "brief classreason: " 
	 << SseDxMsg::signalClassReasonToBriefString(reason1) << endl;
    cu_assert(SseDxMsg::signalClassReasonToString(reason1) == "TEST_SIGNAL_MATCH");
    cu_assert(SseDxMsg::signalClassReasonToBriefString(reason1) == "TestSig");


    SignalClassReason reason2(RFI_SCAN);
    cout << "classreason: " 
	 << SseDxMsg::signalClassReasonToString(reason2) << endl;
    cu_assert(SseDxMsg::signalClassReasonToString(reason2) == "RFI_SCAN");
    cu_assert(SseDxMsg::signalClassReasonToBriefString(reason2) == "RFIScan");

    cu_assert(SseDxMsg::briefStringToSignalClassReason("RFIScan") == RFI_SCAN);


    cout << "brief classreason: " 
	 << SseDxMsg::signalClassReasonToBriefString(reason2) << endl;
    
    SignalClassReason reason3(ZERO_DRIFT);
    cout << "classreason: " 
	 << SseDxMsg::signalClassReasonToString(reason3) << endl;
    cu_assert(SseDxMsg::signalClassReasonToString(reason3) == "ZERO_DRIFT");

    cout << "brief classreason: " 
	 << SseDxMsg::signalClassReasonToBriefString(reason3) << endl;
    cu_assert(SseDxMsg::signalClassReasonToBriefString(reason3) == "ZeroDft");
    cu_assert(SseDxMsg::briefStringToSignalClassReason("ZeroDft") == ZERO_DRIFT);
    
    SignalClassReason reason4(SEEN_GRID_WEST);
    cout << "classreason: " 
	 << SseDxMsg::signalClassReasonToString(reason4) << endl;
    cu_assert(SseDxMsg::signalClassReasonToString(reason4) == "SEEN_GRID_WEST");

    cout << "brief classreason: " 
	 << SseDxMsg::signalClassReasonToBriefString(reason4) << endl;
    cu_assert(SseDxMsg::signalClassReasonToBriefString(reason4) == "SeenWst");
    
    const int anOutOfRangeClassReason = 9999999;
    SignalClassReason reason5((enum SignalClassReason) anOutOfRangeClassReason);
    cout << "Expected out of range class reason: " << endl;
    cout << "classreason: " 
	 << SseDxMsg::signalClassReasonToString(reason5) << endl;
    cout << "brief classreason: " 
	 << SseDxMsg::signalClassReasonToBriefString(reason5) << endl;
    
    
    // resolution
    cu_assert(SseDxMsg::resolutionToString(RES_1HZ) == "1 Hz");
    cu_assert(SseDxMsg::stringToResolution("1 Hz") == RES_1HZ);

    cu_assert(SseDxMsg::resolutionToString(RES_1KHZ) == "1 KHz");
    cu_assert(SseDxMsg::stringToResolution("1 KHz") == RES_1KHZ);


    const int badResolutionValue = 99999;
    Resolution badres((Resolution) badResolutionValue);
    cout << "Expected bad resolution: " << endl;
    cout << "bad res: " << SseDxMsg::resolutionToString(badres);
    cout << endl;


    NssDate nssDate;
    nssDate.tv_sec = 15;
    nssDate.tv_usec = 2;
    NssDate nssDate2;
    nssDate2 = nssDate;

    DxStatus dxstat;
    dxstat.timestamp = nssDate;
    dxstat.numberOfActivities =2;
    dxstat.act[0].activityId = 1234;
    dxstat.act[0].currentState = DX_ACT_NONE;
    dxstat.act[1].activityId = 45678;
    dxstat.act[1].currentState = DX_ACT_ERROR;
    cout << dxstat  << endl;
    cout << endl;

    DxStatus dxstat2;
    dxstat2.timestamp = nssDate;
    dxstat2.numberOfActivities =2;
    dxstat2.act[0].activityId = 4311;
    dxstat2.act[0].currentState = DX_ACT_RUN_SD;
    dxstat2.act[1].activityId = 4322;

    dxstat2.act[1].currentState = DX_ACT_TUNED;
    cout << dxstat2  << endl;
    cout << endl;


    StartActivity startAct;
    startAct.startTime = nssDate;
    cout << startAct;

    SignalId signalId;
    cout << signalId;  // print the defaults
    cout << endl;

    cu_assert(SseDxMsg::stringToSciDataRequestType("freq") == REQ_FREQ);
    cu_assert(SseDxMsg::sciDataRequestTypeToString(REQ_SUBCHANNEL) == "subchan");

    SciDataRequestType badReqType((SciDataRequestType) 9999);
    cout << "Expected bad science data request type: " << endl;
    cout << SseDxMsg::sciDataRequestTypeToString(badReqType) << endl << endl;

    ArchiveRequest archiveRequest;
    cout << archiveRequest << endl;

    ArchiveDataHeader archiveDataHeader;
    cout << archiveDataHeader << endl;

    FollowUpSignal fs;
    cout << endl << fs;

    FollowUpCwSignal fcw;
    cout << endl << fcw;

    FollowUpPulseSignal fp;
    cout << endl << fp;


    testPrintRecentRfiMask();
    cout << endl;

    DetectionStatistics detstats;
    cout << detstats << endl;

    SignalPath path;
    cout << path << endl;

    CwBadBand cwBadBand;
    cwBadBand.maxPath.rfFreq = 1419.41234567;
    cwBadBand.maxPath.drift = 20.301542;
    cout << cwBadBand << endl;

    PulseBadBand pulseBadBand;
    cout << pulseBadBand << endl;


    BaselineLimits baselimits;
    cout << baselimits << endl;

    BaselineStatistics basestats;
    cout << basestats << endl;

    BaselineLimitsExceededDetails limitsDetails;
    cout << limitsDetails << endl;

    CwCoherentSegment segment;
    cout << segment << endl;
    
}


Test *TestSseDxInterfaceLib::suite ()
{
	TestSuite *testSuite = new TestSuite("TestSseDxInterfaceLib");

	testSuite->addTest (new TestCaller <TestSseDxInterfaceLib> ("testFoo", &TestSseDxInterfaceLib::testFoo));

	testSuite->addTest (new TestCaller <TestSseDxInterfaceLib> ("testDxIntrinsics", &TestSseDxInterfaceLib::testDxIntrinsics));

	testSuite->addTest (new TestCaller <TestSseDxInterfaceLib> ("testPrint", &TestSseDxInterfaceLib::testPrint));

	return testSuite;
}