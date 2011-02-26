/*******************************************************************************

 File:    IfcImmedCmds.h
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


#ifndef IfcImmedCmds_H
#define IfcImmedCmds_H

#include <string>
#include <iostream>
#include <sstream>
using std::string;
class Site;

class IfcImmedCmds
{
 public:
    IfcImmedCmds(const string &command);
    virtual ~IfcImmedCmds();

  // ----- immediate commands -----
  const char * attn(int attnLeftDb, int attnRightDb, const string &ifcName);
  const char * names() const;
  const char* intrin(const string& ifcName) const;
  const char * manualsource(const string& source, const string& ifcName);
  const char * off(const string& ifcName);
  const char * reset(const string& ifcName);
  const char * reqstat(const string& ifcName) const;
  const char * resetsocket(const string& ifcName);
  const char * restart(const string& ifcName);
  const char* status(const string& ifcName) const;

  const char * send(const string& ifcName, const string& command,
	    const string & cmdarg1="", const string & cmdarg2="",
	    const string & cmdarg3="", const string & cmdarg4="",  
	    const string & cmdarg5="");

  const char * shutdown(const string& ifcName);
  const char * stxstart(const string& ifcName);
//void stxvariance(const string& ifcName);

  const string & getCommand() const;
  void help(std::ostream& os = std::cout) const;
  void setSite(Site *site);
  Site *getSite() const;

private:
    // Disable copy construction & assignment.
    // Don't define these.

//    IfcImmedCmds(const IfcImmedCmds& rhs);
//    IfcImmedCmds& operator=(const IfcImmedCmds& rhs);

  const char * printCmdFeedback(std::stringstream & strm,
				bool success,
				const string &cmdName,
				const string &methodName, 
				const string &ifcName) const;
  string cmdName_;
  Site *site_;
  mutable string outString_;
  
};

#endif // IfcImmedCmds_H