/*******************************************************************************

 File:    TextUI.h
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


#ifndef _textui_h
#define _textui_h

#include <string>
#include "CmdPattern.h"

class UserCmdHandler;
class ProcessInputCmd;
class StdinHandler;

using std::string;

class TextUI
{
 public:
    TextUI();
    ~TextUI();
    void addUICmd(const string &cmdName, CmdPattern *cmd,
		  const string &cmdHelp = "");
    void printPrompt(void);
    void setPrompt(string prompt);
    void processCmd(const string& cmd);


 private:
    UserCmdHandler *cmdHandler_;   // user input handler
    ProcessInputCmd *procInputCmd_; // process input (command pattern)
    StdinHandler *stdinHandler_;  // ACE stdin input text handler
};


#endif




