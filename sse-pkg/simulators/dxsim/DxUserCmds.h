/*******************************************************************************

 File:    DxUserCmds.h
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


#ifndef _usercmds_h
#define _usercmds_h

#include "CmdPattern.h"

// command pattern that prints dx intrinsics information
class PrintIntrinsicsCmd : public CmdPattern
{
 public:
    PrintIntrinsicsCmd(Dx *dx) : dx_(dx) {};

    virtual void execute(const string & text)
    {
	dx_->printIntrinsics();
    }    
    
 private:
    Dx *dx_;
};

// command pattern that prints dx activity parameter information
class PrintActParamCmd : public CmdPattern
{
 public:
    PrintActParamCmd(Dx *dx) : dx_(dx) {};

    virtual void execute(const string & text)
    {
	dx_->printActivityParameters();
    }    
    
 private:
    Dx *dx_;
};

// command pattern that prints dx config information
class PrintConfigCmd : public CmdPattern
{
 public:
    PrintConfigCmd(Dx *dx) : dx_(dx) {};

    virtual void execute(const string & text)
    {
	dx_->printConfiguration();
    }    
    
 private:
    Dx *dx_;
};

// command pattern that prints dx status information
class PrintStatusCmd : public CmdPattern
{
 public:
    PrintStatusCmd(Dx *dx) : dx_(dx) {};

    virtual void execute(const string & text)
    {
	dx_->printDxStatus();
    }    
    
 private:
    Dx *dx_;
};



class SendErrorCmd : public CmdPattern
{
 public:
    SendErrorCmd(Dx *dx) : dx_(dx) {};

    virtual void execute(const string & text)
    {
	dx_->sendErrorMsgToSse(text);
    }    
    
 private:
    Dx *dx_;
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