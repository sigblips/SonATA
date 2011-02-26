/*******************************************************************************

 File:    TestUserCmdHandler.cpp
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


#include <iostream>
#include "UserCmdHandler.h"

using namespace std;

class TestCmd  : public CmdPattern
{
public:
    TestCmd() {};

    virtual void execute(const string & argStr)
    {
	cout <<"TestCmd execute invoked (arg='"  << argStr << "')" << endl;
    };
private:
    
};


void doTest(UserCmdHandler &handler, const string &cmd)
{
    cout << cmd << endl;  // echo the cmd
    handler.processCmd(cmd);
}


int main()
{
  UserCmdHandler cmdHandler;
  cmdHandler.setPrompt("CmdHandler>>");

  // set up the command keywords & associated methods
  TestCmd *testCmd = new TestCmd();
  cmdHandler.addCmd("test", testCmd);

  // debug 
  doTest(cmdHandler, "test");
  doTest(cmdHandler, "test arg1");
  doTest(cmdHandler, "test arg1 arg2");
  doTest(cmdHandler, " test");  // start with a space
  doTest(cmdHandler, " test ");  // end with a space
  doTest(cmdHandler, "");      // nop should generate a new prompt
  doTest(cmdHandler, "foo"); // this should be an error -- invalid cmd
  doTest(cmdHandler, "help");

}




