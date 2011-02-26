/*******************************************************************************

 File:    Dx.cpp
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


#include <ace/Timer_Queue.h>
#include <ace/Reactor.h>
#include "Dx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "SseProxy.h"
#include "DxArchiverProxy.h"
#include "SseSock.h"
#include "SseDxMsg.h"
#include "SseMsg.h"
#include "Assert.h"
#include "SseUtil.h"
#include <fstream>
#include <sstream>

//#define DX

using std::string;

using namespace std;

static const double KhzPerMhz = 0.001;

static void loadNssBaselines(Polarization pol, 
			     const string &baselineFile, BaselineList &baselineList);
static void loadNssCompAmps(Polarization pol,
			    const string &compampFile, CompAmpList &compampList,
			    int32_t activityId);
static void initDxActStatus(DxActivityStatus &actStatus);
static void initStateMachine (DxStateMachine &stateMachine);
//static void initActivityParameters(DxActivityParameters &ap);
static void initSimulatedBaseline(Polarization pol, Baseline &baseline);
static void initSimulatedCompAmps(Polarization pol, ComplexAmplitudes &compamps,
		int32_t activityId);

const double HALF_FRAME_LEN_SECS = 0.75;
const int SIGNAL_NOT_FOUND_ID_NUMBER = -1;

Dx::Dx(SseProxy *sseProxy, const string &sciDataDir, 
	 const string &sciDataPrefix, const string &dxName, bool remoteMode,
	 bool varyOutputData)
    : sseProxy_(sseProxy), dxName_(dxName), remoteMode_(remoteMode),
      varyOutputData_(varyOutputData)
{
    setup();

    // get some canned Nss science data from datafiles

    // get left & right pol baselines
    // baselines filenames are of the format:  nss.p6.L.baseline
    string baseLineFile = sciDataDir + "/" + sciDataPrefix;
    loadNssBaselines(POL_LEFTCIRCULAR, baseLineFile + ".L.baseline", cannedBaselinesL_);
    loadNssBaselines(POL_RIGHTCIRCULAR, baseLineFile + ".R.baseline", cannedBaselinesR_);

    // get left & right pol complex amplitudes
    // complex amp filenames are of the format:  nss.p6.L.compamp
    string compampFile = sciDataDir + "/" + sciDataPrefix;
    loadNssCompAmps(POL_LEFTCIRCULAR, compampFile + ".L.compamp", cannedCompAmpsL_,
    		dcAct_->actParam_.activityId);
    loadNssCompAmps(POL_RIGHTCIRCULAR, compampFile + ".R.compamp", cannedCompAmpsR_,
    		dcAct_->actParam_.activityId);

}

// constructor for isolated testing
Dx::Dx(SseProxy *sseProxy) 
    : sseProxy_(sseProxy), 
    remoteMode_(false)
{
    setup();
}


void Dx::setup()
{
    dxArchiverProxy_ = 0; 
    dxArchiverSock_ = 0;

    determineDxNumber();

    sseProxy_->setDx(this);

    // assign activity info (for pipelining)
    dcAct_ = &actInfoA_;  // data collection activity 
    sdAct_ = &actInfoB_;  // signal detection activity

    initDxActStatus(dcAct_->actStatus_);
    initDxActStatus(sdAct_->actStatus_);

    // let activity parameters default
    //initActivityParameters(dcAct_->actParam_);
    //initActivityParameters(sdAct_->actParam_);
    
    initIntrinsics();

    initStateMachine(dcAct_->stateMachine_);
    initStateMachine(sdAct_->stateMachine_);

    // more inits here for all local data
    // TBD

    // init the timer Ids
    startDataCollTimerId_ = -1;
    halfFrameTimerId_ = -1;
    endSigDetTimerId_ = -1;
    waitForAllArchiveRequestsTimerId_ = -1;
}



// Pull the dx number out of the dx name.
// EG, '7' from 'dx7'.  Use the default if
// the name has no trailing number.

void Dx::determineDxNumber() 
{
    dxNumber_ = -1;	

    string::size_type pos = dxName_.find_first_of("0123456789");
    if (pos != string::npos)
    {
	// pull out the number
	dxNumber_ = SseUtil::strToInt(dxName_.substr(pos));
    }

}


void Dx::notifyDxArchiverDisconnected()
{
    cout << "Warning:  Dx archiver has disconnected" << endl;
}

void Dx::notifyDxArchiverConnected()
{
    cout << "=======connected to Dx archiver" << endl;
}

bool Dx::dxArchiverIsAlive()
{
    if (dxArchiverProxy_ != 0 && dxArchiverProxy_->isAlive())
    {
	return true;
    }

    return false;
}


static void initStateMachine (DxStateMachine &stateMachine)
{
    // verify the state machine is properly initialized
    Assert(stateMachine.getCurrentState() == DxStateMachine::STARTUP_STATE);

    // Dx currently doesn't get created until SSE is connected, so
    // set the stateMachine accordingly.  
    stateMachine.changeState(DxStateMachine::CONNECTED_TO_SSE_EVENT);
}

// prepare for a new observation
void Dx::observationSetup()
{
    halfFrameNumber_ = 0;

    baselineIteratorL_ = cannedBaselinesL_.begin();
    baselineIteratorR_ = cannedBaselinesR_.begin();
    compAmpsIteratorL_ = cannedCompAmpsL_.begin();
    compAmpsIteratorR_ = cannedCompAmpsR_.begin();

}

Dx::~Dx()
{
    cout << "Dx destructor" << endl;
}

SseProxy * Dx::getSseProxy()
{
    return (sseProxy_);
}


DxArchiverProxy * Dx::getDxArchiverProxy()
{
    // TBD better error handling.  
    //   define a new  error  subclass?
    //   check that proxy exists and is still connected

    if (dxArchiverProxy_ == 0) {
	throw "no dx data archiver attached";
    }

    if (!dxArchiverProxy_->isAlive())
    {
	throw "no longer connected to data archiver";
    }
    return (dxArchiverProxy_);
}


void Dx::requestIntrinsics(SseProxy *sseProxy)
{
    cout << "dx::requestIntrinsics from sseProxy" << endl;

#if 0

    for (int i=0; i<3; ++i)
    {
    // debug test
    NssMessage dxMessage;
    dxMessage.code = 0;
    dxMessage.severity = SEVERITY_INFO;
    SseUtil::strMaxCpy(dxMessage.description,
		      "test of DxMessage, sent before intrinsics",
		      MAX_NSS_MESSAGE_STRING);
    int activityId = NSS_NO_ACTIVITY_ID;
    sseProxy_->sendDxMessage(dxMessage, activityId);
    }
#endif	


    // send intrinsics info to SSE
    sseProxy_->sendIntrinsics(intrinsics_);   

    printIntrinsics();
}


void Dx::requestIntrinsics(DxArchiverProxy *dxArchiverProxy)
{
    cout << "dx::requestIntrinsics from dxArchiverProxy" << endl;

    // send intrinsics info to dxArchiver
    dxArchiverProxy_->sendIntrinsics(intrinsics_);   

    printIntrinsics();
}


void Dx::requestDxStatus()
{
    // send overall dx status info to SSE

    cout << "dx::requestDxStatus" << endl;

    updateDxStatus();

    sseProxy_->sendDxStatus(dxStatus_);

    printDxStatus();
}

void Dx::updateDxStatus()
{
    // update the overall dx status by
    // copying in the status for each pipelined activity

    dxStatus_.timestamp = SseMsg::currentNssDate();
    dxStatus_.numberOfActivities = 0;
    dxStatus_.alignPad = 0;  // to appease purify

    for (int i=0; i< MAX_DX_ACTIVITIES; i++)
    {
	initDxActStatus(dxStatus_.act[i]);
    }

    if (dcAct_->actStatus_.currentState != DX_ACT_NONE)
    {
	dxStatus_.act[dxStatus_.numberOfActivities] = dcAct_->actStatus_;
	dxStatus_.numberOfActivities++;
    }
    if (sdAct_->actStatus_.currentState != DX_ACT_NONE)
    {
	dxStatus_.act[dxStatus_.numberOfActivities] = sdAct_->actStatus_;
	dxStatus_.numberOfActivities++;
    }

}


void Dx::configureDx(DxConfiguration *config)
{
    dcAct_->stateMachine_.changeState(DxStateMachine::CONFIGURE_DX_RCVD_EVENT);
    sdAct_->stateMachine_.changeState(DxStateMachine::CONFIGURE_DX_RCVD_EVENT);

    // store configuration info received from SSE
    config_ = *config;

    makeConnectionToDxArchiver(config_.archiverHostname, 
				 config_.archiverPort);
}

void Dx::makeConnectionToDxArchiver(const char *archiverHostname,
					int archiverPort)
{
    cout << "makeConnectionToDxArchiver on host " << archiverHostname
	 << " port " << archiverPort << endl;

    // try to establish a socket connection to the dxArchiver
    // TBD 
    //    add error handling, for when the attempt fails
    //    also need to clean up the objects when they're no longer needed.
 
    dxArchiverSock_ = new SseSock(archiverPort, archiverHostname);

    if (dxArchiverSock_->connect_to_server() == 0)
    {
	// connection succeeded, attach a proxy

	dxArchiverProxy_ = new DxArchiverProxy(dxArchiverSock_->sockstream());
	dxArchiverProxy_->setDx(this);

	//Register the socket input event handler for reading
	ACE_Reactor::instance()->register_handler(dxArchiverProxy_,
					      ACE_Event_Handler::READ_MASK);

	// tell the proxy that it's connected
	dxArchiverProxy_->setInputConnected();
	
    } else {
	cerr << "Warning: Failed to connect to dxArchiver" << endl;
    }
}


void Dx::sendErrorMsgToSse(const string &text)
{
    sendMsgToSse(SEVERITY_ERROR, text);
}

void Dx::sendMsgToSse(NssMessageSeverity severity, const string &text)
{
    cout << "sendMsgToSse: text is '" << text << "'" <<endl;

    NssMessage dxMessage;
    dxMessage.code = 0;   // TBD add arg to set code
    dxMessage.severity = severity;

    // Don't overrun the destination buffer.
    // need cast to prevent compiler warning about 
    // comparison between signed and unsigned
    
    Assert (static_cast<signed int>(text.length())
	    < MAX_NSS_MESSAGE_STRING); 

    SseUtil::strMaxCpy(dxMessage.description, text.c_str(),
		       MAX_NSS_MESSAGE_STRING); 

    int activityId = NSS_NO_ACTIVITY_ID;
    sseProxy_->sendDxMessage(dxMessage, activityId);
}

static void printFreqBandArray(FrequencyBand freqBandArray[], int arrayLen)
{
    int maxToPrint = 5;
    for (int i=0; i<arrayLen && i < maxToPrint; ++i)
    {
	cout << freqBandArray[i];
    }
    cout << "..." << endl;

}

void Dx::setPermRfiMask(FrequencyMaskHeader *maskHdr,
				FrequencyBand freqBandArray[])
{
    // action TBD
    cout << "Dx::setPermRfiMask:" << endl;

    cout << *maskHdr;

    printFreqBandArray(freqBandArray, maskHdr->numberOfFreqBands);

}

void Dx::setBirdieMask(FrequencyMaskHeader *maskHdr,
				FrequencyBand freqBandArray[])
{
    cout << "Dx::setBirdieMask:" << endl;
    cout << *maskHdr;

    printFreqBandArray(freqBandArray, maskHdr->numberOfFreqBands);
}

void Dx::setRcvrBirdieMask(FrequencyMaskHeader *maskHdr,
				FrequencyBand freqBandArray[])
{
    cout << "Dx::setRcvrBirdieMask:" << endl;
    cout << *maskHdr;

    printFreqBandArray(freqBandArray, maskHdr->numberOfFreqBands);
}

void Dx::setTestSignalMask(FrequencyMaskHeader *maskHdr,
				FrequencyBand freqBandArray[])
{
    cout << "Dx::setTestSignalMask:" << endl;
    cout << *maskHdr;

    printFreqBandArray(freqBandArray, maskHdr->numberOfFreqBands);
}



void Dx::setRecentRfiMask(RecentRfiMaskHeader *maskHdr,
				FrequencyBand freqBandArray[])
{
    // action TBD
    cout << "Dx::setRecentRfiMask:" << endl;

    cout << *maskHdr;

    printFreqBandArray(freqBandArray, maskHdr->numberOfFreqBands);

}



void Dx::setDxActivityParameters(DxActivityParameters *actParam)
{
    if (dcAct_->stateMachine_.changeState(DxStateMachine::ACTIVITY_PARAMETERS_RCVD_EVENT))
    {
	// store activity parameters
	dcAct_->actParam_ = *actParam;

	// grab the dx ops mask bits
	dcAct_->opsMask_.reset();   // clear the mask
	dcAct_->opsMask_ |= dcAct_->actParam_.operations;

	// set the status
	dcAct_->actStatus_.activityId = dcAct_->actParam_.activityId;
	dcAct_->actStatus_.currentState = DX_ACT_INIT;

	// verify activity parameters.  send back error if anything wrong
	// TBD

	// send dxTuned message to SSE
	// tweak the values slightly so we can see the difference

	DxTuned dxTuned;
	double dxTuneAdjustMHz = 0.001000;   // 1 KHz
	double secsPerFrame = 1.5;
	dxTuned.dxSkyFreq =
	    dcAct_->actParam_.dxSkyFreq + dxTuneAdjustMHz;
	dxTuned.dataCollectionLength =
	    dcAct_->actParam_.dataCollectionLength + 1;
	dxTuned.dataCollectionFrames =
	    static_cast<int>(dxTuned.dataCollectionLength / secsPerFrame);

#if 0
	if (dxName_ == "dxsim1001")
	{
	   sleep(200);
	}
#endif
	dcAct_->followUpCwOrigSigIds_.clear();
        dcAct_->followUpPulseOrigSigIds_.clear();
        dcAct_->cwCoherentCandSigDescripList_.clear();

	sseProxy_->dxTuned(dxTuned, dcAct_->actParam_.activityId);
    } 
    else
    {
	cerr << "error: DxActivityParameters message not valid at this time" << endl;

	// send back dx error indicating message
	// received out of order
	// TBD
    }
}

void Dx::cancelTimer(const string &timerName, int &timerId)
{
    if (timerId != -1)
    {
	if (ACE_Reactor::instance()->cancel_timer(timerId))
	{
	   cout << "cancelling " << timerName << " timer "
		<< timerId << endl;	
	}
	else 
	{
	   cerr << "cancelTimer: error cancelling '"
		<< timerName << "' timer " << timerId << endl;
	}
	// reset the id
	timerId = -1;
    }

}

void Dx::stopDxActivity(int activityId)
{
    cout <<"Dx::stopDxActivity actid:" << activityId << endl;

    bool actionTaken = false;

    // wrap up the running activities
    if (dcAct_->actStatus_.currentState != DX_ACT_NONE &&
	(dcAct_->actStatus_.activityId == activityId ||
	activityId == NSS_NO_ACTIVITY_ID ))
    {
	// cancel any pending timers
	// tbd error check: startDataCollTimerId_ and halfFrameTimerId_
	// shouldn't be running at the same time
	    
	cancelTimer("start data coll", startDataCollTimerId_);
	cancelTimer("half frame", halfFrameTimerId_);

	activityWrapup(dcAct_);

	actionTaken = true;
    }

    if (sdAct_->actStatus_.currentState != DX_ACT_NONE &&
	(sdAct_->actStatus_.activityId == activityId ||
	activityId == NSS_NO_ACTIVITY_ID ))
    {
	// cancel any pending timers
	cancelTimer("end signal detect", endSigDetTimerId_);
	cancelTimer("all archive reqs", waitForAllArchiveRequestsTimerId_);

	activityWrapup(sdAct_);

	actionTaken = true;

    }


    if (! actionTaken)
    {
	cout << "stopDxActivity: activity Id not found"
	     << " or no activities running" << endl;
    }


}

void Dx::shutdown()
{
    cout <<"Dx::shutdown" << endl;

    ACE_Reactor::end_event_loop();

}

void Dx::restart()
{
    cout <<"Dx::restart" << endl;

    // TBD: restart the program using
    // the original command line args

    // just shutdown for now
    ACE_Reactor::end_event_loop();
}


void Dx::dxScienceDataRequest(DxScienceDataRequest *dataRequest)
{
    // store data request
    dcAct_->actParam_.scienceDataRequest = *dataRequest;
}


void Dx::sendComplexAmplitudes(CompAmpList &compAmpsList, 
				CompAmpList::iterator &compAmpsIterator,
				int activityId)
{
    // Send the next complex amplitudes in the canned list
    // as pointed to by the iterator.
    // If the list is empty, just return.
    // if we're at the end of the list, reset to the beginning.
    // Note that the iterator is modified.

    if (compAmpsList.size() > 0)
    {
	if (compAmpsIterator == compAmpsList.end())
	{
	    compAmpsIterator = compAmpsList.begin();    
	}

	ComplexAmplitudes &ca = *compAmpsIterator;
	ca.header.halfFrameNumber = halfFrameNumber_;

	// send compamps as separate header & subchannel 'array' (of length 1)
        sseProxy_->sendComplexAmplitudes(ca.header, &ca.compamp, activityId);

	// debug test -- send a dummy variable length array
	if (false)
	{
	    int nSubchannels = 3;
	    ComplexAmplitudeHeader hdr;
	    hdr = ca.header;
	    hdr.numberOfSubchannels = nSubchannels;
	    SubchannelCoef1KHz subchannels[nSubchannels];
	    for (int i = 0; i<nSubchannels; ++i)
	    {
		for (int j=0; j<MAX_SUBCHANNEL_BINS_PER_1KHZ_HALF_FRAME; j++)
		{
		    subchannels[i].coef[j].pair = i;
		}
	    }
	    sseProxy_->sendComplexAmplitudes(hdr, subchannels, activityId);
	}

	++compAmpsIterator;

    }



    
}

void Dx::sendBaseline(BaselineList &baselineList, 
		       BaselineList::iterator &baselineIterator,
		       int activityId)
{
    // Send the next baseline in the list
    // as pointed to by the iterator.
    // If the list is empty, just return.
    // if we're at the end of the list, reset to the beginning.
    // Note that the iterator is modified.

    if (baselineList.size() > 0)
    {
	if (baselineIterator == baselineList.end())
	{
	    baselineIterator = baselineList.begin();    
	}
	Baseline &baseline = *baselineIterator;
	baseline.header.halfFrameNumber = halfFrameNumber_;

	// Send baseline as separate header & baselineValue array.
	// Note the explicit cast from 
	// float32_t baselineValues[]
	// to
	// struct BaselineValue {
	//   float32_t value;
	// }

	BaselineValue *valuesArray = 
	    reinterpret_cast<BaselineValue *>(baseline.baselineValues); 

        sseProxy_->sendBaseline(baseline.header, valuesArray, activityId);


	// debug test -- send an explicit variable length baseline
	if (false)
	{
	    BaselineHeader hdr = baseline.header;
	    int nValues = 3;
	    hdr.numberOfSubchannels = nValues;
	    BaselineValue values[nValues];
	    for (int i=0; i<nValues; ++i)
	    {
		values[i].value = i;
	    }

	    sseProxy_->sendBaseline(hdr, values, activityId);
	}


	++baselineIterator;

    }
}


void Dx::sendBaselineStatistics(Polarization pol, int activityId)
{
    if (dcAct_->actParam_.scienceDataRequest.sendBaselineStatistics)
    {

	BaselineStatistics stats;
	
	stats.mean = 1.0;
	stats.stdDev = 0.5;
	stats.range = 100;
	stats.halfFrameNumber = halfFrameNumber_;
	stats.rfCenterFreqMhz = 1000;
	stats.bandwidthMhz = 10.0;
	stats.pol = pol;
	stats.status = BASELINE_STATUS_GOOD;
	
	sseProxy_->sendBaselineStatistics(stats, activityId);

#if 0

	// test baseline limits warning & error messages

	BaselineLimitsExceededDetails details;
	details.pol = pol;

	if (dcAct_->actParam_.scienceDataRequest.checkBaselineWarningLimits)
	{
	    SseUtil::strMaxCpy(
		details.description, 
		"Test of baseline stats Warning Limits exceeded", 
		MAX_NSS_MESSAGE_STRING);

	    sseProxy_->baselineWarningLimitsExceeded(details, activityId);
	}

	if (dcAct_->actParam_.scienceDataRequest.checkBaselineErrorLimits)
	{
	    SseUtil::strMaxCpy(
		details.description, 
		"Test of baseline stats Error Limits exceeded", 
		MAX_NSS_MESSAGE_STRING);
	    
	    sseProxy_->baselineErrorLimitsExceeded(details, activityId);
	}
#endif


    }

}

void Dx::setStartTime(StartActivity *startAct)
{
    // start data collection if the ops bit is set
    if (dcAct_->opsMask_.test(DATA_COLLECTION))
    {
	if (dcAct_->stateMachine_.changeState(DxStateMachine::START_TIME_RCVD_EVENT))
	{
	    // set the start time for an observation
	    cout << "setStartTime" << endl;
	    cout << *startAct;

	    scheduleObsStart(&startAct->startTime);
	    dcAct_->startTime_ = startAct->startTime;

	    // TBD do something with startAct.dopplerParameters;

	    // set the status
	    dcAct_->actStatus_.currentState = DX_ACT_PEND_DC;

	}
	else
	{
	    // send back dx error message
	    // TBD
	}
    }
}


void Dx::requestArchiveData(ArchiveRequest *archiveRequest)
{
    cout << "request archive data\n" << *archiveRequest;

    // debug test -- send fake archive complex amplitudes
    // to the data archiver.
    // TBD send multiple subchannels per message.

    int activityId = sdAct_->actParam_.activityId;

    CompAmpList &complist = cannedCompAmpsL_;
    //CompAmpList &complist = cannedCompAmpsR_;

    if (complist.size() > 0)
    {

	    
	// this will fail if there's no connection to the dx
	// data archiver 
	
	try 
	{
	    
	    ArchiveDataHeader archiveDataHeader;
	    archiveDataHeader.signalId = archiveRequest->signalId;

	    // send the signal id info
	    getDxArchiverProxy()->archiveSignal(archiveDataHeader, activityId);

	    int nHalfFrames = complist.size();
	    
	    // tell archiver how much data is coming.
	    // send the same data in left & right pols
	    Count count;
	    count.count = nHalfFrames * 2;  // for 2 pols
	    getDxArchiverProxy()->beginSendingArchiveComplexAmplitudes(
		count, activityId);
	    
	    CompAmpList::iterator it;
	    int halfFrame = 0;
	    for (it = complist.begin(); it != complist.end(); ++it)
	    {
		
		ComplexAmplitudes &ca = *it;
	    
		// copy the header
		ComplexAmplitudeHeader compAmpHeader;
		compAmpHeader = ca.header;
		
		// copy the data into the data array for the subchannel(s)
		const int nSubchannel = 16;
		compAmpHeader.numberOfSubchannels = nSubchannel;
		SubchannelCoef1KHz subchannelArray[nSubchannel];
		
		// zero fill all the subchannels
		for (int subchannel=0; subchannel <nSubchannel; subchannel++)
		{
		    for (int j=0; j<MAX_SUBCHANNEL_BINS_PER_1KHZ_HALF_FRAME; j++)
		    {
			subchannelArray[subchannel].coef[j].pair = 0;
		    } 
		}
		
		// put the data into the "center" (nth/2) subchannel
		int dataSubchannel = (nSubchannel / 2);
		for (int j=0; j<MAX_SUBCHANNEL_BINS_PER_1KHZ_HALF_FRAME; j++)
		{
		    subchannelArray[dataSubchannel].coef[j] = ca.compamp.coef[j];
		} 
		
		compAmpHeader.halfFrameNumber = halfFrame++;
		
		compAmpHeader.pol = POL_LEFTCIRCULAR;
		getDxArchiverProxy()->sendArchiveComplexAmplitudes(
		    compAmpHeader,
		    subchannelArray, activityId);
		

		compAmpHeader.pol = POL_RIGHTCIRCULAR;
		
		getDxArchiverProxy()->sendArchiveComplexAmplitudes(
		    compAmpHeader,
		    subchannelArray, activityId);
		
	    }
	    
	    getDxArchiverProxy()->doneSendingArchiveComplexAmplitudes(
		activityId);
	    
	}
	
	catch (...)
	{
	    
	    stringstream strm;
	    
	    strm << "Warning: failed to send data to dx data archiver"
		 << " for archive request: " << endl
		 << *archiveRequest;
	    
	    cerr << strm.str();
	    
	    // more error handling tbd
	    // send error to sse proxy
	    
	    sendMsgToSse(SEVERITY_WARNING, strm.str());
	    
	}
    }
}





void Dx::discardArchiveData(ArchiveRequest *archiveRequest)
{
    cout << "discard archive data\n" << *archiveRequest;

    // Error if sse tries to send request for signalId of
    // a 'not found' signal

    if (archiveRequest->signalId.number == SIGNAL_NOT_FOUND_ID_NUMBER)
    {
       stringstream strm;

       strm << "signal not a candidate:[discardArchive]: signal Id "
            << SIGNAL_NOT_FOUND_ID_NUMBER;

       sendMsgToSse(SEVERITY_ERROR, strm.str());
    }
}


void Dx::signalDetectionWrapup()
{

   int activityId = sdAct_->actParam_.activityId;
   sseProxy_->archiveComplete(activityId);


   sseProxy_->signalDetectionComplete(activityId);
   sdAct_->stateMachine_.changeState(DxStateMachine::SIGNAL_DETECTION_COMPLETE_EVENT);

   // print current time
   cout << "signal detection complete time: " << SseUtil::currentIsoDateTime() << endl;	

   activityWrapup(sdAct_);

   // see if there is a data collection that is pending for signal detection
   if (dcAct_->stateMachine_.getCurrentState() == DxStateMachine::ACTIVITY_PENDING_SD_STATE)
   {
       cout << "signalDetectionWrapup: starting the pending signal detection" << endl;

       // swap the ActivityInfo variables
       swap(dcAct_, sdAct_); 
       
       startSignalDetection();       
   }

}

// incoming followUp candidate messages from sse (to main dx only)
void Dx::beginSendingFollowUpSignals(const Count *count)
{
    cerr << "received: beginSendingFollowUpSignals: count= " << (*count).count << endl;
};

void Dx::sendFollowUpCwSignal(const FollowUpCwSignal *followUpCwSignal)
{
    cerr << "received: sendFollowUpCwSignal:\n " << *followUpCwSignal << endl;
    
    dcAct_->followUpCwOrigSigIds_.push_back(followUpCwSignal->sig.origSignalId);

}

void Dx::sendFollowUpPulseSignal(const FollowUpPulseSignal *followUpPulseSignal)
{
    cerr << "received:sendFollowUpPulseSignal:\n " << *followUpPulseSignal << endl;

    dcAct_->followUpPulseOrigSigIds_.push_back(followUpPulseSignal->sig.origSignalId);
};

void Dx::doneSendingFollowUpSignals()
{
    cerr << "received: doneSendingFollowUpSignals" << endl;
};





// incoming candidate signals for secondary processing
void Dx::beginSendingCandidates(const Count *count)
{
    cerr << "received: beginSendingCandidates" << endl;
};

void Dx::sendCandidateCwPowerSignal(const CwPowerSignal *cwPowerSignal
)
{
    const CwPowerSignal & signal = *cwPowerSignal;

    cerr << "received: sendCandidateCwPowerSignal\n "
	 << signal << endl;

    secondaryCwCandSigIds_.push_back(signal.sig.signalId);

};

void Dx::sendCandidatePulseSignal(const PulseSignalHeader *hdr, Pulse pulses[])
{
    const PulseSignalHeader & signalHdr = *hdr;

    cerr << "received:sendCandidatePulseSignal\n";
    SseDxMsg::printPulseSignal(cerr, signalHdr, pulses);

    secondaryPulseCandSigIds_.push_back(signalHdr.sig.signalId);

};

void Dx::doneSendingCandidates()
{
    cerr << "received: doneSendingCandidates" << endl;
};

void Dx::beginSendingCwCoherentSignals(Count *count)
{
    cerr << "received: beginSendingCwCoherentSignals" << endl;
}

void Dx::sendCwCoherentSignal(CwCoherentSignal *cwCoherentSignal)
{
    cerr << "received: sendCwCoherentSignal\n"
	 << *cwCoherentSignal << endl;
}

void Dx::doneSendingCwCoherentSignals()
{
    cerr << "received: doneSendingCwCoherentSignals" << endl;

    // If we're here, then we're acting as a remote or multibeam dx

#if 0
    Assert(sdAct_->actParam_.mode == DX_REMOTE
	   || sdAct_->actParam_.mode == DX_MULTIBEAM);

    if (sdAct_->actParam_.mode == DX_REMOTE)
    {
	sendCandidatesAndSignals(sdAct_->actParam_.activityId);
    }
#endif
    
    sendCandidateResults(sdAct_->actParam_.activityId);

    startTimerToWaitForAllArchiveRequests();

}

void Dx::sendCandidateResults(int activityId)
{
    // Send a candidate result for each entry in 
    // the pulse & cw SignalIdLists.
    // The signals will have a new signal Id from 
    // this dx, and the original signal id set from
    // the data in the list.
    // Mark some as detected and others as not.

    int nPulseSignals = secondaryPulseCandSigIds_.size();
    int nCwSignals = secondaryCwCandSigIds_.size();
    Count count;
    count.count = nPulseSignals + nCwSignals;

    sseProxy_->beginSendingCandidateResults(count, activityId);

    // give the signals a unique number sequence from this dx
    int signalNumber = 200;

    CwCoherentSignal cwCoherentSignal;
    initCwCoherentSignal(cwCoherentSignal);

    // send cw candidate results
    SignalIdList::iterator cwIt;
    for (cwIt = secondaryCwCandSigIds_.begin(); 
	 cwIt != secondaryCwCandSigIds_.end(); ++cwIt)
    {
	cwCoherentSignal.sig.signalId.number = signalNumber++;
	cwCoherentSignal.sig.origSignalId = (*cwIt);

	// alternate found & not found signals, based
	// on the original dx number
	if (cwCoherentSignal.sig.origSignalId.dxNumber % 2 == 0)
	{   
	    cwCoherentSignal.sig.sigClass = CLASS_CAND;
	    cwCoherentSignal.sig.reason = PASSED_COHERENT_DETECT;
	}
	else
	{
	    cwCoherentSignal.sig.sigClass = CLASS_RFI;
	    cwCoherentSignal.sig.reason = FAILED_COHERENT_DETECT;
	}

	sseProxy_->sendCwCoherentCandidateResult(cwCoherentSignal, activityId);
    }


    // send pulse candidate results
    PulseSignalHeader pulseSignalHeader;
    int nPulses = 3;   // number of pulses in the pulse train
    initPulseSignalHdr(pulseSignalHeader, nPulses);

    // define the pulses in the train
    Pulse pulses[nPulses];
    double dxTuneFreq = sdAct_->actParam_.dxSkyFreq;
    for (int i=0; i < nPulses; ++i)
    {
	pulses[i].rfFreq = dxTuneFreq + 0.024680000;
	pulses[i].power = 10.5;
	pulses[i].spectrumNumber = i;
	pulses[i].binNumber = i+1;
	pulses[i].pol = POL_RIGHTCIRCULAR;
    }

    SignalIdList::iterator pulseIt;
    for (pulseIt = secondaryPulseCandSigIds_.begin(); 
	 pulseIt != secondaryPulseCandSigIds_.end(); ++pulseIt)
    {
	pulseSignalHeader.sig.signalId.number = signalNumber++;
	pulseSignalHeader.sig.origSignalId = (*pulseIt);
	pulseSignalHeader.sig.reason = PASSED_POWER_THRESH;
	if (signalNumber % 2 == 0)
	{
	    pulseSignalHeader.sig.sigClass = CLASS_CAND;
	    pulseSignalHeader.sig.reason = PASSED_POWER_THRESH;
	} 
	else
	{
	    pulseSignalHeader.sig.sigClass = CLASS_RFI;
	    pulseSignalHeader.sig.reason = FAILED_POWER_THRESH;
	}

	sseProxy_->sendPulseCandidateResult(pulseSignalHeader, pulses, activityId);

    }

    sseProxy_->doneSendingCandidateResults(activityId);

#if 0
    // TBD:
    int nPulseSignals = 2;
    int nCwSignals = 2;
    Count count;
    count.count = nPulseSignals + nCwSignals;

    sseProxy_->beginSendingCandidateResults(count, activityId);

    // send some dummy data
    sendPulseCandidateResults(nPulseSignals, activityId);

    CwCoherentSignal cwCoherentSignal;
    initCwCoherentSignal(cwCoherentSignal);

/* 
// obsolete code
    if (sdAct_->actParam_.mode == DX_REMOTE)
    {
	// add a doppler offset to the remote frequency to 
	// more closely simulate the real situation.
	double dopplerOffsetMHz = 0.010000;
	cwCoherentSignal.sig.path.rfFreq += dopplerOffsetMHz;
    }
*/

    // Send a confirmed signal (to help test selective archiving)
    cwCoherentSignal.sig.signalId.number = 200; 
    cwCoherentSignal.sig.reason = PASSED_COHERENT_DETECT;

    // temp testing
    //cout << "slamming CwCoherentCandidateResult dx id to main's number for testing (dxsim 1001) " << endl;
    //cwCoherentSignal.sig.signalId.dxNumber = 1001;

    sseProxy_->sendCwCoherentCandidateResult(cwCoherentSignal, activityId);


    // send a nonconfirmed (RFI) signal
    cwCoherentSignal.sig.signalId.number = 201; 
    cwCoherentSignal.sig.sigClass = CLASS_RFI;
    cwCoherentSignal.sig.reason = RECENT_RFI_MATCH;
    sseProxy_->sendCwCoherentCandidateResult(cwCoherentSignal, activityId);

    sseProxy_->doneSendingCandidateResults(activityId);
