/*******************************************************************************

 File:    Testlib.cpp
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


#include <ace/OS.h>

#include "TestRunner.h"
#include "Testlib.h"
#include "NssProxy.h"
#include "TclProxy.h"
#include "SseUtil.h"
#include "SseCommUtil.h"
#include "SseArchive.h"
#include "Log.h"
#include "StreamMutex.h"
#include "Verbose.h"
#include "ast_const.h"
#include "IfDbOffsetTable.h"

/*
  Include these headers to at least test for compilation errors.
*/
#include "NssAcceptHandler.h"
#include "Timeout.h"

using std::string;
#include <sstream>
#include <stdio.h>

using namespace std;


class TclProxyTest : public TclProxy
{
      virtual void handleIncomingMessage(const string &message) {};
};


void Testlib::setUp ()
{
}

void Testlib::tearDown()
{
}

void Testlib::testFoo()
{
    cout << "testFoo" << endl;
    //cu_assert(false);  // force failure  
}

void Testlib::testUtils()
{
    cout << "testUtils" << endl;

    // test getHostIpAddr
    cout << "get host IP addr: ";
    cout << SseCommUtil::getHostIpAddr(SseUtil::getHostname()) << endl;

    cout << "get localhost IP addr" << endl;
    string localHostIpAddr = SseCommUtil::getHostIpAddr("localhost");
    cout << "localhost ip addr: " << localHostIpAddr << endl;
    cu_assert(localHostIpAddr == "127.0.0.1");

    // try an invalid host
    // update this when we get proper error handling in the routine
    //cout << "invalid host test ip: ";
    //cout << SseCommUtil::getHostIpAddr("thisIsABadHostname") << endl;


    SseException except("exception test", __FILE__, __LINE__);
    cout << except << endl;

}

void Testlib::testGetSseArchiveDir()
{
    cout << "testGetSseArchiveDir" << endl;

    // Unset the sse archive variable so we can control the tests below.
    // According to the putenv man page, the arg must NOT be
    // an automatic (stack) variable, so make it static.
    static const char *emptyEnvString("SSE_ARCHIVE=");
    putenv(const_cast<char *>(emptyEnvString));

    // get the default archive directory
    string defaultArchiveDir = SseArchive::getArchiveDir();
    cout << "default SSE Archive Dir: " << defaultArchiveDir << endl;

    // at a minimum, make sure it's not empty
    cu_assert (defaultArchiveDir != "");

    // now set the sse env var and make sure we get the 
    // same thing back 
    string putArchiveDir="/foo/bar/testArchiveDir";
    static const char *newEnvString = "SSE_ARCHIVE=/foo/bar/testArchiveDir";
    cout << "setting archive dir: " << newEnvString << endl;
    putenv(const_cast<char *>(newEnvString));

    string getArchiveDir = SseArchive::getArchiveDir();
    cout << "fetched SSE Archive Dir: " << getArchiveDir << endl;

    // note that a trailing slash is automatically appended
    // on the 'get'
    cu_assert (getArchiveDir  == putArchiveDir + "/");


}



void Testlib::testLog()
{
    cout << "testLog" << endl;

    string logfile("/tmp/testLog.txt");
    remove(logfile.c_str());     // make sure it's not already there

    ACE_Recursive_Thread_Mutex mutex;

    // send text to the log
    string outbuff("a test of Log output\nline2\n");
    Log(logfile, mutex) << outbuff << endl;

    // read it back
    string inbuff = SseUtil::readFileIntoString(logfile);

    cout << "wrote: " << outbuff << endl;
    cout << "read: " << inbuff << endl;

    // compare what was written to what came back.
    // output may have extra stuff (timestamps etc.), so
    // make sure the written text is at least there as a
    //substring

    cu_assert(inbuff.find(outbuff) != std::string::npos); 

    // clean up
    remove(logfile.c_str());    

}


