/*******************************************************************************

 File:    DxArchiverCmdLineArgs.cpp
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
#include "DxArchiverCmdLineArgs.h" 
#include <iostream>

using namespace std;

DxArchiverCmdLineArgs::DxArchiverCmdLineArgs(
    const string &progName,
    const string &defaultSseHostname,
    int defaultSsePort,
    int defaultDxPort,
    const bool defaultNoUi,
    const string &defaultName)

    :progName_(progName),

     ssePortKeyword_("-sse-port"), 
     ssePort_(defaultSsePort),
     defaultSsePort_(defaultSsePort),

     dxPortKeyword_("-dx-port"), 
     dxPort_(defaultDxPort),
     defaultDxPort_(defaultDxPort),

     sseHostnameKeyword_("-host"), 
     sseHostname_(defaultSseHostname), 
     defaultSseHostname_(defaultSseHostname),

     noUiKeyword_("-noui"), 
     noUi_(defaultNoUi),
     defaultNoUi_(defaultNoUi),

     nameKeyword_("-name"), 
     name_(defaultName),
     defaultName_(defaultName)

{
}

DxArchiverCmdLineArgs::~DxArchiverCmdLineArgs()
{
}

int DxArchiverCmdLineArgs::getSsePort()
{
    return ssePort_;
}

int DxArchiverCmdLineArgs::getDxPort()
{
    return dxPort_;
}

bool DxArchiverCmdLineArgs::getNoUi()
{
    return noUi_;
}



const string &DxArchiverCmdLineArgs::getName()
{
    return name_;
}

const string &DxArchiverCmdLineArgs::getSseHostname()
{
    return sseHostname_;
}


bool DxArchiverCmdLineArgs::parseArgs(int argc, char *argv[])
{
    ACE_Arg_Shifter arg(argc, argv);
    bool status(true);

    // eat arg0
    arg.consume_arg();

    while (arg.is_anything_left() && status)
    {
	const char *currentArg=arg.get_current();

        if (currentArg == sseHostnameKeyword_)
        {
	    arg.consume_arg();
	    status = getSseHostnameValue(arg.get_current());
	}
        else if (currentArg == ssePortKeyword_)
        {
	    arg.consume_arg();
	    status = getPortValue(arg.get_current(), ssePortKeyword_,
				  &ssePort_);
	}
        else if (currentArg == dxPortKeyword_)
        {
	    arg.consume_arg();
	    status = getPortValue(arg.get_current(), dxPortKeyword_,
				  &dxPort_);
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
        else 
        {
            cerr << "error: unknown option: " << arg.get_current() << endl;
            status = false;
        }

        arg.consume_arg();

    }
    return status;
}

bool DxArchiverCmdLineArgs::getSseHostnameValue(const char *value)
{
    bool status = true;

    if (value)
    {
	sseHostname_ = value;
    }
    else
    {
	cerr << "error: no value given for " << 
	    sseHostnameKeyword_ << endl;
	status = false;
    }

    return status;
}

bool DxArchiverCmdLineArgs::getPortValue(const char *value, const string &keyword, 
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


bool DxArchiverCmdLineArgs::getNameValue(const char *value)
{
    bool status = true;

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


void DxArchiverCmdLineArgs::usage()
{
    cerr << "Usage: ";
    cerr << progName_;
    cerr << " [" << sseHostnameKeyword_ << " <sse host>]";
    cerr << " [" << ssePortKeyword_ << " <SSE port number>]";
    cerr << endl;
    cerr << " [" << dxPortKeyword_ << " <DX port number>]";
    cerr << endl;
    cerr << " [-noui (disable UI)]" << endl;
    cerr << " [" << nameKeyword_ << " <archiver name>]";
    cerr << endl;

    cerr << "Default SSE host: " << defaultSseHostname_ << endl;
    cerr << "Default SSE port: " << defaultSsePort_ << endl;
    cerr << "Default DX port: " << defaultDxPort_ << endl;
    cerr << "Default archiver name: " << defaultName_ << endl;
}