#endif

}



//---- private  utilities -------------



// Timer that goes off periodically, waiting for all the
// archive requests to come in.

class WaitForAllArchiveRequestsTimerHandler : public ACE_Event_Handler 
{ 
public: 

    //Method which is called back by the Reactor when timeout occurs. 
    virtual int handle_timeout (const ACE_Time_Value &tv, 
                                const void *arg)
    { 
//       ACE_DEBUG ((LM_DEBUG, 
//		   "wait for all archive requests time: %d %d\n", tv.sec(), tv.usec())); 

       // dx passed in as arg
       Dx * dx = const_cast<Dx *>(static_cast<const Dx *>(arg));

       // tbd change this to check that all requests have been received.
       // For now assume they all have.

       bool allRequestsAreIn = true;  

       // see if we're done
       if (allRequestsAreIn)
       {

	   cout << "WaitForAllArchiveRequestsTimerHandler: calling "
		<< "signalDetectionWrapup() " << endl;

	   // reset timer id for this timer
	   dx->waitForAllArchiveRequestsTimerId_ = -1;

	   dx->signalDetectionWrapup();

	   //ACE_DEBUG ((LM_DEBUG, "end  time: %d\n", tv.sec())); 

	   return -1;  // unregister


       }
       else  // go around another time
       {
	   return 0;
       }
    } 


    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
    {
	delete this;

	return 0;
    }

protected:
    ~WaitForAllArchiveRequestsTimerHandler() {} // force allocation on the heap

}; 



