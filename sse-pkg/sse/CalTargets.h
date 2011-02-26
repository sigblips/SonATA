/*******************************************************************************

 File:    CalTargets.h
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


#ifndef CalTargets_H
#define CalTargets_H

#include "MysqlQuery.h"
#include <vector>

using std::vector;

struct Flux
{
   double freqMhz_;
   double fluxJy_;

   Flux(double freqMhz, double fluxJy)
      : freqMhz_(freqMhz), fluxJy_(fluxJy)
   {
   }
};

struct TargetInfo 
{
   int targetId;
   string name;
   double ra2000Hours;
   double dec2000Deg;
   double fluxJy;
   vector<Flux> fluxVect;
};

class CalTargets
{
 public:
    CalTargets();
    virtual ~CalTargets();

    void loadTargetsFromDb(MYSQL *db, const string & catalog);

    void computeFluxAtFreq(double calFreqMhz);

    const vector<TargetInfo> & getTargetInfo() const;

 private:

    void getCalTargetInfo(MYSQL *db, const string & catalog);
    
    void getCalTargetFluxes(MYSQL *db);

    vector<TargetInfo> targetInfoVect_;

    // Disable copy construction & assignment.
    // Don't define these.
    CalTargets(const CalTargets& rhs);
    CalTargets& operator=(const CalTargets& rhs);

};

#endif // CalTargets_H