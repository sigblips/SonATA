/*******************************************************************************

 File:    TestTarget.cpp
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
#include "TestTarget.h"
#include "ScienceDataArchive.h"
#include "ObserveActivityStatus.h"
#include "ObsSummaryStats.h"
#include "ExpectedNssComponentsTree.h"
#include "CondensedSignalReport.h"
#include "ExpandedSignalReport.h"
#include "MutexBool.h"
#include "MinMaxBandwidth.h"
#include "SseAstro.h"
#include "SseUtil.h"
#include "SseArchive.h"
#include "TargetPosition.h"
#include "Target.h"
#include "TargetMerit.h"
#include "SiteView.h"
#include "AtaInformation.h"

#include <string>
#include <list>

// some defaults for creating Target objects
static const double DefaultParallaxMas(1000);  // a parsec
static const TargetId DefaultTargetId(99);
static const TargetId DefaultPrimaryTargetId(1099);
static const double DefaultMaxDistLightYears(250);
static const double DefaultObsLengthSec(98);
static const double DefaultMinBandwidthMhz(10);
static ObsRange DefaultFreqRangeLimitsMhz; 
static const double DefaultMinRemainingTimeOnTargetRads(0);
static const double DefaultPmRaMasYr(0);
static const double DefaultPmDecMasYr(0);
static const RaDec DefaultMu;  // use default constructor
static const string DefaultCatalog("");
static time_t DefaultObsDate(1105663810);   // Fri Jan 14 00:50:12 GMT 2005


void TestTarget::setUp ()
{
   double horizDeg(16.5);
   siteView_ = new SiteView(AtaInformation::AtaLongWestDeg,
                            AtaInformation::AtaLatNorthDeg,
                            horizDeg);

   DefaultFreqRangeLimitsMhz.addInOrder(1000, 10000);
}

void TestTarget::tearDown()
{
   delete siteView_;
}

void TestTarget::testVisibilityForMaxTrackLessThan12Hours()
{
   double radsTol(0.0001);

   // target position visibility tests
   // visible target, rise/set times are +- 3 hours, no wrap around zero ra

   RaDec target;
   target.ra.setRadian(SseAstro::hoursToRadians(11));
   target.dec.setRadian(SseAstro::degreesToRadians(-15));  // up approx 6 hours
   double currentLmstRads(SseAstro::hoursToRadians(9));
   double timeSinceRiseRads(0);
   double timeUntilSetRads(0);
   double expectedTimeSinceRiseRads(0.380118);
   double expectedTimeUntilSetRads(1.427316);

   cu_assert(siteView_->isTargetVisible(currentLmstRads, target,
                                        timeSinceRiseRads, 
                                        timeUntilSetRads));
   assertDoublesEqual(expectedTimeSinceRiseRads, timeSinceRiseRads, radsTol); 
   assertDoublesEqual(expectedTimeUntilSetRads, timeUntilSetRads, radsTol);   

   // lmst before target rises
   currentLmstRads = SseAstro::hoursToRadians(7);
   cu_assert(! siteView_->isTargetVisible(currentLmstRads, target,
						timeSinceRiseRads,
						timeUntilSetRads));

   // lmst after target sets
   currentLmstRads = SseAstro::hoursToRadians(15);
   cu_assert(! siteView_->isTargetVisible(currentLmstRads, target,
						timeSinceRiseRads, 
						timeUntilSetRads));

   // test target near zero ra (ie handle wrap around)
   // lmst past zero ra
   target.ra.setRadian(SseAstro::hoursToRadians(23));
   currentLmstRads = SseAstro::hoursToRadians(1);
   expectedTimeSinceRiseRads = 1.427316;
   expectedTimeUntilSetRads = 0.380118;
   cu_assert(siteView_->isTargetVisible(currentLmstRads, target,
					      timeSinceRiseRads, 
					      timeUntilSetRads));
   assertDoublesEqual(expectedTimeSinceRiseRads, timeSinceRiseRads, radsTol); 
   assertDoublesEqual(expectedTimeUntilSetRads, timeUntilSetRads, radsTol);   

   // lmst before zero ra
   currentLmstRads = SseAstro::hoursToRadians(23.5);
   expectedTimeSinceRiseRads = 1.034617;
   expectedTimeUntilSetRads = 0.772817;
   cu_assert(siteView_->isTargetVisible(currentLmstRads, target,
					      timeSinceRiseRads,
					      timeUntilSetRads));
   assertDoublesEqual(expectedTimeSinceRiseRads, timeSinceRiseRads, radsTol); 
   assertDoublesEqual(expectedTimeUntilSetRads, timeUntilSetRads, radsTol);   

   // lmst beyond set time
   currentLmstRads = SseAstro::hoursToRadians(3);
   cu_assert(! siteView_->isTargetVisible(currentLmstRads, target,
                                          timeSinceRiseRads,
                                          timeUntilSetRads));

   // lmst before rise 
   currentLmstRads = SseAstro::hoursToRadians(19);
   cu_assert(! siteView_->isTargetVisible(currentLmstRads, target,
                                          timeSinceRiseRads,
                                          timeUntilSetRads));
}

void TestTarget::testVisibilityForMaxTrackGreaterThan12Hours()
{
   double radsTol(0.001);

   // test target visible for more than 12 hours
   // lmst on rise side
   RaDec target;
   target.ra.setRadian(SseAstro::hoursToRadians(11));
   target.dec.setRadian(SseAstro::degreesToRadians(50));
   double currentLmstRads = SseAstro::hoursToRadians(10);
   double expectedTimeSinceRiseRads(1.772827);
   double expectedTimeUntilSetRads(2.296425);

   double timeSinceRiseRads;
   double timeUntilSetRads;
   cu_assert(siteView_->isTargetVisible(currentLmstRads, target,
                                        timeSinceRiseRads,
                                        timeUntilSetRads));
   assertDoublesEqual(expectedTimeSinceRiseRads, timeSinceRiseRads, radsTol); 
   assertDoublesEqual(expectedTimeUntilSetRads, timeUntilSetRads, radsTol);   

   // lmst on set side
   currentLmstRads = SseAstro::hoursToRadians(12);
   expectedTimeSinceRiseRads = 2.296425;
   expectedTimeUntilSetRads = 1.772827;
   cu_assert(siteView_->isTargetVisible(currentLmstRads, target,
					      timeSinceRiseRads,
					 timeUntilSetRads));
   assertDoublesEqual(expectedTimeSinceRiseRads, timeSinceRiseRads, radsTol); 
   assertDoublesEqual(expectedTimeUntilSetRads, timeUntilSetRads, radsTol);   

   // lmst before rise
   currentLmstRads = SseAstro::hoursToRadians(1);
   cu_assert(!siteView_->isTargetVisible(currentLmstRads, target,
					       timeSinceRiseRads,
					       timeUntilSetRads));

   // lmst after set
   currentLmstRads = SseAstro::hoursToRadians(20);
   cu_assert(!siteView_->isTargetVisible(currentLmstRads, target, 
					       timeSinceRiseRads,
					       timeUntilSetRads));

   // test target near zero RA (handle crossover)
   target.ra.setRadian(SseAstro::hoursToRadians(23));
   currentLmstRads = SseAstro::hoursToRadians(1);
   expectedTimeSinceRiseRads = 2.558225;
   expectedTimeUntilSetRads = 1.511027;
   cu_assert(siteView_->isTargetVisible(currentLmstRads, target,
					      timeSinceRiseRads,
					      timeUntilSetRads));
   assertDoublesEqual(expectedTimeSinceRiseRads, timeSinceRiseRads, radsTol); 
   assertDoublesEqual(expectedTimeUntilSetRads, timeUntilSetRads, radsTol);   

   // lmst before rise
   currentLmstRads = SseAstro::hoursToRadians(14.5);
   cu_assert(!siteView_->isTargetVisible(currentLmstRads, target,
					       timeSinceRiseRads,
					       timeUntilSetRads));

   // lmst after set
   currentLmstRads = SseAstro::hoursToRadians(7.5);
   cu_assert(!siteView_->isTargetVisible(currentLmstRads, target,
					       timeSinceRiseRads,
					       timeUntilSetRads));

   // put test target on other side of zero crossing
   target.ra.setRadian(SseAstro::hoursToRadians(1));
   currentLmstRads = SseAstro::hoursToRadians(23);
   expectedTimeSinceRiseRads = 1.511027;
   expectedTimeUntilSetRads = 2.558225;

   cu_assert(siteView_->isTargetVisible(currentLmstRads, target, 
					      timeSinceRiseRads,
					      timeUntilSetRads));
   assertDoublesEqual(expectedTimeSinceRiseRads, timeSinceRiseRads, radsTol); 
   assertDoublesEqual(expectedTimeUntilSetRads, timeUntilSetRads, radsTol);   

}

void TestTarget::testVisibilityForCircumpolar()
{
   double radsTol(0.001);

   RaDec target;
   // circumpolar target
   target.dec.setRadian(SseAstro::degreesToRadians(85));
   target.ra.setRadian(SseAstro::hoursToRadians(1));
   double currentLmstRads = SseAstro::hoursToRadians(23);
   double expectedTimeSinceRiseRads = SseAstro::hoursToRadians(24);
   double expectedTimeUntilSetRads = SseAstro::hoursToRadians(24);

   double timeSinceRiseRads;
   double timeUntilSetRads;
   cu_assert(siteView_->isTargetVisible(currentLmstRads, target,
                                        timeSinceRiseRads,
                                        timeUntilSetRads));
   assertDoublesEqual(expectedTimeSinceRiseRads, timeSinceRiseRads, radsTol); 
   assertDoublesEqual(expectedTimeUntilSetRads, timeUntilSetRads, radsTol);   
}

void TestTarget::testVisibilityForNeverVisible()
{
   RaDec target;
   target.ra.setRadian(SseAstro::hoursToRadians(1));
   target.dec.setRadian(SseAstro::degreesToRadians(-80));
   double currentLmstRads = SseAstro::hoursToRadians(23);
   double timeSinceRiseRads;
   double timeUntilSetRads;

   cu_assert(! siteView_->isTargetVisible(currentLmstRads, target, 
                                          timeSinceRiseRads,
                                          timeUntilSetRads));
}


void TestTarget::testNeverVisibleTarget()
{
   cout << "testNeverVisibleTarget" << endl;

   // pick a target position that's never up
   double targetRaRads = SseAstro::hoursToRadians(0.0);
   double targetDecRads = SseAstro::degreesToRadians(-50.0);
   double lmstRads = siteView_->lmstRads(DefaultObsDate);

   cout << "lmstRads: " << lmstRads 
	<< " lmstHours: " << SseAstro::radiansToHours(lmstRads) <<endl;

   Target * target = new Target(
      RaDec(Radian(targetRaRads), Radian(targetDecRads)),
      DefaultPmRaMasYr, DefaultPmDecMasYr, DefaultParallaxMas, 
      DefaultTargetId, DefaultPrimaryTargetId,
      DefaultCatalog,
      DefaultFreqRangeLimitsMhz, DefaultMaxDistLightYears, 
      siteView_, DefaultObsLengthSec, DefaultMinBandwidthMhz, 
      DefaultMinRemainingTimeOnTargetRads);
   target->setTime(lmstRads, DefaultObsDate);

   cu_assert(target->getTargetId() == DefaultTargetId);
   cu_assert(target->getPrimaryTargetId() == DefaultPrimaryTargetId);
   cu_assert(!target->isVisible());

   cout << "decMerit: " << target->decMerit() << endl << endl;

   delete target;
}

void TestTarget::testAlwaysVisibleTarget()
{
   cout << "testAlwaysVisibleTarget" << endl;

   // pick a target that's always up
   double targetRaRads = SseAstro::hoursToRadians(0.0);
   double targetDecRads = SseAstro::degreesToRadians(80);
   double lmstRads = siteView_->lmstRads(DefaultObsDate);

   cout << "lmstRads: " << lmstRads 
        << " hours: " << SseAstro::radiansToHours(lmstRads) << endl;

   Target * target = new Target(
      RaDec(Radian(targetRaRads), Radian(targetDecRads)),
      DefaultPmRaMasYr, DefaultPmDecMasYr, DefaultParallaxMas, 
      DefaultTargetId, DefaultPrimaryTargetId,
      DefaultCatalog,
      DefaultFreqRangeLimitsMhz, DefaultMaxDistLightYears,
      siteView_, DefaultObsLengthSec, DefaultMinBandwidthMhz, 
      DefaultMinRemainingTimeOnTargetRads);
   target->setTime(lmstRads, DefaultObsDate);

   cu_assert(target->isVisible());

   double meritTol(0.001);

   cout << "obsMerit: " << target->obsMerit() << endl;
   cout << "distMerit: " << target->distMerit() << endl;
   cout << "catalogMerit: " << target->catalogMerit() << endl;
   cout << "completelyObservedMerit: " << target->completelyObservedMerit() << endl;
   cout << "decMerit: " << target->decMerit() << endl;
   cout << "timeLeftMerit: " << target->timeLeftMerit() << endl;
   cout << "nearMeridianMerit: " << target->nearMeridianMerit() << endl;
   cout << "overallMerit: " << target->overallMerit() << endl;

   double expectedObsMerit(1.0);
   assertDoublesEqual(expectedObsMerit, target->obsMerit(), meritTol);

   double expectedDistMerit(256.0);
   assertDoublesEqual(expectedDistMerit, target->distMerit(), meritTol);   

#if 0
// TBD fix this for catalogMerit

   double expectedTargetTypeMerit(0.5);
   assertDoublesEqual(expectedTargetTypeMerit, target->typeMerit(),
		      meritTol);
#endif   

   double expectedTargetCompletelyObservedMerit(1.0);
   assertDoublesEqual(expectedTargetCompletelyObservedMerit, 
		      target->completelyObservedMerit(),
		      meritTol);

   double expectedTargetDecMerit(1.620301);
   assertDoublesEqual(expectedTargetDecMerit, target->decMerit(), meritTol); 

   double expectedTimeLeftMerit(1);
   double expectedTimeLeftMeritTol(0.0001);
   assertDoublesEqual(expectedTimeLeftMerit, target->timeLeftMerit(),
		      expectedTimeLeftMeritTol);

   double expectedNearMeridianMerit(134.4);  // ra in setting side of lmst window 
   double expectedNearMeridianMeritTol(0.1);
   assertDoublesEqual(expectedNearMeridianMerit, target->nearMeridianMerit(),
		      expectedNearMeridianMeritTol);

#if 0
// TBD rework for new merit formula
   cout << "overallMerit: " << target->overallMerit() << endl;

   double expectedOverallMerit(50689.752081);
   assertDoublesEqual(expectedOverallMerit, target->overallMerit(), meritTol);
#endif

   delete target;

}

void TestTarget::testSometimesVisibleTarget()
{
   cout << "testSometimesVisibleTarget" << endl;

   // pick a target that's sometimes up, and is currently visible
   double lmstRads = siteView_->lmstRads(DefaultObsDate);
   double targetRaRads = lmstRads;   // on the meridian
   double targetDecRads = SseAstro::degreesToRadians(10);

   double expectedLmstRads(0.08139);
   double expectedLmstRadsTol(0.0001);
   assertDoublesEqual(expectedLmstRads, lmstRads, expectedLmstRadsTol);

   cout << "lmstRads = " << lmstRads << endl;

   // force the time left merit to be small but nonzero
   double minRemainingTimeOnTargetRads = SseAstro::hoursToRadians(2.0);

   Target * target = new Target(
      RaDec(Radian(targetRaRads), Radian(targetDecRads)),
      DefaultPmRaMasYr, DefaultPmDecMasYr, DefaultParallaxMas, 
      DefaultTargetId, DefaultPrimaryTargetId,
      DefaultCatalog,
      DefaultFreqRangeLimitsMhz, DefaultMaxDistLightYears,
      siteView_, DefaultObsLengthSec, DefaultMinBandwidthMhz, 
      minRemainingTimeOnTargetRads);
   int catalogOrder(0);
   target->setCatalogPriority(Target::CatalogHighPriority, catalogOrder);
   target->setTime(lmstRads, DefaultObsDate);

   cu_assert(target->isVisible());

   cout << "riseTimeUtc: " << target->getRiseTimeUtc() 
	<< " " << SseUtil::isoDateTime(target->getRiseTimeUtc()) << endl;
   cout << "setTimeUtc: " << target->getSetTimeUtc() 
	<< " " << SseUtil::isoDateTime(target->getSetTimeUtc()) << endl;

   cu_assert(SseUtil::isoDateTime(target->getRiseTimeUtc()) == 
				  "2005-01-13 19:43:47 UTC");

   cu_assert(SseUtil::isoDateTime(target->getSetTimeUtc()) == 
				  "2005-01-14 05:57:04 UTC");

   cout << "obsMerit: " << target->obsMerit() << endl;
   cout << "distMerit: " << target->distMerit() << endl;
   cout << "catalogMerit: " << target->catalogMerit() << endl;
   cout << "completelyObservedMerit: " << target->completelyObservedMerit() << endl;
   cout << "decMerit: " << target->decMerit() << endl;
   cout << "timeLeftMerit: " << target->timeLeftMerit() << endl;
   cout << "nearMeridianMerit: " << target->nearMeridianMerit() << endl;
   cout << "overallMerit: " << target->overallMerit() << endl;

#if 0
   double expectedTargetDecMerit = 4.89682;
   assertDoublesEqual(expectedTargetDecMerit, target->decMerit(), meritTol); 
#endif

   double expectedTimeLeftMerit = 1; 
   double expectedTimeLeftMeritTol(0.0001);
   assertDoublesEqual(expectedTimeLeftMerit, target->timeLeftMerit(),
		      expectedTimeLeftMeritTol);

#if 0
// TBD rework for new merit formula
   double expectedOverallMerit = 101035.024932;
   double meritTol(0.001);
   assertDoublesEqual(expectedOverallMerit, target->overallMerit(), meritTol);
#endif

   delete target;

}

void TestTarget::testTargetNearMeridianMerit()
{
   cout << "testTargetNearMeridianMerit" << endl;

   double targetRaRads = SseAstro::hoursToRadians(5.0);
   double targetDecRads = SseAstro::degreesToRadians(20);

   // force the time left merit to be small but nonzero
   double minRemainingTimeOnTargetRads = SseAstro::hoursToRadians(2.0);

   Target * target = new Target(
      RaDec(Radian(targetRaRads), Radian(targetDecRads)),
      DefaultPmRaMasYr, DefaultPmDecMasYr, DefaultParallaxMas, 
      DefaultTargetId, DefaultPrimaryTargetId,
      DefaultCatalog,
      DefaultFreqRangeLimitsMhz, DefaultMaxDistLightYears,
      siteView_, DefaultObsLengthSec, DefaultMinBandwidthMhz, 
      minRemainingTimeOnTargetRads);
   
   // try ra on rise side of lmst, inside meridian window
   double lmstRads = SseAstro::hoursToRadians(3.5);
   target->setTime(lmstRads, DefaultObsDate);

   double expectedNearMeridianMerit(162.5);  // ra in rising side of lmst window 
   double expectedNearMeridianMeritTol(0.1);
   assertDoublesEqual(expectedNearMeridianMerit, target->nearMeridianMerit(),
		      expectedNearMeridianMeritTol);

   // try ra outside lmst rise side window
   lmstRads = SseAstro::hoursToRadians(1.0);
   target->setTime(lmstRads, DefaultObsDate);

   expectedNearMeridianMerit = 100;
   assertDoublesEqual(expectedNearMeridianMerit, target->nearMeridianMerit(),
		      expectedNearMeridianMeritTol);

   // try ra on set side of lmst, inside meridian window
   lmstRads = SseAstro::hoursToRadians(5.5);
   target->setTime(lmstRads, DefaultObsDate);

   expectedNearMeridianMerit = 125;
   assertDoublesEqual(expectedNearMeridianMerit, target->nearMeridianMerit(),
		      expectedNearMeridianMeritTol);

   // try ra on set side of lmst, outside meridian window
   lmstRads = SseAstro::hoursToRadians(6.5);
   target->setTime(lmstRads, DefaultObsDate);

   expectedNearMeridianMerit = 100;
   assertDoublesEqual(expectedNearMeridianMerit, target->nearMeridianMerit(),
		      expectedNearMeridianMeritTol);

   delete target;
}

void TestTarget::testTargetFreqRangeHandling()
{
   cout << "testTargetFreqRangeHandling" << endl;

   // pick a target that's currently visible
   double targetRaRads = SseAstro::hoursToRadians(0.0);
   double targetDecRads = SseAstro::degreesToRadians(80);
   double lmstRads = siteView_->lmstRads(DefaultObsDate);

   // force the time left merit to be small but nonzero
   double minRemainingTimeOnTargetRads = SseAstro::hoursToRadians(2.0);

   Target * target = new Target(
      RaDec(Radian(targetRaRads), Radian(targetDecRads)),
      DefaultPmRaMasYr, DefaultPmDecMasYr, DefaultParallaxMas, 
      DefaultTargetId, DefaultPrimaryTargetId,
      DefaultCatalog,
      DefaultFreqRangeLimitsMhz, DefaultMaxDistLightYears,
      siteView_, DefaultObsLengthSec, DefaultMinBandwidthMhz, 
      minRemainingTimeOnTargetRads);
   target->setTime(lmstRads, DefaultObsDate);

   cu_assert(target->isVisible());
   stringstream observedStrm;
   observedStrm << target->observed();
   cu_assert(observedStrm.str() == "");  // nothing observed yet

   double meritTol(0.001);

#if 0
// TBD rework for new merit formula
   double expectedOverallMerit = 101035.024932;
   assertDoublesEqual(expectedOverallMerit, target->overallMerit(), meritTol);
#endif

   // Test freq range handling

   stringstream rangeStrm;
   rangeStrm << target->unobserved();
   cu_assert(rangeStrm.str() == "1000.000000-10000.000000"); 

   double expectedTotalRange(9000.0);
   double totalRangeTol(0.1);
   assertDoublesEqual(expectedTotalRange, target->totalRangeGt(0), totalRangeTol);   

   double expectedFractionObserved(0.0);
   double fractObsTol(0.1);
   assertDoublesEqual(expectedFractionObserved, target->fractionObserved(),
		      fractObsTol);   

   // add in some observations 
   target->addObserved(1000.0, 1030.0);
   target->addObserved(2200.0, 2250.0);
   observedStrm.str("");
   observedStrm << target->observed();
   cu_assert(observedStrm.str() == "1000.000000-1030.000000 2200.000000-2250.000000");

   stringstream afterRemoveObsStrm;
   afterRemoveObsStrm << target->unobserved();
   cu_assert(afterRemoveObsStrm.str() == "1030.000000-2200.000000 2250.000000-10000.000000"); 

   // add in the observed freqs again, make sure there is no change
   target->addObservedOutOfOrder(1000.0, 1030.0);
   target->addObservedOutOfOrder(2200.0, 2250.0);
   observedStrm.str("");
   observedStrm << target->observed();
   cu_assert(observedStrm.str() == "1000.000000-1030.000000 2200.000000-2250.000000");
   
   stringstream afterRepeatStrm;
   afterRepeatStrm << target->unobserved();
   cu_assert(afterRepeatStrm.str() == "1030.000000-2200.000000 2250.000000-10000.000000"); 

   // pretend some observations failed, and remove them
   target->removeObserved(2200.0, 2250.0);
   observedStrm.str("");
   observedStrm << target->observed();
   cu_assert(observedStrm.str() == "1000.000000-1030.000000");

   target->removeObserved(1025.0, 1030.0);
   observedStrm.str("");
   observedStrm << target->observed();
   cu_assert(observedStrm.str() == "1000.000000-1025.000000");

   stringstream afterRemoveStrm;
   afterRemoveStrm << target->unobserved();
   cu_assert(afterRemoveStrm.str() == "1025.000000-10000.000000"); 

   // Pretend that the target has been completely observed.
   // Reset the obs ranges back to their initial state,
   // and mark the target as completely observed.

   target->resetObservations();
   stringstream afterResetStrm;
   afterResetStrm << target->unobserved();
   cu_assert(afterResetStrm.str() == "1000.000000-10000.000000"); 

   target->setAlreadyCompletelyObserved(true);
   cu_assert(target->isAlreadyCompletelyObserved());

   double expectedTargetCompletelyObservedMerit(0.001);
   assertDoublesEqual(expectedTargetCompletelyObservedMerit, 
		      target->completelyObservedMerit(), meritTol);

#if 0
// TBD rework for new merit formula
   // overall merit should have dropped by the completelyObservedMerit()
   // factor
   expectedOverallMerit *= target->completelyObservedMerit();
   assertDoublesEqual(expectedOverallMerit, target->overallMerit(), meritTol);

#endif

   delete target;
}

void TestTarget::testTargetMerit()
{
   cout << "testTargetMerit" << endl;

   assertLongsEqual(TargetMerit::Dec, TargetMerit::nameToMeritFactor("dec"));

   // get all possible merit factor names
   vector<string> allMeritNames = TargetMerit::getAllMeritNames();
   int expectedNumNames = TargetMerit::MeritEnd-1;
   assertLongsEqual(expectedNumNames, allMeritNames.size());

   for (unsigned int i=0; i<allMeritNames.size(); ++i)
   {
      cout << "merit name: " << allMeritNames[i] << endl;
   }

   vector<TargetMerit::MeritFactor> meritFactors;
   meritFactors.push_back(TargetMerit::Catalog);
   meritFactors.push_back(TargetMerit::CompletelyObs);
   meritFactors.push_back(TargetMerit::Dec);
   meritFactors.push_back(TargetMerit::TimeLeft);

   TargetMerit targetMerit(meritFactors);



   // pick a target that's sometimes up, and is currently visible
   double lmstRads = siteView_->lmstRads(DefaultObsDate);
   double targetRaRads = lmstRads;   // on the meridian
   double targetDecRads = SseAstro::degreesToRadians(10);

   double expectedLmstRads(0.08139);
   double expectedLmstRadsTol(0.0001);
   assertDoublesEqual(expectedLmstRads, lmstRads, expectedLmstRadsTol);

   cout << "lmstRads = " << lmstRads << endl;

   // force the time left merit to be small but nonzero
   double minRemainingTimeOnTargetRads = SseAstro::hoursToRadians(2.0);

   Target * target = new Target(
      RaDec(Radian(targetRaRads), Radian(targetDecRads)),
      DefaultPmRaMasYr, DefaultPmDecMasYr, DefaultParallaxMas, 
      DefaultTargetId, DefaultPrimaryTargetId,
      DefaultCatalog,
      DefaultFreqRangeLimitsMhz, DefaultMaxDistLightYears,
      siteView_, DefaultObsLengthSec, DefaultMinBandwidthMhz, 
      minRemainingTimeOnTargetRads);
   target->setTime(lmstRads, DefaultObsDate);

   int catalogOrder(0);
   target->setCatalogPriority(Target::CatalogHighPriority, catalogOrder);

   double expectedOverallMerit(1375953.206);
   double overallMerit = targetMerit.overallMerit(target);
   double meritTol(0.001);
   assertDoublesEqual(expectedOverallMerit, overallMerit, meritTol);

}

Test *TestTarget::suite()
{
	TestSuite *testSuite = new TestSuite("TestTarget");

        testSuite->addTest (new TestCaller <TestTarget> ("testVisibilityForMaxTrackLessThan12Hours", &TestTarget::testVisibilityForMaxTrackLessThan12Hours));

        testSuite->addTest (new TestCaller <TestTarget> ("testVisibilityForMaxTrackGreaterThan12Hours", &TestTarget::testVisibilityForMaxTrackGreaterThan12Hours));

        testSuite->addTest (new TestCaller <TestTarget> ("testVisibilityForCircumpolar", &TestTarget::testVisibilityForCircumpolar));

        testSuite->addTest (new TestCaller <TestTarget> ("testVisibilityForNeverVisible", &TestTarget::testVisibilityForNeverVisible));

        testSuite->addTest (new TestCaller <TestTarget> ("testNeverVisibleTarget", &TestTarget::testNeverVisibleTarget));

        testSuite->addTest (new TestCaller <TestTarget> ("testAlwaysVisibleTarget", &TestTarget::testAlwaysVisibleTarget));

        testSuite->addTest (new TestCaller <TestTarget> ("testSometimesVisibleTarget", &TestTarget::testSometimesVisibleTarget));

        testSuite->addTest (new TestCaller <TestTarget> ("testTargetNearMeridianMerit", &TestTarget::testTargetNearMeridianMerit));

        testSuite->addTest (new TestCaller <TestTarget> ("testTargetFreqRangeHandling", &TestTarget::testTargetFreqRangeHandling));

        testSuite->addTest (new TestCaller <TestTarget> ("testTargetMerit", &TestTarget::testTargetMerit));

	return testSuite;
}