/*******************************************************************************

 File:    TestSigImmedCmds.cpp
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
#include "TestSigImmedCmds.h"
#include "TestSigList.h"
#include "TestSigProxy.h"
#include "Site.h"
#include "ComponentControlImmedCmds.h"
#include <string>
#include <iostream>

using std::string;

extern ComponentControlImmedCmds componentControlImmedCmdsGlobal;
static const string allComponents("all");

TestSigImmedCmds::TestSigImmedCmds(const string &cmdName) :
   cmdName_(cmdName),
   site_(0)
   {
   }

TestSigImmedCmds::~TestSigImmedCmds()
{
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

void TestSigImmedCmds::help(ostream& os) const
{
  int indentLevel = 1;
  indent(os, indentLevel);
  os << "Command: \"" << getCommand() << "\"" << endl;

  indentLevel = 2;
  indent(os, indentLevel);
  os << "Immediate Commands:" << endl;


  printImmedCmdHelp(os, "intrin [<name>='all'] - display tsig intrinsics");
  printImmedCmdHelp(os, "names - list names of all connected tsigs");
  printImmedCmdHelp(os, "on <name | 'all'> - turn tsig(s) on  ");
  printImmedCmdHelp(os, "off <name | 'all'> - turn tsig(s) off  ");
  printImmedCmdHelp(os, "quiet <name | 'all'> - make tsigs as quiet as possible (turn off, min amplitude, low freq) ");
  printImmedCmdHelp(os, "reqstat [<name>='all'] - request tsig status update");
  printImmedCmdHelp(os, "reset <name | 'all'> - reset tsigs(s)");
  printImmedCmdHelp(os, "resetsocket <name | 'all'> - reset tsig socket");
  printImmedCmdHelp(os, "restart <name | 'all'> - restart tsig(s)");
  printImmedCmdHelp(os, "send <name | 'all'> <command> [arg1] [arg2] ... [arg5] - send command with arguments to tsig(s)");
  printImmedCmdHelp(os, "shutdown <name | 'all'> - shutdown tsig(s)");
  printImmedCmdHelp(os, "status [<name>='all'] - display status of tsig(s)");
}

static void getAllTestSigs(TestSigList &allTestSigs, Site *site)
{
  Assert(site);
  site->testSigManager()->getProxyList(&allTestSigs);
}

const char * TestSigImmedCmds::printCmdFeedback(stringstream & strm,
						bool success,
						const string &cmdName,
						const string &methodName, 
						const string &testSigName) const
{
   strm << cmdName << "::" << methodName; 

   if (success)
   {
      strm << " command sent to tsigs: ";
   }
   else
   {
      strm << ": the requested tsig(s) are not connected: ";
   }
   strm << testSigName << endl;

   outString_ = strm.str();
   return(outString_.c_str());

}

void TestSigImmedCmds::setSite(Site *site)
{
   site_ = site;
}

Site * TestSigImmedCmds::getSite() const
{
   return site_;
}
const string & TestSigImmedCmds::getCommand() const
{
   return cmdName_;
}


const char * TestSigImmedCmds::names() const
{
  TestSigList  allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  stringstream strm;

  for (TestSigList::const_iterator testSig = allTestSigs.begin();
       testSig != allTestSigs.end(); ++testSig)
  {
    strm << (*testSig)->getName() << " ";
  }

  outString_ = strm.str();
  return(outString_.c_str());

}

const char* TestSigImmedCmds::intrin(const string& testSigName) const
{
   const string methodName("intrin");
  TestSigList allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  stringstream strm;

  for (TestSigList::iterator testSig = allTestSigs.begin();
       testSig != allTestSigs.end(); ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all")
    {
      strm << (*testSig)->getIntrinsics();
      found  = true;
    } 
  }

  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);

}


const char * TestSigImmedCmds::on(const string& testSigName)
{
   const string methodName("on");

  TestSigList  allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  for (TestSigList::iterator testSig = allTestSigs.begin();
       testSig != allTestSigs.end(); ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all" )
    {
      (*testSig)->sigGenOn();
      (*testSig)->requestStatusUpdate();
      found = true;
    }
  }
   
  stringstream strm;
  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);
}


const char * TestSigImmedCmds::off(const string& testSigName)
{
   const string methodName("off");

  TestSigList allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  for (TestSigList::iterator testSig = allTestSigs.begin();
       testSig != allTestSigs.end();  ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all")
    {
      (*testSig)->off();
      (*testSig)->requestStatusUpdate();
      found = true;
    } 
  }

  stringstream strm;
  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);

}

const char * TestSigImmedCmds::quiet(const string& testSigName)
{
   const string methodName("quiet");

  TestSigList allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  for (TestSigList::iterator testSig = allTestSigs.begin();
       testSig != allTestSigs.end();  ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all")
    {
      (*testSig)->quiet();
      (*testSig)->requestStatusUpdate();
      found = true;
    } 
  }

  stringstream strm;
  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);

}



const char * TestSigImmedCmds::reset(const string& testSigName)
{
   string methodName("reset");
  TestSigList  allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  for (TestSigList::iterator testSig = allTestSigs.begin();
       testSig != allTestSigs.end(); ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all")
    {
      (*testSig)->reset();
      found = true;
    }
  }

  stringstream strm;
  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);
  
}

// request status update
const char * TestSigImmedCmds::reqstat(const string& testSigName) const
{
  string methodName("reqstat");
  TestSigList  allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  for (TestSigList::iterator testSig = allTestSigs.begin();
       testSig != allTestSigs.end(); ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all")
    {
      (*testSig)->requestStatusUpdate();
      found  = true;
    }
  }

  stringstream strm;
  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);
}

// send a command to the tsig, with optional arguments
const char * TestSigImmedCmds::send(const string& testSigName, const string& command,
		   const string & cmdarg1, const string & cmdarg2,
		   const string & cmdarg3, const string & cmdarg4,  
		   const string & cmdarg5)
{
   const string methodName("send");
  TestSigList  allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  for (TestSigList::iterator testSig = allTestSigs.begin();
       testSig != allTestSigs.end(); ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all") 
    {
	string cmdWithArgs = command + " " + cmdarg1 + " " + cmdarg2 
	   + " " + cmdarg3 + " " + cmdarg4 + " " + cmdarg5; 

      (*testSig)->sendServerCommand(cmdWithArgs);
      found = true;
    }
  }

  stringstream strm;
  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);

}

const char* TestSigImmedCmds::status(const string& testSigName) const
{
   const string methodName("status");
  TestSigList allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  TestSigStatus status;
  stringstream strm;

  for (TestSigList::iterator testSig = allTestSigs.begin();
       testSig != allTestSigs.end(); ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all")
    {
      status = (*testSig)->getStatus();
      strm << status ;
      found  = true;
    }
  }

  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);
}

const char * TestSigImmedCmds::shutdown(const string& testSigName)
{
  string methodName("shutdown");
  TestSigList  allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  for (TestSigList::iterator testSig = allTestSigs.begin(); 
       testSig != allTestSigs.end(); ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all")
    {
       if (componentControlImmedCmdsGlobal.isComponentUnderControl((*testSig)->getName()))
       {
	  // Let the component controller perform a shutdown so that
	  // the component will not be restarted.
	  componentControlImmedCmdsGlobal.shutdown((*testSig)->getName());
       }
       else
       {
	  // Let the proxy handle shutdown on its own.
	  (*testSig)->shutdown();
       }
       found = true;
    }
  }

  stringstream strm;
  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);

}



const char * TestSigImmedCmds::restart(const string& testSigName)
{
  string methodName("restart");
  TestSigList  allTestSigs;
  getAllTestSigs(allTestSigs, getSite());

  bool found(false);
  for (TestSigList::iterator testSig = allTestSigs.begin(); 
       testSig != allTestSigs.end(); ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all")
    {
       if (componentControlImmedCmdsGlobal.isComponentUnderControl((*testSig)->getName()))
       {
	  componentControlImmedCmdsGlobal.restart((*testSig)->getName());
       }
       else
       {
	  // Proxy can't do a restart on its own
       }
       found = true;
    }
  }

  stringstream strm;
  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);

}

const char * TestSigImmedCmds::resetsocket(const string& testSigName)
{
  string methodName("resetsocket");
  TestSigList  allTestSigs;
  getAllTestSigs(allTestSigs, getSite());
  
  bool found(false);
  for (TestSigList::iterator testSig = allTestSigs.begin(); 
       testSig != allTestSigs.end(); ++testSig)
  {
    if (testSigName == (*testSig)->getName() || testSigName == "all")
    {
	(*testSig)->resetSocket();
	found = true;
    } 
  }

  stringstream strm;
  return printCmdFeedback(strm, found, getCommand(), methodName, testSigName);

}
