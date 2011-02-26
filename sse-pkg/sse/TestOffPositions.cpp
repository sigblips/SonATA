/*******************************************************************************

 File:    TestOffPositions.cpp
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
#include "TestOffPositions.h"
#include "OffPositions.h"
#include "AtaInformation.h"
#include "SseException.h"
#include "SseAstro.h"
#include <cmath>

using namespace std;

static const double NorthPoleDecRads(0.5 * M_PI);
static const double SouthPoleDecRads(-0.5 * M_PI);
static const double TWOPI(2 * M_PI);

TestOffPositions::TestOffPositions(std::string name) 
   : TestCase(name),
     offPositions_(0)
{}

void TestOffPositions::setUp()
{
   // Use worse case synth beamsize for determining beam separation.
   // For ata42, Peter says it's approx 13.7 x 3.2 arcmin at dec=-33 deg
   // at 1420 Mhz.
   // 13.7 arcmin = 822 arcsec at 1420 Ghz,  which is ~ 1167 arcsec at one ghz.
   
   const double synthBeamsizeAtOneGhzArcSec(1167);
   const double skyFreqMhz(1420);
   
   synthBeamsizeRads_ = AtaInformation::ataBeamsizeRadians(
      skyFreqMhz, synthBeamsizeAtOneGhzArcSec);

   //cout << "synth beamsize rads = " << synthBeamsizeRads_ << endl;

   double minBeamSepFactor(2.0);
   minPointSepRads_ = synthBeamsizeRads_ * minBeamSepFactor;

   //cout << "minPointSepRads_ = " << minPointSepRads_ << endl;
   
   // primary beam center position:
   primaryCenterRaRads_ = 0.0;
   primaryCenterDecRads_ = 0.0;

   // primary beam size:
   const double primaryBeamsizeAtOneGhzDeg(3.5);
   const double arcSecPerDeg(3600);
   
   const double primaryBeamsizeAtOneGhzArcSec(primaryBeamsizeAtOneGhzDeg * 
					      arcSecPerDeg);

   primaryBeamsizeRads_ = AtaInformation::ataBeamsizeRadians(
      skyFreqMhz, primaryBeamsizeAtOneGhzArcSec);

   //cout << "primaryBeamsizeRads_ = " << primaryBeamsizeRads_ << endl;

   offPositions_ = new OffPositions(synthBeamsizeRads_,
				    minBeamSepFactor,
				    primaryCenterRaRads_,
				    primaryCenterDecRads_,
				    primaryBeamsizeRads_);
}

void TestOffPositions::tearDown()
{
   delete offPositions_;
}

void TestOffPositions::testFoo()
{

}

void TestOffPositions::testMoveNorth()
{
   cout << "testMoveNorth" << endl;

   double arcDistRads(0.1);
   double targetRaRads(0.0);
   double targetDecRads(0.0);
   
   double offRaRads(0.0);
   double offDecRads(0.0);

   // nominal case (not near north pol)
   OffPositions::moveNorth(targetRaRads, targetDecRads,
			   arcDistRads, 
			   &offRaRads, &offDecRads);

   double expectedOffRaRads(targetRaRads);
   double expectedOffDecRads(targetDecRads+arcDistRads);

   double tolRads = 0.000001;
   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);
   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);


   // crossing north pol
   targetDecRads = NorthPoleDecRads - (arcDistRads * 0.1);

   OffPositions::moveNorth(targetRaRads, targetDecRads,
			   arcDistRads, 
			   &offRaRads, &offDecRads);

   expectedOffRaRads = targetRaRads + M_PI;
   expectedOffDecRads = NorthPoleDecRads - (arcDistRads * 0.9);

   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);
   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);


}

void TestOffPositions::testMoveSouth()
{
   cout << "testMoveSouth" << endl;

   double arcDistRads(0.1);
   double targetRaRads(0.0);
   double targetDecRads(0.0);
   
   double offRaRads(0.0);
   double offDecRads(0.0);

   // nominal case (not near south pol)
   OffPositions::moveSouth(targetRaRads, targetDecRads,
			   arcDistRads, 
			   &offRaRads, &offDecRads);

   double expectedOffRaRads(targetRaRads);
   double expectedOffDecRads(targetDecRads-arcDistRads);

   double tolRads = 0.000001;
   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);
   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);


   // crossing south pol

   targetDecRads = SouthPoleDecRads + (arcDistRads * 0.1);

   OffPositions::moveSouth(targetRaRads, targetDecRads,
			   arcDistRads, 
			   &offRaRads, &offDecRads);

   expectedOffRaRads = targetRaRads + M_PI;
   expectedOffDecRads = SouthPoleDecRads + (arcDistRads * 0.9);

   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);
   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);


}

void TestOffPositions::testMoveEast()
{
   cout << "testMoveEast (increasing RA)" << endl;

   double arcDistRads(0.1);
   double targetRaRads(0.0);
   double targetDecRads(0.0);
   
   double offRaRads(0.0);
   double offDecRads(0.0);

   // nominal case (low dec)
   OffPositions::moveEast(targetRaRads, targetDecRads,
			  arcDistRads, 
			  &offRaRads, &offDecRads);

   double expectedOffRaRads(targetRaRads + arcDistRads);
   double expectedOffDecRads(targetDecRads);

   double tolRads = 0.000001;
   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);
   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);


   // midrange ra, dec (with dec negative)
   targetRaRads = 0.25 * M_PI;
   targetDecRads = -0.25 * M_PI;

   expectedOffRaRads = 0.926352;
   expectedOffDecRads = -0.780415;

   OffPositions::moveEast(targetRaRads, targetDecRads,
			  arcDistRads, 
			  &offRaRads, &offDecRads);

   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);
   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);

   // test wrap around ra hour zero
   targetRaRads = (-arcDistRads * 0.3) + TWOPI;
   targetDecRads = 0.0;
   expectedOffRaRads = arcDistRads * 0.7;
   expectedOffDecRads = targetDecRads;

   OffPositions::moveEast(targetRaRads, targetDecRads,
			  arcDistRads, 
			  &offRaRads, &offDecRads);

   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);
   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);



   // extreme case - high dec
   targetRaRads = 0.0;
   targetDecRads = NorthPoleDecRads;

   OffPositions::moveEast(targetRaRads, targetDecRads,
			  arcDistRads, 
			  &offRaRads, &offDecRads);

   expectedOffDecRads = asin(cos(arcDistRads));
   expectedOffRaRads = NorthPoleDecRads;

   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);
   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);

}

void TestOffPositions::testMoveWest()
{
   cout << "testMoveWest (decreasing RA)" << endl;

   double arcDistRads(0.1);
   double tolRads = 0.000001;   
   double offRaRads(0.0);
   double offDecRads(0.0);

   // nominal case: low dec, no wrap around ra 0
   double targetRaRads(M_PI);
   double targetDecRads(0.0);

   OffPositions::moveWest(targetRaRads, targetDecRads,
			  arcDistRads, 
			  &offRaRads, &offDecRads);

   double expectedOffRaRads(targetRaRads - arcDistRads);
   double expectedOffDecRads(targetDecRads);

   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);
   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);


   // low dec, wrap around ra 0
   targetRaRads = 0.0;
   targetDecRads = 0.0;

   OffPositions::moveWest(targetRaRads, targetDecRads,
			  arcDistRads, 
			  &offRaRads, &offDecRads);

   expectedOffRaRads = targetRaRads - arcDistRads + TWOPI;
   expectedOffDecRads = targetDecRads;

   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);
   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);


   // midrange ra, dec (with dec negative)
   targetRaRads = 0.25 * M_PI;
   targetDecRads = -0.25 * M_PI;

   expectedOffRaRads = 0.644444;
   expectedOffDecRads = -0.780415;

   OffPositions::moveWest(targetRaRads, targetDecRads,
			  arcDistRads, 
			  &offRaRads, &offDecRads);

   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);
   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);


   // extreme case - high dec

   targetRaRads = 0.0;
   targetDecRads = NorthPoleDecRads;

   OffPositions::moveWest(targetRaRads, targetDecRads,
			  arcDistRads, 
			  &offRaRads, &offDecRads);

   expectedOffDecRads = asin(cos(arcDistRads));
   expectedOffRaRads = M_PI * 1.5;

   assertDoublesEqual(expectedOffDecRads, offDecRads, tolRads);
   assertDoublesEqual(expectedOffRaRads, offRaRads, tolRads);




}

void TestOffPositions::testTimeSincePreviousObs()
{
   cout << "testTimeSincePreviousObs" << endl;

   // test successful pointing using the time since previous observation

   double targetRaRads(0);
   double targetDecRads(0);
   int timeSinceLastObsSecs(200);

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;
   
   targets.push_back(OffPositions::Position(targetRaRads,
					    targetDecRads));

   offPositions_->getOffPositions(targets, timeSinceLastObsSecs,
				  offs);

   cu_assert(offs.size() == 1);

   double expectedOffRaRads(0.014584);
   double expectedOffDecRads(targetDecRads);

   double tolRads = 0.000001;
   assertDoublesEqual(expectedOffRaRads, offs[0].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[0].decRads_, tolRads);
}

void TestOffPositions::testTimeTooLongSincePreviousObs()
{
   cout << "testTimeTooLongSincePreviousObs" << endl;

   // find an off for the situation where too much time has elapsed,
   // so that the delta time calculation puts the first choice off
   // position outside the primary beam.

   double targetRaRads(primaryCenterRaRads_);
   double targetDecRads(primaryCenterDecRads_);
   int timeSinceLastObsSecs(2000);  // 2000s = ~8.4 raDeltaDeg

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;
   
   targets.push_back(OffPositions::Position(targetRaRads,
					    targetDecRads));

   offPositions_->getOffPositions(targets, timeSinceLastObsSecs,
				  offs);

   cu_assert(offs.size() == 1);

   // south position, inside primary fov
   double expectedOffRaRads(targetRaRads);  
   double expectedOffDecRads(targetDecRads - minPointSepRads_);

   double tolRads = 0.000001;
   assertDoublesEqual(expectedOffRaRads, offs[0].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[0].decRads_, tolRads);

}


void TestOffPositions::testTargetOnSouthEdgeofPrimaryFov()
{
   cout << "testTargetOnSouthEdgeofPrimaryFov" << endl;

   // find an off for the situation where too much time has elapsed,
   // so that the delta time calculation puts the first choice OFF
   // position outside the primary beam, and the target
   // is at the south edge of the primary fov, so that 
   // the north off is required

   double targetRaRads(primaryCenterRaRads_);
   double primarySouthEdgeDecRads(primaryCenterDecRads_ - 
				  primaryBeamsizeRads_/2.0);
   double targetDecRads(primarySouthEdgeDecRads);

#if 0
   cout << "targetRaRads = " << targetRaRads
	<< " targetDecRads = " << targetDecRads << endl;
#endif

   int timeSinceLastObsSecs(2000);  // 2000s = ~8.4 raDeltaDeg

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;
   
   targets.push_back(OffPositions::Position(targetRaRads,
					    targetDecRads));

   offPositions_->getOffPositions(targets, timeSinceLastObsSecs,
				  offs);

   cu_assert(offs.size() == 1);

   //expecting north position, inside primary fov
   double expectedOffRaRads(targetRaRads);  
   double expectedOffDecRads(targetDecRads + minPointSepRads_);

   double tolRads = 0.000001;
   assertDoublesEqual(expectedOffRaRads, offs[0].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[0].decRads_, tolRads);

}

void TestOffPositions::testTargetRepeatedToForceAllPossibleOffs()
{
   cout << "testTargetRepeatedToForceAllPossibleOffs" << endl;
 
   // Give the same target each time, to force each off position
   // to be selected in turn.

   // test successful pointing using the time since previous observation
   double targetRaRads(0);
   double targetDecRads(0);
   int timeSinceLastObsSecs(240);  // 240s = ~ 1.0 deg
   double tolRads = 0.000001;

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;

   const int nOffs(6);
   for (int i=0; i < nOffs; ++i)
   {
      targets.push_back(OffPositions::Position(targetRaRads,
					    targetDecRads));
   }

   offPositions_->getOffPositions(targets, timeSinceLastObsSecs,
				  offs);

   //cout << *offPositions_ << endl;

   cu_assert(offs.size() == static_cast<unsigned int>(nOffs));

   // off due to time since last obs
   double expectedOffRaRads(0.017501);
   double expectedOffDecRads(targetDecRads);
   int offIndex(0);
   assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);


   // next off should be south position
   expectedOffRaRads = targetRaRads;  
   expectedOffDecRads = targetDecRads - minPointSepRads_;
   offIndex++;
   assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);

   // next should be north position
   expectedOffRaRads = targetRaRads;  
   expectedOffDecRads = targetDecRads + minPointSepRads_;
   offIndex++;
   assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);


   // should get west this time (wrapping around ra 0)
   expectedOffRaRads = targetRaRads - minPointSepRads_ + TWOPI;  
   expectedOffDecRads = targetDecRads;
   offIndex++;
   assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);


   // east this time.  note that this is not as far east as was
   // found initially using the timeSinceLastObsSecs, so
   // it still be valid (ie, it falls in between the target and
   // that first off).

   expectedOffRaRads = targetRaRads + minPointSepRads_;  
   expectedOffDecRads = targetDecRads;
   offIndex++;
   assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);


   // finally, OffPositions should use its primary fov search grid
   // to find the first available free position
   expectedOffRaRads = 6.277613;
   expectedOffDecRads = -0.017514;
   offIndex++;
   assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);
   
   offIndex++;
   cu_assert(offIndex == nOffs);



}


void TestOffPositions::testTargetsOnEastEdgeofPrimaryFov()
{
   cout << "testTargetOnEastEdgeofPrimaryFov" << endl;

   /*
     Force a situation such that a target on the eastern edge
     of the primary fov cannot find any offs within the
     minTargetSeparation (call this the "trouble position");
     Do this by starting with a target one minTargetSeparation to the
     west of the trouble position (call this the "blocking target").
     The blocking target should get an OFF to the south.   
     Now put a target in the trouble position.
     All possible offs within one minTargetSeparation to the
     north, south, east, & west should be invalid.
   */

   // First the "blocking target", to the west.
   double primaryEastEdgeRaRads(primaryCenterRaRads_ + 
				primaryBeamsizeRads_/2.0);
   double blockTargetRaRads(primaryEastEdgeRaRads - minPointSepRads_);
   double blockTargetDecRads(primaryCenterDecRads_);

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;

