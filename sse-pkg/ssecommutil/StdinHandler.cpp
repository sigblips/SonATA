/*******************************************************************************

 File:    StdinHandler.cpp
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

// handler for stdin

#include "ace/Thread_Manager.h"
#include "ace/Service_Config.h"
#include "ace/streams.h"

#include "StdinHandler.h"

// create a handler that can handle stdin
// textHandler is a command pattern object whose
// 'execute' method takes an argument string.

StdinHandler::StdinHandler (CmdPattern *textHandler):
    textHandler_(textHandler)
{
    //  cout << "registering stdinhandler..." << endl;

  // Register the STDIN handler.
  if (ACE_Event_Handler::register_stdin_handler
      (this,
     ACE_Reactor::instance (),
     ACE_Thread_Manager::instance ()) == -1)
  {
      cerr << "StdinHandler: failure of register_stdin_handler" << endl;

      //ACE_ERROR ((LM_ERROR, "%p\n", "register_stdin_handler"));
      //ACE_ERROR ((LM_ERROR, "register_stdin_handler"));
  }
}

StdinHandler::~StdinHandler (void)
{
  // remove the STDIN handler.
  if (ACE_Event_Handler::remove_stdin_handler
      (ACE_Reactor::instance (),
     ACE_Thread_Manager::instance ()) == -1)
  {
      cerr << "~StdinHandler: failure of remove_stdin_handler" << endl;
  }

}


int
StdinHandler::handle_input (ACE_HANDLE h)
{
    char buf[BUFSIZ];

    if (h == ACE_STDIN)
    {
	ssize_t result = ACE_OS::read (h, buf, BUFSIZ);

	//cout << "result=" << result << endl;
	if (result > 0)
	{
            buf[result-1] = '\0';  // terminate the string
	    //cout << "echo->" << "[" << buf << "]" << endl;

	    // hand the text off to the text handler
            // which is a command pattern object
	    textHandler_->execute(buf);

	    return 0;
	}
	else if (result == -1)
	{
//	    ACE_ERROR_RETURN ((LM_ERROR,
	    //    	       "can't read from STDIN"), -1);

	    return -1;

	    //  ACE_ERROR_RETURN ((LM_ERROR, "%p\n",
	    //	       "can't read from STDIN"), -1);
	}
	else // result == 0
        {
	    ACE_Reactor::end_event_loop ();
	    return -1;
        }
    }

    return -1;
}

int
StdinHandler::handle_close (ACE_HANDLE h, ACE_Reactor_Mask)
{

// is this necessary, given the return -1 on the handle_input?
#if 0
  if (h == ACE_STDIN)
    {
      ACE_DEBUG ((LM_DEBUG,
                  "STDIN_Events handle removed from reactor.\n"));
      if (ACE_Reactor::instance ()->remove_handler 
          (this, ACE_Event_Handler::READ_MASK) == -1)
        ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "remove_handler"),
                           -1);
    }
#endif

  return 0;
}


#if 0
int 
main (int argc, char *argv[])
{

    StdinHandler stdinhandler;

  // Run the event loop.
  ACE_Reactor::run_event_loop ();


  ACE_DEBUG ((LM_DEBUG,
              "stdinHandler Done.\n"));


  return 0;
}
#endif