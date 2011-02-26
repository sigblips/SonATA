/*******************************************************************************

 File:    DxCmdLineArgs.cpp
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
#include "DxCmdLineArgs.h"
#include <iostream>

using namespace std;

DxCmdLineArgs::DxCmdLineArgs(const string &progName,
			       int defaultMainPort,
			       int defaultRemotePort,
			       const string &defaultHost,
			       const string &defaultSciDataDir,
			       const string &defaultSciDataPrefix,
			       const bool defaultBroadcast,
			       const bool defaultNoUi,
			       const bool defaultVaryOutputData,
			       const string &defaultDxName,
			       const bool defaultRemoteMode)
    :progName_(progName),

     mainPortKeyword_("-mainport"), 
     mainPort_(defaultMainPort),
     defaultMainPort_(defaultMainPort),

     remotePortKeyword_("-remport"), 
     remotePort_(defaultRemotePort),
     defaultRemotePort_(defaultRemotePort),

     hostNameKeyword_("-host"), 
     hostName_(defaultHost), 
     defaultHostName_(defaultHost),

     sciDataDirKeyword_("-sddir"), 
     sciDataDir_(defaultSciDataDir), 
     defaultSciDataDir_(defaultSciDataDir),

     sciDataPrefixKeyword_("-sdprefix"), 
     sciDataPrefix_(defaultSciDataPrefix), 
     defaultSciDataPrefix_(defaultSciDataPrefix),

     broadcastKeyword_("-broadcast"), 
     broadcast_(defaultBroadcast),
     defaultBroadcast_(defaultBroadcast),

     noUiKeyword_("-noui"), 
     noUi_(defaultNoUi),
     defaultNoUi_(defaultNoUi),

     varyOutputDataKeyword_("-vary"), 
     varyOutputData_(defaultNoUi),
     defaultVaryOutputData_(defaultVaryOutputData),

     remoteModeKeyword_("-remote"), 
     remoteMode_(defaultRemoteMode),
     defaultRemoteMode_(defaultRemoteMode),

     dxNameKeyword_("-name"), 
     dxName_(defaultDxName),
     defaultDxName_(defaultDxName)

{
}

DxCmdLineArgs::~DxCmdLineArgs()
{
}

int DxCmdLineArgs::getMainPort()
{
    return mainPort_;
}

int DxCmdLineArgs::getRemotePort()
{
    return remotePort_;
}

const string &DxCmdLineArgs::getDxName()
{
    return dxName_;
}

const string &DxCmdLineArgs::getHostName()
{
    return hostName_;
}

const string &DxCmdLineArgs::getSciDataDir()
{
    return sciDataDir_;
}

const string &DxCmdLineArgs::getSciDataPrefix()
{
    return sciDataPrefix_;
}

bool DxCmdLineArgs::getBroadcast()
{
    return broadcast_;
}

bool DxCmdLineArgs::getNoUi()
{
    return noUi_;
}

bool DxCmdLineArgs::getVaryOutputData()
{
    return varyOutputData_;
}

bool DxCmdLineArgs::getRemoteMode()
{
    return remoteMode_;
}



bool DxCmdLineArgs::parseArgs(int argc, char *argv[])
{
    ACE_Arg_Shifter arg(argc, argv);
    bool status(true);

    // eat arg0
    arg.consume_arg();

    while (arg.is_anything_left() && status)
    {
	const char *currentArg=arg.get_current();

        if (currentArg == hostNameKeyword_)
        {
	    arg.consume_arg();
	    status = getHostnameValue(arg.get_current());
	}
        else if (currentArg == mainPortKeyword_)
        {
	    arg.consume_arg();
	    status = getPortValue(arg.get_current(), mainPortKeyword_,
				  &mainPort_);
	}
        else if (currentArg == remotePortKeyword_)
        {
	    arg.consume_arg();
	    status = getPortValue(arg.get_current(), remotePortKeyword_,
				  &remotePort_);
	}
        else if (currentArg == sciDataDirKeyword_)
        {
	    arg.consume_arg();
	    status = getSciDataDirValue(arg.get_current());
	}
        else if (currentArg == sciDataPrefixKeyword_)
        {
	    arg.consume_arg();
	    status = getSciDataPrefixValue(arg.get_current());
	}
        else if (currentArg == broadcastKeyword_)
        {
            // flag only - don't consume arg
	    broadcast_ = true;
	}
        else if (currentArg == noUiKeyword_)
        {
            // flag only - don't consume arg
	    noUi_ = true;
	}
        else if (currentArg == varyOutputDataKeyword_)
        {
            // flag only - don't consume arg
	    varyOutputData_ = true;
	}
        else if (currentArg == remoteModeKeyword_)
        {
            // flag only - don't consume arg
	    remoteMode_ = true;
	}
        else if (currentArg == dxNameKeyword_)
        {
	    arg.consume_arg();
	    status = getDxNameValue(arg.get_current());
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

bool DxCmdLineArgs::getHostnameValue(const char *value)
{
    bool status = true;

    if (value)
    {
	hostName_ = value;
    }
    else
    {
	cerr << "error: no value given for " << 
	    hostNameKeyword_ << endl;
	status = false;
    }

    return status;
}

bool DxCmdLineArgs::getPortValue(const char *value, const string &keyword, 
				  int *retPort)
{
    bool status = true;

    if (value)
    {
	int port = ACE_OS::atoi(value); 
	int minPort = 1024;
	int maxPort = 65535;
	if (port >= minPort && port <= maxPort)
	{
	    *retPort = port;
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
	cerr << "error: no value given for " << 
	    keyword << endl;
	status = false;
    }

    return status;
}

bool DxCmdLineArgs::getSciDataDirValue(const char *value)
{
    bool status = true;

    if (value)
    {
	sciDataDir_ = value;
    }
    else
    {
	cerr << "error: no value given for " << 
	    sciDataDirKeyword_ << endl;
	status = false;
    }

    return status;
}

bool DxCmdLineArgs::getSciDataPrefixValue(const char *value)
{
    bool status = true;

    if (value)
    {
	sciDataPrefix_ = value;
    }
    else
    {
	cerr << "error: no value given for " << 
	    sciDataPrefixKeyword_ << endl;
	status = false;
    }

    return status;
}

bool DxCmdLineArgs::getDxNameValue(const char *value)
{
    bool status = true;

    if (value)
    {
	dxName_ = value;
    }
    else
    {
	cerr << "error: no value given for " << 
	    dxNameKeyword_ << endl;
	status = false;
    }

    return status;
}


void DxCmdLineArgs::usage()
{
    cerr << "Usage: ";
    cerr << progName_;
    cerr << " [" << hostNameKeyword_ << " <host>]";
    cerr << " [" << mainPortKeyword_ << " <portNumber>]";
    cerr << endl;
    cerr << " [" << remotePortKeyword_ << " <portNumber>] ";
    cerr << " [" << sciDataDirKeyword_ << " <science data dir>]";
    cerr << endl;
    cerr << " [" << sciDataPrefixKeyword_ << " <science data prefix>] ";
    cerr << " [" << broadcastKeyword_ << " (connect to SSE via UDP)]";
    cerr << endl;
    cerr << " [" << noUiKeyword_ << " (disable UI)]";
    cerr << " [" << varyOutputDataKeyword_ << " (vary data output)]";
    cerr << " [" << dxNameKeyword_ << " <dx name>]";
    cerr << endl;
    cerr << " [" << remoteModeKeyword_ << " (attach to SSE remote port)]";
    cerr << endl;

    cerr << "Default host: " << defaultHostName_ << endl;
    cerr << "Default main port: " << defaultMainPort_ << endl;
    cerr << "Default remote port: " << defaultRemotePort_ << endl;
    cerr << "Default science data dir: " << defaultSciDataDir_ << endl;
    cerr << "Default science data prefix: " << defaultSciDataPrefix_ << endl;
    cerr << "Default dx name: " << defaultDxName_ << endl;
}