void Testlib::testStreamMutex()
{
    cout << "testStreamMutex" << endl;

    stringstream strm;
    ACE_Recursive_Thread_Mutex mutex;

    // send text to the log
    string outbuff("a test of StreamMutex output\nline2\n");
    
    StreamMutex(strm, mutex) << outbuff;

    // read it back
    string inbuff = strm.str();

    cout << "wrote: '" << outbuff << "'" << endl;
    cout << "read: '" << inbuff << "'" << endl;

    // compare what was written to what came back.
    cu_assert(inbuff == outbuff);


}

void Testlib::testVerbose()
{
    cout << "testVerbose" << endl;

    // test the verbose macros to make sure they activate
    // when the proper verbose level is set

    for (int verboseLevel=0; verboseLevel <=2; ++verboseLevel)
    {
	cout << "verbose level: " << verboseLevel << endl;
	VERBOSE0(verboseLevel, 
		 "verbose level 0 msg is activated" << endl);

	VERBOSE1(verboseLevel, 
	    "verbose level 1 msg is activated\n");

	VERBOSE2(verboseLevel, 
	    "verbose level 2 msg is activated" << endl);

    }
}

void Testlib::testTclParse()
{
    cout << "testTclParse" << endl;

    TclProxy *tclProxy = new TclProxyTest;

    try {

	stringstream msg1;
	string delimit("\n");

	// test an int field
	string field1Key("field1 =");
	int field1Value = 321;
	msg1 << field1Key << field1Value << delimit;
	// cout << "msg=" << msg.str() << endl;

	int field1back = tclProxy->parseInt(msg1.str(), field1Key, delimit);
	cout << "field1back=" << field1back << endl;
	cu_assert(field1back == field1Value);

	// test a double field

	stringstream msg2;
	string field2Key("field2 =");
	double field2Value = 12.2;

	msg2 << field2Key << field2Value << delimit;
	double field2back = tclProxy->parseDouble(msg2.str(), field2Key, delimit);
	double tol = 0.01;
	cout << "field2back=" << field2back << endl;
	assertDoublesEqual(field2Value, field2back, tol);


	// test a text field
	stringstream msg3;
	string field3Key("field3 = ");
	string field3Value("fred");

	msg3 << field3Key << "    " << field3Value << delimit;
	string field3back =  tclProxy->parseElement(msg3.str(), field3Key, delimit);
	cout << "field3back is: '" << field3back << "'" << endl;
	cu_assert(field3Value == field3back);

	// test an invalid key, should throw
	try {

	   stringstream msg;
	   string badKey("badkey =");
	   string badKeyBack = tclProxy->parseElement(msg.str(), badKey, delimit);

	    // shouldn't get here
	    cu_assert(0);
	}
	catch (const string &errorString)
	{
	    const string expectedResult("key not found");
	   if (errorString.find(expectedResult) == string::npos) 
	   {
	      throw string("did not find string in error message: "
		 + expectedResult + " errorString is: " + errorString);
	   }

	   //cout << "caught correct error as expected: " << errorString << endl;
	}

	try {
	    // test a bad int value, should throw
	   stringstream msg4;
	   string field4Key("field4 = ");
	   string field4Value("badvalue");
	   msg4 << field4Key << field4Value << delimit;
	   tclProxy->parseInt(msg4.str(), field4Key, delimit);
	    
	   // shouldn't get here
	   cu_assert(0);
	}
	catch (const SseException &except)
	{
	   //cout << "caught error as expected: " << except << endl;
	}

	// test a value that is whitespace only (ie, value not found)
	// should throw
	try {
	   // test a text field
	   stringstream msg5;
	   string field5Key("field5 = ");
	   
	   msg5 << field5Key << " " << delimit;
	   tclProxy->parseElement(msg5.str(), field5Key, delimit);

	   cu_assert(0); // shouldn't get here
	}
	catch (const string & errorString)
	{
	   const string expectedResult("no value found for key");
	   if (errorString.find(expectedResult) == string::npos) 
	   {
	      throw string("did not find string in error message: "
			   + expectedResult);
	   }

	   //cout << "caught field5 error as expected" << endl;
	   //cout << errorString << endl;
	}


	// test missing delimiter.
	// should throw
	try {
	   // test a text field
	   stringstream msg6;
	   string field6Key("field6 = ");
	   
	   string badDelimit("bad-delimit");
	   msg6 << field6Key << " " << badDelimit;
	   tclProxy->parseElement(msg6.str(), field6Key, delimit);

	   cu_assert(0); // shouldn't get here
	}
	catch (const string & errorString)
	{
	   const string expectedResult("delimiter not found");
	   if (errorString.find(expectedResult) == string::npos) 
	   {
	      throw string("did not find string in error message: "
			   + expectedResult);
	   }

	   //cout << "caught field6 error as expected" << endl;
	   //cout << errorString << endl;
	}



    }
    catch (const SseException &except)
    {
	cout << "caught unexpected testTclParse SseException: " << except << endl;
	cu_assert(0);
    }
    catch (const string &errorString)
    {
	cout << "caught unexpected testTclParse string exception: " << errorString << endl;
	cu_assert(0);
    }
    catch (...)
    {
	cout << "caught unexpected testTclParse generic exception: " << endl;
	cu_assert(0);
    }

    delete tclProxy;
}