#if 0
   cout << "blocker target: targetRaRads = " << targetRaRads
	<< " targetDecRads = " << targetDecRads << endl;
#endif

   targets.push_back(OffPositions::Position(
      blockTargetRaRads, blockTargetDecRads));

   // now add the "trouble target".  
   double troubleTargetRaRads(primaryEastEdgeRaRads);
   double troubleTargetDecRads(primaryCenterDecRads_);
   targets.push_back(OffPositions::Position(
      troubleTargetRaRads, troubleTargetDecRads));

#if 0
   cout << "trouble target: targetRaRads = " << troubleTargetRaRads
	<< " targetDecRads = " << troubleTargetDecRads << endl;
#endif

   // time doesn't matter much, since the targets are already
   // at the eastern edge
   int timeSinceLastObsSecs(2000);  // 2000s = ~8.4 raDeltaDeg

   try {

      offPositions_->getOffPositions(targets, timeSinceLastObsSecs,
				     offs);
      
      cu_assert(offs.size() == 2);

      //cout << *offPositions_ << endl;
      //offPositions_->printPrimaryFovGrid(cout);
 
      //blocking target should get south position, inside primary fov
      double expectedOffRaRads(blockTargetRaRads);  
      double expectedOffDecRads(blockTargetDecRads - minPointSepRads_);
      
      double tolRads = 0.000001;

      int offIndex(0);
      assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
      assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);

      // check results here for "trouble target". 
      // should have picked one of the grid positions.
      offIndex++;      

      expectedOffRaRads = 6.277613;  
      expectedOffDecRads = -0.017514;
      
      assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
      assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);

   }
   catch(SseException & except)
   {
      // shouldn't get here
      cu_assert(0);
   }
   
}

