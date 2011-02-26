/*******************************************************************************

 File:    IfcImmedCmds.cpp
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



#include "IfcImmedCmds.h" 
#include "IfcList.h"
#include "IfcProxy.h"
#include "Site.h"
#include "ComponentControlImmedCmds.h"

extern ComponentControlImmedCmds componentControlImmedCmdsGlobal;

static const string allComponents("all");

IfcImmedCmds::IfcImmedCmds(const string &cmdName) :
   cmdName_(cmdName),
   site_(0)
{
}

IfcImmedCmds::~IfcImmedCmds()
{
}

void IfcImmedCmds::setSite(Site *site)
{
   site_ = site;
}

Site * IfcImmedCmds::getSite() const
{
   return site_;
}

const string & IfcImmedCmds::getCommand() const
{
   return cmdName_;
}


static void getAllIfcs(IfcList &allIfcs, Site *site)
{
   Assert(site);
   site->ifcManager()->getProxyList(&allIfcs);
}

const char * IfcImmedCmds::printCmdFeedback(stringstream & strm,
					    bool success,
					    const string &cmdName,
					    const string &methodName, 
					    const string &ifcName) const
{
   strm << cmdName << "::" << methodName; 

   if (success)
   {
      strm << " command sent to ifcs: ";
   }
   else
   {
      strm << ": the requested ifcs(s) are not connected: ";
   }
   strm << ifcName << endl;

   outString_ = strm.str();
   return(outString_.c_str());

}


const char * IfcImmedCmds::names() const
{
   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());
   
   stringstream strm;
   for (IfcList::const_iterator ifc = allIfcs.begin();
	ifc != allIfcs.end(); ifc++)
   {
      strm << (*ifc)->getName() << " ";
   }

   outString_ = strm.str();
   return(outString_.c_str());
}

const char* IfcImmedCmds::intrin(const string& ifcName) const
{
   const string methodName("intrin");

   IfcList  allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);
   stringstream strm;
   for (IfcList::iterator ifc = allIfcs.begin(); 
	ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 strm << (*ifc)->getIntrinsics() << endl;
	 found  = true;
      } 
   }

   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}

const char * IfcImmedCmds::off(const string& ifcName)
{
   const string methodName("off");

   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);
   for (IfcList::iterator ifc = allIfcs.begin(); 
	ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 (*ifc)->off();
	 found = true;
      } 
   }

   stringstream strm;
   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);

}


const char * IfcImmedCmds::reset(const string& ifcName)
{
   const string methodName("reset");

   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);
   for (IfcList::iterator ifc = allIfcs.begin();
	ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 (*ifc)->reset();
	 found = true;
      } 
   }

   stringstream strm;
   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}

const char * IfcImmedCmds::reqstat(const string& ifcName) const
{
   const string methodName("reqstat");

   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);

   for (IfcList::const_iterator ifc = allIfcs.begin(); 
	ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 (*ifc)->requestStatusUpdate();
	 found  = true;
      } 
   }

   stringstream strm;
   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}

const char * IfcImmedCmds::send(const string& ifcName, const string& command,
			const string & cmdarg1, const string & cmdarg2,
			const string & cmdarg3, const string & cmdarg4,  
			const string & cmdarg5)
{
   const string methodName("send");

   IfcList  allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);
   for (IfcList::iterator ifc = allIfcs.begin(); ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 string cmdWithArgs = command + " " + cmdarg1 + " " + cmdarg2 
	    + " " + cmdarg3 + " " + cmdarg4 + " " + cmdarg5; 

	 (*ifc)->sendServerCommand(cmdWithArgs);
	 found = true;
      }
   }

   stringstream strm;
   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}

const char * IfcImmedCmds::stxstart(const string& ifcName)
{
   const string methodName("stxstart");

   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);
   for (IfcList::iterator ifc = allIfcs.begin(); ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 (*ifc)->stxStart();
	 found = true;
      } 
   }

   stringstream strm;
   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}


const char* IfcImmedCmds::status(const string& ifcName) const
{
   const string methodName("status");

   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);
   IfcStatus status;
   stringstream strm;

   for (IfcList::const_iterator ifc = allIfcs.begin(); ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 status = (*ifc)->getStatus();
	 strm << status << endl;
	 found  = true;
      } 
   }

   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}


const char * IfcImmedCmds::shutdown(const string& ifcName)
{
   const string methodName("shutdown");
   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);

   for (IfcList::iterator ifc = allIfcs.begin(); ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 if (componentControlImmedCmdsGlobal.isComponentUnderControl((*ifc)->getName()))
	 {
	    // Let the component controller perform a shutdown so that
	    // the component will not be restarted.
	    componentControlImmedCmdsGlobal.shutdown((*ifc)->getName());
	 }
	 else
	 {
	    // Let the proxy handle shutdown on its own.
	    (*ifc)->shutdown();
	 }
	 found = true;
      } 
   }

   stringstream strm;
   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}


const char * IfcImmedCmds::restart(const string& ifcName)
{
   const string methodName("restart");
   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);

   for (IfcList::iterator ifc = allIfcs.begin(); ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 if (componentControlImmedCmdsGlobal.isComponentUnderControl((*ifc)->getName()))
	 {
	    componentControlImmedCmdsGlobal.restart((*ifc)->getName());
	 }
	 else
	 {
	    // ifc has no restart on its own
	 }
	 found = true;
      } 
   }

   stringstream strm;
   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}



const char * IfcImmedCmds::resetsocket(const string& ifcName)
{
   const string methodName("resetsocket");

   IfcList  allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);

   for (IfcList::iterator ifc = allIfcs.begin(); ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 (*ifc)->resetSocket();
	 found = true;
      } 
   }

   stringstream strm;
   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}

const char * IfcImmedCmds::attn(int attnLeftDb, int attnRightDb, const string &ifcName)
{
   const string methodName("attn");

   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());

   bool found(false);
   for (IfcList::iterator ifc = allIfcs.begin(); ifc != allIfcs.end(); ifc++)
   {
      if (ifcName == (*ifc)->getName() || ifcName == allComponents)
      {
	 (*ifc)->attn(attnLeftDb, attnRightDb);
	 (*ifc)->requestStatusUpdate();

	 found = true;
      } 
   }

   stringstream strm;
   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}

// immediately set the IF mode to 'sky' or 'test'
const char * IfcImmedCmds::manualsource(const string& source, const string& ifcName)
{
   string methodName("manualsource");

   stringstream strm;
   if (source != "test" && source != "sky")
   {
      strm << "Error " << methodName 
	   << " source must be 'test' or 'sky': " 
	   << source << endl;

      outString_ = strm.str();
      return outString_.c_str();
   }

   IfcList allIfcs;
   getAllIfcs(allIfcs, getSite());
    
   bool found(false);    
   for (IfcList::iterator ifc = allIfcs.begin();
	ifc != allIfcs.end(); ++ifc) 
   {
      if (ifcName == (*ifc)->getName() || ifcName == "all") 
      {
	 (*ifc)->ifSource(source);
	 (*ifc)->requestStatusUpdate();
	 found = true;
      } 
   }


   return printCmdFeedback(strm, found, getCommand(), methodName, ifcName);
}


// indent to the requested level
static void indent(ostream &os, int level)
{
   for (int i = 0; i < level; i++)
   {
      os << "  ";
   } 
}


static void printImmedCmdHelp(ostream &os, const string &helpText) 
{
   int indentLevel = 3;
   indent(os, indentLevel);
   os << helpText << endl;
}

void IfcImmedCmds::help(ostream& os) const
{
   int indentLevel = 1;
   indent(os, indentLevel);
   os << "Command: \"" << getCommand() << "\"" << endl;

   indentLevel = 2;
   indent(os, indentLevel);
   os << "Immediate Commands:" << endl;

   printImmedCmdHelp(os, "attn <left attn db> <right attn db> <name | 'all'> - send attn settings to ifc(s) ");
   printImmedCmdHelp(os, "help - print this help text");
   printImmedCmdHelp(os, "intrin [<name>='all'] - display ifc intrinsics");
   printImmedCmdHelp(os, "manualsource <'sky' | 'test'> <name | 'all'> - manually set IF source: test or sky");
   printImmedCmdHelp(os, "names - list names of all connected ifcs");
   printImmedCmdHelp(os, "off <name | 'all'> - turn ifc(s) off ");
   printImmedCmdHelp(os, "reqstat [<name>='all'] - request ifc status update");
   printImmedCmdHelp(os, "reset <name | 'all'> - reset ifc(s)");
   printImmedCmdHelp(os, "resetsocket <name | 'all'> - reset ifc socket");
   printImmedCmdHelp(os, "restart <name | 'all'> - restart ifc(s)");
   printImmedCmdHelp(os, "send <name | 'all'> <command> [arg1] [arg2] ... [arg5] - send command with arguments to ifc(s)");
   printImmedCmdHelp(os, "shutdown <name | 'all'> - shutdown ifc(s)");
   printImmedCmdHelp(os, "status [<name>='all'] - display status of ifc(s)");
   printImmedCmdHelp(os, "stxstart <name | 'all'> - (re) start stx(s) ");

// TBD
//  printImmedCmdHelp(os," stxvariance <name | 'all'> - set variance of stx(s)");

}