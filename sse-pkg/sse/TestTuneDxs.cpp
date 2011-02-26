/*******************************************************************************

 File:    TestTuneDxs.cpp
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
#include "TestTuneDxs.h"
#include "TuneDxs.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "NssComponentManager.h"
#include "DxProxy.h"
#include <string>
#include <list>

class DummySubscriber : public Subscriber 
{
public:
    DummySubscriber();
    ~DummySubscriber();
    virtual void update(Publisher *changedPublisher);
private:

};

DummySubscriber::DummySubscriber() {}
DummySubscriber::~DummySubscriber() {}
void DummySubscriber::update(Publisher *changedPublisher)
{
    // do nothing
}

static const int TestTuneVerboseLevel(0);

void TestTuneDxs::setUp ()
{
    dummySubscriber_ = new DummySubscriber;;
    dxManager_ = new NssComponentManager<DxProxy>(dummySubscriber_);
    maxSubchannels_ = 3027;
    hzPerSubchannel_ = 533.333;
    bandWidth_ = (maxSubchannels_ * hzPerSubchannel_)/1000000.0;

    // make a list of dxs

    int ndxs = 5;
    for (int i=0; i< ndxs; ++i)
    {
	DxProxy *dxProxy = new DxProxy(dxManager_);

        // give the dx a name
        stringstream name;
        name << "dxtest" << i;
        dxProxy->setName(name.str());

	// set the bandwidth (equiv to 1.6 Mhz)
	dxProxy->setIntrinsicsBandwidth(maxSubchannels_, hzPerSubchannel_);

	// cout << "dx bw = " << dxProxy->getBandwidthInMHz() << endl;

	dxList_.push_back(dxProxy);
    }

}

void TestTuneDxs::tearDown()
{
    dxList_.clear();
    delete dxManager_;
    delete dummySubscriber_;
}


void TestTuneDxs::checkForExpectedDxSkyFreqValues(
    DxList &dxList,
    list<double> &expectedSkyFreqList,
    double freqTolMhz)
{
    cout << "checkForExpectedDxSkyFreqValues" << endl;

    cu_assert(dxList.size() == expectedSkyFreqList.size());


    list<double>::iterator expectedIt = expectedSkyFreqList.begin();

    // check for the expected values
    for (DxList::iterator dxIt = dxList.begin();
	 dxIt != dxList.end() && expectedIt != expectedSkyFreqList.end();
	 ++dxIt, ++expectedIt)
    {
	DxProxy *dxProxy = *dxIt;

	stringstream strm;
	strm.precision(6);           // show N places after the decimal (6=hz level)
	strm.setf(std::ios::fixed);  // show all decimal places up to precision

	strm << dxProxy->getName() << " " 
	     << dxProxy->getDxSkyFreq() << " MHz" << endl;
	
	cout << strm.str();

	double expectedFreq = *expectedIt;
	double actualFreq = dxProxy->getDxSkyFreq();

	cout << "expected: " << expectedFreq
	     << " actual: " << actualFreq << endl;

	assertDoublesEqual(expectedFreq, actualFreq, freqTolMhz);

    }
    cout << endl;
}

void TestTuneDxs::testTuneDxsRange()
{
    cout << "testTuneDxsRange" << endl;


    // tune the dxs for this range

    int verboseLevel = TestTuneVerboseLevel;
    double beginDxSkyFreqMhz = 1100;
    double endDxSkyFreqMhz = 1118;
    double dxRound = 0.1;
    double dxOverlap = 0.001;
    double dxTuningTolerance = 0.01;

    TuneDxsRangeCenterRound tuneDxs(
	verboseLevel,  Range(beginDxSkyFreqMhz, endDxSkyFreqMhz),
	dxRound, dxOverlap, dxTuningTolerance);

    double freqTolMhz = 0.1;
    list<double> expectedFreqValues;
    float64_t maxDxTuneSeparationMhz = 40;
    float64_t maxAllowedDxSkyFreqMhz = 20000;

    // --- test 1 ----
    tuneDxs.tune(dxList_, maxDxTuneSeparationMhz, maxAllowedDxSkyFreqMhz);

    expectedFreqValues.clear();
	expectedFreqValues.push_back(1100.81);
	expectedFreqValues.push_back(1102.42);
	expectedFreqValues.push_back(1104.04);
	expectedFreqValues.push_back(1105.65);
	expectedFreqValues.push_back(1107.26);

    checkForExpectedDxSkyFreqValues(dxList_, expectedFreqValues,
				     freqTolMhz);

    // --- test 2 ----
    tuneDxs.tune(dxList_, maxDxTuneSeparationMhz, maxAllowedDxSkyFreqMhz);

    expectedFreqValues.clear();
	expectedFreqValues.push_back(1108.88);
	expectedFreqValues.push_back(1110.49);
	expectedFreqValues.push_back(1112.11);
	expectedFreqValues.push_back(1113.72);
    expectedFreqValues.push_back(1115.34);

    checkForExpectedDxSkyFreqValues(dxList_, expectedFreqValues,
				     freqTolMhz);



}


void TestTuneDxs::testTuneDxsObsRange()
{
    cout << "testTuneDxsObsRange" << endl;

    int verboseLevel = TestTuneVerboseLevel;
    double beginDxSkyFreqMhz = 1100;
    double endDxSkyFreqMhz = 1120;
    double dxRound = 0.1;
    double dxOverlap = 0.001;
    double dxTuningTolerance = 0.01;

    ObsRange obsRange;
    obsRange.addOutOfOrder(beginDxSkyFreqMhz, endDxSkyFreqMhz);

    cout << "obsrange: " << obsRange << endl;

    TuneDxsObsRange tuneDxs(verboseLevel, obsRange,
			      dxRound, dxOverlap, dxTuningTolerance);

    double freqTolMhz = 0.1;
    list<double> expectedFreqValues;
    float64_t maxDxTuneSeparationMhz = 40;
    float64_t maxAllowedDxSkyFreqMhz = 20000;

    // --- test 1 ----
    tuneDxs.tune(dxList_, maxDxTuneSeparationMhz, maxAllowedDxSkyFreqMhz);

    expectedFreqValues.clear();
    expectedFreqValues.push_back(1100.8);
    expectedFreqValues.push_back(1102.4);
    expectedFreqValues.push_back(1104);
    expectedFreqValues.push_back(1105.6);
    expectedFreqValues.push_back(1107.2);

    checkForExpectedDxSkyFreqValues(dxList_, expectedFreqValues,
				     freqTolMhz);

    // --- test 2 ----
    tuneDxs.tune(dxList_, maxDxTuneSeparationMhz, maxAllowedDxSkyFreqMhz);

    expectedFreqValues.clear();
    expectedFreqValues.push_back(1108.8);
    expectedFreqValues.push_back(1110.4);
    expectedFreqValues.push_back(1112);
    expectedFreqValues.push_back(1113.6);
    expectedFreqValues.push_back(1115.2);

    checkForExpectedDxSkyFreqValues(dxList_, expectedFreqValues,
				     freqTolMhz);


}

void TestTuneDxs::testTuneDxsObsRangeWithJump()
{
    cout << "testTuneDxsObsRangeWithJump" << endl;

    int verboseLevel = TestTuneVerboseLevel;
    double beginDxSkyFreqMhz = 1100;
    double dxRound = 0.1;
    double dxOverlap = 0.001;
    double dxTuningTolerance = 0.01;

    float64_t maxDxTuneSeparationMhz = 40;
    float64_t maxAllowedDxSkyFreqMhz = 20000;

    ObsRange obsRange;
    obsRange.addOutOfOrder(beginDxSkyFreqMhz, 1107);
    obsRange.addOutOfOrder(1120, 1127);  // jump within maxDxSep
    obsRange.addOutOfOrder(1200, 1212);  // jump outside maxDxSep
    

    cout << "obsrange: " << obsRange << endl;

    TuneDxsObsRange tuneDxs(verboseLevel, obsRange,
			      dxRound, dxOverlap, dxTuningTolerance);

    double freqTolMhz = 0.1;
    list<double> expectedFreqValues;

    // --- test 1 ----
    tuneDxs.tune(dxList_, maxDxTuneSeparationMhz, maxAllowedDxSkyFreqMhz);

    expectedFreqValues.clear();
    expectedFreqValues.push_back(1100.8);
    expectedFreqValues.push_back(1102.4);
    expectedFreqValues.push_back(1104);
    expectedFreqValues.push_back(1105.6);
    expectedFreqValues.push_back(1120.8);

    checkForExpectedDxSkyFreqValues(dxList_, expectedFreqValues,
				     freqTolMhz);

    // --- test 2 ----
    // try the next set of freqs, they should continue
    // at the point where the last ones ended
    tuneDxs.tune(dxList_, maxDxTuneSeparationMhz, maxAllowedDxSkyFreqMhz);

    expectedFreqValues.clear();
    expectedFreqValues.push_back(1122.4);
    expectedFreqValues.push_back(1124);
    expectedFreqValues.push_back(1125.6);
    expectedFreqValues.push_back(-1);
    expectedFreqValues.push_back(-1);

    checkForExpectedDxSkyFreqValues(dxList_, expectedFreqValues,
				     freqTolMhz);



    // -------------------------------

    // --- test 3 ----
    // try the next set of freqs after the big jump

    maxAllowedDxSkyFreqMhz = 1208;
    tuneDxs.tune(dxList_, maxDxTuneSeparationMhz, maxAllowedDxSkyFreqMhz);

    expectedFreqValues.clear();
    expectedFreqValues.push_back(1200.8);  
    expectedFreqValues.push_back(1202.4); 
    expectedFreqValues.push_back(1204);
    expectedFreqValues.push_back(1205.6);
    expectedFreqValues.push_back(1207.2);    // maxallowed cutoff
    checkForExpectedDxSkyFreqValues(dxList_, expectedFreqValues,
				     freqTolMhz);

}

void TestTuneDxs::testTuneDxsObsRangeWithEarlyJump()
{
    cout << "testTuneDxsObsRangeWithEarlyJump" << endl;


    int verboseLevel = TestTuneVerboseLevel;
    double dxRound = 0.1;
    double dxOverlap = 0.001;
    double dxTuningTolerance = 0.01;

    float64_t maxDxTuneSeparationMhz = 40;
    float64_t maxAllowedDxSkyFreqMhz = 20000;

    ObsRange obsRange;
    obsRange.addOutOfOrder(1885, 1886);  // too small a section to use
    obsRange.addOutOfOrder(2040, 2060);  // jump outside maxDxSep

    cout << "obsrange: " << obsRange << endl;

    TuneDxsObsRange tuneDxs(verboseLevel, obsRange,
			      dxRound, dxOverlap, dxTuningTolerance);

    double freqTolMhz = 0.1;
    list<double> expectedFreqValues;

    // --- test 1 ----
    tuneDxs.tune(dxList_, maxDxTuneSeparationMhz, maxAllowedDxSkyFreqMhz);

    expectedFreqValues.clear();
    expectedFreqValues.push_back(2040.8);
    expectedFreqValues.push_back(2042.4);
    expectedFreqValues.push_back(2044);
    expectedFreqValues.push_back(2045.6);
    expectedFreqValues.push_back(2047.2);

    checkForExpectedDxSkyFreqValues(dxList_, expectedFreqValues,
				     freqTolMhz);


}





Test *TestTuneDxs::suite ()
{
	TestSuite *testSuite = new TestSuite("TestTuneDxs");

        testSuite->addTest (new TestCaller <TestTuneDxs> ("testTuneDxsRange", &TestTuneDxs::testTuneDxsRange));
        testSuite->addTest (new TestCaller <TestTuneDxs> ("testTuneDxsObsRange", &TestTuneDxs::testTuneDxsObsRange));
        testSuite->addTest (new TestCaller <TestTuneDxs> ("testTuneDxsObsRangeWithJump", &TestTuneDxs::testTuneDxsObsRangeWithJump));
        testSuite->addTest (new TestCaller <TestTuneDxs> ("testTuneDxsObsRangeWithEarlyJump", &TestTuneDxs::testTuneDxsObsRangeWithEarlyJump));

	return testSuite;
}







