/*******************************************************************************

 File:    TestSigImmedCmds.h
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


#ifndef TESTSIG_IMMEDCMDS
#define TESTSIG_IMMEDCMDS

#include <string>
#include <iostream>
using std::string;
class Site;


class TestSigImmedCmds
{
 public:

  enum SignalType {CW, PULSE};

  TestSigImmedCmds(const string &command);
  virtual ~TestSigImmedCmds();

  // ----- immediate commands -----
  const char *names() const;
  const char *intrin(const string& tsigName) const;
  const char *on(const string& tsigName);
  const char *off(const string& tsigName);
  const char *quiet(const string& tsigName);
  const char *reset(const string& tsigName);
  const char *reqstat(const string& tsigName) const;
  const char *restart(const string& tsigName);
  const char *send(const string& tsigName, const string& command,
		   const string & cmdarg1="", const string & cmdarg2="",
		   const string & cmdarg3="", const string & cmdarg4="",  
		   const string & cmdarg5="");
  const char *status(const string& tsigName) const;
  const char *shutdown(const string& tsigName);
  const char *resetsocket(const string& tsigName);

  // ----------
  void setSite(Site *site);
  Site * getSite() const;
  const string & getCommand() const;
  void help(std::ostream& os = std::cout) const;

private:

  const char * printCmdFeedback(std::stringstream & strm,
				bool success,
				const string &cmdName,
				const string &methodName, 
				const string &ifcName) const;
  string cmdName_;
  Site *site_;
  mutable string outString_;
};

#endif