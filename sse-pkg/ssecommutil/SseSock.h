/*******************************************************************************

 File:    SseSock.h
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


#ifndef _ssesock_h
#define _ssesock_h

#include "ace/SOCK_Connector.h" 
#include "ace/INET_Addr.h" 

class SseSock { 

  ACE_SOCK_Stream ssesock_stream_; 
  ACE_INET_Addr remote_addr_; 
  ACE_SOCK_Connector connector_;

  // Disable copy construction & assignment.
  // Don't define these.
  SseSock(const SseSock& rhs);
  SseSock& operator=(const SseSock& rhs);

 public: 
  SseSock(int port, const char *hostname);
  SseSock();
  void set(int port);
  void set(int port, const char *hostname);
  int connect_to_server(); 
  int close(); 
  int sendMsg(void *msg, int len);
  ACE_SOCK_Stream & sockstream() { return ssesock_stream_;}

}; 



#endif