void TestOffPositions::testTargetsNearNorthPole()
{
   cout << "testTargetsNearNorthPole" << endl;
   // create OffPositions with primary fov near the north pole
   
   double npPrimaryCenterRaRads(0);
   double npPrimaryCenterDecRads(NorthPoleDecRads);

   double npPrimaryBeamsizeRads(SseAstro::degreesToRadians(3));
   double npSynthBeamsizeRads(npPrimaryBeamsizeRads * 0.1);
   double npMinBeamSepFactor(2.0);

   OffPositions * npOffPositions = new OffPositions(
      npSynthBeamsizeRads,
      npMinBeamSepFactor,
      npPrimaryCenterRaRads,
      npPrimaryCenterDecRads,
      npPrimaryBeamsizeRads);

   // Give the same target each time, to force each off position
   // to be selected in turn.

   // test successful pointing using the time since previous observation
   double targetRaRads(npPrimaryCenterRaRads);
   double targetDecRads(npPrimaryCenterDecRads);
   int timeSinceLastObsSecs(240);  // 240s = ~ 1.0 deg
   double tolRads = 0.00001;

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;

   const int nOffs(6);
   for (int i=0; i < nOffs; ++i)
   {
      targets.push_back(OffPositions::Position(targetRaRads,
					    targetDecRads));
   }

   vector<OffPositions::Position> expectedOffs;
   expectedOffs.push_back(OffPositions::Position(0.000000, 1.560324)); // south
   expectedOffs.push_back(OffPositions::Position(3.141593, 1.560324)); // north
   expectedOffs.push_back(OffPositions::Position(6.267252, 1.548797)); // grids
   expectedOffs.push_back(OffPositions::Position(0.464588, 1.548784));
   expectedOffs.push_back(OffPositions::Position(5.740813, 1.548155));
   expectedOffs.push_back(OffPositions::Position(0.989959, 1.548127));

   try {

      npOffPositions->getOffPositions(targets, timeSinceLastObsSecs,
				      offs);

      //cout << "north pole test\n" << *npOffPositions << endl;
      //npOffPositions->printPrimaryFovGrid(cout);

      cu_assert(offs.size() == static_cast<unsigned int>(nOffs));
      cu_assert(expectedOffs.size() == offs.size());

      for (unsigned int i=0; i<offs.size(); ++i)
      {
	 assertDoublesEqual(expectedOffs[i].raRads_, offs[i].raRads_, tolRads);
	 assertDoublesEqual(expectedOffs[i].decRads_, offs[i].decRads_, tolRads);
      }

   }
   catch (SseException & except)
   {
      cout << except << endl;

      //cout << *npOffPositions << endl;

      // shouldn't get here
      cu_assert(0);
   }

   delete npOffPositions;
}

