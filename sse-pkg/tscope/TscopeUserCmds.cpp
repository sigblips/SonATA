/*******************************************************************************

 File:    TscopeUserCmds.cpp
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


#include "TscopeUserCmds.h"
#include "ace/Reactor.h"
#include "TextUI.h"
#include "CmdPattern.h"
#include "SseUtil.h"
#include "SseCommUtil.h"
#include "SseSystem.h"
#include "Tscope.h"
#include "TscopeCmdLineArgs.h"
#include "TscopeEventHandler.h"
#include "SseProxy.h"
#include "SseTscopeMsg.h"
#include <sstream>

using namespace std;

static bool validAtaSubarray(string subarray)
{
   string errorText;
   if (!SseCommUtil::validAtaSubarray(subarray, errorText))
   {
      cerr << errorText << endl;
      return false;
   }
   return true;
}

class AllocateCmd : public CmdPattern
{
 public:
    AllocateCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	tscope_.allocate();
    }
    
private:
    Tscope& tscope_;

};

class AssignSubarrayCmd : public CmdPattern
{
 public:
    AssignSubarrayCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");

        enum indices { beamNameIndex, subarrayIndex, expectedNumArgs };	
	if (tokens.size() != expectedNumArgs)
	{
	    cerr << "assign command needs 2 args: beamxxx antxx[,antxx..]" 
                 << endl;
	    return;
	}

        string beamName(tokens[beamNameIndex]);
        TscopeBeam beam = 
           SseTscopeMsg::nameToBeam(SseUtil::strToUpper(beamName));
        if (beam == TSCOPE_INVALID_BEAM)
        {
           cerr << "invalid beam name: " << beamName << endl;
           return;
        }

        if (!validAtaSubarray(tokens[subarrayIndex]))
        {
           return;
        }
        TscopeAssignSubarray assignSub;
        assignSub.beam = beam;
        SseUtil::strMaxCpy(assignSub.subarray, tokens[subarrayIndex].c_str(),
                           MAX_TEXT_STRING);

        tscope_.assignSubarray(assignSub);
    }
    
private:
  Tscope& tscope_;

};



class CalCmd : public CmdPattern
{
 public:
    CalCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {

// tbd change to new cal cmd
#if 0
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");

        enum indices { subarrayIndex, valueIndex, expectedNumArgs };	
	if (tokens.size() != expectedNumArgs)
	{
	    cerr << "cal command needs 2 args: antxx[,antxx..] <on|off>" 
                 << endl;
	    return;
	}
        
        bool_t turnOn(SSE_FALSE);
	if (tokens[valueIndex] == "on")
	{
	   turnOn = SSE_TRUE;
	}
	else if (tokens[valueIndex] == "off")
	{
	    turnOn = SSE_FALSE;
	}
	else 
	{
	    cerr << "cal value must be 'on' or 'off'" << endl;
            return;
	}

        TscopeCalRequest cal;
        cal.turnOn = turnOn;

        if (!validAtaSubarray(tokens[subarrayIndex]))
        {
           return;
        }
        SseUtil::strMaxCpy(cal.subarray, tokens[subarrayIndex].c_str(),
                           MAX_TEXT_STRING);

        tscope_.cal(cal);
#endif

    }
    
private:
  Tscope& tscope_;

};



class ConnectCmd : public CmdPattern
{
 public:
    ConnectCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	try {

	    vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	    if (tokens.size() == 0) 
	    {
		tscope_.connect();
	    }
	    else
	    {
		cerr << "error - connect command takes no arguments" << endl;
	    }
	    
	}
	catch (...)
	{
	    cerr << "failed to connect to tscope" << endl;
	}

    }
    
private:
    Tscope& tscope_;
    
};

class DeallocateCmd : public CmdPattern
{
public:
    
    DeallocateCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	tscope_.deallocate();
    }
    
private:
    
  Tscope& tscope_;

};

class DisconnectCmd : public CmdPattern
{
 public:
  DisconnectCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	tscope_.disconnect();
    }

private:
  Tscope& tscope_;

};

class ExitCmd : public CmdPattern
{
 public:

  ExitCmd(void){};

    virtual void execute(const string & cmdArgs)
    {
	ACE_Reactor::end_event_loop ();
    }    
};



class NameCmd : public CmdPattern 
{
 public:

    NameCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	if (tokens.size() < 1)
	{
	    cerr << "name command needs one argument: " << endl;
	    return;
	}

	tscope_.setName(tokens[0].c_str());
  }

private:
  Tscope& tscope_;


};


/*
 point either synth beam or subarray,
in azel, j2000, or gal coords
*/
class PointCmd : public CmdPattern 
{
 public:
  PointCmd(Tscope& tscope) : tscope_(tscope) {};