void Dx::startTimerToWaitForAllArchiveRequests()
{
    // Start a timer that goes off periodically,
    // checking to make sure all the expected archive requests 
    // have come in.


    // Initial delay in seconds.
    // For now wait long enough so that the initial
    // interval gives enough time for all the archive requests &
    // responses to be exchanged the first time the timer
    // goes off.

    int startDelaySecs = 5;       
    double repeatIntervalSecs = 1;
    double repeatInterval_uSecs = repeatIntervalSecs * 
	ACE_U_ONE_SECOND_IN_USECS;  
    
    WaitForAllArchiveRequestsTimerHandler *th = 
	new WaitForAllArchiveRequestsTimerHandler; 
    
    waitForAllArchiveRequestsTimerId_ = ACE_Reactor::instance()->
	schedule_timer (th, 
			this, // timer arg
			ACE_Time_Value (startDelaySecs), // start time
			ACE_Time_Value (0, static_cast<long>(repeatInterval_uSecs)) );
    
}


// timer that goes off at the end of signal detection
class EndSigDetTimerHandler : public ACE_Event_Handler 
{ 
public: 

    //Method which is called back by the Reactor when timeout occurs. 
    virtual int handle_timeout (const ACE_Time_Value &tv, 
                                const void *arg)
    {
       //ACE_DEBUG ((LM_DEBUG, "start time: %d\n", tv.sec())); 

       // dx passed in as arg
       Dx *dx = const_cast<Dx *>(static_cast<const Dx *>(arg));
       
       // reset timer id for this timer
       dx->endSigDetTimerId_ = -1;
       
       int activityId = dx->sdAct_->actParam_.activityId;
       
       dx->sendCandidatesAndSignals(activityId);

       // If doing primary candidate detection, then the only thing left to do
       // is wait for archive requests to wrap up signal detection. 
       // Secondary processing must wait for incoming
       // doneSendingCwCoherentSignal message to arrive before they
       // can complete their work.
       
       if (! dx->sdAct_->opsMask_.test(PROCESS_SECONDARY_CANDIDATES))
       {
	   dx->startTimerToWaitForAllArchiveRequests();
       }

       // Note that the actual 'signalDetectionComplete' message shouldn't
       // get sent to the SSE until after all the archive
       // message requests are received.

       return -1;  // unregister with reactor

    }

    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
    {
	delete this;

	return 0;
    }

protected:
    ~EndSigDetTimerHandler() {}  // force allocation on the heap

};



