/*******************************************************************************

 File:    TestSseInterfaceLib.cpp
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
#include "sseInterfaceLib.h"
#include "TestSseInterfaceLib.h"
#include "Signals.h"
#include <iostream>
#include <cstring>

using namespace std;

void TestSseInterfaceLib::setUp ()
{
}

void TestSseInterfaceLib::tearDown()
{
}

// verify that all of the interface typedef'd words
// are the expected size

void TestSseInterfaceLib::testWordSizes()
{
    cout << "testWordSizes" << endl;

    cu_assert(1 == sizeof(char8_t));
    cu_assert(1 == sizeof(int8_t));
    cu_assert(1 == sizeof(uint8_t));
    cu_assert(4 == sizeof(int32_t));
    cu_assert(4 == sizeof(uint32_t));
    cu_assert(4 == sizeof(float32_t));
    cu_assert(8  == sizeof(float64_t));
}

void TestSseInterfaceLib::testPrint()
{

    cout << "testPrint" << endl;

    SseInterfaceHeader hdr;
    cout << hdr;

    NssMessage nssMessage;

    cout << "default NssMessage: " << endl << nssMessage << endl;

    nssMessage.code = 0;
    nssMessage.severity = SEVERITY_INFO;
    strcpy(nssMessage.description,"testPrint test of NssMessage");
    cout << nssMessage;

    cu_assert(SseMsg::nssMessageSeverityToString(SEVERITY_INFO) == "Info");

    nssMessage.severity = SEVERITY_FATAL;
    cout << nssMessage;


    // test NssMessage constructor
    uint32_t code = 27;
    NssMessageSeverity severity = SEVERITY_WARNING;
    string descrip("this is a test");
    NssMessage nssMessage2(code, severity, descrip.c_str());
    cu_assert(nssMessage2.code == code);
    cu_assert(nssMessage2.severity == severity);
    cu_assert(nssMessage2.description == descrip);


    Polarization pol(POL_RIGHTCIRCULAR);
    cout << "pol: " << SseMsg::polarizationToString(pol) << endl;
    cu_assert(SseMsg::polarizationToString(pol) == "right");
    cu_assert(SseMsg::polarizationToSingleUpperChar(pol) == 'R');
    cu_assert(SseMsg::stringToPolarization("right") == POL_RIGHTCIRCULAR);

    pol = POL_LEFTCIRCULAR;
    cu_assert(SseMsg::polarizationToString(pol) == "left");
    cu_assert(SseMsg::polarizationToSingleUpperChar(pol) == 'L');
    cu_assert(SseMsg::stringToPolarization("left") == POL_LEFTCIRCULAR);

    pol = POL_XLINEAR;
    cu_assert(SseMsg::polarizationToString(pol) == "xlin");
    cu_assert(SseMsg::polarizationToSingleUpperChar(pol) == 'X');
    cu_assert(SseMsg::stringToPolarization("xlin") == POL_XLINEAR);


    pol = (Polarization) 99999;  // bad pol
    cout << "test bad pol to string: " <<  SseMsg::polarizationToString(pol) << endl;

    // bad string to pol
    cu_assert(SseMsg::stringToPolarization("badpol") == POL_UNINIT);


    SiteId siteId = SITE_ID_UNINIT;
    cout << "Site id: " <<  SseMsg::getSiteIdName(siteId) << endl;
    
    siteId = SITE_ID_ARECIBO;
    cout << "Site id: " <<  SseMsg::getSiteIdName(siteId) << endl;

    siteId = SITE_ID_ATA;
    cout << "Site id: " <<  SseMsg::getSiteIdName(siteId) << endl;
}

void TestSseInterfaceLib::testDates()
{
    cout << "testDates: " << endl;

    NssDate defaultDate;
    cout << "default date: " << defaultDate << endl; 

    cout << "current NssDate: ";
    NssDate nssDate = SseMsg::currentNssDate();
    cout << "sec: " << nssDate.tv_sec <<
	" usec: " << nssDate.tv_usec << endl;

    cout << "current NssDate in iso format: " <<
	SseMsg::isoDateTime(nssDate) << endl;

    cout << "date using operator <<: " << nssDate << endl;

}


void TestSseInterfaceLib::testHostNetByteorder()
{
    cout << "testHostNetByteorder: " << endl;

#ifdef WORDS_BIGENDIAN
    cout << "host is big endian" << endl;
#else
    cout << "host is little endian" << endl;
#endif

    // convert from host to net order & back, then compare
    float hostValueFloat = 1234.5;
    float netValueFloat = htonf(hostValueFloat);
    float backFloat = ntohf(netValueFloat);
    assertDoublesEqual(hostValueFloat, backFloat,  0.01);

    double hostValueDouble = 1234.5;
    double netValueDouble = htond(hostValueDouble);
    double backDouble = ntohd(netValueDouble);
    assertDoublesEqual(hostValueDouble, backDouble, 0.01);

}

void TestSseInterfaceLib::testSignals()
{
    cout << "testSignals" << endl;

    Tone tone;

    cout << "Tone: \n" <<  tone << endl;

    DriftingTone driftTone;

    cout << "DriftTone \n" << driftTone << endl;

    PulsedTone pulsedTone;
    
    cout << "PulsedTone\n" << pulsedTone << endl;
}



Test *TestSseInterfaceLib::suite ()
{
	TestSuite *testSuite = new TestSuite("TestSseInterfaceLib");

	testSuite->addTest (new TestCaller <TestSseInterfaceLib> ("testWordSizes", &TestSseInterfaceLib::testWordSizes));

	testSuite->addTest (new TestCaller <TestSseInterfaceLib> ("testPrint", &TestSseInterfaceLib::testPrint));

	testSuite->addTest (new TestCaller <TestSseInterfaceLib> ("testDates", &TestSseInterfaceLib::testDates));

	testSuite->addTest (new TestCaller <TestSseInterfaceLib> ("testHostNetByteorder", &TestSseInterfaceLib::testHostNetByteorder));

	testSuite->addTest (new TestCaller <TestSseInterfaceLib> ("testSignals", &TestSseInterfaceLib::testSignals));

	return testSuite;
}