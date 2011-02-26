/*******************************************************************************

 File:    TestDx.cpp
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

#include "TestDx.h"
#include "Dx.h"
#include "SseProxy.h"
#include "DxCmdLineArgs.h"
#include "ArrayLength.h"

void TestDx::setUp ()
{
}

void TestDx::tearDown()
{
}

void TestDx::testFoo()
{
    cout << "testFoo" << endl;
    //cu_assert(false);  // force failure  
}

// verify we can print intrinsics info
void TestDx::testPrintIntrinsics()
{
    ACE_SOCK_STREAM dummyStream;
    SseProxy proxy(dummyStream);

    Dx dx(&proxy);
    //DxDebug dx(&proxy);

    // test the dx
    dx.printIntrinsics();


}

void TestDx::testDxCmdLineArgs()
{
    string progName("TestDxCmdLineArgs");
    int defaultMainPort(9999);
    int defaultRemotePort(5555);
    string defaultHost("thisHost");
    string defaultSciDataDir("fooDir");
    string defaultSciDataPrefix("nss.p10");
    bool defaultBroadcast(false);
    bool defaultNoUi(false);
    bool defaultVaryOutputData(false);
    string defaultDxName("dxsim33");
    bool defaultRemoteMode(false);

    DxCmdLineArgs cmdArgs(progName, 
			   defaultMainPort, defaultRemotePort,
			   defaultHost,
			   defaultSciDataDir, defaultSciDataPrefix,
			   defaultBroadcast,
			   defaultNoUi,
			   defaultVaryOutputData,
			   defaultDxName,
			   defaultRemoteMode);


    // test usage message output
    cmdArgs.usage();
    cerr << endl;

    // check the defaults
    cu_assert (cmdArgs.getMainPort() == 9999);
    cu_assert (cmdArgs.getRemotePort() == 5555);
    cu_assert (cmdArgs.getHostName() == "thisHost");
    cu_assert (cmdArgs.getSciDataDir() == "fooDir");
    cu_assert (cmdArgs.getSciDataPrefix() == "nss.p10");
    cu_assert (! cmdArgs.getBroadcast());
    cu_assert (! cmdArgs.getNoUi());
    cu_assert (! cmdArgs.getVaryOutputData());
    cu_assert (cmdArgs.getDxName() == "dxsim33");
    cu_assert (! cmdArgs.getRemoteMode());


    // try setting good parameters
    const char *argv[] = 
    { "ProgName",
      "-host", "matrix",
      "-mainport", "8888",
      "-remport", "9999",
      "-sddir", "../scienceData",
      "-sdprefix", "nss.p6",
      "-broadcast",
      "-name", "dxsim127",
      "-remote",
      "-noui",
      "-vary"
    };
    const int argc = ARRAY_LENGTH(argv);

    // verify that everything parses
    cu_assert(cmdArgs.parseArgs(argc, const_cast<char **>(argv)));

    // check the values
    cu_assert (cmdArgs.getHostName() == "matrix");
    cu_assert (cmdArgs.getMainPort() == 8888);
    cu_assert (cmdArgs.getRemotePort() == 9999);
    cu_assert (cmdArgs.getSciDataDir() == "../scienceData");
    cu_assert (cmdArgs.getSciDataPrefix() == "nss.p6");
    cu_assert (cmdArgs.getBroadcast());
    cu_assert (cmdArgs.getDxName() == "dxsim127");
    cu_assert (cmdArgs.getRemoteMode());
    cu_assert (cmdArgs.getNoUi());
    cu_assert (cmdArgs.getVaryOutputData());

    cout << "Test a bad port number:" << endl;
    const char *argvBadPort[] = 
    { "ProgName",
      "-host",
      "matrix",
      "-mainport",
      "badportnumber",
    };
    const int argcBadPort = ARRAY_LENGTH(argvBadPort);
    cu_assert(cmdArgs.parseArgs(argcBadPort, const_cast<char **>(argvBadPort)) == false);

}

Test *TestDx::suite ()
{
	TestSuite *testSuite = new TestSuite("TestDx");

	testSuite->addTest (new TestCaller <TestDx> ("testFoo", &TestDx::testFoo));
	testSuite->addTest (new TestCaller <TestDx> ("testPrintIntrinsics", &TestDx::testPrintIntrinsics));
	testSuite->addTest (new TestCaller <TestDx> (" testDxCmdLineArgs", &TestDx:: testDxCmdLineArgs));

	return testSuite;
}