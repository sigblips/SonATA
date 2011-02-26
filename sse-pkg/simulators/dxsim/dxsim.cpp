/*******************************************************************************

 File:    dxsim.cpp
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

/*
  dxsim.cpp
  DX simulator
 */
#include "ace/Reactor.h"

#include "SseSock.h"
#include "Dx.h"
#include "TextUI.h"
#include "DxUserCmds.h"
#include "SseProxy.h"
#include "DxCmdLineArgs.h"
#include "SseUtil.h"
#include "SseDatagramHandler.h"
#include "ssePorts.h"

using namespace std;

void addUICmds(TextUI &ui, Dx &dx);

static const string defaultSciDataDir("../scienceData");
static const string defaultSciDataPrefix("nss.DriftTestSig");
static const bool defaultBroadcast = false;
static const bool defaultRemoteMode = false;
static const bool defaultNoUi = false;
static const bool defaultVaryDataOutput = false;
static const string defaultDxName("dxsim1001");

int main(int argc, char * argv[])
{
    string defaultHost = "localhost";
    DxCmdLineArgs cmdArgs(argv[0], 
			   ACE_OS::atoi(DEFAULT_DX_PORT),
			   ACE_OS::atoi(DEFAULT_DX_PORT),
			   defaultHost,
			   defaultSciDataDir, defaultSciDataPrefix,
			   defaultBroadcast,
			   defaultNoUi,
			   defaultVaryDataOutput,
			   defaultDxName,
			   defaultRemoteMode);
    if (!cmdArgs.parseArgs(argc, argv))
    {
	cmdArgs.usage();
	exit(1);
    }

    bool remoteMode = cmdArgs.getRemoteMode();
    int port;
    if (remoteMode)
	port = cmdArgs.getRemotePort();
    else
	port = cmdArgs.getMainPort();
    string host = cmdArgs.getHostName();
    bool broadcast = cmdArgs.getBroadcast();

    // Connect to sse in one of two ways:
    // 1. direct tcp socket connect using the above host & port.
    // 2. broadcast a "HereIAm" datagram on the multicast addr &
    //    above port,
    //    and then make a direct tcp socket connect using
    //    hostIp & port information sent back in the
    //    "ThereYouAre" response.

    u_short socketPort(port);
    string socketHost(host);

    if (broadcast)
    {
	// try to connect via HereIAm datagram multicast to Sse.
	u_short multicastPort = port;
	
	SseDatagramHandler dgramHandler (DEFAULT_MULTICASTADDR,
					 multicastPort,
					 *ACE_Reactor::instance ());

	// get back the IP addr & port to connect to
	string hostIpString("");
	dgramHandler.contact(&socketPort, &hostIpString);

	socketHost = hostIpString;
    }

    // put all the ace event handlers on the heap
    // for easier cleanup management

    SseSock * ssesock = new SseSock(socketPort, socketHost.c_str());
    ssesock->connect_to_server();

    SseProxy * proxy = new SseProxy(ssesock->sockstream());

    //Register the socket event handler for reading
    if (ACE_Reactor::instance()->register_handler(
	proxy, ACE_Event_Handler::READ_MASK) == -1)
    {
	cerr << "***failure registering sseproxy handler***" << endl;
    }


    string sciDataDir = cmdArgs.getSciDataDir();
    string sciDataPrefix = cmdArgs.getSciDataPrefix();
    string dxName = cmdArgs.getDxName();
    bool varyOutputData = cmdArgs.getVaryOutputData();

    // put this on the heap to avoid overrunning the stack
    Dx *dx = new Dx(proxy, sciDataDir, sciDataPrefix, dxName, remoteMode,
	    varyOutputData);


//    DxDebug dx(proxy);

    // creat a text UI unless it was disabled on the command line
    TextUI *ui(0);

    if (! cmdArgs.getNoUi())
    {
	// create a text user interface on stdin
	ui = new TextUI();
	ui->setPrompt("dxsim>>");

	// add user interface commands
	addUICmds(*ui, *dx);

	//Start the event loop
	//while(1)
	//ACE_Reactor::instance()->handle_events();

	ui->printPrompt();  // print the first prompt

    }

    //Start the event loop
    ACE_Reactor::run_event_loop();

    // unregister the event handler so that the
    // reactor can cleanly exit
    if (ACE_Reactor::instance()->remove_handler(
	proxy,
	ACE_Event_Handler::READ_MASK) == -1)
    {
	// If remove handler fails it's not necessarily an error
	// since the handler may have already been removed as the
	// result of a closed connection
	// cerr << "***failure removing sseproxy event handler"
	// << "from ace reactor***" << endl;
    }


    // Shut down the Reactor explicitly, so that the
    // registered inputHandlers can properly clean up 
    ACE_Reactor::close_singleton();
    
    //cout << "leaving event loop..." << endl;

    // tbd in gcc3.3 this delete seems to cause a
    // 'pure virtual method' call and core dump
    //delete ui;

    return 0;

}


void addUICmds(TextUI &ui, Dx &dx)
{
    // add some user commands
    // who deletes the memory for these? tbd.

    PrintIntrinsicsCmd *prIntrCmd = new PrintIntrinsicsCmd(&dx);
    ui.addUICmd("intrin", prIntrCmd, "print dx intrinsics");

    PrintConfigCmd *prConfigCmd = new PrintConfigCmd(&dx);
    ui.addUICmd("config", prConfigCmd, "print dx configuration info");

    PrintActParamCmd *prActParamCmd = new PrintActParamCmd(&dx);
    ui.addUICmd("actparam", prActParamCmd, "print activity parameters");

    PrintStatusCmd *prStatusCmd = new PrintStatusCmd(&dx);
    ui.addUICmd("status", prStatusCmd, "print status");

    QuitCmd *quitCmd = new QuitCmd();
    ui.addUICmd("quit", quitCmd, "quit this program (alias for exit)");

    ExitCmd *exitCmd = new ExitCmd();
    ui.addUICmd("exit", exitCmd, "exit this program");

    SendErrorCmd *sendErrorCmd = new SendErrorCmd(&dx);
    ui.addUICmd("senderror", sendErrorCmd, "<error text> send error text to SSE as an NssMessage");


}