void Testlib::testIfDbOffsetTable()
{
    cout << "testIfDbOffsetTable" << endl;

    string filename("/tmp/ifDbOffsetTestFile.txt");
    remove(filename.c_str());     // make sure it's not already there

    // open an output text stream attached to a file
    ofstream strm;
    strm.open(filename.c_str(), (ios::app));
    cu_assert(strm.is_open());

    // create db offset lookup table 
    // <lowerfreqMhz> <upperfreqMhz> <dbOffsetLeft> <dbOffsetRight>
    strm << "# Test config file for IfDbOffsetTable" << endl;
    strm << "" << endl;  // blank line

    strm << "1000 10 -10\n"
         << "2000 20 -5\n"
         << "3000 35 10\n"
         << "5000 40\t 32\t\n"  // extra tabs should be ok
         << "4000 15 8\n" // add out of freq order, should be ok
         << endl;

    // invalid atten offset numbers, should generate warnings
    strm << "5000 xxx 10" << endl;
    strm << "5000 11 yyy" << endl;

    //  not enough tokens, should generate warning
    strm << "10000 3" << endl;

    strm.close();

    // read it back
    string inbuff = SseUtil::readFileIntoString(filename);
    cout << "read: " << endl
	 << inbuff << endl;

    // load it
    IfDbOffsetTable *table = new IfDbOffsetTable(filename);
    cout << *table;

    // try some lookups
    double skyfreqMhz;
    int dbOffset;
    int expectedDbOffset;
    
    // -------------
    // match first freq exactly
    skyfreqMhz = 1000;

    //left
    expectedDbOffset = 10;
    dbOffset = table->getDbOffsetLeft(skyfreqMhz);
    assertLongsEqual(expectedDbOffset, dbOffset);

    //right
    expectedDbOffset = -10;
    dbOffset = table->getDbOffsetRight(skyfreqMhz);
    assertLongsEqual(expectedDbOffset, dbOffset);

    // -------------
    // halfway between first two values
    skyfreqMhz = 1500;

    //left
    expectedDbOffset = 15;
    dbOffset = table->getDbOffsetLeft(skyfreqMhz);
    assertLongsEqual(expectedDbOffset, dbOffset);

    //right
    expectedDbOffset = -7;  // truncated
    dbOffset = table->getDbOffsetRight(skyfreqMhz);
    assertLongsEqual(expectedDbOffset, dbOffset);

    // ------------
    // before first freq (outside of table)
    skyfreqMhz = 10;
    expectedDbOffset = 0;  
    dbOffset = table->getDbOffsetLeft(skyfreqMhz);
    assertLongsEqual(expectedDbOffset, dbOffset);

    
    // -------------
    // 1/4 way between last 2 values
    skyfreqMhz = 4250;

    //left
    expectedDbOffset = 21;
    dbOffset = table->getDbOffsetLeft(skyfreqMhz);
    assertLongsEqual(expectedDbOffset, dbOffset);

    //right
    expectedDbOffset = 14;  
    dbOffset = table->getDbOffsetRight(skyfreqMhz);
    assertLongsEqual(expectedDbOffset, dbOffset);

    // ------------
    // after last freq (outside of table)
    skyfreqMhz = 50000;
    expectedDbOffset = 0;  
    dbOffset = table->getDbOffsetLeft(skyfreqMhz);
    assertLongsEqual(expectedDbOffset, dbOffset);

    // clean up
    delete table;
    remove(filename.c_str());    
}

