/*******************************************************************************

 File:    TargetMerit.h
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


#ifndef TargetMerit_H
#define TargetMerit_H

#include <vector>
#include <map>
#include <string>
class Target;

using std::string;
using std::vector;
using std::map;

/*
  Computes overall target merit based on selected
  merit factors.
 */

class TargetMerit
{
 public:

   enum MeritFactor { MeritStart, Dist, Catalog, Dec, CompletelyObs,
                      Meridian, TimeLeft, PrimaryId, MeritEnd };

   TargetMerit(const vector<MeritFactor> & factors);
   virtual ~TargetMerit();
   
   double overallMerit(Target *target);

   static vector<string> getAllMeritNames();
   
   static MeritFactor nameToMeritFactor(const string & name);
 private:

   // TargetMethod points to a method of Target that takes no args
   typedef double (Target::*TargetMethod)() const; 

   typedef map<MeritFactor, TargetMethod> TargetMethodMap;
   TargetMethodMap targetMethodMap_;

   const vector<MeritFactor> factors_;

   // Disable copy construction & assignment.
   // Don't define these.
   TargetMerit(const TargetMerit& rhs);
   TargetMerit& operator=(const TargetMerit& rhs);

};

#endif // TargetMerit_H