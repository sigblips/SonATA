/*******************************************************************************

 File:    SseSock.cpp
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


#include "SseSock.h"
#include "ace/Log_Priority.h"
#include "ace/Log_Msg.h"
#include <iostream>

using namespace std;

SseSock::SseSock(int port, const char *hostname)
    :remote_addr_(port,hostname)
{}

SseSock::SseSock() {}

void SseSock::set(int port, const char *hostname)
{
    remote_addr_.set(port, hostname);
}

void SseSock::set(int port)
{
    remote_addr_.set(port);
}

//Uses a connector component connector_ to connect to a remote machine 
//and pass the connection into a stream component client_stream_ 
int SseSock::connect_to_server()
{

  // Initiate blocking connection with server. 
//  ACE_DEBUG((LM_DEBUG, "(%P|%t) Starting connect to %s:%d\n",
    //    remote_addr_.get_host_name(), remote_addr_.get_port_number()));
  if (connector_.connect(ssesock_stream_, remote_addr_) == -1)
  {
      return -1;
      //    ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "connection failed"), -1);
  }
  else
  {
//    ACE_DEBUG((LM_DEBUG, "(%P|%t) connected to %s\n",
//        remote_addr_.get_host_name()));
  }

  return 0;
}

int SseSock::sendMsg(void *msg, int len)
{
  if (ssesock_stream_.send_n(msg, len, 0) == -1)
  {
      //ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "send_n"), 0);
    return -1;
  }
  return 0;
}

//Close down the connection properly. 
int SseSock::close() 
{
  if (ssesock_stream_.close() == -1)
  {
      // ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "close"), -1);
      return -1;
  }
  else
  {
    return 0;
  }
}