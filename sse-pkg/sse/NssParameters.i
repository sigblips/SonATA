/*******************************************************************************

 File:    NssParameters.i
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

/* test
*/
// -*-c++-*-   (set Emacs editing mode)

// Swig input file defining interfaces to sse parameter classes

// Revert to old swig version 1.3.21 behavior of handling default arguments
%feature("compactdefaultargs");


%{
#include "Scheduler.h"
#include "NssParameters.h"
#include "IfcParameters.h"
#include "IfcImmedCmds.h"
#include "TscopeParameters.h"
#include "TestSigImmedCmds.h"
#include "TestSigParameters.h"
#include "DxParameters.h"
#include "DxArchiverParameters.h"
#include "ChannelizerParameters.h"
#include "ActParameters.h"
#include "DbParameters.h"
#include "SchedulerParameters.h"
#include "ComponentControlImmedCmds.h"

// for program & interface versions
#include "sseVersion.h"
#include "sseDxInterface.h"
#include "sseDxArchiverInterface.h" 
#include "sseChannelizerInterface.h" 
#include "sseTscopeInterface.h" 
#include "sseIfcInterface.h"
#include "sseTestSigInterface.h"

#include <fstream>
#include <sstream>

%}

// From SwigScheduler.h
void start(const char * strategyNames);
void stop();
void wrapup();

class VerboseForSwig
{
public:
  int level(int verboseLevel) const;
  int showlevel() const;
};

extern VerboseForSwig verboseGlobal;


%inline %{
  const char * version()
    {
      return SSE_VERSION;
    }
  %}

%inline %{
  const char * interfaceversions()
    {
	cout << "ifc     : " << SSE_IFC_INTERFACE_VERSION << endl;
	cout << "dx     : " << SSE_DX_INTERFACE_VERSION << endl;
	cout << "dx arch: " << SSE_DX_ARCHIVER_INTERFACE_VERSION << endl;
	cout << "channel : " << ssechan::SSE_CHANNELIZER_INTERFACE_VERSION << endl;
	cout << "tscope  : " << SSE_TSCOPE_INTERFACE_VERSION << endl;
	cout << "tsig    : " << SSE_TESTSIG_INTERFACE_VERSION << endl;
	return "";
    }
  %}



// ----- NssParameters -----




class IfcImmedCmds
{

public:

   const char * attn(int attnLeftDb, int attnRightDb, const char* ifcName);
   const char * intrin(const char *ifcName = "all") const;
   const char * manualsource(const char* source, const char* ifcName);
   const char * names() const;
   const char * off(const char* ifcName);
   const char * reset(const char* ifcName);
   const char * reqstat(const char* ifcName = "all");
   const char * resetsocket(const char* ifcName);
   const char * restart(const char* ifcName);

   const char * send(const char* ifcName, const char* command,
                     const char* cmdarg1="", const char* cmdarg2="", 
                     const char* cmdarg3="", const char* cmdarg4="", 
                     const char* cmdarg5="");
   
   const char * shutdown(const char* ifcName);
   const char * status(const char* ifcName = "all");
   const char * stxstart(const char* ifcName);

// TBD needs parameters
//  void        stxvariance(const char* ifcName = "all");

   %extend {

      const char* stat(const char* ifcName = "all") {
         return(self->status(ifcName));
      }

      const char * help() { return ::help(IfcCommandName); }

   }

};
extern IfcImmedCmds ifcGlobal;


class IfcParameters
{

public:

   const char * save(const char* filename);
   const char * set(const char* pname, const char * val, const char* ptype = "");
   const char * show(const char* paramname = "all", const char* ptype = "");

// TBD move to IfcImmedCmds
   void stxvariance(const char* ifcName = "all");

   %extend {

      const char * default() {return self->setDefault();}
      bool isvalid() {return(self->isValid());}

      // help is the same for all ifcXparams, so just use the first one
      const char * help() { return ::help(Ifc1CommandName); }
   }

};


extern IfcParameters ifc1Global;
extern IfcParameters ifc2Global;
extern IfcParameters ifc3Global;

class TscopeParameters {

public:

   const char * allocate(const char* subarray, const char* tscopeName = "all");
   const char * deallocate(const char* subarray, const char* tscopeName = "all");