   virtual void execute(const string & cmdArgs) 
   {
      vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");

      enum indices { pointTypeIndex, thingToPointIndex,
                     coordTypeIndex, coord1Index, coord2Index, 
		     expectedNumArgs };

      if (tokens.size() != expectedNumArgs)
      {
	 cerr << "point command needs " 
	      << expectedNumArgs << " args:, see 'help' " << endl;
	 return;
      }

      TscopeTargetRequest request;

      const string pointBeamKeyword("beam");
      const string pointSubarrayKeyword("subarray");
      if (tokens[pointTypeIndex] == pointBeamKeyword)
      {
         string beamName(tokens[thingToPointIndex]);
         TscopeBeam beam = 
            SseTscopeMsg::nameToBeam(SseUtil::strToUpper(beamName));
         if (beam == TSCOPE_INVALID_BEAM)
         {
            cerr << "invalid beam name: " << beamName << endl;
            return;
         }
         request.pointType = TscopeTargetRequest::POINT_BEAM;
         request.beam = beam;

      }
      else if (tokens[pointTypeIndex] == pointSubarrayKeyword)
      {
        if (!validAtaSubarray(tokens[thingToPointIndex]))
        {
           return;
        }
        request.pointType = TscopeTargetRequest::POINT_SUBARRAY;
        SseUtil::strMaxCpy(request.subarray, 
                           tokens[thingToPointIndex].c_str(),
                           MAX_TEXT_STRING);
      }
      else
      {
         cerr << "must specify either '" << pointBeamKeyword
              << "' or '" << pointSubarrayKeyword << "'"
              << endl;
         return;
      }


      double coord1(0);
      double coord2(0);
      try {
	 coord1 = SseUtil::strToDouble(tokens[coord1Index]);
	 coord2 = SseUtil::strToDouble(tokens[coord2Index]);
      }	
      catch (SseException &except)
      {
	 cerr << "invalid coordinate: " << except << endl;
	 return;
      }

      const string coordTypeJ2000("j2000");
      const string coordTypeAzEl("azel");
      const string coordTypeGal("gal");
      string coordType = tokens[coordTypeIndex];
      if (coordType == coordTypeJ2000) 
      {
	 request.coordType = TscopeTargetRequest::J2000;
	 request.raHours = coord1;
	 request.decDeg = coord2;
      } 
      else if (coordType == coordTypeAzEl)
      {
	 request.coordType = TscopeTargetRequest::AZEL;
	 request.azDeg = coord1;
	 request.elDeg = coord2;
      } 
      else if (coordType == coordTypeGal)
      {
	 request.coordType = TscopeTargetRequest::GALACTIC;
	 request.galLongDeg = coord1;
	 request.galLatDeg = coord2;
      } 
      else 
      {
	 cerr << "Unsupported coordinate type: '" << coordType
	      << "', must be: "
	      << coordTypeJ2000 << " | "
	      << coordTypeAzEl << " | " 
	      << coordTypeGal
	      << endl;
	   
	 return;
      }

      tscope_.point(request);

   }    
private:
  Tscope& tscope_;


};

class QuitCmd : public CmdPattern
{
 public:
    QuitCmd(void){};
    
    virtual void execute(const string & cmdArgs)
    {
	ACE_Reactor::end_event_loop();
    }    
};

class ResetCmd : public CmdPattern
{
public:
    ResetCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	tscope_.reset();
    }
    
private:
    Tscope& tscope_;
    
};

class SendErrorCmd : public CmdPattern
{
public:
    SendErrorCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	tscope_.reportErrorToSse(cmdArgs);
    }
    
private:
    Tscope& tscope_;
    
};


class TuneCmd : public CmdPattern
{
public:
    TuneCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	enum indices { tuningIndex, freqIndex, expectedNumArgs };
	if (tokens.size() != expectedNumArgs) 
	{
	    cerr << "tune command needs "
		 <<  expectedNumArgs << " args: " 
		 << "<tuning{a-d}> <skyfreq in MHz> " << endl;
	    return;
	}
	
