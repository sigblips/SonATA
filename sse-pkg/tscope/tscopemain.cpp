/*******************************************************************************

 File:    tscopemain.cpp
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

// Telescope control, acting as an intermediary between the seeker 
// control program and the ATA telescope interface.

#include "ace/Reactor.h"
#include "TextUI.h"
#include "CmdPattern.h"
#include "SseUtil.h"
#include "SseSystem.h"
#include "ssePorts.h"
#include "Tscope.h"
#include "TscopeCmdLineArgs.h"
//#include "TscopeUserCmds.h"
#include "SseProxy.h"
#include "SseSock.h"
#include <fstream>
#include <signal.h>
using namespace std;

static void blockUndesiredSignals();

int main(int argc, char * argv[])
{
  string defaultSseHost = "localhost";
  string defaultAntControlServer = "localhost"; // for simulator
  const int defaultControlPort = 1083; //Prelude is 1081
  const int defaultMonitorPort = 1084; //Prelude is 1082
  const bool defaultNoUi = false;
  const string defaultName("tscope1");
  bool defaultSimulate(false);
  int defaultVerboseLevel(0);

  TscopeCmdLineArgs cmdArgs(argv[0], 
			    defaultSseHost, 
			    atoi(DEFAULT_TSCOPE_PORT), 
			    defaultAntControlServer,
			    defaultControlPort,
			    defaultMonitorPort,
			    defaultNoUi,
			    defaultName,
			    defaultSimulate,
			    defaultVerboseLevel);
  if (!cmdArgs.parseArgs(argc, argv))
  {
      cmdArgs.usage();
      exit(1);
  }

  blockUndesiredSignals();

  cout << "Connecting to seeker." << endl;

  // create ACE event handlers on the heap for easier
  // Reactor cleanup
  SseSock * ssesock = new SseSock(cmdArgs.getSsePort(),
				  cmdArgs.getSseHostName().c_str());
  ssesock->connect_to_server();

  SseProxy *sseProxy = new SseProxy(ssesock->sockstream());
  ACE_Reactor::instance()->register_handler(sseProxy,
					    ACE_Event_Handler::READ_MASK);

  Tscope * tscope = new Tscope(*sseProxy, cmdArgs.getAntControlServerName(), 
			       cmdArgs.getControlPort(),
			       cmdArgs.getMonitorPort());

  tscope->setName(cmdArgs.getName());
  tscope->setVerboseLevel(cmdArgs.getVerboseLevel());
  tscope->setSimulated(cmdArgs.getSimulate());

  if (! cmdArgs.getNoUi())
  {
     TextUI *ui = new TextUI();
     ui->setPrompt("tscope> ");
     //addUICmds(*ui, *tscope);
  }

  ACE_Reactor::run_event_loop();
  
  // unregister the sseproxy event handler so that the
  // reactor can cleanly exit
  if (ACE_Reactor::instance()->remove_handler(
      sseProxy,
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
  

} 

// Block any signal thay may cause the ACE Reactor to fail and exit
// prematurely (and make this program exit as well),
// particularly a 'broken pipe' signal due to the antenna
// server going away.
// However, don't block signals that would prevent this program 
// from responding to a kill that is issued on a remote ssh session
// used to start it.
// (eg, from the controlcomponents script)

static void blockUndesiredSignals()
{
  sigset_t signal_set;

  // init signal_set
  if (sigemptyset(&signal_set) == -1)
  { 
     cerr << "failure calling sigemptyset" << endl;
     return;
  }

#if 0
  // block all signals
  if (sigfillset(&signal_set) == -1)
  { 
     cerr << "failure calling sigfillset" << endl;
     return;
  }
#endif

  if (sigaddset(&signal_set, SIGPIPE) == -1)
  {
      cerr << "failure calling sigaddset" << endl;
      return;

  }

  //  Put the <signal_set>.
  if (ACE_OS::pthread_sigmask (SIG_BLOCK, &signal_set, 0) != 0)
  {
      cerr << "failure calling pthread_sigmask SIG_BLOCK" << endl;
  }

}
