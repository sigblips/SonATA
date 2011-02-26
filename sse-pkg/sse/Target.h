/*******************************************************************************

 File:    Target.h
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

/* -*-c++-*- */
/*****************************************************************
 * target.h - declaration of functions defined in target.h
 *
 * PURPOSE:  
 *****************************************************************/

#ifndef TARGET_H
#define TARGET_H

#include "TargetPosition.h"
#include "SiteView.h"
#include "Range.h"
#include "TargetId.h"
#include <vector>
#include <set>
#include <map>
#include <string>


class SiteView;
using std::string;

class Target
{
public:

   enum CatalogPriority { CatalogHighPriority, CatalogLowPriority };

   Target(RaDec raDec, 
          double pmRaMasYr,
          double pmDecMasYr,
	  double parallaxMas,
	  TargetId targetId, 
	  TargetId primaryTargetId, 
	  const string & catalog,
	  const ObsRange & desiredObservingFreqRangesMhz, 
	  double maxDistLightYears,
	  SiteView * siteView,
	  double obsLengthSec,
	  float64_t minBandwidthMhz,
	  double minRemainingTimeOnTargetRads
      );
   void setCatalogPriority(CatalogPriority priority, int order);
   void setPreferredPrimaryTargetId(TargetId targetId);
   void setTime(double lmstRads, time_t obsDate);
   TargetId getTargetId() const;
   TargetId getPrimaryTargetId() const;
   RaDec getRaDec() const;		 // RA and Dec J2000
   RaDec getPosition() const;    // RA and Dec for current epoch
   string getCatalog() const;
   time_t getTime() const;

   double overallMerit();
   double obsMerit() const;
   double distMerit() const;
   double catalogMerit() const;
   double completelyObservedMerit() const;
   double decMerit() const;
   double nearMeridianMerit() const;
   double timeLeftMerit() const;
   double primaryTargetIdMerit() const;
					
   void addObserved(float64_t lowFreq, float64_t highFreq);
   void addObservedOutOfOrder(float64_t lowFreq, float64_t highFreq);
   void removeObserved(float64_t lowFreq, float64_t highFreq);

   void printSeqs(ostream& os) const;
   void printSetTime(ostream &os);

   ObsRange observed() const;
   ObsRange unobserved() const;
   bool hasRangeGt(float64_t minimumBandwidth) const;
   double totalRangeGt(float64_t minBandwidth) const;
   float64_t fractionObserved() const;

   friend ostream &operator<<(ostream& os, const Target &target);

   void resetObservations();
   void setAlreadyCompletelyObserved(bool value);
   bool isAlreadyCompletelyObserved();

   bool isVisible() const;
   double getTimeSinceRiseRads() const;
   double getTimeUntilSetRads() const;

   time_t getRiseTimeUtc() const;
   time_t getSetTimeUtc() const;

protected:
   void setVisibility();
   void calculateUnobserved() const;

   void computeOverallMerit();
   void computeTimeLeftMerit();
   void computeRiseSetTimes();

private:

   TargetPosition targetPosition_;
   TargetId setiTargetId_;
   TargetId primaryTargetId_;
   string catalog_;
   double maxDistLightYears_;
   SiteView *siteView_;
   mutable double obsLengthSec_;
   double minUnobsSegmentBwMhz_;
   double minRemainingTimeOnTargetRads_;
   double lmstRads_;
   time_t obsDate_;
   bool alreadyCompletelyObserved_;
   mutable bool unobservedUpToDate_;
   mutable double overallMerit_;
   mutable double timeLeftMerit_;
   mutable bool meritCalculated_;
   mutable double beforeSet_;
   time_t riseTimeUtc_;
   time_t setTimeUtc_;
   double catalogPriorityMerit_;
   TargetId preferredPrimaryTargetId_;

   typedef std::map<const string, int> value_t;
   typedef value_t::iterator valueIterator;

   mutable ObsRange unobservedFreqRange_;
   ObsRange desiredFreqRange_;
   ObsRange observedFreqRange_;

   bool isVisible_;
   double timeSinceRiseRads_;
   double timeUntilSetRads_;

};



#endif /* TARGET_H */