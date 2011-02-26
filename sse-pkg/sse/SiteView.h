/*******************************************************************************

 File:    SiteView.h
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

#ifndef SiteView_H
#define SiteView_H

/*
  Site location and visibility services for that location.
 */
#include "Angle.h"
#include <time.h>

class SiteView
{
public:

   SiteView(double longWestDeg, double latNorthDeg, double horizDeg);
   virtual ~SiteView();

   virtual double getHourAngle(double decDeg) const;
   virtual double getHourAngleRads(double decRads) const;

   virtual double lmstRads(time_t time) const;

   virtual double getLatRads();

   virtual bool isTargetVisible(double lmstRads, const RaDec & raDec,
				double & timeSinceRiseRads,
				double & timeUntilSetRads) const;

protected:

   enum Visibility { VISIBILITY_UNINIT, ALWAYS_UP, SOMETIMES_UP, NEVER_UP};

   virtual Visibility riseSet(const RaDec &raDec, double &haRiseRads,
			      double &haSetRads) const;

private:
   // Disable copy construction & assignment.
   // Don't define these.
   SiteView(const SiteView& rhs);
   SiteView& operator=(const SiteView& rhs);

   double longWestDeg_;
   double latNorthDeg_;
   double horizDeg_;
   double horizRefractDeg_;
};

#endif // SiteView_H