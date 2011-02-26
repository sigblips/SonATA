/*******************************************************************************

 File:    TestChan.cpp
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


#include "ace/OS.h"
#include "ace/SOCK_Connector.h" 
#include "TestRunner.h"

#include "TestChan.h"
#include "Channelizer.h"
#include "SseProxy.h"
#include "ArrayLength.h"

void TestChan::setUp ()
{
}

void TestChan::tearDown()
{
}

void TestChan::testFoo()
{
    cout << "testFoo" << endl;
    //cu_assert(false);  // force failure  
}

// verify we can print intrinsics info
void TestChan::testPrintIntrinsics()
{
    ACE_SOCK_STREAM dummyStream;
    SseProxy proxy(dummyStream);

    const string &name("chanTest1");
    int totalChans(16);
    int outputChans(8);
    double outBwMhz(4);

    Channelizer chanzer(
       &proxy, name,
       totalChans, outputChans,
       outBwMhz);

    chanzer.printIntrinsics();
}

Test *TestChan::suite ()
{
	TestSuite *testSuite = new TestSuite("TestChan");

	testSuite->addTest (new TestCaller <TestChan> ("testFoo", 
                                                       &TestChan::testFoo));

	testSuite->addTest (new TestCaller <TestChan> ("testPrintIntrinsics",
                                                       &TestChan::testPrintIntrinsics));

	return testSuite;
}