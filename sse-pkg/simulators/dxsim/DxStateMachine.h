/*******************************************************************************

 File:    DxStateMachine.h
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


#ifndef DXSTATEMACHINE_H
#define DXSTATEMACHINE_H

// Keeps track of DX internal state.
// DxStateEvent lists the events
// used to transition from state to state.

#include <string>
#include <map>

using std::string;
using std::pair;
using std::map;

class DxStateMachine
{
 public:

    enum DxStateEvent
    {
	CONNECTED_TO_SSE_EVENT,         // sse socket connection established
	CONFIGURE_DX_RCVD_EVENT,       // received CONFIGURE_DX message
	ACTIVITY_PARAMETERS_RCVD_EVENT, // received SEND_DX_ACTIVITY_PARAMETERS msg
	START_TIME_RCVD_EVENT,          // received activity START_TIME msg
	DATA_COLLECTION_STARTED_EVENT,  // dx self-generated event
	DATA_COLLECTION_COMPLETE_EVENT,  // dx self-generated event
	SIGNAL_DETECTION_STARTED_EVENT, // dx self-generated event
	SIGNAL_DETECTION_COMPLETE_EVENT // dx self-generated event
    };

    enum DxState
    {
	ERROR_STATE,                   // error state
	STARTUP_STATE,                 // initial state
	CONNECTED_SSE_STATE,           // sse socket connection made
	READY_FOR_ACTIVITY_STATE,      // ready for new activity
	ACTIVITY_PARAMS_RECEIVED_STATE, // received activity parameters
	ACTIVITY_PENDING_DC_STATE,     // received start time
	ACTIVITY_RUNNING_DC_STATE,     // in data collection
	ACTIVITY_COMPLETED_DC_STATE,   // done with data collection
	ACTIVITY_PENDING_SD_STATE,     // waiting for sig detect to begin
	ACTIVITY_RUNNING_SD_STATE      // in signal detection
    };

    DxStateMachine();
    DxStateMachine(DxState initialState);
    ~DxStateMachine();

    string getCurrentStateName();
    string getStateEventName(DxStateEvent stateEvent);
    DxState getCurrentState();
    bool changeState(DxStateEvent stateEvent);
    void setState(DxState state);

 private:
    void initializeStateTable();
    void addStateTransitionToTable(DxStateEvent stateEvent,
	     DxState currentState, DxState nextState);
    bool lookupStateTransition(DxStateEvent stateEvent,
	     DxState currentState, DxState *nextState);


 private:
    // Disable copy construction & assignment.
    // Don't define these.
    DxStateMachine(const DxStateMachine& rhs);
    DxStateMachine& operator=(const DxStateMachine& rhs);

 private:
    typedef pair<DxState, DxState> DxStatePair;
    typedef map<DxStateEvent, DxStatePair> StateMap;

    DxState currentState_;
    StateMap stateMap_;
};



#endif // DXSTATEMACHINE_H
