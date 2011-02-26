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


#include "TestRunner.h"
#include "Assert.h"
#include "TestLib.h"
#include "SseUtil.h"

#include <iostream>

using namespace ssechan;

void TestLib::setUp ()
{
}

void TestLib::tearDown()
{
}

void TestLib::testFoo()
{
    cout << "testFoo" << endl;
    //cu_assert(false);  // force failure  
}


void TestLib::testIntrinsics()
{
    Intrinsics intrin;

    SseUtil::strMaxCpy(intrin.interfaceVersion,
		       SSE_CHANNELIZER_INTERFACE_VERSION, MAX_TEXT_STRING);
    
    cout << intrin  << endl;

    intrin.marshall();
    intrin.demarshall();
}

void TestLib::testStatus()
{
    Status status;
    cout << status  << endl;

    status.marshall();
    status.demarshall();
}

void TestLib::testStart()
{
    Start start;
    
    cout << start  << endl;

    start.marshall();
    start.demarshall();
}


void TestLib::testStarted()
{
    Started started;
    
    cout << started  << endl;

    started.marshall();
    started.demarshall();
}


void TestLib::initSseInterfaceHdr(SseInterfaceHeader &hdr,
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


void TestLib::testPrint()
{
#if 0
    // loop through all the message codes
    unsigned int firstCodeIndex = REQUEST_INTRINSICS;
    unsigned int lastCodeIndex = CHANNELIZER_MSG_CODE_END;
    for (unsigned int i=firstCodeIndex; i< lastCodeIndex; ++i)
    {
	initSseInterfaceHdr(hdr, i);
	cout << hdr;
	cout << SseDxMsg::getMessageCodeString(i) << endl;
    }
    cout << endl;

#endif


    NssDate nssDate;
    nssDate.tv_sec = 15;
    nssDate.tv_usec = 2;
#if 0
    DxArchiverStatus stat;
    stat.timestamp = nssDate; 
    stat.numberOfConnectedDxs =2;

    cout << stat  << endl;
    cout << endl;
#endif

}


Test *TestLib::suite ()
{
	TestSuite *testSuite = new TestSuite("TestLib");

	testSuite->addTest (new TestCaller <TestLib> ("testFoo", &TestLib::testFoo));

	testSuite->addTest (new TestCaller <TestLib> ("testIntrinsics", &TestLib::testIntrinsics));
	testSuite->addTest (new TestCaller <TestLib> ("testStatus", &TestLib::testStatus));
	testSuite->addTest (new TestCaller <TestLib> ("testStart", &TestLib::testStart));
	testSuite->addTest (new TestCaller <TestLib> ("testStarted", &TestLib::testStarted));

	testSuite->addTest (new TestCaller <TestLib> ("testPrint", &TestLib::testPrint));

	return testSuite;
}