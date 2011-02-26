/*******************************************************************************

 File:    TestDxStateMachine.cpp
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

#include "TestDxStateMachine.h"
#include "DxStateMachine.h"

void TestDxStateMachine::setUp ()
{
}

void TestDxStateMachine::tearDown()
{
}

// put the dx state machine through its paces
void TestDxStateMachine::testStateChanges()
{
    cout << "testDxStateMachine" << endl;
    DxStateMachine dxsm;

    // check initial state
    cu_assert (dxsm.getCurrentState() == DxStateMachine::STARTUP_STATE);
    cu_assert (dxsm.getCurrentStateName() == "STARTUP_STATE");

    // try to send an event out of order
    //cu_assert (dxsm.changeState(DxStateMachine::SIGNAL_DETECTION_COMPLETE_EVENT));

    // try getting an event name
    cu_assert(dxsm.getStateEventName(DxStateMachine::CONFIGURE_DX_RCVD_EVENT)
      == "CONFIGURE_DX_RCVD_EVENT");

    // walk through a few state transitions
    cu_assert(dxsm.changeState(DxStateMachine::CONNECTED_TO_SSE_EVENT));
    cu_assert (dxsm.getCurrentState() == DxStateMachine::CONNECTED_SSE_STATE);

    cu_assert (dxsm.changeState(DxStateMachine::CONFIGURE_DX_RCVD_EVENT));
    cu_assert (dxsm.getCurrentState() == DxStateMachine::READY_FOR_ACTIVITY_STATE);

    cu_assert (dxsm.changeState(DxStateMachine::ACTIVITY_PARAMETERS_RCVD_EVENT));
    cu_assert (dxsm.changeState(DxStateMachine::START_TIME_RCVD_EVENT));
    cu_assert (dxsm.changeState(DxStateMachine::DATA_COLLECTION_STARTED_EVENT));
    cu_assert (dxsm.changeState(DxStateMachine::DATA_COLLECTION_COMPLETE_EVENT));

    cu_assert (dxsm.changeState(DxStateMachine::SIGNAL_DETECTION_STARTED_EVENT));

    cu_assert (dxsm.changeState(DxStateMachine::SIGNAL_DETECTION_COMPLETE_EVENT));

    // set the state, check it
    dxsm.setState(DxStateMachine::ACTIVITY_RUNNING_SD_STATE);
    cu_assert (dxsm.getCurrentState() == DxStateMachine::ACTIVITY_RUNNING_SD_STATE);
    cu_assert (dxsm.getCurrentStateName() == "ACTIVITY_RUNNING_SD_STATE");

    // try the final "loop back" state transition
    cu_assert (dxsm.changeState(DxStateMachine::SIGNAL_DETECTION_COMPLETE_EVENT));
    cu_assert (dxsm.getCurrentState() == DxStateMachine::READY_FOR_ACTIVITY_STATE);

}



Test *TestDxStateMachine::suite ()
{
	TestSuite *testSuite = new TestSuite("TestDxStateMachine");

	testSuite->addTest (new TestCaller <TestDxStateMachine> ("testStateChanges", &TestDxStateMachine::testStateChanges));

	return testSuite;
}