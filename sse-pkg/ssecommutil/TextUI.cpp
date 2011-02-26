/*******************************************************************************

 File:    TextUI.cpp
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

// Text based User Interface
#include "ace/ACE.h"
#include "ace/Reactor.h"

#include "TextUI.h"
#include "StdinHandler.h"
#include "UserCmdHandler.h"
#include "ProcessInputCmd.h"

TextUI::TextUI()
{
    // create a user command handler
    cmdHandler_ = new UserCmdHandler();

    // create a command pattern that will link the UserCmdHandler
    // to the StdinHandler
    procInputCmd_ = new ProcessInputCmd(cmdHandler_);

    // attach an input handler to stdin for commands
    stdinHandler_ = new StdinHandler(procInputCmd_); 

}


void TextUI::addUICmd(const string &cmdName, CmdPattern *cmd, const string &cmdHelp)
{
    // add some user commands
    // who deletes the memory for these? tbd.
    cmdHandler_->addCmd(cmdName, cmd, cmdHelp);
}


TextUI::~TextUI()
{
    delete cmdHandler_;
    delete procInputCmd_;
    delete stdinHandler_;
}

void TextUI::printPrompt()
{
    cmdHandler_->printPrompt();
}

void TextUI::setPrompt(string prompt)
{
    cmdHandler_->setPrompt(prompt);
}

void TextUI::processCmd(const string& cmd)
{
    cmdHandler_->processCmd(cmd);
}