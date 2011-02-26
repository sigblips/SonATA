/*******************************************************************************

 File:    TargetMerit.cpp
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


#include "TargetMerit.h" 
#include "Target.h"
#include "SseException.h"
#include "SseUtil.h"

using namespace std;

static const char * AllMeritNames[] = { "MeritStart",
                                        "dist", "catalog", "dec", 
                                        "completelyobs", "meridian", "timeleft",
                                        "primaryid",
                                        "MeritEnd" };

#define CALL_METHOD(object,ptrToMethod)  ((object)->*(ptrToMethod)) 

TargetMerit::TargetMerit(const vector<MeritFactor> & factors):
   factors_(factors)
{
   // Store all possible Target merit methods indexed by merit factor 
   targetMethodMap_[Dist] = &Target::distMerit;
   targetMethodMap_[Catalog] = &Target::catalogMerit;
   targetMethodMap_[Dec] = &Target::decMerit;
   targetMethodMap_[CompletelyObs] = &Target::completelyObservedMerit;
   targetMethodMap_[Meridian] = &Target::nearMeridianMerit;
   targetMethodMap_[TimeLeft] = &Target::timeLeftMerit;
   targetMethodMap_[PrimaryId] = &Target::primaryTargetIdMerit;

   // validate requested factors
   for (unsigned int i=0; i<factors_.size(); ++i)
   {
      if (targetMethodMap_.find(factors_[i]) == targetMethodMap_.end())
      {
         throw SseException("TargetMerit: unexpected merit factor: "
                            + SseUtil::intToStr(factors_[i]), __FILE__, __LINE__);
      }
   }

}

TargetMerit::~TargetMerit()
{
}

/*
  Multiply all requested merit factors together
 */
double TargetMerit::overallMerit(Target *target)
{
   double merit(1);

   for (unsigned int i=0; i<factors_.size(); ++i)
   {
      merit *= CALL_METHOD(target, targetMethodMap_[factors_[i]])();
   }

   return merit;
}

vector<string> TargetMerit::getAllMeritNames()
{
   vector<string> meritNames;

   for (int i=MeritStart+1; i<MeritEnd; ++i)
   {
      meritNames.push_back(AllMeritNames[i]);
   }
   return meritNames;
}

TargetMerit::MeritFactor TargetMerit::nameToMeritFactor(const string &name)
{
   for (int i=MeritStart+1; i<MeritEnd; ++i)
   {
      if (name == AllMeritNames[i])
      {
         return static_cast<MeritFactor>(i);
      }
   }

   throw SseException("TargetMerit: unknown merit name: "
                      + name, __FILE__, __LINE__);
}