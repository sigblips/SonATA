/*******************************************************************************

 File:    SseDatagramHandler.cpp
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


// Sends 'I am Here'dgrams to multicast address.
// Receives directly sent 'There you are' datagrams.

#include "SseDatagramHandler.h"
#include "sseDxInterface.h"
#include "SseUtil.h"

using namespace std;

SseDatagramHandler::SseDatagramHandler (const char *multicastAddr,
					u_short multicastPort,
					ACE_Reactor &reactor):
  dgram_(dgramReceiveAddr_,
	 PF_INET,  // protocol family
	 0,   // protocol
	 1),  // reuse address
  thereYouAreReceived_(false),
  ssePort_(-1),
  multicastAddr_(multicastAddr),
  multicastPort_(multicastPort)
{
  // Register handler to listen for direct dgrams
  if (reactor.register_handler (dgram_.get_handle (),
                                     this,
                                     ACE_Event_Handler::READ_MASK) == -1)
  {
//    ACE_ERROR ((LM_ERROR, 
      //               "%p\n",
      //         "can't register with Reactor\n"));

      cerr << "SseDatagramHandler can't register with Reactor" << endl;
  }


  // Check the current ttl (time-to-live) setting.
  // This affects how far the packet travels.
  // get_option method is a wrapper around the socket library
  // getsockopt() function.

  // TTL values:
  // 0 = local host
  // 1 = local subnet
  // ...
  // 255 = worldwide
  // default is 1
  
  u_char ttl = 0;

#if 0
 int ns =sizeof(ttl);
 if (dgram_.get_option(IPPROTO_IP, IP_MULTICAST_TTL,&ttl,&ns) == -1)
 {
        ACE_ERROR((LM_ERROR,"get_opt failed : %p\n"));
 }
 else
 {
        printf("get_option: ttl: %d sizeof %d\n",(int)ttl,ns);
 }

#endif
 
 // Set TTL to something that will broadcast just far enough
 // to reach the server
 ttl =16;
 if (dgram_.set_option(IPPROTO_IP, IP_MULTICAST_TTL,&ttl,sizeof(ttl)) == -1)
 {
//     ACE_ERROR((LM_ERROR,"set_opt failed : %p\n"));
     cerr << "set_opt failed" << endl;
 }

#if 0
 // read it back to verify
 if (dgram_.get_option(IPPROTO_IP, IP_MULTICAST_TTL,&ttl,&ns) == -1)
 {
        ACE_ERROR((LM_ERROR,"get_opt failed : %p\n"));
 }
 else
      printf("get_option: ttl: %d sizeof %d\n",(int)ttl,ns);
#endif


}

SseDatagramHandler::~SseDatagramHandler (void)
{

}

ACE_HANDLE SseDatagramHandler::get_handle (void) const
{
  return this->dgram_.get_handle ();
}

int
SseDatagramHandler::handle_input (ACE_HANDLE h)
{

    ACE_INET_Addr remote_addr;

    // Receive direct 'ThereYouAre' dgram message from SSE
    ThereYouAre thereYouAre;

    ssize_t result = this->dgram_.recv (&thereYouAre,
					sizeof thereYouAre,
					remote_addr);
    if (result != -1)
    {
#if 0
	ACE_DEBUG ((LM_DEBUG,
		    "received datagram from host %s on port %d bytes = %d\n",
		    remote_addr.get_host_name (),
		    remote_addr.get_port_number (),
		    result));
#endif

	thereYouAre.demarshall();
	cout << thereYouAre;

	sseIpAddrString_ = thereYouAre.sseIp;
	ssePort_ = thereYouAre.portId;
	thereYouAreReceived_ = true;

	// check to make sure the interface versions
	// are compatible.

	if (strcmp(thereYouAre.interfaceVersionNumber,
		   SSE_DX_INTERFACE_VERSION) != 0)
	{
	    cerr << "Warning: SSE-DX Interface mismatch!" << endl;
	    cerr << "SSE version: " << thereYouAre.interfaceVersionNumber;
	    cerr << endl;
	    cerr << "Dx version: " << SSE_DX_INTERFACE_VERSION;
	    cerr << endl;

	    // TBD error handling
	}


         //	return 0;
	return -1;  // unregister this dgram event handler
    }

#if 0
    ACE_ERROR_RETURN ((LM_ERROR,
		       "%p\n",
		       "something amiss"),
		      -1);
#endif

    return -1;
}

int
SseDatagramHandler::handle_close (ACE_HANDLE h, ACE_Reactor_Mask)
{
//    ACE_DEBUG ((LM_DEBUG,
    //               "dcast_Events handle removed from reactor.\n"));

  return 0;
}

// Timer that periodically sends "i am here" dgram.
// Once the corresponding 'there you are' dgram has been received,
// this timer unregisters itself, and on cleanup ends
// the Reactor event loop.
//
class HereIAmTimerHandler : public ACE_Event_Handler 
{ 
public: 

    //Method which is called back by the Reactor when timeout occurs. 
    virtual int handle_timeout (const ACE_Time_Value &tv, 
                                const void *arg)
    {
	// ACE_DEBUG ((LM_DEBUG, "hereiam timer: %d\n", tv.sec())); 

       SseDatagramHandler *dgramHandler = const_cast<SseDatagramHandler *>(
	   static_cast<const SseDatagramHandler *>(arg));


       if (dgramHandler->thereYouAreReceived_)
       {

	   // we're connected, so disable this timer
	   return -1;  // unregister
       }
       else
       {
	   // send datagram message to multicast port
	   cout << "sending Here I Am..." << endl;

	   ACE_INET_Addr remote_addr(dgramHandler->multicastPort_,
				     dgramHandler->multicastAddr_);

	   HereIAm hereIAm; 	   // sseDxInterface structure
	   SseUtil::strMaxCpy(hereIAm.interfaceVersionNumber, 
			      SSE_DX_INTERFACE_VERSION, MAX_TEXT_STRING);
	   hereIAm.marshall();

	   int len = sizeof(HereIAm);
	   if (dgramHandler->dgram_.send (&hereIAm, len, remote_addr) != len)
	   {
#if 0
	       ACE_ERROR_RETURN ((LM_ERROR,
				  "%p\n",
				  "send error"),
				 -1);
#endif
	       return -1;  // unregister
	   }

	   return 0;
	   //return -1;  // unregister

       }

    }


    int handle_close (ACE_HANDLE h, ACE_Reactor_Mask)
    {
#if 0
        ACE_DEBUG ((LM_DEBUG,
                "hereIAmTimer Handler removed from reactor.\n"));
#endif

	// once this timer exits we know we're done with the Reactor
	ACE_Reactor::end_event_loop();

        return 0;
    }


};


int SseDatagramHandler::getSsePort()
{
    return ssePort_;
}

string SseDatagramHandler::getSseIpAddrString()
{
    return sseIpAddrString_;
}



// Periodically broadcast 'hereIam' messages on the multicast port.
// Once a 'ThereYouAre' datagram is received in reply,
// return the ssePort & sseIpAddress.

void SseDatagramHandler::contact(
    u_short *ssePort,             // returned port
    string *sseIpAddrString)      // returned address
{

  // register 'here i am' periodic timer
  int startDelay = 1;       // initial delay 
  int repeatInterval = 3;   // wake up every N seconds
  HereIAmTimerHandler *th=new HereIAmTimerHandler; 
  // int timer_id = 
       ACE_Reactor::instance()->schedule_timer (th, 
	  this, // timer arg
	  ACE_Time_Value (startDelay), // start time
          ACE_Time_Value (repeatInterval)  // repeat interval
      );

  // tbd error check on schedule_timer


  // keep looping til we get a datagram
  ACE_Reactor::run_event_loop ();

#if 0
  // once we reach here, we've received a datagram
  ACE_DEBUG ((LM_DEBUG,
              "SseDatagramHandler Done.\n"));
#endif

  // clean up Reactor so event loop can be run again later
  ACE_Reactor::reset_event_loop();

  delete th;  // timer handler


  // return the values we received
  *ssePort = getSsePort();
  *sseIpAddrString = getSseIpAddrString();

}

