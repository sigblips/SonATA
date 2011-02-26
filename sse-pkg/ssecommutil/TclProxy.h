/*******************************************************************************

 File:    TclProxy.h
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


#ifndef _tcl_proxy_h
#define _tcl_proxy_h

#include <ace/SOCK_Acceptor.h>
#include "ComponentProxy.h"
#include <string>

using std::string;

class TclProxyInternal;

class TclProxy : public ComponentProxy
{

 public:

  TclProxy();
  TclProxy(ACE_SOCK_STREAM& aceSockStream);
  virtual ~TclProxy();
  virtual int send(const string & message);

  // for the ACE_Event_Handler
  virtual int  handle_input(ACE_HANDLE fd);

  //Note: delimit default includes \r which is apparently being
  //appended by the tcl interpreter to all incoming messages

  string parseElement(const string& message, const string& key,
		      const string& delimit = "\r\n");
  double parseDouble(const string& message, const string& key,
		     const string& delimit = "\r\n");
  int    parseInt(const string& message, const string& key,
		  const string& delimit = "\r\n");

 protected:

  // subclasses need to define this:
  virtual void handleIncomingMessage(const string &message) = 0;

  virtual int receive(string& message);

 private:

  // Disable copy construction & assignment.
  // Don't define these.
  TclProxy(const TclProxy& rhs);
  TclProxy& operator=(const TclProxy& rhs);

  TclProxyInternal* internal_;
  friend class TclProxyInternal;

};

#endif