/*******************************************************************************

 File:    TestLib.cpp
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


#include "TestLib.h"
#include "TestRunner.h"
#include "Assert.h"
#include "sseTscopeInterfaceLib.h"
#include "SseException.h"
#include <iostream>

using namespace std;


void TestLib::setUp ()
{
}

void TestLib::tearDown()
{
}


void TestLib::testPrint()
{
    TscopeIntrinsics intrin;
    cout << intrin << endl;

    TscopeAssignSubarray assign;
    cout << assign << endl;

    TscopeCalRequest cal;
    cout << cal << endl;

    TscopeNullType nullType;
    cout << nullType << endl;

    TscopeStopRequest stopReq;
    cout << stopReq << endl;

    TscopeStowRequest stowReq;
    cout << stowReq << endl;

    TscopeTuneRequest tuneRequest;
    cout << tuneRequest << endl;

    TscopeBeamCoords beamCoords;
    cout << beamCoords << endl;

    TscopeSubarrayCoords subCoords;
    cout << subCoords << endl;

    TscopeWrapRequest wrap;
    cout << wrap << endl;

    TscopeMonitorRequest monitor;
    cout << monitor << endl;

    // loop through all the message codes
    unsigned int firstCodeIndex = TSCOPE_MSG_UNINIT + 1;
    unsigned int lastCodeIndex = TSCOPE_MESSAGE_CODE_END;
    for (unsigned int i=firstCodeIndex; i< lastCodeIndex; ++i)
    {
       string messageCodeString(SseTscopeMsg::getMessageCodeString(i));
       
       cout << messageCodeString << endl;
       cu_assert(messageCodeString.find("message code out of range")
		 == string::npos);
    }
    cout << endl;


    TscopeStatusMultibeam statusMultibeam;
    cout << statusMultibeam << endl;

    TscopeZfocusRequest zfocus;
    cout << zfocus << endl;

    TscopeAntgroupAutoselect antAuto;
    cout << antAuto << endl;

}

void TestLib::testBeamNames()
{
   cu_assert(SseTscopeMsg::beamToName(TSCOPE_BEAMXA1) == "BEAMXA1");
   cu_assert(SseTscopeMsg::beamToName(TSCOPE_BEAMYD4) == "BEAMYD4");

   // Test bad beam
   TscopeBeam badBeam = static_cast<TscopeBeam>(-5);
   cu_assert(SseTscopeMsg::beamToName(badBeam).find("Error") != string::npos);

   cu_assert(SseTscopeMsg::nameToBeam("BEAMXA1") == TSCOPE_BEAMXA1);
   cu_assert(SseTscopeMsg::nameToBeam("BEAMYD4") == TSCOPE_BEAMYD4);

   // verify that case shouldn't matter
   cu_assert(SseTscopeMsg::nameToBeam("beamyd4") == TSCOPE_BEAMYD4);

   // test invalid beam name
   cu_assert(SseTscopeMsg::nameToBeam("BAD_BEAM") == TSCOPE_INVALID_BEAM);

}

void TestLib::testTuningNames()
{
   cu_assert(SseTscopeMsg::tuningToName(TSCOPE_TUNINGA) == "TUNINGA");
   cu_assert(SseTscopeMsg::tuningToName(TSCOPE_TUNINGB) == "TUNINGB");

   // Test bad tuning
   TscopeTuning badTuning = static_cast<TscopeTuning>(-5);
   cu_assert(SseTscopeMsg::tuningToName(badTuning).find("Error") != string::npos);

   cu_assert(SseTscopeMsg::nameToTuning("TUNINGA") == TSCOPE_TUNINGA);
   cu_assert(SseTscopeMsg::nameToTuning("TUNINGB") == TSCOPE_TUNINGB);

   // verify that case shouldn't matter
   cu_assert(SseTscopeMsg::nameToTuning("tuningb") == TSCOPE_TUNINGB);

   // look up an invalid tuning name
   cu_assert(SseTscopeMsg::nameToTuning("BAD_TUNING") == TSCOPE_INVALID_TUNING);

}


void TestLib::testMisc()
{
   cu_assert(SseTscopeMsg::coordSysToString(TscopePointing::AZEL) == "AZEL");
   cu_assert(SseTscopeMsg::stringToCoordSys("AZEL") == TscopePointing::AZEL);

   cu_assert(SseTscopeMsg::calTypeToString(TscopeCalRequest::DELAY) == "delay");
   cu_assert(SseTscopeMsg::stringToCalType("delay") == TscopeCalRequest::DELAY);
   cu_assert(SseTscopeMsg::stringToCalType("DELAY") == TscopeCalRequest::DELAY);


   cu_assert(SseTscopeMsg::nullTypeToString(TscopeNullType::AXIAL) == "axial");
   cu_assert(SseTscopeMsg::stringToNullType("axial") == TscopeNullType::AXIAL);
   cu_assert(SseTscopeMsg::stringToNullType("AXIAL") == TscopeNullType::AXIAL);

}

Test *TestLib::suite()
{
   TestSuite *testSuite = new TestSuite("TestLib");
   
   testSuite->addTest(new TestCaller<TestLib>("testPrint", &TestLib::testPrint));
   testSuite->addTest(new TestCaller<TestLib>("testBeamNames", &TestLib::testBeamNames));
   testSuite->addTest(new TestCaller<TestLib>("testTuningNames", &TestLib::testTuningNames));
   testSuite->addTest(new TestCaller<TestLib>("testMisc", &TestLib::testMisc));
   
   return testSuite;
}