	try {
	   string tuningName(tokens[tuningIndex]);
	   TscopeTuning tuning = 
	      SseTscopeMsg::nameToTuning(SseUtil::strToUpper(tuningName));
	   if (tuning == TSCOPE_INVALID_TUNING)
	   {
	      cerr << "invalid tuning: " << tuningName << endl;
	      return;
	   }

	   TscopeTuneRequest tuneReq;
	   tuneReq.tuning = tuning;
	   tuneReq.skyFreqMhz = SseUtil::strToDouble(tokens[freqIndex]);
	   tscope_.tune(tuneReq);
	}
	catch (SseException &except)
	{
	    cerr << "invalid frequency: " << except << endl;
	}
    }
    
private:
    Tscope& tscope_;
    
};


class ZfocusCmd : public CmdPattern
{
public:
    ZfocusCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	enum indices { subarrayIndex, freqIndex, expectedNumArgs };
	if (tokens.size() != expectedNumArgs) 
	{
	    cerr << "zfocus command needs "
		 <<  expectedNumArgs << " arg(s): " 
		 << "antxx[,antxx..] <skyfreq in MHz> " << endl;
	    return;
	}
	
	try {
           if (!validAtaSubarray(tokens[subarrayIndex]))
           {
              return;
           }
	   TscopeZfocusRequest zfocusReq;
           SseUtil::strMaxCpy(zfocusReq.subarray, 
                              tokens[subarrayIndex].c_str(),
                              MAX_TEXT_STRING);

	   zfocusReq.skyFreqMhz = SseUtil::strToDouble(tokens[freqIndex]);
	   tscope_.zfocus(zfocusReq);
	}
	catch (SseException &except)
	{
	    cerr << "invalid frequency: " << except << endl;
	}
    }
    
private:
    Tscope& tscope_;
    
};


class SimulateCmd : public CmdPattern
{
public:
    SimulateCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	tscope_.setSimulated(true);
    }
    
private:
    Tscope& tscope_;
    
};

class StopCmd : public CmdPattern
{
public:
    StopCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	enum indices { subarrayIndex, expectedNumArgs };
	if (tokens.size() != expectedNumArgs) 
	{
	    cerr << "stop command needs "
		 <<  expectedNumArgs << " arg(s): " 
		 << "antxx[,antxx..]" << endl;
	    return;
	}

        if (!validAtaSubarray(tokens[subarrayIndex]))
        {
           return;
        }
        TscopeStopRequest stopReq;
        SseUtil::strMaxCpy(stopReq.subarray, 
                           tokens[subarrayIndex].c_str(),
                           MAX_TEXT_STRING);
	tscope_.stop(stopReq);
    }
    
private:
    Tscope& tscope_;
    
};

class StowCmd : public CmdPattern
{
public:
    StowCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	enum indices { subarrayIndex, expectedNumArgs };
	if (tokens.size() != expectedNumArgs) 
	{
	    cerr << "stow command needs "
		 <<  expectedNumArgs << " arg(s): " 
		 << "antxx[,antxx..]" << endl;
	    return;
	}

        if (!validAtaSubarray(tokens[subarrayIndex]))
        {
           return;
        }
        TscopeStowRequest stowReq;
        SseUtil::strMaxCpy(stowReq.subarray, 
                           tokens[subarrayIndex].c_str(),
                           MAX_TEXT_STRING);
	tscope_.stow(stowReq);
    }
    
private:
    Tscope& tscope_;
    
};

class StatusCmd : public CmdPattern
{
public:
    StatusCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	cout << tscope_.getStatusMultibeam();
	cout << tscope_;
    }
    
private:
    Tscope& tscope_;

};


class IntrinsicsCmd : public CmdPattern
{
public:
    IntrinsicsCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	cout << tscope_.getIntrinsics();
    }
    
private:
    Tscope& tscope_;

};

class TrackingCmd : public CmdPattern
{
 public:
    TrackingCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	
	if (tokens.size() != 1)
	{
	    cerr << "Tracking command needs one argument: 'on' or 'off' " << endl;
	    return;
	}
	
