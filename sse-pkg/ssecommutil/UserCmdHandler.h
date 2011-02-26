/*******************************************************************************

 File:    UserCmdHandler.h
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


#ifndef _usercmdhandler_h
#define _usercmdhandler_h

#include "CmdPattern.h"
#include <string>
#include <map>

using std::string;
using std::map;

struct CmdInfo {
    CmdPattern *cmdPattern_;
    string helpText_;

    CmdInfo(CmdPattern *cmdPattern, const string &helpText)
	:
	cmdPattern_(cmdPattern), helpText_(helpText)
    {

    }
};

// index map by command name
typedef map<string, CmdInfo*> CmdMap;

class UserCmdHandler 
{
public:
    UserCmdHandler();
    ~UserCmdHandler();
    void addCmd(const string & keyword, CmdPattern *cmd, const string &helpText = "");
    void processCmd(const string &cmdStr);
    void listCommands();
    void printPrompt();
    void setPrompt(const string &prompt);
private:

    void deleteCmdInfoFromCmdTable();

    CmdMap cmdTable_;
    string prompt_;
};


#endif