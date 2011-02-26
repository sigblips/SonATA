/*******************************************************************************

 File:    TestSiteView.cpp
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
#include "TestSiteView.h"
#include "SseAstro.h"
#include "SiteView.h"
#include "AtaInformation.h"

void TestSiteView::setUp()
{
   double horizDeg(18);
   siteView_ = new SiteView(AtaInformation::AtaLongWestDeg,
                            AtaInformation::AtaLatNorthDeg,
                            horizDeg);
}

void TestSiteView::tearDown()
{
   delete siteView_;
}

void TestSiteView::testFoo()
{
   cu_assert(1);
}

void TestSiteView::testHourAngle()
{
   cout << "TestSiteView::testHourAngle" << endl;
   double hourAngleTol(0.0001);
   
   double decDeg(0.15698);
   double expectedHourAngle(4.40787);  // includes atmos refract

   double hourAngle(siteView_->getHourAngle(decDeg));
   assertDoublesEqual(expectedHourAngle, hourAngle, hourAngleTol);
}

void TestSiteView::testHourAngleRads()
{
   cout << "TestSiteView::testHourAngleRads" << endl;
   double hourAngleTol(0.0001);
   
   double decDeg(0.15698);
   double expectedHourAngle(4.40787);  // includes atmos refract

   double hourAngleRads(siteView_->getHourAngleRads(
                           SseAstro::degreesToRadians(decDeg)));

   assertDoublesEqual(SseAstro::hoursToRadians(expectedHourAngle), 
                      hourAngleRads, hourAngleTol);
}

void TestSiteView::testLmst()
{
   cout << "TestSiteView::testLmst" << endl;
/*
From atamainsystime:

nss@sse3% printIsoDateTime 1254438370
2009-10-01 23:06:10 UTC
LAST      15.7257992   
*/
   time_t testTime(1254438370); // 2009-10-01 23:06:10 UTC
   double expectedLmstHours(15.7257992);
   double tol(0.001);
   double lmstRads = siteView_->lmstRads(testTime);

   assertDoublesEqual(expectedLmstHours, SseAstro::radiansToHours(lmstRads), tol);
}

void TestSiteView::testTargetNotVisible()
{
   cout << "TestSiteView::testTargetNotVisible" << endl;

   time_t testTime(1254438370); // 2009-10-01 23:06:10 UTC
   double lmstRads = siteView_->lmstRads(testTime);

   RaDec raDec;
   raDec.ra.setRadian(M_PI);
   raDec.dec.setRadian(-M_PI/2.0);  // south pole
   
   // cout << raDec << endl;

   double timeSinceRiseRads(0), timeUntilSetRads(0);
   bool isVisible = siteView_->isTargetVisible(lmstRads, raDec,
                              timeSinceRiseRads,
                              timeUntilSetRads);

   cout << "timeSinceRiseRads: " << timeSinceRiseRads
        << "  timeUntilSetRads: " << timeUntilSetRads << endl;

   cu_assert(! isVisible);

}

void TestSiteView::runTest()
{
   testFoo();
   testHourAngle();
   testHourAngleRads();
   testLmst();
   testTargetNotVisible();
}

Test *TestSiteView::suite()
{
   TestSuite *testSuite = new TestSuite("TestSiteView");
   testSuite->addTest(new TestSiteView("TestSiteView"));
   
   return testSuite;
}