void Dx::startSignalDetection()
{
    // print current time
    cout << "signal detection start time: " << SseUtil::currentIsoDateTime() << endl;	
    // clear the secondary signal ids
    secondaryCwCandSigIds_.clear();
    secondaryPulseCandSigIds_.clear();

    sseProxy_->signalDetectionStarted(sdAct_->actParam_.activityId);
    sdAct_->stateMachine_.changeState(DxStateMachine::SIGNAL_DETECTION_STARTED_EVENT);

    // set the status
    sdAct_->actStatus_.currentState = DX_ACT_RUN_SD;

    // make signal detection length proportional to
    // data collection length

    const double sigDetPercentOfDataColl = 0.25;

    int signalDetLenSecs =  static_cast<int>(sdAct_->actParam_.dataCollectionLength * sigDetPercentOfDataColl);

    // DEBUG: test sd longer than dc;
    //signalDetLenSecs+=10;

    //set timer to go off at designated signal detection end time
    EndSigDetTimerHandler *th=new EndSigDetTimerHandler; 
    endSigDetTimerId_ = 
       ACE_Reactor::instance()->schedule_timer (th, 
       static_cast<const void *>(this), // time handler arg
              ACE_Time_Value (signalDetLenSecs));

       // tbd error check here

}

void Dx::activityWrapup(DxActivityInfo *act)
{
    act->actStatus_.currentState = DX_ACT_COMPLETE;

    //if (dxName_ != "dxsim1001")
    {
	getSseProxy()->activityComplete(act->actStatus_, act->actParam_.activityId);
    }

    // set the status
    initDxActStatus(act->actStatus_);

    act->stateMachine_.setState(DxStateMachine::READY_FOR_ACTIVITY_STATE);

}

