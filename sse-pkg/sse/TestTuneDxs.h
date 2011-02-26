/*******************************************************************************

 File:    TestTuneDxs.h
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


#ifndef TEST_TUNE_DXS_H
#define TEST_TUNE_DXS_H

#include "DebugLog.h"  // keep this early in the headers for VERBOSE macros

#include "TestCase.h"
#include "TestSuite.h"
#include "TestCaller.h"
#include "DxList.h"
#include "NssComponentManager.h"
class DummySubscriber;

class TestTuneDxs : public TestCase
{
 public:
    TestTuneDxs (std::string name) : TestCase (name) {}

    void setUp ();
    void tearDown();
    static Test *suite ();

 protected:

    void testTuneDxsRange();
    void testTuneDxsObsRange();
    void testTuneDxsObsRangeWithJump();
    void testTuneDxsObsRangeWithEarlyJump();

    void checkForExpectedDxSkyFreqValues(
	DxList &dxList,
	std::list<double> &expectedSkyFreqList,
	double freqTol);

    DummySubscriber *dummySubscriber_;
    NssComponentManager<DxProxy> *dxManager_;
    DxList dxList_;

    int32_t maxSubchannels_;
    float64_t hzPerSubchannel_;
    float64_t bandWidth_;

};


#endif