   const char * setup(const char* tscopeName = "all");
   const char * cleanup(const char* tscopeName = "all");
       
   const char * connect(const char* tscopeName = "all");
   const char * disconnect(const char* tscopeName = "all");

   const char * assign(const char* beamName, const char* subarray,
                       const char* tscopeName = "all");

   const char * monitor(int periodSecs, const char* tscopeName = "all");       
   const char * bfinit(const char* tscopeName = "all");
   const char * bfreset(const char* tscopeName = "all");

   const char * bfpoint(const char* tscopeName = "all");

   const char * bfsetcoords(
      const char * beamName,
      const char * coordSystem, 
      double coord1,
      double coord2,
      const char * tscopeName = "all");

   const char * bfcal(
      const char * calType,  // delay, phase, freq
      const char * integrateKeyword,
      int integrateSecs,
      const char * cyclesKeyword,
      int numCycles,
      const char * iterateKeyword,
      int iterations,
      const char* tscopeName = "all");

   const char * bfstop(const char* tscopeName = "all");
   const char * bfclearcoords(const char* tscopeName = "all");
   const char * bfclearants(const char* tscopeName = "all");
       
   const char * intrin(const char *tscopeName = "all");
   const char * names();

   const char * point(
      const char* subarray,
      const char* coordSystem, 
      double coord1,
      double coord2,
      const char* tscopeName = "all");
      
   const char * reset(const char* tscopeName = "all");
   const char * resetsocket(const char* tscopeName = "all");

   const char * restart(const char* tscopeName = "all");

   const char * save(const char* filename);
   const char * set(const char* pname, const char * val, const char* ptype = "");

   const char * tune(const char* tuningName,
                     double skyFreqMhz, 
                     const char* tscopeName = "all");

   const char * show(const char* paramname = "all", const char* ptype = "");
   const char * shutdown(const char* tscopeName = "all");
   const char * reqstat(const char* tscopeName = "all");
   const char * status(const char* tscopeName = "all");

   const char * stop(const char *subarray, const char* tscopeName = "all");
   const char * stow(const char *subarray, const char* tscopeName = "all");
   const char * wrap(const char *subarray, int wrapNumber, const char* tscopeName = "all");

   const char * sim(const char* tscopeName = "all");
   const char * unsim(const char* tscopeName = "all");

   const char * zfocus(const char *subarray, double skyFreqMhz, 
                       const char* tscopeName = "all");

   const char * send(const char* tscopeName, const char* cmd,
                     const char* cmdArg1="", const char* cmdArg2="", 
                     const char* cmdArg3="", const char* cmdArg4="", 
                     const char* cmdArg5="", const char* cmdArg6="", 
                     const char* cmdArg7="", const char* cmdArg8="");

   const char * autoselectants(const char*bflist,
                               const char* tscopeName = "all");

   %extend {

      const char * default() {return self->setDefault();}
      bool isvalid() {return(self->isValid());}

      const char * stat(const char* tscopeName = "all") {
         return(self->status(tscopeName));
      }

      const char * help() { return ::help(TscopeCommandName); }
   }

};

extern TscopeParameters tscopeGlobal;


class TestSigImmedCmds {
public:
   const char * names();
   const char * intrin(const char *tsigName = "all") const;
   const char * off(const char* tsigName);
   const char * on(const char* tsigName);
   const char * quiet(const char* tsigName);
   const char * reqstat(const char* tsigName = "all") const;
   const char * reset(const char* tsigName);
   const char * resetsocket(const char* tsigName);
   const char * restart(const char* tsigName);
   const char * send(const char* tsigName, const char* command,
                     const char* cmdarg1="", const char* cmdarg2="", 
                     const char* cmdarg3="", const char* cmdarg4="", 
                     const char* cmdarg5="");
   const char * shutdown(const char* tsigName);
   const char* status(const char* tsigName = "all") const;

   %extend {

      const char * help() { return ::help(TsigCommandName); }
   }
};
extern TestSigImmedCmds tsigGlobal;

