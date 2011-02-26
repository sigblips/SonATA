/*******************************************************************************

 File:    TestSseDxArchiverInterfaceLib.cpp
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
#include <iostream>
#include "TestSseDxArchiverInterfaceLib.h"

void TestSseDxArchiverInterfaceLib::setUp ()
{
}

void TestSseDxArchiverInterfaceLib::tearDown()
{
}

void TestSseDxArchiverInterfaceLib::testFoo()
{
    cout << "testFoo" << endl;
    //cu_assert(false);  // force failure  
}

void TestSseDxArchiverInterfaceLib::testDxArchiverIntrinsics()
{
    DxArchiverIntrinsics intrin;
    
    cout << intrin  << endl;

    intrin.marshall();
    intrin.demarshall();

}

void TestSseDxArchiverInterfaceLib::initSseInterfaceHdr(SseInterfaceHeader &hdr,
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


void TestSseDxArchiverInterfaceLib::testPrint()
{
#if 0
    SseInterfaceHeader hdr;
    unsigned int code = BIRDIE_MASK;
    initSseInterfaceHdr(hdr, code);
    cout << hdr;
    cout << SseDxMsg::getMessageCodeString(code) << endl;

    unsigned int BAD_MESSAGE_CODE = 999999;
    initSseInterfaceHdr(hdr, BAD_MESSAGE_CODE);
    cout << "Expected bad message code error:" << endl;
    cout << hdr;
    cout << SseDxMsg::getMessageCodeString(BAD_MESSAGE_CODE) << endl;

    BAD_MESSAGE_CODE = 0;
    initSseInterfaceHdr(hdr, BAD_MESSAGE_CODE);
    cout << "Expected bad message code error:" << endl;
    cout << hdr;
    cout << SseDxMsg::getMessageCodeString(BAD_MESSAGE_CODE) << endl;


    // loop through all the message codes
    unsigned int firstCodeIndex = REQUEST_DX_ARCHIVER_INTRINSICS;
    unsigned int lastCodeIndex = DX_ARCHIVER_MESSAGE_CODE_END;
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

    DxArchiverStatus stat;
    stat.timestamp = nssDate; 
    stat.numberOfConnectedDxs =2;

    cout << stat  << endl;
    cout << endl;


}


Test *TestSseDxArchiverInterfaceLib::suite ()
{
	TestSuite *testSuite = new TestSuite("TestSseDxArchiverInterfaceLib");

	testSuite->addTest (new TestCaller <TestSseDxArchiverInterfaceLib> ("testFoo", &TestSseDxArchiverInterfaceLib::testFoo));

	testSuite->addTest (new TestCaller <TestSseDxArchiverInterfaceLib> ("testDxArchiverIntrinsics", &TestSseDxArchiverInterfaceLib::testDxArchiverIntrinsics));

	testSuite->addTest (new TestCaller <TestSseDxArchiverInterfaceLib> ("testPrint", &TestSseDxArchiverInterfaceLib::testPrint));

	return testSuite;
}