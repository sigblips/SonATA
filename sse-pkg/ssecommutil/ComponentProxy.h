/*******************************************************************************

 File:    ComponentProxy.h
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


#ifndef _component_proxy_h
#define _component_proxy_h

#include <ace/OS.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <string>

using std::string;

class ComponentProxyInternal;

class ComponentProxy : public ACE_Event_Handler
{

 public:

  ComponentProxy(); // expects to be connected via an ACE Acceptor
  ComponentProxy(ACE_SOCK_STREAM& aceSockStream); // connect via stream
  virtual ~ComponentProxy();

  // These are connection status routines that only the
  // ACE connection manager should call:

  virtual void setInputConnected();
  virtual void setInputDisconnected();

  // subclasses are meant to override these as desired
  virtual void requestIntrinsics();
  virtual void requestStatusUpdate();
  virtual void shutdown();

  // utility methods
  virtual bool validInterfaceVersion();
  virtual void resetSocket();
  virtual bool isAlive();   // indicates live socket connection
  virtual void setVerboseLevel(int level);
  virtual int getVerboseLevel();
  virtual string getName();  // component name
  virtual string getModuleName();
  virtual void setModuleName(string mName);
  virtual const string& getRemoteHostname();
  virtual void endAceEventLoopOnClose(bool flag);

  // for the ACE_Event_Handler

  virtual int handle_input(ACE_HANDLE fd) = 0; // subclass to define
  virtual int handle_close(ACE_HANDLE fd, ACE_Reactor_Mask close_mask);
  virtual int handle_exception(ACE_HANDLE fd);

  // TBD handle_timeout(), handle_exit()

  virtual ACE_HANDLE get_handle() const;
  virtual ACE_SOCK_Stream& getAceSockStream();

 protected:

  virtual bool isConnectionOpen();   // indicates live socket connection

  virtual void notifyInputConnected();
  virtual void notifyInputDisconnected();
  virtual void logError(const string& errorMsg);

  virtual string expectedInterfaceVersion();
  virtual string receivedInterfaceVersion();

 private:

  // Disable copy construction & assignment.
  // Don't define these.
  ComponentProxy(const ComponentProxy& rhs);
  ComponentProxy& operator=(const ComponentProxy& rhs);

  ComponentProxyInternal* cpInternal_;

  friend class ComponentProxyInternal;
};

#endif