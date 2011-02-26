/*******************************************************************************

 File:    DxArchiverUserCmds.h
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



#ifndef _dx_archiver_user_cmds_h
#define _dx_archiver_user_cmds_h

#include "CmdPattern.h"
#include "SseUtil.h"

using std::endl;
using std::cerr;
using std::cout;

// command pattern that prints dx intrinsics information
class PrintIntrinsicsCmd : public CmdPattern
{
 public:
    PrintIntrinsicsCmd(DxArchiver *dxArchiver) : dxArchiver_(dxArchiver) {};

    virtual void execute(const string & text)
    {
	dxArchiver_->printIntrinsics();
    }    
    
 private:
    DxArchiver *dxArchiver_;
};

// command pattern that prints dx status information
class PrintStatusCmd : public CmdPattern
{
 public:
    PrintStatusCmd(DxArchiver *dxArchiver) : dxArchiver_(dxArchiver) {};

    virtual void execute(const string & text)
    {
	dxArchiver_->printStatus();
    }    
    
 private:
    DxArchiver *dxArchiver_;
};

// command pattern that prints dx status information
class PrintDxNamesCmd : public CmdPattern
{
 public:
    PrintDxNamesCmd(DxArchiver *dxArchiver) : dxArchiver_(dxArchiver) {};

    virtual void execute(const string & text)
    {
	dxArchiver_->printDxNames();
    }    
    
 private:
    DxArchiver *dxArchiver_;
};



class SendErrorCmd : public CmdPattern
{
 public:
    SendErrorCmd(DxArchiver *dxArchiver) : dxArchiver_(dxArchiver) {};

    virtual void execute(const string & text)
    {
	dxArchiver_->sendErrorMsgToSse(text);
    }    
    
 private:
    DxArchiver *dxArchiver_;
};



class VerboseCmd : public CmdPattern
{
 public:
    VerboseCmd(DxArchiver *dxArchiver) : dxArchiver_(dxArchiver) {};

    virtual void execute(const string & text)
    {
	if (text == "")
	{
	    cout << "verbose level is currently: " 
		 << dxArchiver_->getVerboseLevel()  << endl;

	} else
	{
	    int level = -1;
	    try {
		level = SseUtil::strToInt(text);
	    } catch (...)
	    {
		cerr << "Invalid verbose level: " << text 
		     << ".  Level should be: 0 - 2" << endl;
		return;
	    }

	    dxArchiver_->setVerboseLevel(level);
	    cout << "verbose set to: " << level << endl;
	}
    }    
    
 private:
    DxArchiver *dxArchiver_;
};


class ResetDxSocketCmd : public CmdPattern
{
 public:
    ResetDxSocketCmd(DxArchiver *dxArchiver) : dxArchiver_(dxArchiver) {};

    virtual void execute(const string & text)
    {
	if (text == "")
	{
	    cout << "resetsocket: must give the name of a dx " << endl;
	} 
	else
	{
	    const string & dxName = text;
	    dxArchiver_->resetDxSocket(dxName);
	}
    }    
    
 private:
    DxArchiver *dxArchiver_;
};



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


// command pattern that exits the program
class ExitCmd : public CmdPattern
{
 public:
    ExitCmd(void){};

    virtual void execute(const string & text)
    {
	ACE_Reactor::end_event_loop ();
    }    
    
 private:

};



#endif