void TestOffPositions::testTargetsNearSouthPole()
{
   cout << "testTargetsNearSouthPole" << endl;
   // create OffPositions with primary fov near the south pole

   double spPrimaryBeamsizeRads(SseAstro::degreesToRadians(3));
   double spSynthBeamsizeRads(spPrimaryBeamsizeRads * 0.1);
   double spMinBeamSepFactor(2.0);
   
   // move the primary fov a few synth beams off the pole in dec
   double spPrimaryCenterRaRads(0);
   double primaryDecOffsetRads(spSynthBeamsizeRads * 3);
   double spPrimaryCenterDecRads(SouthPoleDecRads + primaryDecOffsetRads);

   OffPositions * spOffPositions = new OffPositions(
      spSynthBeamsizeRads,
      spMinBeamSepFactor,
      spPrimaryCenterRaRads,
      spPrimaryCenterDecRads,
      spPrimaryBeamsizeRads);

   // Give the same target each time, to force each off position
   // to be selected in turn.

   // test successful pointing using the time since previous observation
   double targetRaRads(spPrimaryCenterRaRads);
   double targetDecRads(spPrimaryCenterDecRads);
   int timeSinceLastObsSecs(240);  // 240s = ~ 1.0 deg
   double tolRads = 0.00001;

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;

   const int nOffs(6);
   for (int i=0; i < nOffs; ++i)
   {
      targets.push_back(OffPositions::Position(targetRaRads,
					    targetDecRads));
   }

   vector<OffPositions::Position> expectedOffs;

   expectedOffs.push_back(OffPositions::Position(0.000000, -1.565560)); // (south) 
   expectedOffs.push_back(OffPositions::Position(0.000000, -1.544616)); // (north) 
   expectedOffs.push_back(OffPositions::Position(5.695147, -1.551918)); // (west)  
   expectedOffs.push_back(OffPositions::Position(0.588038, -1.551918)); // (east)  
   expectedOffs.push_back(OffPositions::Position(0.418142, -1.534340)); // (grid)  
   expectedOffs.push_back(OffPositions::Position(0.128049, -1.534881)); // (grid)  

   try {

      spOffPositions->getOffPositions(targets, timeSinceLastObsSecs,
				      offs);

      //cout << "south pole test\n" << *spOffPositions << endl;
      //spOffPositions->printPrimaryFovGrid(cout);

      cu_assert(offs.size() == static_cast<unsigned int>(nOffs));
      cu_assert(expectedOffs.size() == offs.size());

      for (unsigned int i=0; i<offs.size(); ++i)
      {
	 assertDoublesEqual(expectedOffs[i].raRads_, offs[i].raRads_, tolRads);
	 assertDoublesEqual(expectedOffs[i].decRads_, offs[i].decRads_, tolRads);
      }

   }
   catch (SseException & except)
   {
      cout << except << endl;



      // shouldn't get here
      cu_assert(0);
   }

   delete spOffPositions;
}



