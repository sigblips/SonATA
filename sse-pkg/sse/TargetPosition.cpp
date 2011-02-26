/*******************************************************************************

 File:    TargetPosition.cpp
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


#include "TargetPosition.h"
#include "SseAstro.h"
#include "Assert.h"
#include <iostream>

using namespace std;
 
static const double LIGHTYEARS_PER_PARSEC (3.26163626);
static const double MasPerArcSec(1000);

TargetPosition::TargetPosition(RaDec raDec, 
                               double pmRaMasYr, double pmDecMasYr,
                               double parallaxMas):
   raDec2000_(raDec), pmRaMasYr_(pmRaMasYr), pmDecMasYr_(pmDecMasYr),
   parallaxArcSec_(parallaxMas / MasPerArcSec)
{
}


RaDec TargetPosition::getJ2000RaDec() const 
{
   return raDec2000_;
}

double TargetPosition::getParallaxArcSec() const
{
   return parallaxArcSec_;
};


double TargetPosition::distanceLightYears() const 
{ 
   return LIGHTYEARS_PER_PARSEC / getParallaxArcSec();
}


/*
  Returns position in coordinates of date
   corrected for proper motion & precession.
*/
RaDec TargetPosition::positionAtNewEpochAndEquinox(const time_t &time) const
{
   double newRaRads, newDecRads;
   SseAstro::positionAtNewEpochAndEquinox(
      time, 
      raDec2000_.ra.getRadian(),
      raDec2000_.dec.getRadian(),
      pmRaMasYr_,
      pmDecMasYr_,
      &newRaRads, &newDecRads);

   RaDec outputRaDec;
   outputRaDec.ra.setRadian(newRaRads);
   outputRaDec.dec.setRadian(newDecRads);

   return outputRaDec;
}

/*
  Returns position at new epoch
  (i.e., corrects for proper motion only)
*/
RaDec TargetPosition::positionAtNewEpoch(const time_t &time) const
{
   double newRaRads, newDecRads;
   SseAstro::positionAtNewEpoch(
      time, 
      raDec2000_.ra.getRadian(),
      raDec2000_.dec.getRadian(),
      pmRaMasYr_,
      pmDecMasYr_,
      &newRaRads, &newDecRads);
   
   RaDec outputRaDec;
   outputRaDec.ra.setRadian(newRaRads);
   outputRaDec.dec.setRadian(newDecRads);

   return outputRaDec;
}