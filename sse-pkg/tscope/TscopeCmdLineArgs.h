/*******************************************************************************

 File:    TscopeCmdLineArgs.h
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


#ifndef TscopeCmdLineArgs_H
#define TscopeCmdLineArgs_H

// processes (parses) command line arguments

#include <string>
using std::string;

class TscopeCmdLineArgs
{

 public:

  TscopeCmdLineArgs(const string& progName,
		    const string& defaultSseHostName,
		    int defaultSsePort,
		    const string& defaultAntControlServerName,
		    int defaultControlPort,
		    int defaultMonitorPort,
		    bool defaultNoUi,
		    const string & defaultName,
		    bool defaultSimulate,
		    int defaultVerboseLevel);

  virtual ~TscopeCmdLineArgs() {};

  bool parseArgs(int argc, char *argv[]);
  void usage();

  const string& getProgName();
  const string& getSseHostName();
  int getSsePort();
  int getControlPort();
  int getMonitorPort();
  const string& getAntControlServerName();
  bool getNoUi();
  const string & getName();
  bool getSimulate();
  int getVerboseLevel();

 private:

  bool getSseHostNameValue(const char* value);
  bool getSsePortValue(const char* value);
  bool getControlPortValue(const char* value);
  bool getMonitorPortValue(const char* value);
  bool getAntControlServerNameValue(const char* value);
  bool getNameValue(const char *value);
  bool getVerboseLevelValue(const char *value);

  string progName_;
  
  string sseHostNameKeyword_;
  string sseHostName_;
  string defaultSseHostName_;

  string ssePortKeyword_;
  int ssePort_;
  int defaultSsePort_;

  // telescope control

  string antControlServerNameKeyword_;
  string antControlServerName_;
  string defaultAntControlServerName_;

  string controlPortKeyword_;
  int controlPort_;
  int defaultControlPort_;

  string monitorPortKeyword_;
  int monitorPort_;
  int defaultMonitorPort_;

  string noUiKeyword_;
  bool noUi_;
  bool defaultNoUi_;

  string nameKeyword_;
  string name_;
  string defaultName_;

  string simulateKeyword_;
  bool simulate_;
  bool defaultSimulate_;

  string verboseLevelKeyword_;
  int verboseLevel_;
  int defaultVerboseLevel_;
  
  // Disable copy construction & assignment.
  // Don't define these.
  TscopeCmdLineArgs(const TscopeCmdLineArgs& rhs);
  TscopeCmdLineArgs& operator=(const TscopeCmdLineArgs& rhs);


};

#endif 