void TestOffPositions::testMultipleGetOffPositionsCalls()
{
   cout << "testMultipleGetOffPositionsCalls" << endl;

   // test successful pointing using the time since previous observation

   double targetRaRads(0);
   double targetDecRads(0);
   int timeSinceLastObsSecs(200);

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;
   
   targets.push_back(OffPositions::Position(targetRaRads,
					    targetDecRads));

   // should get the same result each time
   int repeatCount(2);
   for (int i=0; i<repeatCount; ++i)
   {
      offs.clear();
      offPositions_->getOffPositions(targets, timeSinceLastObsSecs,
				  offs);

      cu_assert(offs.size() == 1);

      double expectedOffRaRads(0.014584);
      double expectedOffDecRads(targetDecRads);
      
      double tolRads = 0.000001;
      assertDoublesEqual(expectedOffRaRads, offs[0].raRads_, tolRads);
      assertDoublesEqual(expectedOffDecRads, offs[0].decRads_, tolRads);
      
   }

   //offPositions_->printPrimaryFovGrid(cout);
}

void TestOffPositions::testTargetOutsidePrimaryFov()
{
   cout << "testTargetOutsidePrimaryFov" << endl;

   // force an error, catch the exception

   // pick a target outside the primary fov
   double targetRaRads(primaryCenterRaRads_);
   double targetDecRads(primaryCenterDecRads_ + primaryBeamsizeRads_);
   int timeSinceLastObsSecs(200);

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;
   
   targets.push_back(OffPositions::Position(targetRaRads,
					    targetDecRads));

   try 
   {
      offPositions_->getOffPositions(targets, timeSinceLastObsSecs,
				  offs);

      //cout << *offPositions_ << endl;

      // shouldn't get here
      cu_assert(0);
   }
   catch (SseException &except)
   {
      cu_assert(1);

      //cout << except << endl;

      string reason(except.descrip());

      cu_assert(reason.find("target is outside the primary") != string::npos);

   }

}