void Testlib::testValidAtaSubarray()
{
    cout << "testValidAtaSubarray" << endl;

    string errorText("");

    // single ant 
    cu_assert(SseCommUtil::validAtaSubarray("ant3d", errorText));

    // 2 ants, comma separated 
    cu_assert(SseCommUtil::validAtaSubarray("ant3d,ant3e", errorText));

    // 2 ants, comma separated, no 'ant' prefix 
    cu_assert(SseCommUtil::validAtaSubarray("3d,3e", errorText));

    // invalid separator
    cu_assert(! SseCommUtil::validAtaSubarray("ant3d/ant3e", errorText));
    cout << errorText << endl;
    cu_assert(errorText.find("Invalid character '/'") != string::npos);

    // duplicate ant
    cu_assert(! SseCommUtil::validAtaSubarray("ant2d,ant3d,ant2d", errorText));
    cout << errorText << endl;
    cu_assert(errorText.find("Duplicate ant name: ant2d") != string::npos);

    // spaces
    cu_assert(! SseCommUtil::validAtaSubarray("ant4f, ant4g", errorText));
    cout << errorText << endl;
    cu_assert(errorText.find("Invalid character ' '") != string::npos);

    // ant name does not start with 'ant'
    cu_assert(! SseCommUtil::validAtaSubarray("ant4f,badant4g", errorText));
    cout << errorText << endl;
    cu_assert(errorText.find("Invalid ant name") != string::npos);

    // ant name does not contain digit
    cu_assert(! SseCommUtil::validAtaSubarray("antnodigitf", errorText));
    cout << errorText << endl;
    cu_assert(errorText.find("must contain a digit: antnodigitf")
              != string::npos);

    // empty string
    cu_assert(! SseCommUtil::validAtaSubarray("", errorText));
    cout << errorText << endl;
    cu_assert(errorText.find("Ant list is empty")
              != string::npos);

    cu_assert(SseCommUtil::validAtaSubarray("antgroup", errorText));

}


Test *Testlib::suite ()
{
    TestSuite *testSuite = new TestSuite("Testlib");

    testSuite->addTest (new TestCaller <Testlib> ("testVerbose", &Testlib::testVerbose));

    testSuite->addTest (new TestCaller <Testlib> ("testLog", &Testlib::testLog));
    testSuite->addTest (new TestCaller <Testlib> ("testStreamMutex", &Testlib::testStreamMutex));
    testSuite->addTest (new TestCaller <Testlib> ("testUtils", &Testlib::testUtils));
    testSuite->addTest (new TestCaller <Testlib> ("testGetSseArchiveDir", &Testlib::testGetSseArchiveDir));
    testSuite->addTest (new TestCaller <Testlib> ("testTclParse", &Testlib::testTclParse));
    testSuite->addTest (new TestCaller <Testlib> ("testIfDbOffsetTable", &Testlib::testIfDbOffsetTable));
    testSuite->addTest (new TestCaller <Testlib> ("testValidAtaSubarray", &Testlib::testValidAtaSubarray));
    testSuite->addTest (new TestCaller <Testlib> ("testFoo", &Testlib::testFoo));

    
    return testSuite;
}