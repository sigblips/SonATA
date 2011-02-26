/*******************************************************************************

 File:    Tscope.h
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


#ifndef tscope_h
#define tscope_h

#include <sseTscopeInterface.h>
#include <vector>
using std::string;
using std::vector;

class TscopeEventHandler;
class CmdPattern;
class SseProxy;

class Tscope 
{
  
public:

  Tscope(SseProxy& sseProxy, const string &antControlServerName,
	 int controlPort, int monitorPort);

  virtual ~Tscope();
  
  TscopeIntrinsics getIntrinsics();
  SseProxy & getSseProxy();

  void setVerboseLevel(int level);
  int getVerboseLevel();

  void setName(const string &name);
  string getName();

  TscopeStatusMultibeam getStatusMultibeam() const;

  bool getSimulated();
  void setSimulated(bool simulated);

  void connect();
  void disconnect();
  void setAntControlServerName(const string &serverName);

  friend ostream& operator << (ostream& strm, Tscope& tscope);


  // ----- handles incoming messages from SSE ------

  virtual void allocate(const TscopeSubarray & subarray);
  virtual void deallocate(const TscopeSubarray & subarray);

  virtual void assignSubarray(const TscopeAssignSubarray & assignSub);
  virtual void antgroupAutoselect(const TscopeAntgroupAutoselect &antAuto);

  virtual void beamformerReset();
  virtual void beamformerInit();
  //JR - Added ti send destination ip:port to bfinit.rb
  virtual void beamformerDest(TscopeBackendCmd args);
  virtual void beamformerAutoatten();

  virtual void beamformerSetCoords(const TscopeBeamCoords & coords);
  virtual void beamformerSetNullType(const TscopeNullType& nullType);
  virtual void beamformerClearNulls();
  virtual void beamformerAddNullCoords(const TscopeBeamCoords & coords);
  virtual void beamformerPoint();
  virtual void beamformerCal(const TscopeCalRequest &cal);

  virtual void beamformerClearBeamCoords();
  virtual void beamformerClearAnts();

  virtual void beamformerStop();

  virtual void monitor(const TscopeMonitorRequest & request);

  virtual void beginSendingCommandSequence();
  virtual void doneSendingCommandSequence();

  virtual void pointSubarray(const TscopeSubarrayCoords & coords);

  virtual void requestPointingCheck(const TscopeSubarrayCoords & coords);

  virtual void reset();
  virtual void requestStatus();
  virtual void requestIntrinsics();

  virtual void tune(const TscopeTuneRequest & tuneReq);
  virtual void shutdown();
  virtual void stop(const TscopeStopRequest & stopReq);
  virtual void stow(const TscopeStowRequest & stowReq);
  virtual void wrap(TscopeWrapRequest wrapreq);
  virtual void zfocus(const TscopeZfocusRequest & zfocusReq);

  virtual void lnaOn(const TscopeLnaOnRequest & lnaOnReq);
  virtual void pamSet(const TscopePamSetRequest & pamSetReq);

  virtual void sendBackendCmd(const TscopeBackendCmd &backendCmd);

  friend ostream& operator << (ostream &strm, const Tscope& tscope);

  void reportErrorToSse(const string & msg);

  // handle incoming messages from antenna server
  virtual void handleControlResponse(const string & msg);
  virtual void handleMonitorResponse(const string & msg);

  virtual void parseStatus(const string &text);

 private:

  TscopeEventHandler & getControlHandler();
  TscopeEventHandler & getMonitorHandler();

  void dispatchStatusLineForParsing(vector<string> &words);
  void parseArrayLine(TscopeStatusMultibeam &status, vector<string> &words);
  void parseSubarrayLine(TscopeSubarrayStatus &status, vector<string> &words);
  void parseBeamPointingLine(TscopePointing &pointing, vector<string> &words);
  void parseIfChainLine(TscopeIfChainStatus &ifChainStatus, vector<string> &words);
  void parseTuningLine(TscopeTuningStatus &tuningStatus, vector<string> &words);

  void reportTargetTracking();
  bool isRequestedTargetPositionCloseToCurrentPosition(
         const TscopePointing& requested, TscopePointing & current);
  void reportTunings();
  void reportStatusToSse();

  void handleDeferredBfCommand(const string &command);
  void handleCommand(const string &command);
  bool sendCommand(const string &command);

  double beamsizeDeg() const;
  double maxPointingErrorDeg() const;
  TscopeBeam getBeamIndexForSubarray(const string &subarray);

  bool haveRepeatedPointingRequest(const TscopeSubarrayCoords & coords);

  void printPointingRequestsAndStatus(const string &header);

  enum CommandSequenceState
     { COMMAND_SEQUENCE_NOT_ACTIVE, COMMAND_SEQUENCE_INCOMING, 
       COMMAND_SEQUENCE_FULLY_RECEIVED };

  SseProxy & sseProxy_;
  CmdPattern *controlEventCallback_;
  TscopeEventHandler *controlEventHandler_;
  CmdPattern *monitorEventCallback_;
  TscopeEventHandler *monitorEventHandler_;
  TscopeIntrinsics intrinsics_;
  int verboseLevel_;
  bool simulated_;
  vector<TscopeTuneRequest> tuneRequests_;
  vector<TscopeSubarrayCoords> subarrayPointingRequests_;
  vector<TscopeAssignSubarray> ataAssignSubarray_;
  double rcvrSkyTuneReqMhz_;
  bool tracking_; // tracking target(s)
  bool tuned_; // all tune commands are valid
  bool fullCommandSequenceReceived_; 
  TscopeStatusMultibeam statusMultibeam_;
  int notTrackingTargetCount_;
  CommandSequenceState commandSequenceState_;
  string deferredBeamformerCmd_;
  
  // Disable copy construction & assignment.
  // Don't define these.
  Tscope(const Tscope& rhs);
  Tscope& operator=(const Tscope& rhs);

  string intToString(const int val);
  string doubleToString(const double val);
};





#endif // tscope_h