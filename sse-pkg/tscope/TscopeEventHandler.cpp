/*******************************************************************************

 File:    TscopeEventHandler.cpp
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



#include "TscopeEventHandler.h" 
#include "SseProxy.h"
#include "Verbose.h"
#include "Tscope.h"
#include "CmdPattern.h"
#include <sstream>

using namespace std;

TscopeEventHandler::TscopeEventHandler(
    const string &name, SseProxy& proxy, CmdPattern & msgCallback,
    const string &serverName, int port)
    : name_(name), sseProxy_(proxy), msgCallback_(msgCallback), 
    port_(port), serverName_(serverName),
    verboseLevel_(0)
{}

TscopeEventHandler::~TscopeEventHandler()
{}

// return true on success
bool TscopeEventHandler::sendCommand(const string &cmd)
{
    if (sockStream_.send_n(cmd.c_str(), cmd.length()) == -1)
    {
	// error in sending
	return false;
    }
    
    return true;
}

void TscopeEventHandler::setPort(int port)
{
    port_ = port;
}

int TscopeEventHandler::getPort() const
{
    return(port_);
}


void TscopeEventHandler::setServerName(const string &name)
{
    serverName_ = name;
}

string TscopeEventHandler::getServerName()
{
    return serverName_;
}

string TscopeEventHandler::getName() const 
{
    return name_;
}


SseProxy & TscopeEventHandler::getSseProxy() 
{
    return sseProxy_;
}

void TscopeEventHandler::setVerboseLevel(int level)
{
    verboseLevel_ = level;
}

int TscopeEventHandler::getVerboseLevel()
{
    return verboseLevel_;
}

bool TscopeEventHandler::connect(const string& serverName) 
{
    setServerName(serverName);
    return(connect());
}

bool TscopeEventHandler::connect(const string& serverName, int port)
{
    setServerName(serverName); 
    setPort(port);
    return(connect());
}

bool TscopeEventHandler::connect()
{
    string portInfo("telescope's " + name_ + " port\n");
    string errorHeader("*** Failed to connect to " + portInfo);

    //reportInfo("attempting to connect to " + portInfo);

    if (sockConnector_.connect(
	sockStream_, ACE_INET_Addr(getPort(), getServerName().c_str())) == -1)
    {
	reportError(errorHeader);
	return(false);
    }

    if (ACE_Reactor::instance()->register_handler(
	sockStream_.get_handle(), this,
	ACE_Event_Handler::READ_MASK))
    {
	reportError(errorHeader + ", stream reactor register failure.\n");
	return(false);
    }

    VERBOSE1(getVerboseLevel(), 
             "TScope_Event_Handler::connect complete for "
             << portInfo << "\n");
    
    reportInfo("connected to " + portInfo);
    
    return(true);
}

// Read text from input fd.  The text is appended to the end of
// the 'text' argument
void TscopeEventHandler::getInputText(ACE_HANDLE fd, string & text)
{
    ssize_t maxBytesToRead = RECEIVE_BUFFER_SIZE - 1;  // save room for trailing null
    ssize_t readStatus = ACE_OS::read(fd, receiveBuffer_, maxBytesToRead);
    
    if (readStatus == 0 || readStatus == -1)
    {
	reactor()->remove_handler(fd, ACE_Event_Handler::READ_MASK);
	VERBOSE1(getVerboseLevel(),
                 "TscopeEventHandler::handle_input() EOF.\n");

        // clean up to allow reconnection
        disconnect();

	text = "";

	stringstream errorStrm;
	errorStrm << "telescope's " << name_ << " port connection closed by "
		  << "antenna server (EOF on input) " << endl;
	reportError(errorStrm.str());

	return;
    }
    
    // null terminate the input
    int nBytesRead = readStatus;
    receiveBuffer_[nBytesRead] = '\0';
    
    text += receiveBuffer_;

    VERBOSE3(getVerboseLevel(), "TScope_Event_Handler::handle_input("
             << getName() << "): accumulated text\n" 
             << "[" << text << "]" << endl;);


}

// Read input text.  Each logical line of text is terminated
// by a newline.  All full messages are sent on for further processing,
// partial lines are remembered and processed as part of the
// next read.

int TscopeEventHandler::handle_input(ACE_HANDLE fd)
{
  getInputText(fd, inputText_);

  // A newline is the expected terminator for each logical input line.
  // If a partial line is read (ie, text trailing the last terminator)
  // then save it for later parsing.
  
  string terminator("\n");
  string::size_type lastTerminatorStartPos = 
      inputText_.rfind(terminator);
  if (lastTerminatorStartPos == string::npos)
  {
      // no terminator found at all, so this is an
      // incomplete message. Wait for the rest of it.
      
      return 0;   // stay registered with reactor
  }

  // There is at least one terminator.  Grab everything up to and
  // including the last terminator as the 'completeMessages',
  // and save anything after the last terminator for parsing next time
  // as the 'remainderText'.

  string::size_type lastTerminatorEndPos = lastTerminatorStartPos 
      + terminator.length();
  string::size_type messageLen = lastTerminatorEndPos;

  string completeMessages(inputText_.substr(0,messageLen)); 

  // get the remainder
  string remainderText("");
  messageLen = inputText_.length() - lastTerminatorEndPos;
  if (messageLen > 0)
  {
      remainderText = (inputText_.substr(lastTerminatorEndPos, messageLen));
      
      // cout << "remainder text: " << "{" <<  remainderText << "}" << endl;
  }

  msgCallback_.execute(completeMessages);

  inputText_ = remainderText;

  return 0;   // stay registered with ACE reactor

}

void TscopeEventHandler::disconnect() 
{
    VERBOSE2(getVerboseLevel(), "TscopeEventHandler::starting disconnect\n");

    string portInfo("telescope's " + name_ + " port");
    string errorHeader("*** Failed to disconnect from " + portInfo);

    bool success(true);
    bool sendErrorReport(false);
    
    // unregister handler 
    if (ACE_Reactor::instance()->remove_handler(sockStream_.get_handle(),
						ACE_Event_Handler::READ_MASK) == -1)
    {
       success = false;
       if (sendErrorReport) 
       {
	  reportError(errorHeader + ", reactor stream handler remove failure.\n" );
       }
    }

    if (sockStream_.close())
    {
       success = false;
       if (sendErrorReport)
       {
	  reportError(errorHeader + ", stream close failure.\n");
       }
    }


    VERBOSE1(getVerboseLevel(), "TscopeEventHandler::disconnect complete.\n");

    if (success) 
    {
       reportInfo("disconnected from " + portInfo);
    }
}

void TscopeEventHandler::reportError(const string & msg)
{
      cerr << msg << endl;
      getSseProxy().sendErrorMsgToSse(SEVERITY_ERROR, msg);
}

void TscopeEventHandler::reportInfo(const string & msg)
{
      cerr << msg << endl;
      getSseProxy().sendMsgToSse(SEVERITY_INFO, msg);
}


