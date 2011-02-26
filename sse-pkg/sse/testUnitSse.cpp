/*******************************************************************************

 File:    testUnitSse.cpp
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

// Test unit for sse package

#include "TestSite.h"
#include "TestMisc.h"
#include "TestTuneDxs.h"
#include "TestTarget.h"
#include "TestOffPositions.h"
#include "TestRecentRfiMask.h"
#include "TestPosition.h"
#include "TestSiteView.h"
#include "TestRunner.h"

int main (int argc, char **argv)
{
    TestRunner runner;
    runner.addTest("TestSite", TestSite::suite());
    runner.addTest("TestTuneDxs", TestTuneDxs::suite());
    runner.addTest("TestMisc", TestMisc::suite());
    runner.addTest("TestOffPositions", TestOffPositions::suite());
    runner.addTest("TestRecentRfiMask", TestRecentRfiMask::suite());
    runner.addTest("TestPosition", TestPosition::suite());
    runner.addTest("TestSiteView", TestSiteView::suite());
    runner.addTest("TestTarget", TestTarget::suite());
    return runner.run(argc, argv);
}