void TestOffPositions::testFailedToFindOff()
{
   cout << "testFailedToFindOff" << endl;

   // make it impossible for a valid off to be found:
   // make the synth beamsize larger than the primary fov

   double testPrimaryBeamsizeRads(SseAstro::degreesToRadians(3));
   double testSynthBeamsizeRads(SseAstro::degreesToRadians(9));
   double testMinBeamSepFactor(2.0);

   double testPrimaryCenterRaRads(0);
   double testPrimaryCenterDecRads(0);

   OffPositions * testOffPositions = new OffPositions(
      testSynthBeamsizeRads,
      testMinBeamSepFactor,
      testPrimaryCenterRaRads,
      testPrimaryCenterDecRads,
      testPrimaryBeamsizeRads);

   double targetRaRads(testPrimaryCenterRaRads);
   double targetDecRads(testPrimaryCenterDecRads);
   int timeSinceLastObsSecs(200);

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;
   
   targets.push_back(OffPositions::Position(targetRaRads,
					    targetDecRads));

   try 
   {
      testOffPositions->getOffPositions(targets, timeSinceLastObsSecs,
				  offs);

      // shouldn't get here
      cu_assert(0);
   }
   catch (SseException &except)
   {
      cu_assert(1);

      //cout << except << endl;

      string reason(except.descrip());

      //cout << *testOffPositions << endl;

      cu_assert(reason.find("failed to find valid OFF position") != string::npos);

   }

   delete testOffPositions;

}


