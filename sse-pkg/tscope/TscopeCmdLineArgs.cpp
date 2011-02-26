/*******************************************************************************

 File:    TscopeCmdLineArgs.cpp
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



#include "ace/Arg_Shifter.h"
#include "ace/OS.h"
#include <iostream>
#include "TscopeCmdLineArgs.h" 
#include "SseSystem.h"

using namespace std;

TscopeCmdLineArgs::TscopeCmdLineArgs(const string& progName,
				     const string& defaultSseHostName,
				     int defaultSsePort,
				     const string& defaultAntControlServerName,
				     int defaultControlPort,
				     int defaultMonitorPort,
				     bool defaultNoUi,
				     const string & defaultName,
				     bool defaultSimulate,
				     int defaultVerboseLevel)
  : progName_(progName),
   
    sseHostNameKeyword_("-host"), 
    sseHostName_(defaultSseHostName), 
    defaultSseHostName_(defaultSseHostName),
   
    ssePortKeyword_("-sseport"), 
    ssePort_(defaultSsePort),
    defaultSsePort_(defaultSsePort),

    antControlServerNameKeyword_("-antserver"), 
    antControlServerName_(defaultAntControlServerName), 
    defaultAntControlServerName_(defaultAntControlServerName),

    controlPortKeyword_("-controlport"), 
    controlPort_(defaultControlPort),
    defaultControlPort_(defaultControlPort),

    monitorPortKeyword_("-monitorport"), 
    monitorPort_(defaultMonitorPort),
    defaultMonitorPort_(defaultMonitorPort),
   
    noUiKeyword_("-noui"), 
    noUi_(defaultNoUi),
    defaultNoUi_(defaultNoUi),

    nameKeyword_("-name"), 
    name_(defaultName), 
    defaultName_(defaultName),

    simulateKeyword_("-sim"), 
    simulate_(defaultSimulate),
    defaultSimulate_(defaultSimulate),

    verboseLevelKeyword_("-verbose"), 
    verboseLevel_(defaultVerboseLevel),
    defaultVerboseLevel_(defaultVerboseLevel)
{}


const string& TscopeCmdLineArgs::getProgName() 
{
    return progName_;
}

const string& TscopeCmdLineArgs::getSseHostName()
{
    return sseHostName_;
}

int TscopeCmdLineArgs::getSsePort()
{
    return ssePort_;
}

const string& TscopeCmdLineArgs::getAntControlServerName() 
{
    return antControlServerName_;
}

int TscopeCmdLineArgs::getControlPort()
{
    return controlPort_;
}

int TscopeCmdLineArgs::getMonitorPort()
{
    return monitorPort_;
}

bool TscopeCmdLineArgs::getNoUi()
{
    return noUi_;
}

const string& TscopeCmdLineArgs::getName() 
{
    return name_;
}

bool TscopeCmdLineArgs::getSimulate()
{
    return simulate_;
}

int TscopeCmdLineArgs::getVerboseLevel()
{
    return verboseLevel_;
}

bool TscopeCmdLineArgs::parseArgs(int argc, char *argv[])
{
  ACE_Arg_Shifter arg(argc, argv);
  bool status(true);
  
  // eat arg0
  arg.consume_arg();
  
  while (arg.is_anything_left() && status)
  {
    const char *currentArg=arg.get_current();
    
    if (currentArg == ssePortKeyword_)
    {
      arg.consume_arg();
      status = getSsePortValue(arg.get_current());
    }
    else if (currentArg == sseHostNameKeyword_)
    {
      arg.consume_arg();
      status = getSseHostNameValue(arg.get_current());
    }
    else if (currentArg == antControlServerNameKeyword_)
    {
      arg.consume_arg();
      status = getAntControlServerNameValue(arg.get_current());
    } 
    else if (currentArg == controlPortKeyword_)
    {
      arg.consume_arg();
      status = getControlPortValue(arg.get_current());
    }
    else if (currentArg == monitorPortKeyword_)
    {
      arg.consume_arg();
      status = getMonitorPortValue(arg.get_current());
    }
    else if (currentArg == noUiKeyword_)
    {
       // flag only - don't consume arg
       noUi_ = true;
    }
    else if (currentArg == nameKeyword_) 
    {
      arg.consume_arg();
      status = getNameValue(arg.get_current());
    } 
    else if (currentArg == verboseLevelKeyword_) 
    {
      arg.consume_arg();
      status = getVerboseLevelValue(arg.get_current());
    } 
    else if (currentArg == simulateKeyword_)
    {
       // flag only - don't consume arg
       simulate_ = true;
    }
    else
    {
      cerr << "error: unknown option: " << arg.get_current() << endl;
      status = false;
    }
    
    arg.consume_arg();
    
  }
  return status;
}

bool TscopeCmdLineArgs::getSseHostNameValue(const char *value)
{
  bool status(true);

  if (value)
  {
    sseHostName_ = value;
  }
  else
  {
    cerr << "error: no value given for " << 
      sseHostNameKeyword_ << endl;
    status = false;
  }
  return status;
}

bool TscopeCmdLineArgs::getSsePortValue(const char *value)
{
  bool status(true);
  
  if (value)
  {
    int port = ACE_OS::atoi(value); 
    int minPort = 1024;
    int maxPort = 65535;
    if (port >= minPort && port <= maxPort)
    {
      ssePort_ = port;
    }
    else
    {
      cerr << "error: port must be >= " << minPort;
      cerr << " and <= " << maxPort;
      cerr << endl;
      status = false;
    }
  } 
  else 
  {
    cerr << "error: no value given for " << ssePortKeyword_ << endl;
    status = false;
  }
  
  return status;
}


bool TscopeCmdLineArgs::getAntControlServerNameValue(const char *value)
{
  bool status(true);
  
  if (value)
  {
    antControlServerName_ = value;
  }
  else
  {
    cerr << "error: no value given for " << 
      antControlServerNameKeyword_ << endl;
    status = false;
  }
  return status;
}

bool TscopeCmdLineArgs::getControlPortValue(const char *value)
{
  bool status(true);
  
  if (value)
  {
    int port = ACE_OS::atoi(value); 
    int minPort = 1024;
    int maxPort = 65535;
    if (port >= minPort && port <= maxPort)
    {
      controlPort_ = port;
    }
    else
    {
      cerr << "error: port must be >= " << minPort;
      cerr << " and <= " << maxPort;
      cerr << endl;
      status = false;
    }
  } 
  else 
  {
    cerr << "error: no value given for " << controlPortKeyword_ << endl;
    status = false;
  }
  
  return status;
}

bool TscopeCmdLineArgs::getMonitorPortValue(const char *value)
{
  bool status(true);
  
  if (value)
  {
    int port = ACE_OS::atoi(value); 
    int minPort = 1024;
    int maxPort = 65535;
    if (port >= minPort && port <= maxPort)
    {
      monitorPort_ = port;
    }
    else
    {
      cerr << "error: port must be >= " << minPort;
      cerr << " and <= " << maxPort;
      cerr << endl;
      status = false;
    }
  } 
  else 
  {
    cerr << "error: no value given for " << monitorPortKeyword_ << endl;
    status = false;
  }
  
  return status;
}


bool TscopeCmdLineArgs::getNameValue(const char *value)
{
  bool status(true);

  if (value)
  {
    name_ = value;
  }
  else
  {
    cerr << "error: no value given for " << 
      nameKeyword_ << endl;
    status = false;
  }
  return status;
}

bool TscopeCmdLineArgs::getVerboseLevelValue(const char *value)
{
  bool status(true);
  
  if (value)
  {
    verboseLevel_ = ACE_OS::atoi(value); 
  } 
  else 
  {
    cerr << "error: no value given for " << verboseLevelKeyword_ << endl;
    status = false;
  }
  
  return status;
}



void TscopeCmdLineArgs::usage()
{
  cerr << "Usage: ";
  cerr << progName_;
  cerr << " [" << sseHostNameKeyword_ << " <seeker host>]";
  cerr << " [" << ssePortKeyword_ << " <sse port number>]";
  cerr << " [" << antControlServerNameKeyword_ << " <antenna control server name>]";
  cerr << " [" << controlPortKeyword_ << " <tscope control port number>]";
  cerr << " [" << monitorPortKeyword_ << " <tscope monitor port number>]";
  cerr << " [" << noUiKeyword_ << " ]";
  cerr << " [" << nameKeyword_ << " <name>]";
  cerr << " [" << simulateKeyword_ << " ]";
  cerr << " [" << verboseLevelKeyword_ << " <level> ]";
  cerr << endl;

  cerr << "Default sse host: " << defaultSseHostName_ << endl; 
  cerr << "Default sse port: " << defaultSsePort_ << endl;
  cerr << "Default antenna control server: " 
       << defaultAntControlServerName_ << endl;
  cerr << "Default control port: " << defaultControlPort_ << endl;
  cerr << "Default monitor port: " << defaultMonitorPort_ << endl;
  cerr << "Default noui mode: " << defaultNoUi_ << endl;
  cerr << "Default name: " << defaultName_ << endl;
  cerr << "Default simulate mode: " << defaultSimulate_ << endl;
  cerr << "Default verbose level: " << defaultVerboseLevel_ << endl;
}