void Dx::dataCollectionWrapup()
{

    cout << "DC Complete" << endl;

#if 0
    // debug - timeout returning data coll complete
    //if (dxName_ == "dxsim1001")
    {
       sleep(200);
    }
#endif

    // send msg to SSE
    getSseProxy()->dataCollectionComplete(dcAct_->actParam_.activityId);
    dcAct_->stateMachine_.changeState(DxStateMachine::DATA_COLLECTION_COMPLETE_EVENT);
    dcAct_->actStatus_.currentState = DX_ACT_PEND_SD;

    // start signal detection if requested
    if ( dcAct_->opsMask_.test(PULSE_DETECTION) ||
	 dcAct_->opsMask_.test(POWER_CWD) ||
	 dcAct_->opsMask_.test(COHERENT_CWD))
    {

	// see if it's ok to transition from data collection to signal detection
	// If signal detection is busy, the data collection activity will have 
	// to stay pending until the SD is free later.

	if (sdAct_->stateMachine_.getCurrentState() == DxStateMachine::READY_FOR_ACTIVITY_STATE)
	{
	    cout << "dataCollectionWrapup: starting the pending signal detection" << endl;

            // swap the ActivityInfo variables
	    swap(dcAct_, sdAct_); 

	    startSignalDetection();
	}
    }
    else
    {
	activityWrapup(dcAct_);
    }
}


// Timer that goes off for every halfFrame, for the duration
// of the data collection, that sends science data products.
class HalfFrameTimerHandler : public ACE_Event_Handler 
{ 
public: 

    //Method which is called back by the Reactor when timeout occurs. 
    virtual int handle_timeout (const ACE_Time_Value &tv, 
                                const void *arg)
    { 
//       ACE_DEBUG ((LM_DEBUG, "half frame time: %d %d\n", tv.sec(), tv.usec())); 

       // dx passed in as arg
       Dx *dx = const_cast<Dx *>(static_cast<const Dx *>(arg));

       // send complex amplitudes
       if (dx->dcAct_->actParam_.scienceDataRequest.sendComplexAmplitudes == SSE_TRUE)
       {
	   // left pol
	   dx->sendComplexAmplitudes(dx->cannedCompAmpsL_,
				      dx->compAmpsIteratorL_,
				      dx->dcAct_->actStatus_.activityId);

	   // right pol
	   dx->sendComplexAmplitudes(dx->cannedCompAmpsR_,
				      dx->compAmpsIteratorR_,
				      dx->dcAct_->actStatus_.activityId);
       }

       // for every Kth halfFrame, send a baseline
       int nHalfFramesPerBaseline = 
	   dx->dcAct_->actParam_.scienceDataRequest.baselineReportingHalfFrames;

       if (dx->halfFrameNumber_ % nHalfFramesPerBaseline == 0)
       {
	   if (dx->dcAct_->actParam_.scienceDataRequest.sendBaselines == SSE_TRUE)
	   {
	       // left Pol
	       dx->sendBaseline(dx->cannedBaselinesL_,
				 dx->baselineIteratorL_,
				 dx->dcAct_->actStatus_.activityId);
	       

	       dx->sendBaselineStatistics(POL_LEFTCIRCULAR,
					   dx->dcAct_->actStatus_.activityId);

	       // right Pol
	       dx->sendBaseline(dx->cannedBaselinesR_,
				 dx->baselineIteratorR_,
				 dx->dcAct_->actStatus_.activityId);

	       dx->sendBaselineStatistics(POL_RIGHTCIRCULAR,
					   dx->dcAct_->actStatus_.activityId);

	   }
       }
       dx->halfFrameNumber_ += 1;


       // see if this is the last 1/2 frame, indicating we're done
       if (dx->halfFrameNumber_ >= dx->totalHalfFrames_)
       {

	   // reset timer id for this timer
	   dx->halfFrameTimerId_ = -1;

	   dx->dataCollectionWrapup();

	   //ACE_DEBUG ((LM_DEBUG, "end  time: %d\n", tv.sec())); 

	   return -1;  // unregister

	   // need to delete "this" to cleanup mem space??

       }
       else  // go around another time
       {
	   return 0;
       }
    } 


    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
    {
	delete this;

	return 0;
    }

protected:
    ~HalfFrameTimerHandler() {} // force allocation on the heap

}; 


// timer that goes off to begin data collection 
class StartDataCollTimerHandler : public ACE_Event_Handler 
{ 
public: 

    //Method which is called back by the Reactor when timeout occurs. 
    virtual int handle_timeout (const ACE_Time_Value &tv, 
                                const void *arg)
    {
       //ACE_DEBUG ((LM_DEBUG, "start time: %d\n", tv.sec())); 

       // dx passed in as arg
       Dx *dx = const_cast<Dx *>(static_cast<const Dx *>(arg));

       cout << "DC Started" << endl;

       // reset timer id for this timer
       dx->startDataCollTimerId_ = -1;

       dx->observationSetup();

       // print current time
       cout << "start time: " << SseUtil::currentIsoDateTime() << endl;	
       // send msg to SSE via its proxy


       dx->getSseProxy()->dataCollectionStarted(dx->dcAct_->actParam_.activityId);

       // update the state
       dx->dcAct_->stateMachine_.changeState(DxStateMachine::DATA_COLLECTION_STARTED_EVENT);

       // set the status
       dx->dcAct_->actStatus_.currentState = DX_ACT_RUN_DC;

       cerr << "sendBaselines flag: ";
       cerr << dx->dcAct_->actParam_.scienceDataRequest.sendBaselines << endl;

       cerr << "sendCompamps flag: " ;
       cerr << dx->dcAct_->actParam_.scienceDataRequest.sendComplexAmplitudes << endl;

       // compute how many half frames in this observation
       // so we know when to stop data collection

       // get data collection length out of activity params
       double dataCollectLengthSecs = dx->dcAct_->actParam_.dataCollectionLength;
       dx->totalHalfFrames_ = int((dataCollectLengthSecs / HALF_FRAME_LEN_SECS) + 1);

       // Start a timer that goes off every halfFrame
       // for the duration of the observation.
       int startDelaySecs = 0;       // initial delay in seconds
       double repeatIntervalSecs = HALF_FRAME_LEN_SECS; 
       double repeatInterval_uSecs = repeatIntervalSecs * 
	   ACE_U_ONE_SECOND_IN_USECS;  

       HalfFrameTimerHandler *th=new HalfFrameTimerHandler; 
       dx->halfFrameTimerId_ =
       ACE_Reactor::instance()->schedule_timer (th, 
           dx, // timer arg
	   ACE_Time_Value (startDelaySecs), // start time
           ACE_Time_Value (0, static_cast<long>(repeatInterval_uSecs)) 
	   );

      // tbd error check on schedule_timer

       return -1;  // unregister

    } 

    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
    {
	delete this;

	return 0;
    }

protected:
    ~StartDataCollTimerHandler() {} // force allocation on the heap


}; 


void Dx::baselineInitAccum()
{
    // TBD more accurately replicate the baseline accum period
    // with a timer, update the state-machine, etc.
    // but just send the messages for now.
    
    getSseProxy()->baselineInitAccumStarted(dcAct_->actParam_.activityId);
    sleep(1);
    getSseProxy()->baselineInitAccumComplete(dcAct_->actParam_.activityId);
    sleep(1);

}


