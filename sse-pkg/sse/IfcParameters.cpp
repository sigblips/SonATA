/*******************************************************************************

 File:    IfcParameters.cpp
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

 
#include <ace/OS.h>            // for SwigScheduler.h
#include <IfcParameters.h>
#include <typeinfo>
#include <IfcProxy.h>
#include "Site.h"
#include "RangeParameter.h"
#include "ChoiceParameter.h"
#include "AtaInformation.h"

static const string allComponents("all");
const double DefaultIfcSkyFreqMhz =  AtaInformation::AtaDefaultSkyFreqMhz; 

// Since there are multiple ifc parameter sets, 
// use the command name as the prefix to the database table name
// and column.

static const char *DbTableNameSuffix="Parameters";
static const char *IdColNameInActsTableSuffix="ParametersId";

static const char *IfSourceSkyChoice = "sky";
static const char *IfSourceTestChoice = "test";

static const char *AttnCntlUserChoice = "user";
static const char *AttnCntlAutoChoice = "auto";


struct IfcParametersInternal
{
public:

   // methods:
   IfcParametersInternal();  
   // default copy constructor and assignment operator are safe

   ChoiceParameter<string>    ifSource;

   ChoiceParameter<string>    attnCtrl;
   RangeParameter<int32_t>    attnL;
   RangeParameter<int32_t>    attnR;

   RangeParameter<int32_t>    histogramLen;

   RangeParameter<float64_t>  varL;
   RangeParameter<float64_t>  varR;

   RangeParameter<float64_t>  varTol;
};

IfcParametersInternal::IfcParametersInternal():
   ifSource("ifsource", "", "set switch for IF source", IfSourceTestChoice),
   attnCtrl("attnctrl", "", "attenuator control", AttnCntlUserChoice),
   attnL("attnl", "dB", "Left attenuator", 0, 0, 11),
   attnR("attnr", "dB", "Right attenuator", 0, 0, 11),
   histogramLen("hlen", "", "histogram length", 100, 0, 1000000000),
   varL("varl", "", "left pol variance", 16, 0, 1000),
   varR("varr", "", "right pol variance", 16, 0, 1000),
   varTol("vartol", "%", "variance tolerance", 15, 0, 100)
{
}

// Since there are multiple ifc parameter sets, 
// use the command name as the prefix to the database table name
// and column.

IfcParameters::IfcParameters(const string &command) : 
   SeekerParameterGroup(command, string(command + DbTableNameSuffix),
			string(command +  IdColNameInActsTableSuffix)),
   internal_(new IfcParametersInternal())

{
   internal_->attnCtrl.addChoice(AttnCntlUserChoice);
   internal_->attnCtrl.addChoice(AttnCntlAutoChoice);

   internal_->ifSource.addChoice(IfSourceSkyChoice);
   internal_->ifSource.addChoice(IfSourceTestChoice);

   addParameters();
   addAllImmedCmdHelp();
}


IfcParameters::IfcParameters(const IfcParameters& rhs):
   SeekerParameterGroup(rhs.getCommand(), rhs.getDbTableName(), 
			rhs.getIdColNameInActsTable()),
   internal_(new IfcParametersInternal(*rhs.internal_))
{
   setSite(rhs.getSite());
   addParameters();
}

IfcParameters& IfcParameters::operator=(const IfcParameters& rhs)
{
   if (this == &rhs)
   {
      return *this;
   }
  
   setCommand(rhs.getCommand());
   eraseParamList();
   setSite(rhs.getSite());
   delete internal_;
   internal_ = new IfcParametersInternal(*rhs.internal_);

   addParameters();
   return *this;
}

IfcParameters::~IfcParameters()
{
   delete internal_;
}

void IfcParameters::addParameters()
{
   addParam(internal_->attnL);
   addParam(internal_->attnR);
   addParam(internal_->attnCtrl);
   addParam(internal_->histogramLen);
   addParam(internal_->varL);
   addParam(internal_->varR);
   addParam(internal_->varTol);
   addParam(internal_->ifSource);

   sort();
}


void IfcParameters::addAllImmedCmdHelp()
{
   addImmedCmdHelp("stxvariance <name | 'all'> - set variance of stx(s)");
}

static void getAllIfcs(IfcList &allIfcs, Site *site)
{
   Assert(site);
   site->ifcManager()->getProxyList(&allIfcs);
}


static void printNameError(const string &cmdName, const string &methodName, 
			   const string &ifcName)
{
   cerr << cmdName << " " << methodName 
	<< ": The requested Ifc(s) are not connected: " 
	<< ifcName << endl;
}


// TBD move to IfcImmedCmds
void IfcParameters::stxvariance(const string& ifcName) 
{
   cout << "STX set variance: " << ifcName << endl;

   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);
   for (IfcList::iterator ifc = allIfcs.begin(); ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents) 
      {
	 (*ifc)->stxSetVariance(getVarLeft(), getVarRight(), getStxTolerance(),
				getStxLength());
	 found = true;
      } 
   }

   if (!found) 
   {
      printNameError(getCommand(), "stxvariance", ifcName);
   }

}

int32_t IfcParameters::getHistogramLength() const
{
   return(internal_->histogramLen.getCurrent());
}

bool IfcParameters::useAutoAttnCtrl() const
{
   if (internal_->attnCtrl.getCurrent() == AttnCntlAutoChoice)
   {
      return(true);
   }

   return(false);
}


int32_t IfcParameters::getAttnl() const 
{
   return internal_->attnL.getCurrent();
}

int32_t IfcParameters::getAttnr() const 
{
   return internal_->attnR.getCurrent();
}

float64_t IfcParameters::getVarLeft() const 
{
   return internal_->varL.getCurrent();
}

float64_t IfcParameters::getVarRight() const 
{
   return internal_->varR.getCurrent();
}

float64_t IfcParameters::getStxTolerance() const 
{
   return internal_->varTol.getCurrent();
}

int32_t IfcParameters::getStxLength() const 
{
   return internal_->histogramLen.getCurrent();
}


const bool IfcParameters::ifSourceIsTest() 
{
  if (internal_->ifSource.getCurrent() == IfSourceTestChoice)
  {
    return(true);
  }
  else 
  {
    return(false);
  }
}

const bool IfcParameters::ifSourceIsSky() 
{
  if (internal_->ifSource.getCurrent() == IfSourceSkyChoice)
  {
    return(true);
  }
  else
  {
    return(false);
  }
}

