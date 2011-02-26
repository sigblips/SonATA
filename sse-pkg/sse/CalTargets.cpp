/*******************************************************************************

 File:    CalTargets.cpp
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



#include "CalTargets.h" 
#include "SseUtil.h"
#include "Interpolate.h"
#include <sstream>

// tbd temp scaffolding
const double MinFreqMhz(500);
const double MaxFreqMhz(11200);

using namespace std;

CalTargets::CalTargets()
{
}

CalTargets::~CalTargets()
{
}

void CalTargets::loadTargetsFromDb(MYSQL *db, const string & catalog)
{
   getCalTargetInfo(db, catalog);

   getCalTargetFluxes(db);
}
 
const vector<TargetInfo> & CalTargets::getTargetInfo() const
{
   return targetInfoVect_;
}


void CalTargets::getCalTargetInfo(MYSQL *db, const string & catalog)
{
   stringstream targetCatQuery;
   
   targetCatQuery << "select targetId, aliases, ra2000Hours, dec2000Deg "
                  << "from TargetCat WHERE ";
   targetCatQuery << "catalog = '" << catalog << "'";

   enum resultCols { targetIdCol, aliasesCol, raCol, decCol,
                     numCols };

   MysqlQuery query(db);
   query.execute(targetCatQuery.str(), numCols, __FILE__, __LINE__);

   while (MYSQL_ROW row = mysql_fetch_row(query.getResultSet()))
   {
      int targetId(query.getInt(row, targetIdCol, 
                                __FILE__, __LINE__));
      
      string aliases(query.getString(row, aliasesCol,
                                     __FILE__, __LINE__));
      
      double ra2000Hours(query.getDouble(row, raCol, 
                                         __FILE__, __LINE__));
      
      double dec2000Deg(query.getDouble(row, decCol, 
                                        __FILE__, __LINE__));

      string name("");
      // grab the first alias
      vector<string> tokens(SseUtil::tokenize(aliases, " "));
      if (tokens.size() > 0)
      {
         name = tokens[0];
      }

      TargetInfo info;
      info.targetId = targetId;
      info.name = name;
      info.ra2000Hours = ra2000Hours;
      info.dec2000Deg = dec2000Deg;
      info.fluxJy = 0;

      targetInfoVect_.push_back(info);
   }
}

/*
  Get fluxes for each cal target
*/
void CalTargets::getCalTargetFluxes(MYSQL *db)
{
   for (unsigned int i=0; i<targetInfoVect_.size(); ++i)
   {
      stringstream queryStrm;
      
      queryStrm << "select freqMhz, fluxJy "
                << "from CalTargetFlux "
                << "WHERE "
                << "targetId = " << targetInfoVect_[i].targetId;
      
      enum resultCols {freqMhzCol, fluxJyCol, numCols};
      
      MysqlQuery query(db);
      query.execute(queryStrm.str(), numCols, __FILE__, __LINE__);

      while (MYSQL_ROW row = mysql_fetch_row(query.getResultSet()))
      {
         double freqMhz(query.getDouble(row, freqMhzCol, 
                                        __FILE__, __LINE__));
         
         double fluxJy(query.getDouble(row, fluxJyCol, 
                                       __FILE__, __LINE__));
         
         targetInfoVect_[i].fluxVect.push_back(Flux(freqMhz, fluxJy));
      }
   }
}

/*
  For each cal target:
  Interpolate flux for cal freq, update that freq in targetInfoVect.
*/
void CalTargets::computeFluxAtFreq(double calFreqMhz)
{
   for (unsigned int i=0; i<targetInfoVect_.size(); ++i)
   {
      InterpolateLinear interpFluxes;
 
      // add extreme values so can interpolate across entire band
      const double extremeFluxJy(0);
      const double freqPadMhz(10);
      interpFluxes.addValues(MinFreqMhz-freqPadMhz, extremeFluxJy);
      interpFluxes.addValues(MaxFreqMhz+freqPadMhz, extremeFluxJy);

      TargetInfo & targetInfo(targetInfoVect_[i]);
      vector<Flux> & fluxVect(targetInfo.fluxVect);
      
      for (unsigned int fluxIndex=0; fluxIndex < fluxVect.size(); ++fluxIndex)
      {
         interpFluxes.addValues(fluxVect[fluxIndex].freqMhz_, 
                                fluxVect[fluxIndex].fluxJy_);
      }

      double calFluxJy = -1;
      if (! interpFluxes.inter(calFreqMhz, calFluxJy))
      {
         calFluxJy = 0;
      }
      targetInfo.fluxJy = calFluxJy;
   }
}
