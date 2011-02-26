/*******************************************************************************

 File:    TestRecentRfiMask.cpp
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
#include "TestRecentRfiMask.h"
#include "RecentRfiMask.h"
#include "SseException.h"
#include <iostream>

using namespace std;

TestRecentRfiMask::TestRecentRfiMask(string name) 
   : TestCase(name)
{}

void TestRecentRfiMask::setUp()
{
}

void TestRecentRfiMask::tearDown()
{

}

void TestRecentRfiMask::testFoo()
{
   //cu_assert(false);
}

void TestRecentRfiMask::testNoSignals()
{
   // empty signal list, should get empty mask back
   
   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(0.001);
   
   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;
   RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);
   
   cu_assert(maskCenterFreqMhz.size() == maskWidthMhz.size());
   cu_assert(maskCenterFreqMhz.size() == 0);

}

void TestRecentRfiMask::testOneSignal()
{
   // one signal, should get it back as the sole mask element

   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(0.001);

   signalFreqMhz.push_back(1420.001000);

   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;
   RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);

   cu_assert(maskCenterFreqMhz.size() == maskWidthMhz.size());
   cu_assert(maskCenterFreqMhz.size() == 1);

   double tolMhz(0.000001);

   double expectedMaskCenterFreqMhz(1420.001000);
   assertDoublesEqual(expectedMaskCenterFreqMhz,
		      maskCenterFreqMhz[0], tolMhz);

   double expectedMaskWidthMhz(minMaskElementWidthMhz);
   assertDoublesEqual(expectedMaskWidthMhz,
		      maskWidthMhz[0], tolMhz);

}



void TestRecentRfiMask::testMergeTwoSignals()
{
   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(0.001);

    // two signals within the min mask width.
    // should get merged into a single mask element.

   signalFreqMhz.push_back(1420.001000);
   signalFreqMhz.push_back(1420.001300);

   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;
   RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);

   cu_assert(maskCenterFreqMhz.size() == maskWidthMhz.size());
   cu_assert(maskCenterFreqMhz.size() == 1);

   double tolMhz(0.000001);

   double expectedMaskCenterFreqMhz(1420.001150);
   assertDoublesEqual(expectedMaskCenterFreqMhz,
		      maskCenterFreqMhz[0], tolMhz);

   double expectedMaskWidthMhz(0.001300);
   assertDoublesEqual(expectedMaskWidthMhz,
		      maskWidthMhz[0], tolMhz);

}

void TestRecentRfiMask::testTwoSignalsNoMerge()
{
   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(0.001);

   // two signals outside the min mask width.
   // should stay as separate mask elements.

   double signal0(1520.001000);
   double signal1(signal0 + 2 * minMaskElementWidthMhz);

   signalFreqMhz.push_back(signal0);
   signalFreqMhz.push_back(signal1);

   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;
   RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);

   cu_assert(maskCenterFreqMhz.size() == maskWidthMhz.size());
   cu_assert(maskCenterFreqMhz.size() == 2);

   double tolMhz(0.000001);

   double expectedMaskCenterFreqMhz(signal0);
   assertDoublesEqual(expectedMaskCenterFreqMhz,
		      maskCenterFreqMhz[0], tolMhz);

   double expectedMaskWidthMhz(minMaskElementWidthMhz);
   assertDoublesEqual(expectedMaskWidthMhz,
		      maskWidthMhz[0], tolMhz);

   expectedMaskCenterFreqMhz = signal1;
   assertDoublesEqual(expectedMaskCenterFreqMhz,
		      maskCenterFreqMhz[1], tolMhz);

   assertDoublesEqual(expectedMaskWidthMhz,
		      maskWidthMhz[1], tolMhz);

}

void TestRecentRfiMask::testMergeThreeSignals()
{
   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(0.001);

    // three signals within the min mask width.
    // should get merged into a single mask element.

   signalFreqMhz.push_back(1700.001000);
   signalFreqMhz.push_back(1700.001010);
   signalFreqMhz.push_back(1700.001499);

   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;
   RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);

   cu_assert(maskCenterFreqMhz.size() == maskWidthMhz.size());
   cu_assert(maskCenterFreqMhz.size() == 1);

   double tolMhz(0.0000001);

   double expectedMaskCenterFreqMhz(1700.0012495);
   assertDoublesEqual(expectedMaskCenterFreqMhz,
		      maskCenterFreqMhz[0], tolMhz);

   double expectedMaskWidthMhz(0.001499);
   assertDoublesEqual(expectedMaskWidthMhz,
		      maskWidthMhz[0], tolMhz);

}

void TestRecentRfiMask::testMergeMultipleSignalsInMultipleGroups()
{
   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(0.001);

   double group0StartFreqMhz(1800.001000);
   signalFreqMhz.push_back(group0StartFreqMhz);
   group0StartFreqMhz += 0.000450;
   signalFreqMhz.push_back(group0StartFreqMhz);
   group0StartFreqMhz += 0.000450;
   signalFreqMhz.push_back(group0StartFreqMhz);

   double group1StartFreqMhz(1805.000100);
   signalFreqMhz.push_back(group1StartFreqMhz);

   double group2StartFreqMhz(1810.001000);
   signalFreqMhz.push_back(group2StartFreqMhz);
   group2StartFreqMhz += 0.000020;
   signalFreqMhz.push_back(group2StartFreqMhz);
   group2StartFreqMhz += 0.000490;
   signalFreqMhz.push_back(group2StartFreqMhz);
   group2StartFreqMhz += 0.000490;
   signalFreqMhz.push_back(group2StartFreqMhz);


   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;
   RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);

   cu_assert(maskCenterFreqMhz.size() == maskWidthMhz.size());
   cu_assert(maskCenterFreqMhz.size() == 3);

   double tolMhz(0.0000001);

   // group0
   double expectedMaskCenterFreqMhz(1800.001450);
   assertDoublesEqual(expectedMaskCenterFreqMhz,
		      maskCenterFreqMhz[0], tolMhz);

   double expectedMaskWidthMhz(0.001900);
   assertDoublesEqual(expectedMaskWidthMhz,
		      maskWidthMhz[0], tolMhz);

   // group1
   expectedMaskCenterFreqMhz = group1StartFreqMhz;
   assertDoublesEqual(expectedMaskCenterFreqMhz,
		      maskCenterFreqMhz[1], tolMhz);

   expectedMaskWidthMhz = minMaskElementWidthMhz;
   assertDoublesEqual(expectedMaskWidthMhz,
		      maskWidthMhz[1], tolMhz);

   // group2

   expectedMaskCenterFreqMhz = 1810.001500;
   assertDoublesEqual(expectedMaskCenterFreqMhz,
		      maskCenterFreqMhz[2], tolMhz);

   expectedMaskWidthMhz = .002000;
   assertDoublesEqual(expectedMaskWidthMhz,
		      maskWidthMhz[2], tolMhz);

}

void TestRecentRfiMask::testSignalOutOfOrder()
{
   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(0.001);

   // insert signals in reverse freq order
   signalFreqMhz.push_back(1520.001000);
   signalFreqMhz.push_back(1522.001000);
   signalFreqMhz.push_back(1420.001000);

   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;

   // should throw exception
   try {
      RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);

      cu_assert(0);  // should not get here
   }
   catch (SseException &except)
   {
      string reason(except.descrip());
      cu_assert(reason.find("Signal frequency out of sorted order")
		!= string::npos);
   }

}

void TestRecentRfiMask::testNegativeSignalFreq()
{
   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(0.001);

   signalFreqMhz.push_back(-1520.001000);

   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;

   // should throw exception
   try {
      RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);

      cu_assert(0);  // should not get here
   }
   catch (SseException &except)
   {
      string reason(except.descrip());
      cu_assert(reason.find("Negative signal frequency") != string::npos);
   }

}

void TestRecentRfiMask::testMinElementWidthTooSmall()
{
   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(-0.001);
   signalFreqMhz.push_back(1520.001000);

   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;

   // should throw exception
   try {
      RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);

      cu_assert(0);  // should not get here
   }
   catch (SseException &except)
   {
      string reason(except.descrip());
      cu_assert(reason.find("Minimum mask element width is <= 0.0") 
		!= string::npos);
   }

}


void TestRecentRfiMask::testRepeatedSignals()
{
   // one signal, repeated.
   // should get it back as the sole mask element

   vector<double> signalFreqMhz;
   double minMaskElementWidthMhz(0.001);

   double freqMhz(1420.001000);
   signalFreqMhz.push_back(freqMhz);
   signalFreqMhz.push_back(freqMhz);
   signalFreqMhz.push_back(freqMhz * 100 / 100);

   vector<double> maskCenterFreqMhz;
   vector<double> maskWidthMhz;
   RecentRfiMask::createMask(signalFreqMhz, minMaskElementWidthMhz,
			     maskCenterFreqMhz, maskWidthMhz);

   cu_assert(maskCenterFreqMhz.size() == maskWidthMhz.size());
   cu_assert(maskCenterFreqMhz.size() == 1);

   double tolMhz(0.000001);

   double expectedMaskCenterFreqMhz(freqMhz);
   assertDoublesEqual(expectedMaskCenterFreqMhz,
		      maskCenterFreqMhz[0], tolMhz);

   double expectedMaskWidthMhz(minMaskElementWidthMhz);
   assertDoublesEqual(expectedMaskWidthMhz,
		      maskWidthMhz[0], tolMhz);

}



Test *TestRecentRfiMask::suite()
{
	TestSuite *testSuite = new TestSuite("TestRecentRfiMask");

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testFoo", &TestRecentRfiMask::testFoo));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testNoSignals", &TestRecentRfiMask::testNoSignals));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testOneSignal", &TestRecentRfiMask::testOneSignal));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testTwoSignalsNoMerge", &TestRecentRfiMask::testTwoSignalsNoMerge));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testMergeTwoSignals", &TestRecentRfiMask::testMergeTwoSignals));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testMergeThreeSignals", &TestRecentRfiMask::testMergeThreeSignals));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testMergeMultipleSignalsInMultipleGroups", &TestRecentRfiMask::testMergeMultipleSignalsInMultipleGroups));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testSignalOutOfOrder", &TestRecentRfiMask::testSignalOutOfOrder));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testNegativeSignalFreq", &TestRecentRfiMask::testNegativeSignalFreq));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testRepeatedSignals", &TestRecentRfiMask::testRepeatedSignals));

        testSuite->addTest(new TestCaller <TestRecentRfiMask>("testMinElementWidthTooSmall", &TestRecentRfiMask::testMinElementWidthTooSmall));

	return testSuite;
}