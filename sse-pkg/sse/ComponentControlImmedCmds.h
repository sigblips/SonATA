/*******************************************************************************

 File:    ComponentControlImmedCmds.h
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


#ifndef ComponentControlImmedCmds_H
#define ComponentControlImmedCmds_H

#include <string>
#include <iostream>
#include <sstream>
using std::string;
class Site;

class ComponentControlImmedCmds
{
 public:
   ComponentControlImmedCmds(const string &command);
   virtual ~ComponentControlImmedCmds();

  // ----- immediate commands -----
   const char * names() const;
   const char * intrin(const string& componentControlName) const;
   const char * reset(const string& componentControlName);
   const char * reqstat(const string& componentControlName) const;
   const char * resetsocket(const string& componentControlName);
   const char * status(const string& componentControlName) const;

   const char * send(const string& componentControlName, const string& command,
		     const string & cmdarg1="", const string & cmdarg2="",
		     const string & cmdarg3="", const string & cmdarg4="",  
		     const string & cmdarg5="");

   const char * start(const string& componentName0,
		      const string& componentName1="",
		      const string& componentName2="",
		      const string& componentName3="",
		      const string& componentName4="",
		      const string& componentName5="",
		      const string& componentName6="",
		      const string& componentName7="",
		      const string& componentName8="",
		      const string& componentName9="");

   const char * shutdown(const string& componentName0,
		     const string& componentName1="",
		     const string& componentName2="",
		     const string& componentName3="",
		     const string& componentName4="",
		     const string& componentName5="",
		     const string& componentName6="",
		     const string& componentName7="",
		     const string& componentName8="",
		     const string& componentName9="");

   const char * restart(const string& componentName0,
			const string& componentName1="",
			const string& componentName2="",
			const string& componentName3="",
			const string& componentName4="",
			const string& componentName5="",
			const string& componentName6="",
			const string& componentName7="",
			const string& componentName8="",
			const string& componentName9="");


   const char * sendCommandToAllControllers(
      const string& command,
      const string& componentName0,
      const string& componentName1="",
      const string& componentName2="",
      const string& componentName3="",
      const string& componentName4="",
      const string& componentName5="",
      const string& componentName6="",
      const string& componentName7="",
      const string& componentName8="",
      const string& componentName9="");
  
   const char * selfshutdown(const string& componentControlName);

   const string & getCommand() const;
   void help(std::ostream& os = std::cout) const;
   void setSite(Site *site);
   Site *getSite() const;

   bool controllersAvailable();
   bool isComponentUnderControl(const string &componentName);

 private:
   // Disable copy construction & assignment.
   // Don't define these.

//    ComponentControlImmedCmds(const ComponentControlImmedCmds& rhs);
//    ComponentControlImmedCmds& operator=(const ComponentControlImmedCmds& rhs);

   const char * printCmdFeedback(std::stringstream & strm,
				 bool success,
				 const string &cmdName,
				 const string &methodName, 
				 const string &componentControlName) const;
   string cmdName_;
   Site *site_;
   mutable string outString_;
  
};

#endif // ComponentControlImmedCmds_H