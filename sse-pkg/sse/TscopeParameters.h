/*******************************************************************************

 File:    TscopeParameters.h
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


#ifndef TSCOPE_PARAMETERS_H
#define TSCOPE_PARAMETERS_H

#include "SeekerParameterGroup.h"
#include "sseInterface.h"
#include "RangeParameter.h"
#include "sseTscopeInterface.h"

class TscopeParametersInternal;
class TscopeOperation;

class TscopeParameters : public SeekerParameterGroup
{
 public:

   TscopeParameters(string command);
   TscopeParameters(const TscopeParameters& rhs);
   TscopeParameters& operator=(const TscopeParameters& rhs);
   ~TscopeParameters();

   double getBasebandTuneOffsetMhz() const;
   bool setBasebandTuneOffsetMhz(double offsetMhz);

   double getBasebandCenterTuneOffsetMhz() const;
   double getPrimaryFovAtOneGhzDeg() const;
   double getBeamsizeAtOneGhzArcSec() const;

   bool setTuningSkyFreqMhz(const string &tuningName, double skyFreqMhz);
   double getTuningSkyFreqMhz(const string &tuningName) const;

   TscopeCalRequest getCalRequest() const;

   const string getCalType() const;
   bool setCalType(const string & calTypeName);

   int getCalIterations() const;
   bool setCalIterations(int calIterations);

   int getCalTimeSecs() const;
   bool setCalTimeSecs(int timeSecs);

   int getCalNumCycles() const;
   bool setCalNumCycles(int numCycles);

   double getSiteLatNorthDeg();
   double getSiteLongWestDeg();
   double getSiteHorizDeg();
   string getSiteName();

   bool isAntListSourceParam();
   string getXpolAnts();
   string getYpolAnts();
   string getPrimaryAnts();

   // ----- immediate commands -----

   const char * allocate(const string & subarray, const string& tscopeName);
   const char * deallocate(const string & subarray, const string& tscopeName);

   const char * setup(const string& tscopeName);
   const char * cleanup(const string& tscopeName);

   const char * connect(const string& tscopeName);
   const char * disconnect(const string& tscopeName);

   const char * assign(const string& beamName, const string& subarray,
                       const string& tscopeName);
   const char * monitor(int periodSecs, const string& tscopeName);

   const char * bfstop(const string& tscopeName);
   const char * bfreset(const string& tscopeName);
   const char * bfpoint(const string& tscopeName);
   const char * bfinit(const string& tscopeName);

   const char * bfsetcoords(
      const string& beamName,
      const string& coordSystem,
      double coord1,
      double coord2, 
      const string& tscopeName);

   const char * bfcal(
      const string& calType,  // delay, phase, freq
      const string& integrateKeyword,
      int integrateSecs,
      const string& cyclesKeyword,
      int numCycles,
      const string& iterateKeyword,
      int calIterations,
      const string& tscopeName);

   const char * names();

   const char * point(
      const string& subarray,
      const string& coordSystem,
      double coord1,
      double coord2, 
      const string& tscopeName);

   const char * bfclearcoords(const string& tscopeName);
   const char * bfclearants(const string& tscopeName);
   
   const char * intrin(const string& tscopeName);
   const char * reset(const string& tscopeName);

   const char * reqstat(const string& tscopeName);
   const char * status(const string& tscopeName);

   const char * resetsocket(const string& tscopeName);
   const char * restart(const string& tscopeName);

   const char * statusupdate(const string& tscopeName);
   const char * tune(const string& tuningName,
		     double rfSkyFreqMhz, const string& tscopeName);

   const char * shutdown(const string& tscopeName);

   const char * stop(const string & subarray,
                     const string& tscopeName);
   const char * stow(const string & subarray,
                     const string& tscopeName);
   const char * wrap(const string & subarray, int wrapNumber, 
                     const string& tscopeName);

   const char * sim(const string& tscopeName);
   const char * unsim(const string& tscopeName);

   const char * zfocus(const string & subarray, 
                       double rfSkyFreqMhz, const string& tscopeName);

   const char * send(
      const string & tscopeName,
      const string & cmd,
      const string & cmdArg1,
      const string & cmdArg2,
      const string & cmdArg3,
      const string & cmdArg4,
      const string & cmdArg5,
      const string & cmdArg6,
      const string & cmdArg7,
      const string & cmdArg8);

   const char * autoselectants(const string & bflist,
                               const string& tscopeName);

 protected:
   void addParameters();
   virtual void addAllImmedCmdHelp();

   const char * runSubarrayCmd(
      const string &subarray,
      TscopeOperation &operation,
      const string & tscopeName);

   const char * runCmd(
      TscopeOperation &operation,
      const string & tscopeName);

   RangeParameter<double> * findTuningParamForTuningName(
      const string & tuningName) const;

 private:
   
   TscopeParametersInternal *internal_;

};

#endif