// schedule an observation to begin at the absolute
// time given in the arg
void Dx::scheduleObsStart(NssDate *startTime)
{

    time_t dxStartTime = static_cast<time_t>(startTime->tv_sec);

    cout << "scheduled start time: ";
    cout << SseUtil::isoDateTime(dxStartTime) << endl;

    // get delta from current time
    int obsStartOffset = int(difftime(dxStartTime, time(NULL)));
    //cout << "reconstructed obsStartOffset:" << obsStartOffset << endl;

    //add error check: if offset is negative (start time already passed)




    //set timer to go off at designated start time
    StartDataCollTimerHandler *th=new StartDataCollTimerHandler; 
    startDataCollTimerId_ = 
       ACE_Reactor::instance()->schedule_timer (th, 
       static_cast<const void *>(this), // time handler arg
              ACE_Time_Value (obsStartOffset));


    // tbd add check for failure to set timer

    baselineInitAccum();

    cout << "waiting for scheduled start time: " 
	 << SseUtil::isoDateTime(dxStartTime) << " ..." << endl;
}

void Dx::printConfiguration()
{
    cout << config_;
}

void Dx::printActivityParameters()
{
    cout << "Data Collection Activity: " << endl;
    cout << dcAct_->actParam_;

    cout << "Signal Detection Activity: " << endl;
    cout << sdAct_->actParam_;
}

void Dx::printIntrinsics()
{
    cout << intrinsics_;
}

void Dx::printDxStatus()
{
    updateDxStatus();

    cout << dxStatus_ << endl;

    if (dxArchiverIsAlive())
    {
	cout << "archiver: is attached" << endl;
    }
    else
    {
	cout << "archiver: NOT attached" << endl;
    }
}

void Dx::printScienceDataRequest()
{
    cout << "DC data request:" << endl;
    cout << dcAct_->actParam_.scienceDataRequest;

    cout << "SD data request:" << endl;
    cout << sdAct_->actParam_.scienceDataRequest;
}

// initialize the intrinsics information
void Dx::initIntrinsics() 
{
    // use fill_n to zero out all the char arrays 
    // (to appease the memory checking tools)

    fill_n(intrinsics_.interfaceVersionNumber, MAX_TEXT_STRING, '\0');
    SseUtil::strMaxCpy(intrinsics_.interfaceVersionNumber,
		       SSE_DX_INTERFACE_VERSION, MAX_TEXT_STRING);

    // debug
#if 0
    SseUtil::strMaxCpy(intrinsics_.interfaceVersionNumber,
		       "bad interface version test", MAX_TEXT_STRING);
#endif

#ifdef DX
    intrinsics_.maxSubchannels = 3072; // dx bandwidth
    intrinsics_.hzPerSubchannel = 678.168;
#else
    //DX
    intrinsics_.channelBase = DxBaseAddr();
    intrinsics_.foldings = 10;
    intrinsics_.oversampling = 0.25;
    intrinsics_.filterName[0] = '\0';
    intrinsics_.maxSubchannels = 768; 
    intrinsics_.hzPerSubchannel = 533.3333;
#endif

/*
    // identify as main/remote for debugging
    if (remoteMode_)
	dxName += "remote";
    else 
	dxName += "main";
*/

    fill_n(intrinsics_.name, MAX_TEXT_STRING, '\0');
    fill_n(intrinsics_.host, MAX_TEXT_STRING, '\0'); 
    SseUtil::strMaxCpy(intrinsics_.name, dxName_.c_str(),
		       MAX_TEXT_STRING);
    SseUtil::strMaxCpy(intrinsics_.host, dxName_.c_str(),
		       MAX_TEXT_STRING);

    // use the dx number for the serial number
    intrinsics_.serialNumber = dxNumber_;


    fill_n(intrinsics_.codeVersion, MAX_TEXT_STRING, '\0'); 
    SseUtil::strMaxCpy(intrinsics_.codeVersion, "testrev 0.0 (sse-pkg dxsim)",
		       MAX_TEXT_STRING);

    // use default date for masks

}

static void initDxActStatus(DxActivityStatus &actStatus)
{
    actStatus.activityId = NSS_NO_ACTIVITY_ID;
    actStatus.currentState = DX_ACT_NONE;  
}


#if 0

// initialize the DxActivityParameters
static void initActivityParameters(DxActivityParameters &ap)
{
  ap.activityId = NSS_NO_ACTIVITY_ID;
  ap.dataCollectionLength = 5; // seconds
  ap.ifcSkyFreq = 2295;
  ap.dxSkyFreq = 2295;

  ap.assignedBandwidth= 1;
  
  // operations
  ap.operations = 0;

  ap.sensitivityRatio = 10.0;
  ap.maxNumberOfCandidates = 8;
  ap.clusteringFreqTolerance = 1000.0;
  ap.zeroDriftTolerance = 0.003; 

  // CW
  ap.cwClusteringDeltaFreq = 2;
  ap.badBandCwPathLimit = 100; 
  ap.daddResolution = RES_1HZ;
  ap.daddThreshold = 2.0;             
  ap.cwCoherentThreshold = -20.0;    
  ap.secondaryCwCoherentThreshold = -25.0;    
  ap.limitsForCoherentDetection = 0.0;

  // Pulse
  ap.pulseClusteringDeltaFreq = 25;
  ap.badBandPulseTripletLimit = 1200;
  ap.badBandPulseLimit = 200;
  ap.pulseTrainSignifThresh = -40.0;
  ap.secondaryPulseTrainSignifThresh = -50.0;

  for (int i=0; i< MAX_RESOLUTIONS; ++i)
  {
      ap.requestPulseResolution[i] = SSE_FALSE;  
      ap.pd[i].pulseThreshold = 12.0; 
      ap.pd[i].tripletThreshold = 48.0;
      ap.pd[i].singletThreshold = 0.0;
  }
  // enable a few pulse resolutions
  ap.requestPulseResolution[RES_1HZ] = SSE_TRUE;  
  ap.requestPulseResolution[RES_2HZ] = SSE_TRUE;  
  ap.requestPulseResolution[RES_4HZ] = SSE_TRUE;  


  //setDxScienceDataRequest
  ap.scienceDataRequest.sendBaselines = SSE_TRUE;  
  ap.scienceDataRequest.baselineReportingHalfFrames = 20;
  ap.scienceDataRequest.sendComplexAmplitudes = SSE_TRUE;
  ap.scienceDataRequest.requestType = REQ_FREQ;
  ap.scienceDataRequest.subchannel = 0;
  ap.scienceDataRequest.rfFreq = 0;

  ap.baselineSubchannelAverage = 1; 
  ap.baselineInitAccumHalfFrames = 1;


}
#endif 

// simulate signal detection by returning signal reports, etc.
void Dx::sendCandidatesAndSignals(int activityId)
{
    if (sdAct_->opsMask_.test(CANDIDATE_SELECTION))
    {
	sendCwPowerAndPulseCandidates(activityId);
    }

    sendAllCwPowerAndPulseSignals(activityId);

    sendBadBands(activityId);

    sendCoherentCwSignals(activityId);
    
}

void Dx::sendCwPowerAndPulseCandidates(int activityId)
{
    cerr << "sendCwPowerAndPulseCandidates()" << endl;

    int nPulseCandidates = 3;
    int nCwCandidates = 5;

    if (sdAct_->opsMask_.test(FOLLOW_UP_CANDIDATES))
    {
       nPulseCandidates = sdAct_->followUpPulseOrigSigIds_.size();
       nCwCandidates = sdAct_->followUpCwOrigSigIds_.size();
    }

    const int nTotalCandidates = nPulseCandidates + nCwCandidates;
    Count count;
    count.count = nTotalCandidates;

    sseProxy_->beginSendingCandidates(count, activityId);

    sendCandidatePulseSignals(nPulseCandidates, activityId);

    SignalIdList::iterator origSigIdIt(
       sdAct_->followUpCwOrigSigIds_.begin());

    // put the signal numbers in sequence
    int signalNumber = nPulseCandidates;
    for (int i=0; i<nCwCandidates; ++i)
    {
	// test followups
	CwPowerSignal cwPowerSignal;
	initCwPowerSignal(cwPowerSignal);

	cwPowerSignal.sig.signalId.number=signalNumber++;

        /* offset the freqs a bit */
        cwPowerSignal.sig.path.rfFreq += (i * KhzPerMhz);

        // set orig signal id
        if (sdAct_->opsMask_.test(FOLLOW_UP_CANDIDATES))
        {
           // put orig signal ids in each candidate 
           if (origSigIdIt != sdAct_->followUpCwOrigSigIds_.end())
           {
              cwPowerSignal.sig.origSignalId = *origSigIdIt;
              ++origSigIdIt;
           }
        }

        /*
          Return at least one candidate as found, and one not found,
          if the list is long enough.
        */
	if (nCwCandidates > 0 && i==0 && sdAct_->opsMask_.test(FOLLOW_UP_CANDIDATES))
	{
	    cwPowerSignal.sig.sigClass = CLASS_RFI;
	    cwPowerSignal.sig.reason = NO_SIGNAL_FOUND;
            cwPowerSignal.sig.signalId.number = SIGNAL_NOT_FOUND_ID_NUMBER;
	}
        else
        {
           // remember which cw coherent reports need to be sent later
           sdAct_->cwCoherentCandSigDescripList_.push_back(cwPowerSignal.sig);
        }

	sseProxy_->sendCandidateCwPowerSignal(cwPowerSignal, activityId);
    }

    sseProxy_->doneSendingCandidates(activityId);

}


