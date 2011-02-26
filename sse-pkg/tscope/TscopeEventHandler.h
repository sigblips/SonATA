/*******************************************************************************

 File:    TscopeEventHandler.h
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


#ifndef TscopeEventHandler_H
#define TscopeEventHandler_H

#include <ace/Event_Handler.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <string>

class SseProxy;
class CmdPattern;

using std::string;

class TscopeEventHandler : public ACE_Event_Handler
{

 public:

  TscopeEventHandler(const string & name, SseProxy& proxy,
			    CmdPattern & msgCallback,
			    const string &serverName,
			    int port);
  virtual ~TscopeEventHandler();
  virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);
  virtual bool sendCommand(const string &cmd);
  virtual void setPort(int port);
  virtual int getPort() const;
  virtual void setServerName(const string &serverName);
  string getServerName();
  string getName() const;
  SseProxy & getSseProxy();
  virtual bool connect();
  virtual bool connect(const string& host);
  virtual bool connect(const string& host, int port);
  virtual void disconnect();

  int getVerboseLevel();
  void setVerboseLevel(int level);

 protected:

  void reportError(const string & msg);
  void reportInfo(const string & msg);


 private:

  void getInputText(ACE_HANDLE fd, string &text);

  ACE_SOCK_Connector sockConnector_;
  ACE_SOCK_Stream    sockStream_;
  string name_;
  SseProxy & sseProxy_;
  CmdPattern & msgCallback_;
  int port_;
  string serverName_;
  string inputText_;
  int verboseLevel_;

  static const int RECEIVE_BUFFER_SIZE = 256;
  char receiveBuffer_[RECEIVE_BUFFER_SIZE];

  // Disable copy construction & assignment.
  // Don't define these.
  TscopeEventHandler(const TscopeEventHandler& rhs);
  TscopeEventHandler& operator=(const TscopeEventHandler& rhs);
  
};

#endif 