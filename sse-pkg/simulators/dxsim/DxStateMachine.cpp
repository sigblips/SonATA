/*******************************************************************************

 File:    DxStateMachine.cpp
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


#include "DxStateMachine.h"
#include "Assert.h"
#include "ArrayLength.h"

using namespace std;

// Code not pipelined yet.  TBD.

static const char *DxStateNames[] = {
    "ERROR_STATE",                   // error condition
    "STARTUP_STATE",                 // initial state
    "CONNECTED_SSE_STATE",           // sse socket connection made
    "READY_FOR_ACTIVITY_STATE",      // ready for new activity
    "ACTIVITY_PARAMS_RECEIVED_STATE",    // received activity parameters
    "ACTIVITY_PENDING_DC_STATE",     // received start time
    "ACTIVITY_RUNNING_DC_STATE",     // in data collection
    "ACTIVITY_COMPLETED_DC_STATE",   // done with data collection
    "ACTIVITY_PENDING_SD_STATE",     // waiting for sig detect to begin
    "ACTIVITY_RUNNING_SD_STATE"      // running, in signal detection
};

static const char *DxStateEventNames[] = {
    "CONNECTED_TO_SSE_EVENT",         // sse socket connection established
    "CONFIGURE_DX_RCVD_EVENT",       // received CONFIGURE_DX message
    "ACTIVITY_PARAMETERS_RCVD_EVENT",  // received SEND_DX_ACTIVITY_PARAMETERS msg
    "START_TIME_RCVD_EVENT",           // received activity START_TIME msg
    "DATA_COLLECTION_STARTED_EVENT",  // dx self-generated event
    "DATA_COLLECTION_COMPLETE_EVENT",  // dx self-generated event
    "SIGNAL_DETECTION_STARTED_EVENT", // dx self-generated event
    "SIGNAL_DETECTION_COMPLETE_EVENT" 
};

DxStateMachine::DxStateMachine()
    : currentState_(STARTUP_STATE)
{
    initializeStateTable();
}

// initialize to specific state
DxStateMachine::DxStateMachine(DxState initialState)
    : currentState_(initialState)
{
    initializeStateTable();
}

void DxStateMachine::initializeStateTable()
{
    // fill in the state transition table
    // <incoming event> <current state> <next state>

    addStateTransitionToTable(
        CONNECTED_TO_SSE_EVENT, STARTUP_STATE, 
        CONNECTED_SSE_STATE);

    addStateTransitionToTable(
        CONFIGURE_DX_RCVD_EVENT, CONNECTED_SSE_STATE,
        READY_FOR_ACTIVITY_STATE);

    addStateTransitionToTable(
        ACTIVITY_PARAMETERS_RCVD_EVENT, READY_FOR_ACTIVITY_STATE, 
        ACTIVITY_PARAMS_RECEIVED_STATE);

    addStateTransitionToTable(
        START_TIME_RCVD_EVENT, ACTIVITY_PARAMS_RECEIVED_STATE,
        ACTIVITY_PENDING_DC_STATE);

    addStateTransitionToTable(
        DATA_COLLECTION_STARTED_EVENT, ACTIVITY_PENDING_DC_STATE,
        ACTIVITY_RUNNING_DC_STATE);

    addStateTransitionToTable(
        DATA_COLLECTION_COMPLETE_EVENT, ACTIVITY_RUNNING_DC_STATE,
        ACTIVITY_PENDING_SD_STATE);

    addStateTransitionToTable(
        SIGNAL_DETECTION_STARTED_EVENT, ACTIVITY_PENDING_SD_STATE,
        ACTIVITY_RUNNING_SD_STATE);

    // loop back to next activity
    addStateTransitionToTable(
        SIGNAL_DETECTION_COMPLETE_EVENT, ACTIVITY_RUNNING_SD_STATE,
        READY_FOR_ACTIVITY_STATE);
}

void DxStateMachine::addStateTransitionToTable(DxStateEvent stateEvent,
	    DxState currentState, DxState nextState)
{
    DxStatePair statePair(currentState, nextState);
    stateMap_.insert(StateMap::value_type(stateEvent, statePair));
}

DxStateMachine::~DxStateMachine()
{

}

DxStateMachine::DxState DxStateMachine::getCurrentState()
{
    return currentState_; 
}

void DxStateMachine::setState(DxState state)
{
    currentState_=state;
}



string DxStateMachine::getCurrentStateName()
{
    Assert(currentState_ >= 0 && currentState_ < ARRAY_LENGTH(DxStateNames));
    return string(DxStateNames[currentState_]);
}

string DxStateMachine::getStateEventName(DxStateEvent stateEvent)
{
    Assert(stateEvent >= 0 && stateEvent < ARRAY_LENGTH(DxStateEventNames));
    return string(DxStateEventNames[stateEvent]);
}

bool DxStateMachine::changeState(DxStateEvent stateEvent)
{
    bool status = false;
    DxState nextState = ERROR_STATE;

    if (lookupStateTransition(stateEvent, currentState_, &nextState))
    {
	currentState_ = nextState;
	status = true;
    }
    
    return status;
}

// Using the given state transition event and the current state,
// return the next state, if valid.
//
bool DxStateMachine::lookupStateTransition(DxStateEvent stateEvent,
        DxState currentState, DxState *nextState)
{
    bool status=false;

    // find the event
    StateMap::iterator it=stateMap_.find(stateEvent);
    if (it != stateMap_.end() )
    {
	// verify that the event is valid for the currentState
	DxStatePair statePair= (*it).second;
	if (statePair.first == currentState)
	{
	    *nextState = statePair.second; 
            status = true;
	}					       
	else
	{
	    string errString = "ERROR: " + getStateEventName(stateEvent) +
                " event not valid for current state " +
		getCurrentStateName();
	    cerr << errString << endl;
//	    AssertMsg(false, errString); //"event not valid for current state");
	}
    }
    else
    {
        string errString = "ERROR: " + getStateEventName(stateEvent) +
                " event not found in table";
	AssertMsg(false, errString); 
    }

    return status;
}