void Dx::sendAllCwPowerAndPulseSignals(int activityId)
{
    cerr << "sendAllCwPowerAndPulseSignals" << endl;

    const int nPulseSignals = 4;
    const int nCwSignals = 5 + 2;
    const int nTotalSignals = nPulseSignals + nCwSignals;

    DetectionStatistics stats;
    initDetectionStats(stats);
    stats.totalSignals = nTotalSignals;
    // other fields TBD

    sseProxy_->beginSendingSignals(stats, activityId);

    sendPulseSignals(nPulseSignals, activityId);

    CwPowerSignal cwPowerSignal;
    initCwPowerSignal(cwPowerSignal);
    for (int i=0; i<nCwSignals -2; ++i)
    {
	sseProxy_->sendCwPowerSignal(cwPowerSignal, activityId);
    }
    cwPowerSignal.sig.path.drift = 0.0;
    cwPowerSignal.sig.sigClass = CLASS_RFI;
    cwPowerSignal.sig.reason = ZERO_DRIFT;
    sseProxy_->sendCwPowerSignal(cwPowerSignal, activityId);
    cwPowerSignal.sig.path.drift = 0.1;
    cwPowerSignal.sig.sigClass = CLASS_RFI;
    cwPowerSignal.sig.reason = RECENT_RFI_MATCH;
    sseProxy_->sendCwPowerSignal(cwPowerSignal, activityId);

    sseProxy_->doneSendingSignals(activityId);
}



void Dx::sendBadBands(int activityId)
{
    cerr << "sendBadBands" << endl;

    const int nPulseBadBands = 2;
    const int nCwBadBands = 3;
    const int nTotalBadBands = nPulseBadBands + nCwBadBands;

    Count count;
    count.count = nTotalBadBands;

    sseProxy_->beginSendingBadBands(count, activityId);

    PulseBadBand pulseBadBand;
    pulseBadBand.band.bandwidth = 0.5;
    pulseBadBand.pol = POL_LEFTCIRCULAR;
    pulseBadBand.res = RES_2HZ;
    for (int i=0; i<nPulseBadBands; ++i)
    {
        pulseBadBand.band.centerFreq = i + 1000.123456;
	sseProxy_->sendPulseBadBand(pulseBadBand, activityId);
    }

    CwBadBand cwBadBand;
    cwBadBand.band.bandwidth = 0.7;  // MHz
    for (int i=0; i<nCwBadBands; ++i)
    {
	cwBadBand.band.centerFreq = i + 2000.123456;  // MHz
	cwBadBand.maxPath.rfFreq = i + 2005.1234567; // MHz
	cwBadBand.maxPath.drift = 0.2; // Hz/s
	sseProxy_->sendCwBadBand(cwBadBand, activityId);
    }

    sseProxy_->doneSendingBadBands(activityId);


}

void Dx::sendCoherentCwSignals(int activityId)
{
    cerr << "sendCoherentCwSignals()" << endl;
    Count cwCount;

    /* 
       Always send the begin/done messages, but
       only send signals when mode is enabled.
    */

    cwCount.count = 0;   
    if (sdAct_->opsMask_.test(COHERENT_CWD))
    {
       cwCount.count = sdAct_->cwCoherentCandSigDescripList_.size();
    }

    sseProxy_->beginSendingCwCoherentSignals(cwCount, activityId);

    if (sdAct_->opsMask_.test(COHERENT_CWD))
    {
       for (int i=0; i < cwCount.count; ++i)
       {
          CwCoherentSignal cwSig;

          initCwCoherentSignal(cwSig);

          // copy cw power signal Id information so they match up
          cwSig.sig.path =
             sdAct_->cwCoherentCandSigDescripList_[i].path;

          cwSig.sig.pol =
             sdAct_->cwCoherentCandSigDescripList_[i].pol;

          cwSig.sig.subchannelNumber =
             sdAct_->cwCoherentCandSigDescripList_[i].subchannelNumber;

          cwSig.sig.signalId =
             sdAct_->cwCoherentCandSigDescripList_[i].signalId;

          cwSig.sig.origSignalId = 
             sdAct_->cwCoherentCandSigDescripList_[i].origSignalId;
          
          if (i == (cwCount.count - 1))
          {
             // Send the last signal with its pfa set such
             // that it's below the confirmation threshhold, and thus
             // tagged as RFI
             
             cwSig.cfm.pfa = sdAct_->actParam_.cwCoherentThreshold - 1;
             cwSig.sig.sigClass = CLASS_RFI;
             cwSig.sig.reason = FAILED_COHERENT_DETECT;
             ++i;
          }

          sseProxy_->sendCwCoherentSignal(cwSig, activityId);
       }
    }
#if 0
     // debug - force sse activity to timeout waiting for this message
    //if (dxName_ == "dxsim1001")
    {
       sleep(200);
    }
#endif

    sseProxy_->doneSendingCwCoherentSignals(activityId);


}


void Dx::initSignalId(SignalId &signalId, int signalNumber)
{
    signalId.dxNumber = dxNumber_;
    signalId.activityId = sdAct_->actParam_.activityId;
    signalId.activityStartTime = sdAct_->startTime_;
    signalId.number = signalNumber;
}

void Dx::initDetectionStats(DetectionStatistics &stats)
{
    stats.totalCandidates = 100;
    stats.totalCandidates = 101;
    stats.cwCandidates = 102;     
    stats.pulseCandidates = 103;
    stats.candidatesOverMax = 104;
    stats.totalSignals = 105;
    stats.cwSignals = 106;
    stats.pulseSignals = 107;
    stats.leftCwHits = 108;
    stats.rightCwHits = 109;
    stats.leftCwClusters = 110;
    stats.rightCwClusters = 111;
    stats.totalPulses = 112;   
    stats.leftPulses = 113;    
    stats.rightPulses = 114;   
    stats.triplets = 115;      
    stats.pulseTrains = 116;   
    stats.pulseClusters = 117; 
    

}

void Dx::initPulseSignalHdr(PulseSignalHeader &ps, int nPulses)
{

    // put the simulated signal rfFreq in the dx band
    double dxTuneFreq = sdAct_->actParam_.dxSkyFreq;

    // Signal Description
    ps.sig.path.rfFreq = dxTuneFreq + 0.021456789;
    ps.sig.subchannelNumber = 1;   // value tbd
    ps.sig.containsBadBands = SSE_TRUE;
    initSignalId(ps.sig.signalId, 1);   // value tbd
    
    ps.sig.path.drift = 0.3;
 
    // vary the output data from dx to dx
    if (varyOutputData_)
    {
	// TBD fix me
	ps.sig.path.drift += intrinsics_.serialNumber;
	//cout << "warning: varyOutputData currently disabled" << endl;
    }

    ps.sig.path.width = 4.4;
    ps.sig.path.power = 5.5;
    ps.sig.pol = POL_RIGHTCIRCULAR;
    ps.sig.sigClass = CLASS_CAND;
    ps.sig.reason = PASSED_POWER_THRESH;

    // Confirmation stats
    ps.cfm.pfa = 0.10;            

    // set PFA to -Nan for sse error handling testing
    //ps.cfm.pfa = sqrt(-1.0) * -1;

    ps.cfm.snr = 0.15;                     // signal SNR in a 1Hz channel 

    // PulseTrainDescription
    ps.train.pulsePeriod = 0.25;
    ps.train.numberOfPulses = nPulses;
    ps.train.res = RES_2HZ;
}

void Dx::initCwPowerSignal(CwPowerSignal &cwps)
{
    // put the simulated signal rfFreq in the dx band
    double dxTuneFreq = sdAct_->actParam_.dxSkyFreq;

    // Signal Description
    cwps.sig.path.rfFreq = dxTuneFreq - 0.011222333;
    cwps.sig.subchannelNumber = 51;  
    initSignalId(cwps.sig.signalId, 5);   // value tbd
    cwps.sig.path.drift = 3.351234;
    cwps.sig.path.width = 4.45;
    cwps.sig.path.power = 5.55;
    cwps.sig.pol = POL_RIGHTCIRCULAR;
    cwps.sig.sigClass = CLASS_CAND;
    cwps.sig.reason = PASSED_POWER_THRESH;

}

void Dx::initCwCoherentSignal(CwCoherentSignal &cws)
{
    // put the simulated signal rfFreq in the dx band
    double dxTuneFreq = sdAct_->actParam_.dxSkyFreq;

    // Signal Description
    cws.sig.path.rfFreq = dxTuneFreq + 0.0423456789;
    cws.sig.subchannelNumber = 11; 
    initSignalId(cws.sig.signalId, 1);  // value tbd
    cws.sig.path.drift = 3.12345;
    cws.sig.path.width = 4.4;
    cws.sig.path.power = 35.5;
    cws.sig.pol = POL_RIGHTCIRCULAR;
    cws.sig.sigClass = CLASS_CAND;
    cws.sig.reason = PASSED_COHERENT_DETECT;


    // Confirmation stats
    cws.cfm.pfa = 0.50;            
    cws.cfm.snr = 0.35;                     // signal SNR in a 1Hz channel 

    // segments
    cws.nSegments = 2;
    for (int i=0; i<cws.nSegments; ++i)
    {
	// put in some uniq, dummy values
	cws.segment[i].path.rfFreq = dxTuneFreq + 0.000000123 * i;
	cws.segment[i].path.drift = -0.5 * i;
	cws.segment[i].path.width = i+2;
	cws.segment[i].path.power = i+4;
	cws.segment[i].pfa=-i;
	cws.segment[i].snr=-i*5;

    }

}


