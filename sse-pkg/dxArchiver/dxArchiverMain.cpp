/*******************************************************************************

 File:    dxArchiverMain.cpp
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
  dxArchiverMain.cpp
  Archive dx data products to disk.
 */
#include "ace/Reactor.h"

#include "SseSock.h"
#include "DxArchiver.h"
#include "TextUI.h"
#include "DxArchiverUserCmds.h"
#include "SseProxy.h"
#include "DxArchiverCmdLineArgs.h"
#include "SseUtil.h"
#include "ssePorts.h"
#include "SseException.h"

void addUICmds(TextUI &ui, DxArchiver &dxArchiver);

int main(int argc, char * argv[])
{
    const string defaultSseHostname = "localhost";
    const string defaultName("archiver1");
    const bool defaultNoUi = false;

    DxArchiverCmdLineArgs cmdArgs(
	argv[0], 
	defaultSseHostname,
	ACE_OS::atoi(DEFAULT_DX_ARCHIVER_TO_SSE_PORT),
	ACE_OS::atoi(DEFAULT_DX_TO_DX_ARCHIVER1_PORT),
	defaultNoUi,
	defaultName);

    if (!cmdArgs.parseArgs(argc, argv))
    {
	cmdArgs.usage();
	exit(1);
    }

    int	ssePort = cmdArgs.getSsePort();
    int	dxPort = cmdArgs.getDxPort();
    string sseHostname = cmdArgs.getSseHostname();

    try 
    {
	// put all the ace event handlers on the heap
	// for easier cleanup management

	// Connect to sse via 
	// direct tcp socket connect using the above host & port.

	u_short socketPort(ssePort);
	string socketHost(sseHostname);

	SseSock *ssesock = new SseSock(socketPort, socketHost.c_str());
	ssesock->connect_to_server();

	SseProxy *proxy = new SseProxy(ssesock->sockstream());

	//Register the socket input event handler for reading
	ACE_Reactor::instance()->register_handler(proxy,
						  ACE_Event_Handler::READ_MASK);

	string name = cmdArgs.getName();

	// put this on the heap also, since it contains an ACE event handler
	DxArchiver * dxArchiver = new DxArchiver(proxy, name, dxPort);

//    DxDebug dx(proxy);

	// creat a text UI unless it was disabled on the command line
	TextUI *ui(0);
	if (! cmdArgs.getNoUi())
	{
	    // create a text user interface on stdin
	    ui = new TextUI();
	    ui->setPrompt("archiver>> ");

	    // add user interface commands
	    addUICmds(*ui, *dxArchiver);

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
	// delete ui;

	// this delete causes an ACE error (problem with
	// the lifetime of the Reactor vs. the event handler)
	//delete dxArchiver;

    } 
    catch (SseException &exception)
    {
	//cerr << "Caught SseException: " << endl;
	cerr << exception.descrip() << endl;

	exit(1);
    }
    catch (...)
    {
	cerr << "dx archiver: caught unknown exception" << endl;
	exit(1);
    }
    return 0;

}


void addUICmds(TextUI &ui, DxArchiver &dxArchiver)
{
    // add some user commands
    // who deletes the memory for these? tbd.

    PrintIntrinsicsCmd *prIntrCmd = new PrintIntrinsicsCmd(&dxArchiver);
    ui.addUICmd("intrin", prIntrCmd, "- print archiver intrinsics");

    PrintStatusCmd *prStatusCmd = new PrintStatusCmd(&dxArchiver);
    ui.addUICmd("status", prStatusCmd, "- print archiver status");

    PrintDxNamesCmd *prNamesCmd = new PrintDxNamesCmd(&dxArchiver);
    ui.addUICmd("names", prNamesCmd, "- print names of attached dxs");

    VerboseCmd *verboseCmd = new VerboseCmd(&dxArchiver);
    ui.addUICmd("verbose", verboseCmd, "[<level>] - print/set verbose level [0=none 1=some 2=all]");

    ResetDxSocketCmd *resetDxSocketCmd = new ResetDxSocketCmd(&dxArchiver);
    ui.addUICmd("resetsocket", resetDxSocketCmd, "<dx name> - reset socket for dx");

    QuitCmd *quitCmd = new QuitCmd();
    ui.addUICmd("quit", quitCmd, "- quit this program (alias for exit)");

    ExitCmd *exitCmd = new ExitCmd();
    ui.addUICmd("exit", exitCmd, "- exit this program");

    SendErrorCmd *sendErrorCmd = new SendErrorCmd(&dxArchiver);
    ui.addUICmd("senderror", sendErrorCmd, "<error text> - send error text to SSE as an NssMessage");


}


