/*******************************************************************************

 File:    TestDxArchiver.cpp
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

#include "TestDxArchiver.h"
#include "DxArchiver.h"
#include "SseProxy.h"
#include "DxArchiverCmdLineArgs.h"
#include "ArrayLength.h"

void TestDxArchiver::setUp ()
{
}

void TestDxArchiver::tearDown()
{
}

void TestDxArchiver::testFoo()
{
    cout << "testFoo" << endl;
    //cu_assert(false);  // force failure  
}

// verify we can print intrinsics info
void TestDxArchiver::testPrintIntrinsics()
{
    ACE_SOCK_STREAM dummyStream;
    SseProxy proxy(dummyStream);

    const string &name("dxArchiverTest");
    int dxPort = 2002;
    DxArchiver dxArchiver(&proxy, name, dxPort);
    //DxDebug dx(&proxy);

    // test the dx
    dxArchiver.printIntrinsics();


}

void TestDxArchiver::testDxArchiverCmdLineArgs()
{
    string progName("TestDxArchiverCmdLineArgs");
    int defaultSsePort(2222);
    int defaultDxPort(3333);
    string defaultSseHostname("fred-main");
    string defaultName("archiver7");
    bool defaultNoUi(false);

    DxArchiverCmdLineArgs cmdArgs(progName, 
				   defaultSseHostname,
				   defaultSsePort, 
				   defaultDxPort, 
				   defaultNoUi,
				   defaultName);

    // test usage message output
    cmdArgs.usage();
    cerr << endl;

    // check the defaults
    cu_assert (cmdArgs.getSsePort() == defaultSsePort);
    cu_assert (cmdArgs.getDxPort() == defaultDxPort);
    cu_assert (cmdArgs.getSseHostname() == defaultSseHostname);
    cu_assert (cmdArgs.getNoUi() == defaultNoUi);
    cu_assert (cmdArgs.getName() == defaultName);

    // try setting alternate parameters
    const char *argv[] = 
    { "ProgName",
      "-host", "barney",
      "-sse-port", "8888",
      "-dx-port", "8877",
      "-name", "archiver8"
    };
    const int argc = ARRAY_LENGTH(argv);

    // verify that everything parses
    cu_assert(cmdArgs.parseArgs(argc, const_cast<char **>(argv)));

    // check the values
    cu_assert (cmdArgs.getSseHostname() == "barney");
    cu_assert (cmdArgs.getSsePort() == 8888);
    cu_assert (cmdArgs.getDxPort() == 8877);
    cu_assert (cmdArgs.getName() == "archiver8");

    cout << "test a bad bort number" << endl;
    const char *argvBadPort[] = 
    { "ProgName",
      "-host",
      "matrix",
      "-sse-port",
      "badportnumber",
    };
    const int argcBadPort = ARRAY_LENGTH(argvBadPort);
    cu_assert(cmdArgs.parseArgs(argcBadPort, const_cast<char **>(argvBadPort)) == false);

}

Test *TestDxArchiver::suite ()
{
	TestSuite *testSuite = new TestSuite("TestDxArchiver");

	testSuite->addTest (new TestCaller <TestDxArchiver> ("testFoo", &TestDxArchiver::testFoo));
	testSuite->addTest (new TestCaller <TestDxArchiver> ("testPrintIntrinsics", &TestDxArchiver::testPrintIntrinsics));
	testSuite->addTest (new TestCaller <TestDxArchiver> (" testDxArchiverCmdLineArgs", &TestDxArchiver:: testDxArchiverCmdLineArgs));

	return testSuite;
}