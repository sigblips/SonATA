/*******************************************************************************

 File:    UserCmdHandler.cpp
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


// Process commands from a user.
// Implements a command string handler using a GOF command pattern mechanism.
// I.e., command keywords and their associated objects (command patterns)
// are registered.  When the user enters a command (keyword) it
// can be looked up and the associated command object executed.


#include "UserCmdHandler.h"
#include "Assert.h"
#include <iostream>

using namespace std;

// print all the commands ("built-in" command)
class HelpCmd : public CmdPattern
{
public:
    HelpCmd(UserCmdHandler *cmdHandler) : 
	cmdHandler_(cmdHandler) {};

    virtual void execute(const string &argStr)
    {
	cmdHandler_->listCommands();
    };
private:
    UserCmdHandler *cmdHandler_;
};


// do nothing (NOP)
class DoNothingCmd : public CmdPattern
{
public:
    DoNothingCmd(UserCmdHandler *cmdHandler) : 
	cmdHandler_(cmdHandler) {};

    virtual void execute(const string &argStr)
    {
	// do nothing
    };
private:
    UserCmdHandler *cmdHandler_;
};



UserCmdHandler::UserCmdHandler()
   : prompt_("")
{
    // prebuild in a help command
    HelpCmd *helpcmd = new HelpCmd(this);
    addCmd("help", helpcmd, " - print this help information");

    // prebuild in a 'do nothing' command for 'enter/return' only
    DoNothingCmd *nop = new DoNothingCmd(this);
    addCmd("", nop);
}

// destructor

UserCmdHandler::~UserCmdHandler()
{
    // free all the CmdPattern objects?

    deleteCmdInfoFromCmdTable();  
}


// delete all the CmdInfo objects added by
// the addCmd methods
void UserCmdHandler::deleteCmdInfoFromCmdTable()
{
    CmdMap::iterator it;
    for (it = cmdTable_.begin(); it != cmdTable_.end(); ++it)
    {
	CmdInfo *cmdInfo = (*it).second;

	Assert(cmdInfo != 0);
	delete cmdInfo;
    }
}


// add a command keyword & associated command to the cmd list
void UserCmdHandler::addCmd(const string &keyword, CmdPattern *cmd, const string &helpText)
{
    Assert(cmd != 0);

    CmdInfo *cmdInfo = new CmdInfo(cmd, helpText);
    // TBD this should be deleted eventually

    cmdTable_[keyword] = cmdInfo;
}

// process an incoming user command
// parse it, lookup keyword, dispatch it
void UserCmdHandler::processCmd(const string & cmdStr)
{
    // pull off keyword & args
    string keyword(cmdStr);
    string args("");

    if (! cmdStr.empty())
    {
	string::size_type start = 0;
	string::size_type end = 0;

	start   = cmdStr.find_first_not_of(" ", start);
	end     = cmdStr.find_first_of(" ", start);

	keyword = cmdStr.substr(start, end - start);
	if (end != string::npos) {
	    args    = cmdStr.substr(end+1);
	}
    }
    // cout << "processCmd: key=" << keyword << " args=" << args << endl;

    // use keyword to look up action
    if ( cmdTable_.count(keyword) )   // make sure command is there
    {
	CmdInfo *cmdInfo = cmdTable_[keyword];

	// invoke action, passing its args, if any
	cmdInfo->cmdPattern_->execute(args);
    }
    else
    {
	cout << "Command not found: " << keyword << endl;
	cout << "'help' lists available commands" << endl;
    }

    printPrompt();

}

void UserCmdHandler::printPrompt()
{
    // echo the prompt (with no newline)

    cout << prompt_ << flush;
}

void UserCmdHandler::setPrompt(const string & prompt)
{
    prompt_ = prompt;
}

void UserCmdHandler::listCommands()
{
	cout << "Available commands:" << endl;

	CmdMap::iterator it;
        for (it = cmdTable_.begin(); it != cmdTable_.end(); ++it)
	{
	    const string & keyword = (*it).first;
	    CmdInfo *cmdInfo = (*it).second;

	    cout << "\t" << keyword;

	    if (cmdInfo->helpText_ != "")
	    {
		cout << "  " << cmdInfo->helpText_ ;
	    }
	    cout << endl;
	}
}


#if 0

class TestCmd  : public CmdPattern
{
public:
    TestCmd() {};

    virtual void execute(string argStr)
    {
	cout <<" TestCmd execute invoked with arg:"  << argStr << endl;
    };
private:
    
};

int main()
{
  UserCmdHandler cmdHandler;
  TestCmd *testCmd = new TestCmd();

  // set up the command keywords & associated methods
  cmdHandler.addCmd("test", testCmd, "this is a test command");

  // debug 
  cmdHandler.processCmd("test");
  cmdHandler.processCmd("foo");  // this should fail -- invalid cmd
  cmdHandler.processCmd("help");
}
#endif





