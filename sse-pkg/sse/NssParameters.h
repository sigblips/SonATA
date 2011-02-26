/*******************************************************************************

 File:    NssParameters.h
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



#ifndef NssParameters_H
#define NssParameters_H

#include "ActivityId.h"
#include <iostream>
#include <string>
#include <mysql.h>
#include <tcl.h>

using std::ostream;
using std::string;
using std::cout;

class IfcParameters;
class IfcImmedCmds;
class TscopeParameters;
class TestSigImmedCmds;
class TestSigParameters;
class DxParameters;
class DxArchiverParameters;
class ChannelizerParameters;
class ActParameters;
class SchedulerParameters;
class DbParameters;
class ComponentControlImmedCmds;

class Site;


const char * const IfcCommandName = "ifc";
const char * const Ifc1CommandName = "ifc1";
const char * const Ifc2CommandName = "ifc2";
const char * const Ifc3CommandName = "ifc3";
const char * const TscopeCommandName = "tscope";
const char * const TsigCommandName = "tsig";
const char * const Tsig1CommandName = "tsig1";
const char * const Tsig2CommandName = "tsig2";
const char * const Tsig3CommandName = "tsig3";
const char * const DxCommandName = "dx";
const char * const DxArchiverCommandName = "arch";
const char * const ChannelizerCommandName = "channel";
const char * const ActParamCommandName = "act";
const char * const SchedulerParamCommandName = "sched";
const char * const DbParamCommandName = "db";
const char * const ComponentControlName = "control";

class NssParameters {

 public:

  // TBD references would be better, but
  // use pointers to make swig happy

  IfcImmedCmds* ifc_;  
  IfcParameters* ifc1_;  
  IfcParameters* ifc2_;  
  IfcParameters* ifc3_;  
  TscopeParameters* tscope_;
  TestSigImmedCmds* tsig_;
  TestSigParameters* tsig1_;
  TestSigParameters* tsig2_;
  TestSigParameters* tsig3_;
  DxParameters* dx_;
  DxArchiverParameters* arch_;
  ChannelizerParameters* chan_;
  ActParameters* act_;
  SchedulerParameters* sched_;
  DbParameters* db_;
  ComponentControlImmedCmds* componentControl_;

  NssParameters(IfcImmedCmds* ifcImmedCmds,
		IfcParameters* ifc1Parameters,
		IfcParameters* ifc2Parameters,
		IfcParameters* ifc3Parameters,
		TscopeParameters* tscopeParameters, 
		TestSigImmedCmds* testSigImmedCmds, 
		TestSigParameters* testSig1Parameters, 
		TestSigParameters* testSig2Parameters, 
		TestSigParameters* testSig3Parameters, 
		DxParameters* dxParameters,
		DxArchiverParameters* dxArchiverParameters,
		ChannelizerParameters* channelizerParameters,
		ActParameters* actParameters,
		SchedulerParameters* schedulerParameters,
		DbParameters* db,
		ComponentControlImmedCmds *componentControlImmedCmds);

  NssParameters(const NssParameters& rhs);
  NssParameters& operator=(const NssParameters& rhs);
  ~NssParameters();
  bool isValid() const;
  const char * setDefault();
  const char * show() const;
  const char * names() const;
  void printHelpDescription(ostream &os = cout) const;
  void printHelpParametersOverview(ostream &os = cout) const;
  void printHelpImmediateCommandsOverview(ostream &os = cout) const;
  void printMiscCommandsHelp(ostream &os = cout) const;
  void help(ostream& os = cout) const;
  const char * save(const string& filename) const;
  void setSite(Site *site);
  Site *getSite();
  friend ostream& operator << (ostream &strm, const class NssParameters& a);
  void restore(MYSQL* db, ActivityId_t activityId);

  const char *status() const;

 private:
  mutable string outString_;
  Site *site_;
  bool cleanup_;		// if true, delete members
				// otherwise, do not delete members
				// TODO:  this should be changed
  
};

extern IfcImmedCmds ifcGlobal;
extern IfcParameters ifc1Global;
extern IfcParameters ifc2Global;
extern IfcParameters ifc3Global;
extern TscopeParameters tscopeGlobal;
extern TestSigImmedCmds tsigGlobal;
extern TestSigParameters tsig1Global;
extern TestSigParameters tsig2Global;
extern TestSigParameters tsig3Global;
extern DxParameters dxGlobal;
extern DxArchiverParameters archGlobal;
extern ChannelizerParameters chanGlobal;
extern ActParameters actGlobal;
extern SchedulerParameters schedGlobal;
extern DbParameters dbGlobal;
extern ComponentControlImmedCmds componentControlImmedCmdsGlobal;
extern NssParameters paraGlobal;

const char *utc();
const char *help(const char *subsystemName);
void systemlog(const char *msg);
const char * expectedcomponents();
const char * reconfig(const char *expectedComponentsFilename);
void start(const char * strategyNames);
void stop();
void wrapup();
void swigExit(ClientData);

class VerboseForSwig
{
public:

  int level(int verboseLevel) const;
  int showlevel() const;
    
};

extern VerboseForSwig verboseGlobal;


#endif