	if (tokens[0] == "on")
	{
	    tscope_.getSseProxy().trackingOn();
	}
	else if (tokens[0] == "off")
	{
	    tscope_.getSseProxy().trackingOff();
	}
	else 
	{
	    cerr << "tracking arg must be 'on' or 'off'" << endl;
	}
	
    }
    
private:
  Tscope& tscope_;

};

class UnSimulateCmd : public CmdPattern
{
 public:
    UnSimulateCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs) 
    {
	tscope_.setSimulated(false);
    }

private:
    Tscope& tscope_;
    
};


class TestParseCmd : public CmdPattern
{
public:
    TestParseCmd(Tscope& tscope) : tscope_(tscope) {};
    
    virtual void execute(const string & cmdArgs)
    {
	tscope_.parseStatus(cmdArgs);
    }
    
private:
  Tscope& tscope_;

};



class VerboseCmd : public CmdPattern
{
public:
  VerboseCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	if (tokens.size() < 1) 
	{
	    cerr << "Verbose command needs one argument <0,1,2> " << endl;
	    return;
	}
	
	// TBD check for number.  Use SseUtil::strToInt.

	tscope_.setVerboseLevel(atoi(tokens[0].c_str()));
    }

private:
  Tscope& tscope_;

};

class WrapCmd : public CmdPattern
{
 public:
  WrapCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	
	enum indices { subarrayIndex, wrapIndex, expectedNumArgs };
	if (tokens.size() != expectedNumArgs) 
	{
	    cerr << "wrap command needs "
		 <<  expectedNumArgs << " arg(s): " 
		 << "antxx[,antxx..] <wrap number> " << endl;
	    return;
	}

	try {
           if (!validAtaSubarray(tokens[subarrayIndex]))
           {
              return;
           }
	   TscopeWrapRequest wrapReq;
           SseUtil::strMaxCpy(wrapReq.subarray, 
                              tokens[subarrayIndex].c_str(),
                              MAX_TEXT_STRING);

	   wrapReq.wrapNumber = SseUtil::strToInt(tokens[wrapIndex]);
           tscope_.wrap(wrapReq);
	}
	catch (SseException &except)
	{
	    cerr << "invalid wrap: " << except << endl;
	}
  }

private:
  Tscope& tscope_;


};

class ServerNameCmd : public CmdPattern
{
public:
  ServerNameCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	if (tokens.size() < 1) 
	{
	    cerr << "Server command needs one argument: name of antenna server" << endl;
	    return;
	}
	
	tscope_.setAntControlServerName(tokens[0]);
    }

private:
  Tscope& tscope_;

};


class MonitorCmd : public CmdPattern
{
 public:
  MonitorCmd(Tscope& tscope) : tscope_(tscope) {};

    virtual void execute(const string & cmdArgs)
    {
	vector <string> tokens = SseUtil::tokenize(cmdArgs, " \t\n");
	if (tokens.size() != 1)
	{
	    cerr << "Monitor command takes one argument: "
		 << "<period in secs> " << endl;
	    return;
	}

	try {
	    int periodSecs = SseUtil::strToInt(tokens[0]);
	    TscopeMonitorRequest request;
	    request.periodSecs = periodSecs;
	    tscope_.monitor(request);
	}
	catch (...)
	{
	    cerr << "invalid period: " << tokens[0] << endl;
	}
	
  }

private:
  Tscope& tscope_;


};