// Load canned NSS format baseslines from the given file
// into the BaselineList.
static void loadNssBaselines(Polarization pol, const string &baselineFile, 
			     BaselineList &baselineList)
{

    cerr << "trying to read " << baselineFile << endl;

    // try to open file for reading 
    ifstream fin;
    fin.open(baselineFile.c_str(), ios::in | ios::binary);
    if (!fin.is_open())
    {
        cerr << "Can't open file: " << baselineFile << endl;

	cerr << "Using simulated (generated) baseline instead" << endl;

	// create a simulated baseline instead;
	Baseline baseline;
	initSimulatedBaseline(pol, baseline);
	baselineList.push_back(baseline);
	    
    }
    else
    {
	// verify that file size is a multiple of struct size
	int filesize = SseUtil::getFileSize(baselineFile);

	//AssertMsg(filesize % sizeof(Baseline) == 0, 
	//      "Invalid Baseline filesize");

	if (filesize % sizeof(Baseline) != 0) 
	{
	    cerr <<  "Error: Invalid Baselines filesize" << endl;
	}
	else
	{
	    // read in all the baselines
	    Baseline baseline;
	    while (fin.read((char*)&baseline, sizeof baseline))
	    {
		baseline.demarshall();

		// put in the list, by value
		baselineList.push_back(baseline);
	    }
	}
    }


    cerr << "loadNssBaselines: " << baselineList.size() << " loaded" << endl;
}


// Load canned NSS format complex amplitudes from the given file
// into the compampsList.
static void loadNssCompAmps(Polarization pol, const string &compampFile, 
			    CompAmpList &compampList, int32_t activityId)
{

    cerr << "Trying to open " << compampFile << endl;

    // try to open file for reading 
    ifstream fin;
    fin.open(compampFile.c_str(), ios::in | ios::binary);
    if (!fin.is_open())
    {
        cerr << "Can't open file: " << compampFile << endl;

	cerr << "Using simulated (generated) compamps instead" << endl;

	// create simulated data instead;
	ComplexAmplitudes compamp;
	initSimulatedCompAmps(pol, compamp, activityId);

	// put in the list, by value
	int nCompamps=10;
	for (int i=0; i<nCompamps; ++i)
	{
	    compampList.push_back(compamp);
	}
    }
    else
    {
	// verify that file size is a multiple of struct size
	int filesize = SseUtil::getFileSize(compampFile);

	//AssertMsg(filesize % sizeof(ComplexAmplitudes) == 0, 
	//     "Invalid Complex amplitudes filesize");

        if (filesize % sizeof(ComplexAmplitudes) != 0)
	{
	    cerr <<  "Error: Invalid Complex amplitudes filesize" << endl;
	}
	else
	{
	    // read in all the compamps
	    ComplexAmplitudes compamp;
	    while (fin.read((char*)&compamp, sizeof compamp))
	    {
		compamp.demarshall();

		// put in the list, by value
		compampList.push_back(compamp);
	    }
	}

    }


    cerr << "loadNssCompAmps: " << compampList.size() << " loaded" << endl;
}

void Dx::sendPulseSignals(int nSignals, int activityId)
{
    PulseSignalHeader pulseSignalHeader;
    int nPulses = 3;   // number of pulses in the pulse train
    initPulseSignalHdr(pulseSignalHeader, nPulses);

    double dxTuneFreq = sdAct_->actParam_.dxSkyFreq;

    // define the pulses in the train
    Pulse pulses[nPulses];
    for (int i=0; i < nPulses; ++i)
    {
	pulses[i].rfFreq = dxTuneFreq + 0.0144555666;
	pulses[i].power = 10.5;
	pulses[i].spectrumNumber = i;
	pulses[i].binNumber = i+1;
	pulses[i].pol = POL_BOTH;
    }

    for (int i=0; i< nSignals; i++)
    {
       pulseSignalHeader.sig.path.rfFreq += (i * KhzPerMhz);

       sseProxy_->sendPulseSignal(pulseSignalHeader, pulses, activityId);
    }
}

void Dx::sendCandidatePulseSignals(int nSignals, int activityId)
{
    int nPulses = 3;   // number of pulses in the pulse train
    double dxTuneFreq = sdAct_->actParam_.dxSkyFreq;

    // define the pulses in the train
    Pulse pulses[nPulses];
    for (int i=0; i < nPulses; ++i)
    {
	pulses[i].rfFreq = dxTuneFreq + 0.0222987654;
	pulses[i].power = 10.5;
	pulses[i].spectrumNumber = i;
	pulses[i].binNumber = i+1;
	pulses[i].pol = POL_BOTH;
    }

    SignalIdList::iterator origSigIdIt(
       sdAct_->followUpPulseOrigSigIds_.begin());

    for (int i=0; i< nSignals; i++)
    {
	PulseSignalHeader pulseSignalHeader;
	initPulseSignalHdr(pulseSignalHeader, nPulses);

        pulseSignalHeader.sig.path.rfFreq += (i * KhzPerMhz);

	// set the pfa  above & below the pulseTrainSignifThresh
	// to test confirmation code.  (below is confirmed, above is not)

	pulseSignalHeader.cfm.pfa = 
	    sdAct_->actParam_.pulseTrainSignifThresh + 1 - i;
	pulseSignalHeader.sig.signalId.number = i;

	// test followups

	if (i==0 && sdAct_->opsMask_.test(FOLLOW_UP_CANDIDATES))
	{
	    pulseSignalHeader.sig.sigClass = CLASS_RFI;
	    pulseSignalHeader.sig.reason = NO_SIGNAL_FOUND;
            pulseSignalHeader.sig.signalId.number = SIGNAL_NOT_FOUND_ID_NUMBER;
	}

        // set orig signal id
        if (sdAct_->opsMask_.test(FOLLOW_UP_CANDIDATES))
        {
           // put orig signal ids in each candidate 
           if (origSigIdIt != sdAct_->followUpPulseOrigSigIds_.end())
           {
              pulseSignalHeader.sig.origSignalId = *origSigIdIt;
              ++origSigIdIt;
           }
        }

	sseProxy_->sendCandidatePulseSignal(pulseSignalHeader, pulses, activityId);
    }
}

void Dx::sendPulseCandidateResults(int nSignals, int activityId)
{
    PulseSignalHeader pulseSignalHeader;
    int nPulses = 3;   // number of pulses in the pulse train
    initPulseSignalHdr(pulseSignalHeader, nPulses);

    // add a doppler offset to the remote frequency to 
    // more closely simulate the real situation.
    double dopplerOffsetMHz = 0.005000;
    pulseSignalHeader.sig.path.rfFreq += dopplerOffsetMHz;


    double dxTuneFreq = sdAct_->actParam_.dxSkyFreq;

    // define the pulses in the train
    Pulse pulses[nPulses];
    for (int i=0; i < nPulses; ++i)
    {
	pulses[i].rfFreq = dopplerOffsetMHz + dxTuneFreq + 0.024680000;
	pulses[i].power = 10.5;
	pulses[i].spectrumNumber = i;
	pulses[i].binNumber = i+1;
	pulses[i].pol = POL_RIGHTCIRCULAR;
    }

    int signalNumber = 100; // make signal number unique
    for (int i=0; i< nSignals; i++)
    {
	// alternate confirmed & rfi signals based on the signal number
	if (signalNumber % 2 == 0)
	{
	    pulseSignalHeader.sig.sigClass = CLASS_CAND;
	    pulseSignalHeader.sig.reason = PASSED_POWER_THRESH;
	} 
	else
	{
	    pulseSignalHeader.sig.sigClass = CLASS_RFI;
	    pulseSignalHeader.sig.reason = SNR_TOO_LOW;
	}

	pulseSignalHeader.sig.signalId.number = signalNumber++;
	sseProxy_->sendPulseCandidateResult(pulseSignalHeader, pulses, activityId);
    }
}

// fill in some dummy baseline data.
// this uses the deprecated Baseline struct and needs
// to eventually be converted
void initSimulatedBaseline(Polarization pol, Baseline &baseline)
{
    BaselineHeader &hdr = baseline.header;
    hdr.rfCenterFreq=1000;
    hdr.bandwidth=1;  // MHz
    hdr.halfFrameNumber=1;
    hdr.numberOfSubchannels=MAX_BASELINE_SUBCHANNELS;
    hdr.pol = pol; 

    for (int i=0; i<MAX_BASELINE_SUBCHANNELS; ++i)
    {
	baseline.baselineValues[i] = i;
    }

}


// fill in some dummy complex amplitude data.
// this uses the deprecated ComplexAmplitudes struct and needs
// to eventually be converted
void initSimulatedCompAmps(Polarization pol, ComplexAmplitudes &compamps,
		int32_t activityId)
{
    ComplexAmplitudeHeader &hdr = compamps.header;
    hdr.rfCenterFreq=1000;  // MHz
    hdr.halfFrameNumber = 1;
    hdr.activityId = activityId;
    hdr.hzPerSubchannel = 1000;
    hdr.startSubchannelId = 1;
    hdr.numberOfSubchannels = 1;
    /////////////////////////////////////////////////////////
    // Hack!!  Must get oversampling from DX intrinsics when structure
    // has been modified to include it
    /////////////////////////////////////////////////////////
//    hdr.res = RES_1HZ;            // replaced by overSampling
    hdr.overSampling = .25;
    hdr.pol = pol; 

    for (int i=0; i<MAX_SUBCHANNEL_BINS_PER_1KHZ_HALF_FRAME; ++i)
    {
	compamps.compamp.coef[i].pair = i % 256;
    }

}

