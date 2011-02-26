/*******************************************************************************

 File:    TestTextUI.cpp
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


// Test driver for the Text UI lib calls
#include "ace/Reactor.h"
#include "TextUI.h"
#include "CmdPattern.h"

void addUICmds(TextUI &ui);

// command pattern that exits the program
class QuitCmd : public CmdPattern
{
 public:
    QuitCmd(void){};

    virtual void execute(const string & text)
    {
        ACE_Reactor::end_event_loop ();
    }    
    
 private:

};

int main(int argc, char * argv[]){

/*
    if (argc < 3) {
	ACE_DEBUG((LM_DEBUG, "Usage: TBD\n"));
	ACE_OS::exit(1);
    }
*/
    // create a text user interface on stdin
    TextUI * newui = new TextUI();
    TextUI & ui = *newui;

    ui.setPrompt("textui>>");

    // add user interface commands
    addUICmds(ui);

    ui.printPrompt();  // print the first prompt

    //Start the event loop
    ACE_Time_Value timeout(0, 1000);
    while (ACE_Reactor::event_loop_done() == 0)
    {
	ACE_Reactor::run_event_loop (timeout);
    }

    // Shut down the Reactor explicitly, so that the
    // registered inputHandlers can properly clean up.
    //
    // TBD in ace5.3/gcc3.3 calling this followed by
    // the ui delete below results in a pure virtual function call
    // It doesn't seem to be necessary anyway.
    //ACE_Reactor::close_singleton();


    delete newui;

}


void addUICmds(TextUI &ui)
{
    // add some user commands
    // who deletes the memory for these? tbd.

    QuitCmd *quitCmd = new QuitCmd();
    const string cmdName = "quit";
    const string cmdHelp = "exit this program";
    ui.addUICmd(cmdName, quitCmd, cmdHelp);
}