void TestOffPositions::testTargetsNearPrimaryCenter()
{
   cout << "testTargetsNearPrimaryCenter" << endl;

   // a nominal case:
   // cluster three targets near the primary fov center,
   // with a timeSinceLastObsSecs that falls within the
   // primary FOV:
   //    target1 | target2
   //          --+--
   //            | target3
   // target1's off will go east by time
   // target2's off will go north (blocked to the south by target3)
   // target3's off will go south

   // long enough for off to fall in primary fov for target1, 
   // but not the others
   int timeSinceLastObsSecs(230);

   vector<OffPositions::Position> targets;
   vector<OffPositions::Position> offs;

   // assuming: primary center is at ra=0, dec=0
   // and min target sep is 2 synth beamsizes

   // upper left of primary center
   // wrap around ra=0
   double target1RaRads = primaryCenterRaRads_ - synthBeamsizeRads_ + TWOPI;
   double target1DecRads = primaryCenterDecRads_ + synthBeamsizeRads_;

   targets.push_back(OffPositions::Position(target1RaRads,
					    target1DecRads));

   // upper right of primary center
   double target2RaRads = primaryCenterRaRads_ + synthBeamsizeRads_;
   double target2DecRads = primaryCenterDecRads_ + synthBeamsizeRads_;
   
   targets.push_back(OffPositions::Position(target2RaRads,
					    target2DecRads));

   // lower right of primary center
   double target3RaRads = primaryCenterRaRads_ + synthBeamsizeRads_;
   double target3DecRads = primaryCenterDecRads_ - synthBeamsizeRads_;
   
   targets.push_back(OffPositions::Position(target3RaRads,
					    target3DecRads));

   offPositions_->getOffPositions(targets, timeSinceLastObsSecs,
				  offs);

   //cout << *offPositions_ << endl;
   //offPositions_->printPrimaryFovGrid(cout);
   
   cu_assert(offs.size() == targets.size());

   double tolRads = 0.000001;
   int offIndex(0);

   // target 1, off to the east, selected by elapsed time:
   double expectedOffRaRads(0.012788);
   double expectedOffDecRads(target1DecRads);
   assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);

   offIndex++;

   // target 2, north
   expectedOffRaRads = target2RaRads;
   expectedOffDecRads = target2DecRads + minPointSepRads_;

   assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);

   offIndex++;

   // target 3, south
   expectedOffRaRads = target3RaRads;
   expectedOffDecRads = target3DecRads - minPointSepRads_;

   assertDoublesEqual(expectedOffRaRads, offs[offIndex].raRads_, tolRads);
   assertDoublesEqual(expectedOffDecRads, offs[offIndex].decRads_, tolRads);

}


