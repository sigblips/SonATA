/*******************************************************************************

 File:    SiteView.cpp
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

#include "SiteView.h" 
#include "SseAstro.h"
#include "Assert.h"
#include <iostream>

using namespace std;

SiteView::SiteView(double longWestDeg, double latNorthDeg, double horizDeg)
   :
   longWestDeg_(longWestDeg),
   latNorthDeg_(latNorthDeg),
   horizDeg_(horizDeg)
{
   // Take care of atmospheric refraction by adjusting the horizon.
   double atmRefractDeg(SseAstro::atmosRefractDeg(horizDeg_));
   horizRefractDeg_ = horizDeg_ - atmRefractDeg;
}

SiteView::~SiteView()
{
}

double SiteView::getHourAngle(double decDeg) const
{
   return SseAstro::hourAngle(decDeg, latNorthDeg_, horizRefractDeg_);
}

double SiteView::getHourAngleRads(double decRads) const
{
   return SseAstro::hoursToRadians(
      SseAstro::hourAngle(SseAstro::radiansToDegrees(decRads),
                          latNorthDeg_, horizRefractDeg_));
}

double SiteView::getLatRads()
{
   return SseAstro::degreesToRadians(latNorthDeg_);
}

// return local mean sidereal time in radians for this antenna
double SiteView::lmstRads(time_t time) const
{
   return SseAstro::lmstRads(time, 
                             SseAstro::degreesToRadians(longWestDeg_));
}

/* Look up the rise and set hour angles for the RaDec target.
 * The function value returns one of:  ALWAYS_UP (i.e., circumpolar),
 * SOMETIMES_UP, or NEVER_UP.
 */

SiteView::Visibility SiteView::riseSet(
   const RaDec &raDec, double &haRiseRads, double &haSetRads) const
{
   double hourAngleRads = getHourAngleRads(raDec.dec);
#if 0
   cout << "riseset: "
        << "raDec: " << endl
        << " hourangrads: " << hourAngleRads << endl;
#endif
   if (hourAngleRads <= 0.0)
   {
      return NEVER_UP;
   } 
   else
   {
      haRiseRads = hourAngleRads;
      haSetRads = hourAngleRads;

      // check total up time to see if the target is always up
      if ((haSetRads + haRiseRads) >= 2 * M_PI)
      {
	 return ALWAYS_UP;
      }

      return SOMETIMES_UP;
   }
}


/* Determine if a target is visible at the given lmst (local mean sidereal time).
 * Inputs:
 *   lmst 
 *   Target RA & Dec
 * Outputs:
 *   Returns true if target is visible, and if so, then also returns:
 *   time since the target rose (timeSinceRiseRads), sidereal
 *   time until the target sets (timeUntilSetRads), sidereal
 *
 * Note: all units are in radians.
 */

bool SiteView::isTargetVisible(double lmstRads, const RaDec & raDec,
                               double & timeSinceRiseRads,
                               double & timeUntilSetRads) const
{
   double haRiseRads;
   double haSetRads;

   Visibility visibility = riseSet(raDec, haRiseRads, haSetRads);

   if (visibility == NEVER_UP)
   {
      return false;
   }
   else if (visibility == ALWAYS_UP)
   {
      timeSinceRiseRads = 2 * M_PI;
      timeUntilSetRads = 2 * M_PI;
      return true;
   }

   Assert(visibility == SOMETIMES_UP);

   double targetRiseHaRads = raDec.ra - haRiseRads;
   if (targetRiseHaRads < 0)
   {
      targetRiseHaRads += 2 * M_PI;
   }

   double targetSetHaRads = raDec.ra + haSetRads;
   if (targetSetHaRads > 2 * M_PI)
   {
      targetSetHaRads -= 2 * M_PI;
   }

// debug
#if 0
   cout << "lmst rads = " << lmstRads 
	<< " hours = " << SseAstro::radiansToHours(lmstRads) << endl

	<< "ra rads = " << raDec.ra.getRadian() 
	<< " hours = " << SseAstro::radiansToHours(raDec.ra.getRadian()) << endl
	
	<< "dec rads = " << raDec.dec.getRadian()
	<< " deg = " << SseAstro::radiansToDegrees(raDec.dec.getRadian()) << endl

	<< "targetRiseHaRads = " << targetRiseHaRads 
	<< " hours = " << SseAstro::radiansToHours(targetRiseHaRads) << endl

	<< "targetSetHaRads = " << targetSetHaRads 
	<< " hours = " << SseAstro::radiansToHours(targetSetHaRads) << endl
      
	<< "haRiseRads rads = " << haRiseRads
	<< " hours = " << SseAstro::radiansToHours(haRiseRads) << endl

	<< "haSetRads rads = " << haSetRads
	<< " hours = " << SseAstro::radiansToHours(haSetRads) << endl

	<< endl;

#endif

   // determine if the target is currently visible, ie, if the
   // lmst falls within the target's rise and set hour angles.

   bool visible = false;
   if (targetRiseHaRads <= targetSetHaRads)
   {
      if (lmstRads > targetRiseHaRads && lmstRads < targetSetHaRads)
      {
	 visible = true;
	 
	 timeSinceRiseRads = lmstRads - targetRiseHaRads;
	 timeUntilSetRads = targetSetHaRads - lmstRads;
      }
   }
   else
   {
      // rise & set hour angles straddle zero hours RA
      if (lmstRads >= targetSetHaRads && lmstRads <= targetRiseHaRads)
      {
	 visible = false;
      } 
      else 
      {
	 visible = true;

	 timeSinceRiseRads = lmstRads - targetRiseHaRads;
	 if (timeSinceRiseRads < 0)
	 {
	    timeSinceRiseRads += 2 * M_PI;
	 }
	 
	 timeUntilSetRads = targetSetHaRads - lmstRads;
	 if (timeUntilSetRads < 0)
	 {
	    timeUntilSetRads += 2 * M_PI;
	 }
	 
      }
 
   }

   return visible;
}