class TestSigParameters {

public:
   const char *save(const char* filename);
   const char *set(const char* pname, const char * val, const char* ptype = "");
   const char *show(const char* paramname = "all", const char* ptype = "");
   %extend {

      const char * default() {return self->setDefault();}
      bool isvalid() {return(self->isValid());}

      // help is the same for all tsigXparams, so just use the first one
      const char * help() { return ::help(Tsig1CommandName); }
   }

};

extern TestSigParameters tsig1Global;
extern TestSigParameters tsig2Global;
extern TestSigParameters tsig3Global;


class DxParameters {

public:

   const char * set(const char* paramname, const char * val, 
                    const char* ptype = "");
   const char * show(const char* paramname = "all", 
                     const char* ptype = "");

   const char * save(const char* filename);
   void dumpstruct() const;
   const char *names();
   const char *status(const char *dxName = "all") const;
   const char *intrin(const char *dxName = "all") const;
   const char *config(const char *dxName = "all");
   const char *reqstat(const char *dxName = "all");

   const char *shutdown(const char *dxName0,
                        const char *dxName1="",
                        const char *dxName2="",
                        const char *dxName3="",
                        const char *dxName4="",
                        const char *dxName5="",
                        const char *dxName6="",
                        const char *dxName7="",
                        const char *dxName8="",
                        const char *dxName9="");

   const char *stop(const char *dxName);
   const char *restart(const char *dxName); 
   const char *senddatareq(const char *dxName);
   const char *reqfreq(double rfFreq, const char *dxName);
   const char *reqsub(int subchannel, const char *dxName);
   const char *resetsocket(const char *dxName);
   const char *load(const char *paramName, const char *paramValue, 
                    const char* dxName);

   %extend {

      const char * default() {return self->setDefault();}
      bool isvalid() {return(self->isValid());}

      const char *stat(const char *dxName = "all")
         {return self->status(dxName);}

      const char * help() { return ::help(DxCommandName); }

   }
};

extern DxParameters dxGlobal; 



class DxArchiverParameters {

public:

   const char * set(const char* paramname, const char * val, 
                    const char* ptype = "");

   const char * show(const char* paramname = "all", 
                     const char* ptype = "");

   const char * save(const char* filename);

   // immediate commands
   const char *names();
   const char *status(const char *dxArchiverName = "all") const;
   const char *intrin(const char *dxArchiverName = "all") const;
   const char *reqstat(const char *dxArchiverName = "all");
   const char *shutdown(const char *dxArchiverName);
   const char *resetsocket(const char *dxArchiverName);
   const char *restart(const char *dxArchiverName);

   %extend {

      const char * default() {return self->setDefault();}
      bool isvalid() {return(self->isValid());}

      const char *stat(const char *dxArchiverName = "all")
         {return self->status(dxArchiverName);}


      const char * help() { return ::help(DxArchiverCommandName); }
   }


};

extern DxArchiverParameters archGlobal; 



class ChannelizerParameters {

public:

   const char * set(const char* paramname, const char * val, 
                    const char* ptype = "");

   const char * show(const char* paramname = "all", 
                     const char* ptype = "");

   const char * save(const char* filename);

   // immediate commands
   const char *names();
   const char *status(const char *channelizerName = "all") const;
   const char *intrin(const char *channelizerName = "all") const;
   const char *reqstat(const char *channelizerName = "all");
   const char *shutdown(const char *channelizerName);
   const char *resetsocket(const char *channelizerName);
   const char *restart(const char *channelizerName);
   const char *start(int delaySecs, double skyFreqMhz, const char *channelizerName);
   const char *stop(const char *channelizerName);

   %extend {

      const char * default() {return self->setDefault();}
      bool isvalid() {return(self->isValid());}

      const char *stat(const char *channelizerName = "all")
         {return self->status(channelizerName);}


      const char * help() { return ::help(ChannelizerCommandName); }
   }


};

extern ChannelizerParameters chanGlobal; 




class ActParameters {

public:

   const char* set(const char* paramname, const char * val, 
                    const char* ptype = "");

   const char* save(const char* filename);

   const char* show(const char* paramname = "all", 
                     const char* ptype = "");

   const char* status() const;
   void clearfollowuplist();
   void addfollowupactid(int actId); 

   %extend {

      bool isvalid()  {return(self->isValid());}
      const char* default()  {return self->setDefault();}

      const char* stat() {return self->status();}


      const char * help() { return ::help(ActParamCommandName); }
   }

    
};