void addUICmds(TextUI &ui, Tscope& tscope)
{

  // add some user commands
  // who deletes the memory for these? tbd.

  AllocateCmd* allocateCmd = new AllocateCmd(tscope);
  ui.addUICmd("allocate", allocateCmd, "- allocate telescope");

  AssignSubarrayCmd* assignSubCmd = new AssignSubarrayCmd(tscope);
  ui.addUICmd("assign", assignSubCmd, 
              "beamxxx antxx[,antxx...] - assign subarray to a beam");

/*
TBD add this:
  ListSubarrayCmd* listSubCmd = new ListSubarrayCmd(tscope);
  ui.addUICmd("list", listSubCmd, 
              "beamxxx - list the antennas that make up a beam");
*/

  CalCmd* calCmd = new CalCmd(tscope);
  ui.addUICmd("cal", calCmd, "antxx[,antxx...] <on|off> - turn on/off calibration source for subarray");

  ConnectCmd* connectCmd = new ConnectCmd(tscope);
  ui.addUICmd("connect", connectCmd, "- connect to telescope server" );

  DeallocateCmd* deallocateCmd = new DeallocateCmd(tscope);
  ui.addUICmd("deallocate", deallocateCmd, "- deallocate telescope");

  DisconnectCmd* disconnectCmd = new DisconnectCmd(tscope);
  ui.addUICmd("disconnect", disconnectCmd, " - disconnect from telescope");

  ExitCmd* exitCmd = new ExitCmd();
  ui.addUICmd("exit", exitCmd, "- exit from program");

  IntrinsicsCmd* intrinsicsCmd = new IntrinsicsCmd(tscope);
  ui.addUICmd("intrin", intrinsicsCmd, "- show telescope intrinsics");

  MonitorCmd* monitorCmd = new MonitorCmd(tscope);
  ui.addUICmd("monitor", monitorCmd, "<period secs> - turn on periodic status monitor");

  NameCmd* nameCmd = new NameCmd(tscope);
  ui.addUICmd("name", nameCmd, "<name> - set name of this server");

  PointCmd* pointCmd = new PointCmd(tscope);
  stringstream pointHelp;
  pointHelp << "<beam beamxxx>|subarray antxx[,antxx...]> "
            << "<coordsys> <coord1> <coord2>\n"
	    << "\t  where beamName = 'beam{x|y}{a-d}{1-4}'\n"
            << "\t  and <coordsys> <coord1> <coord2> is one of:\n"
	    << "\t    azel <azDeg> <elDeg>\n"
	    << "\t    j2000 <raHours> <decDeg>\n"
	    << "\t    gal <longDeg> <latDeg>";
  ui.addUICmd("point", pointCmd, pointHelp.str());

  QuitCmd* quitCmd = new QuitCmd();
  ui.addUICmd("quit", quitCmd, "- quit this program");

  ResetCmd* resetCmd = new ResetCmd(tscope);
  ui.addUICmd("reset", resetCmd, "- reset telescope");

  SendErrorCmd* sendErrorCmd = new SendErrorCmd(tscope);
  ui.addUICmd("senderror", sendErrorCmd, "<text> - send error message to SSE");

  ServerNameCmd* serverNameCmd = new ServerNameCmd(tscope);
  ui.addUICmd("server", serverNameCmd, "<server name> - set name of telescope server host");

  SimulateCmd* simulateCmd = new SimulateCmd(tscope);
  ui.addUICmd("sim", simulateCmd, "- turn on simulator mode");
  
  StatusCmd* statusCmd = new StatusCmd(tscope);
  ui.addUICmd("status", statusCmd, "- get status");

  StopCmd* stopCmd = new StopCmd(tscope);
  ui.addUICmd("stop", stopCmd, "antxx[,antxx...] - stop array motion");

  StowCmd* stowCmd = new StowCmd(tscope);
  ui.addUICmd("stow", stowCmd, "antxx[,antxx...] - stow array");

  TrackingCmd* trackingCmd = new TrackingCmd(tscope);
  ui.addUICmd("track", trackingCmd, " <'on'|'off'> - send specified tracking status to SSE");

  TuneCmd* tuneCmd = new TuneCmd(tscope);
  ui.addUICmd("tune", tuneCmd, "<tuning{a-d}> <skyfreq in MHz> - tune receiver");

  UnSimulateCmd* unsimulateCmd = new UnSimulateCmd(tscope);
  ui.addUICmd("unsim", unsimulateCmd, "- turn off simulator mode");

  VerboseCmd* verboseCmd = new VerboseCmd(tscope);
  ui.addUICmd("verbose", verboseCmd, " <verbose level = 0,1,2> - set verbose level");

  WrapCmd* wrapCmd = new WrapCmd(tscope);
  ui.addUICmd("wrap", wrapCmd, "antxx[,antxx...] <wrap number=-1,0,1,2> - send wrap number to telescope");

  TestParseCmd* testParseCmd = new TestParseCmd(tscope);
  ui.addUICmd("testparse", testParseCmd, "<status string> - test parsing");

  ZfocusCmd* zfocusCmd = new ZfocusCmd(tscope);
  ui.addUICmd("zfocus", zfocusCmd, "antxx[,antxx...] <skyfreq in MHz> - set antenna zfocus for subarray");

}