Test *TestOffPositions::suite()
{
	TestSuite *testSuite = new TestSuite("TestOffPositions");

        testSuite->addTest(new TestCaller <TestOffPositions>("testMoveNorth", &TestOffPositions::testMoveNorth));

        testSuite->addTest(new TestCaller <TestOffPositions>("testMoveSouth", &TestOffPositions::testMoveSouth));

        testSuite->addTest(new TestCaller <TestOffPositions>("testMoveEast", &TestOffPositions::testMoveEast));

        testSuite->addTest(new TestCaller <TestOffPositions>("testMoveWest", &TestOffPositions::testMoveWest));

        testSuite->addTest(new TestCaller <TestOffPositions>("testTimeSincePreviousObs", &TestOffPositions::testTimeSincePreviousObs));

        testSuite->addTest(new TestCaller <TestOffPositions>("testTargetOnSouthEdgeofPrimaryFov", &TestOffPositions::testTargetOnSouthEdgeofPrimaryFov));


        testSuite->addTest(new TestCaller <TestOffPositions>("testMultipleGetOffPositionsCalls", &TestOffPositions::testMultipleGetOffPositionsCalls));

        testSuite->addTest(new TestCaller <TestOffPositions>("testTargetOutsidePrimaryFov", &TestOffPositions::testTargetOutsidePrimaryFov));

        testSuite->addTest(new TestCaller <TestOffPositions>("testFailedToFindOff", &TestOffPositions::testFailedToFindOff));



#if 0
// disable tests depending on time,south,north,west,east off position order

        testSuite->addTest(new TestCaller <TestOffPositions>("testTimeTooLongSincePreviousObs", &TestOffPositions::testTimeTooLongSincePreviousObs));

        testSuite->addTest(new TestCaller <TestOffPositions>("testTargetRepeatedToForceAllPossibleOffs", &TestOffPositions::testTargetRepeatedToForceAllPossibleOffs));

        testSuite->addTest(new TestCaller <TestOffPositions>("testTargetsOnEastEdgeofPrimaryFov", &TestOffPositions::testTargetsOnEastEdgeofPrimaryFov));

         testSuite->addTest(new TestCaller <TestOffPositions>("testTargetsNearNorthPole", &TestOffPositions::testTargetsNearNorthPole));

         testSuite->addTest(new TestCaller <TestOffPositions>("testTargetsNearSouthPole", &TestOffPositions::testTargetsNearSouthPole));

        testSuite->addTest(new TestCaller <TestOffPositions>("testTargetsNearPrimaryCenter", &TestOffPositions::testTargetsNearPrimaryCenter));

#endif

        testSuite->addTest(new TestCaller <TestOffPositions>("testFoo", &TestOffPositions::testFoo));


	return testSuite;
}