extern ActParameters actGlobal; 

class SchedulerParameters {

public:

   const char * set(const char* paramname, const char * val, 
                    const char* ptype = "");
   const char * show (const char* paramname = "all",
                      const char* ptype = "");
   const char * save(const char* filename);

   %extend {
      bool isvalid() {return(self->isValid());}
      const char * default() {return self->setDefault();}
      const char * help() { return ::help(SchedulerParamCommandName); }
   }
  
};

extern SchedulerParameters schedGlobal;

class DbParameters {

public:

   const char * set(const char* paramname, const char * val, 
                    const char* ptype = "");

   const char * show (const char* paramname = "all", 
                      const char* ptype = "");

   const char * save(const char* filename);

   %extend {

      bool isvalid() {return(self->isValid());}
      const char * default() {return self->setDefault();}
      const char * help() { return ::help(DbParamCommandName); }
   }
};

extern DbParameters dbGlobal;




class ComponentControlImmedCmds
{

public:

   const char * intrin(const char *componentControlName = "all") const;
   const char * names() const;
   const char * reset(const char* componentControlName);
   const char * reqstat(const char* componentControlName = "all");
   const char * resetsocket(const char* componentControlName);

   const char * send(const char* componentControlName, const char* command,
                     const char* cmdarg1="", const char* cmdarg2="", 
                     const char* cmdarg3="", const char* cmdarg4="", 
                     const char* cmdarg5="");
   
   const char * start(
      const char* componentName0, 
      const char* componentName1="", const char* componentName2="", 
      const char* componentName3="", const char* componentName4="", 
      const char* componentName5="", const char* componentName6="", 
      const char* componentName7="", const char* componentName8="", 
      const char* componentName9="");

   const char * shutdown(
      const char* componentName0, 
      const char* componentName1="", const char* componentName2="", 
      const char* componentName3="", const char* componentName4="", 
      const char* componentName5="", const char* componentName6="", 
      const char* componentName7="", const char* componentName8="", 
      const char* componentName9="");
 
   const char * restart(
      const char* componentName0, 
      const char* componentName1="", const char* componentName2="", 
      const char* componentName3="", const char* componentName4="", 
      const char* componentName5="", const char* componentName6="", 
      const char* componentName7="", const char* componentName8="", 
      const char* componentName9="");
  
   const char * selfshutdown(const char* componentControlName);
   const char* status(const char* componentControlName = "all");

   %extend {

      const char* stat(const char* componentControlName = "all") {
         return(self->status(componentControlName));
      }

      const char * help() { return ::help(ComponentControlName); }

   }

};
extern ComponentControlImmedCmds componentControlImmedCmdsGlobal;




// From NssParameters.h
class NssParameters {

public:

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


   %extend {

      void help() {self->help();}
      bool isvalid() {return(self->isValid());}
      const char * default() {return self->setDefault();}
      const char * save(const char* filename) {return self->save(filename);}
      const char * names() const {return self->names();}

   }

};

extern NssParameters paraGlobal;

const char * help(const char *subsystemName = "help");  // default to 'help help'

// wrappers for top level commands

%inline %{
  const char * save(const char *filename)
    {
	return paraGlobal.save(filename);
    }
  %}

%inline %{
    const char * show()
    {
	return paraGlobal.show();
    }
  %}

%inline %{
    const char * names()
    {
	return paraGlobal.names();
    }
  %}

%inline %{
    const char *status()
    {
	return paraGlobal.status();
    }
  %}


const char *utc();
void systemlog(const char *msg);
const char * expectedcomponents();
const char * reconfig(const char *expectedComponentsFilename="");

/*
 make the swig 'defined but not used' warnings go away
 for swig version 1.3.21 and below
*/
#if SWIG_VERSION <= 0x010321

%inline %{
static void dummySwigRoutine()
{
    if (false)
    {	
	SWIG_Tcl_TypeDynamicCast(0, 0);
	SWIG_Tcl_TypeName(0);
	SWIG_Tcl_TypeQuery(0);	
	SWIG_Tcl_PointerTypeFromString(0); 
	SWIG_Tcl_ConvertPacked(0,0,0,0,0,0);
    }
}
%}

#endif

