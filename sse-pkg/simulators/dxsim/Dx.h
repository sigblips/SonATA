/*******************************************************************************

 File:    Dx.h
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


#ifndef _dx_h
#define _dx_h

#include "sseDxInterface.h"
#include "DxStateMachine.h"
#include <vector>
#include "DxOpsBitset.h"

using std::vector;

typedef vector<SignalId> SignalIdList;
typedef vector<SignalDescription> SignalDescripList;

// A place to store activity specific information
// for pipelining.
struct DxActivityInfo
{
   DxStateMachine stateMachine_;      // keeps track of dx state
   DxActivityStatus actStatus_;
   DxActivityParameters actParam_;
   DxOpsBitset opsMask_;
   NssDate startTime_;
   SignalIdList followUpCwOrigSigIds_;
   SignalIdList followUpPulseOrigSigIds_;
   SignalDescripList cwCoherentCandSigDescripList_;
};

class SseProxy;
class SseSock;
class DxArchiverProxy;

typedef vector<Baseline> BaselineList;
typedef vector<ComplexAmplitudes> CompAmpList;

class Dx {
private:

public:
    Dx(SseProxy *sseProxy);
    Dx(SseProxy *sseProxy, const string &sciDataDir, 
	const string &sciDataPrefix, const string &dxName, bool remoteMode,
	bool varyOutputData);
    virtual ~Dx();

    // --- incoming messages from SSE ------
    virtual void requestIntrinsics(SseProxy *sseProxy);
    virtual void requestIntrinsics(DxArchiverProxy *dxArchiverProxy);
    virtual void configureDx(DxConfiguration *config);
    virtual void setPermRfiMask(FrequencyMaskHeader *maskHdr,
				FrequencyBand freqBandArray[]);
    virtual void setBirdieMask(FrequencyMaskHeader *maskHdr,
				FrequencyBand freqBandArray[]);
    virtual void setRcvrBirdieMask(FrequencyMaskHeader *maskHdr,
				FrequencyBand freqBandArray[]);
    virtual void setTestSignalMask(FrequencyMaskHeader *maskHdr,
				FrequencyBand freqBandArray[]);
    virtual void setRecentRfiMask(RecentRfiMaskHeader *maskHdr,
				FrequencyBand freqBandArray[]);
    virtual void setDxActivityParameters(DxActivityParameters *actParam);
    virtual void requestDxStatus();
    virtual void dxScienceDataRequest(DxScienceDataRequest *dataRequest);
    virtual void setStartTime(StartActivity *startAct);
    virtual void stopDxActivity(int activityId);
    virtual void shutdown();
    virtual void restart();

    virtual void requestArchiveData(ArchiveRequest *archiveRequest);
    virtual void discardArchiveData(ArchiveRequest *archiveRequest);

    // incoming to main dx only
    virtual void beginSendingFollowUpSignals(const Count *count);
    virtual void sendFollowUpCwSignal(const FollowUpCwSignal *followUpCwSignal);
    virtual void sendFollowUpPulseSignal(const FollowUpPulseSignal *followUpPulseSignal);
    virtual void doneSendingFollowUpSignals();

    // relayed from dx main to remote Dx via SSE
    virtual void beginSendingCandidates(const Count *count);
    virtual void sendCandidateCwPowerSignal(const CwPowerSignal *cwPowerSignal
);
    virtual void sendCandidatePulseSignal(const PulseSignalHeader *hdr, Pulse pulses[]);
    virtual void doneSendingCandidates();

    virtual void beginSendingCwCoherentSignals(Count *count);
    virtual void sendCwCoherentSignal(CwCoherentSignal *cwCoherentSignal);
    virtual void doneSendingCwCoherentSignals();

    // ----- public utility methods ------------------
    virtual void printConfiguration();
    virtual void printActivityParameters();
    virtual void printIntrinsics();
    virtual void printDxStatus();
    virtual void printScienceDataRequest();

    virtual void notifyDxArchiverConnected();
    virtual void notifyDxArchiverDisconnected();
    virtual bool dxArchiverIsAlive();

    // test methods
    virtual void sendErrorMsgToSse(const string &text);
    virtual void sendMsgToSse(NssMessageSeverity severity, const string &text);

protected:
    virtual void setup();
    virtual void observationSetup();
    virtual void dataCollectionWrapup();
    virtual void startSignalDetection();
    virtual void baselineInitAccum();
    virtual void scheduleObsStart(NssDate *startTime);
    virtual SseProxy *getSseProxy();
    virtual DxArchiverProxy *getDxArchiverProxy();
    virtual void signalDetectionWrapup();
    virtual void activityWrapup(DxActivityInfo *act);

    virtual void initIntrinsics();
    virtual void determineDxNumber();

    virtual void sendBaseline(BaselineList &baselineList,
			      BaselineList::iterator &baselineIterator,
			      int activityId);
    virtual void sendComplexAmplitudes(CompAmpList &compAmpsList, 
				       CompAmpList::iterator &compAmpsIterator,
				       int activityId);
    virtual void sendBaselineStatistics(Polarization pol, int activityId);
    virtual void sendCandidatesAndSignals(int activityId);
    virtual void sendCwPowerAndPulseCandidates(int activityId);
    virtual void sendAllCwPowerAndPulseSignals(int activityId);
    virtual void sendBadBands(int activityId);
    virtual void sendCoherentCwSignals(int activityId);
    virtual void sendCandidateResults(int activityId);
    virtual void sendPulseSignals(int nSignals, int activityId);
    virtual void sendCandidatePulseSignals(int nSignals, int activityId);
    virtual void sendPulseCandidateResults(int nSignals, int activityId);

    virtual void initSignalId(SignalId &signalId, int signalNumber);
    virtual void initDetectionStats(DetectionStatistics &stats);
    virtual void initPulseSignalHdr(PulseSignalHeader &hdr, int nPulses);
    virtual void initCwPowerSignal(CwPowerSignal &cwPowerSignal);
    virtual void initCwCoherentSignal(CwCoherentSignal &cwCoherentSignal);

    virtual void cancelTimer(const string & timerName, int &timerId);
    virtual void updateDxStatus();
    virtual void startTimerToWaitForAllArchiveRequests();
    virtual void makeConnectionToDxArchiver(const char *archiverHostname,
					int archiverPort);

    // assisting timer classes
    friend class StartDataCollTimerHandler;
    friend class HalfFrameTimerHandler;
    friend class EndSigDetTimerHandler;
    friend class WaitForAllArchiveRequestsTimerHandler;

    // Data
    SseProxy *sseProxy_;
    DxArchiverProxy *dxArchiverProxy_;
    SseSock *dxArchiverSock_;

    string dxName_;
    bool remoteMode_;
    bool varyOutputData_;  // vary the output data from dx to dx
    DxIntrinsics intrinsics_;
    DxConfiguration config_;
    DxStatus dxStatus_;
    int dxNumber_;

    // pre-generated baselines & complex amplitudes (science data)
    string sciDataDir;     // directory containing science data
    string sciDataPrefix;  // prefix of science data files in sciDataDir
    BaselineList cannedBaselinesL_;  // baselines, left pol
    BaselineList cannedBaselinesR_;  // baselines, right pol
    CompAmpList cannedCompAmpsL_;    // complex amps, left pol
    CompAmpList cannedCompAmpsR_;    // complex amps, right pol

    int halfFrameNumber_;    // current half frame num in data collection
    int totalHalfFrames_;    // total number of 1/2 frames in DC
    BaselineList::iterator baselineIteratorL_;
    BaselineList::iterator baselineIteratorR_;
    CompAmpList::iterator compAmpsIteratorL_;
    CompAmpList::iterator compAmpsIteratorR_;

    int startDataCollTimerId_;  // data collection start timer id
    int halfFrameTimerId_;      // data collection half frame timer id
    int endSigDetTimerId_;      // end signal detection timer id
    int waitForAllArchiveRequestsTimerId_; // timer id

    // Activity Info for pipelined activities.
    // act A & B flip between data collection & signal detection activities.
    DxActivityInfo actInfoA_;    // Activity A
    DxActivityInfo actInfoB_;    // Activity B

    DxActivityInfo *dcAct_;  // data collection activity 
    DxActivityInfo *sdAct_;  // signal detection activity

    SignalIdList secondaryCwCandSigIds_;
    SignalIdList secondaryPulseCandSigIds_;

private:
    // Disable copy construction & assignment.
    // Don't define these.
    Dx(const Dx& rhs);
    Dx& operator=(const Dx& rhs);

};



#endif //_dx_h