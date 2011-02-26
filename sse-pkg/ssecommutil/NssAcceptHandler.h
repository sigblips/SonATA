/*******************************************************************************

 File:    NssAcceptHandler.h
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


// Generic code for creating an NssAcceptHandler for
// a proxy (component) type.  It tries to open the
// socket connection for the acceptor using the given
// port number, and if that succeeds it hands the
// acceptor off to the ace reactor so that it can begin
// to accept connections for that component type.
 
#ifndef nss_accept_handler_h
#define nss_accept_handler_h

#include "ace/SOCK_Acceptor.h"
#include "ace/Reactor.h"
#include "NssComponentManager.h"
#include "SseException.h"
#include <typeinfo>
#include <sstream>

using std::cerr;
using std::endl;

class NssAcceptHandlerException : public SseException
{
 public:
   NssAcceptHandlerException(const string& what):
      SseException(what)  { }
};


template<class T_NssProxy> class NssAcceptHandler: public ACE_Event_Handler
{
 public:

   NssAcceptHandler(const string &port,
                    NssComponentManager<T_NssProxy>* componentMgr) :
      component_manager_(componentMgr)
   {
      open(port);
      registerWithReactor();
   }

   ~NssAcceptHandler()
   {
      unregister();
   }

   int handle_input(ACE_HANDLE handle) 
   {
      // An SSE server program just connected on a socket.
      // Create a T_NssProxy (which includes an NssInputHandler).
      // Register the InputHandler with the ACE event handler. 
      
      T_NssProxy* proxy = new T_NssProxy(component_manager_);

      //Accept the connection "into" the Event Handler
      if (this->peer_acceptor_.accept (
             proxy->getAceSockStream(),
             0, // remote address
             0, // timeout
             1) ==-1) //restart if interrupted

         //  ACE_DEBUG((LM_ERROR,"Error in connection\n"));
         cerr << "NssAcceptHandler: Error in connection" << endl;

      //ACE_DEBUG((LM_DEBUG,"Connection established\n"));
  
      //Register the input event handler for reading
      ACE_Reactor::instance()->register_handler(
         proxy, 
         ACE_Event_Handler::READ_MASK | ACE_Event_Handler::EXCEPT_MASK);

      // Notify the proxy that it's connected to its stream input.
      // Note that this is done *after* the input handler
      // has been registered with the Ace reactor so
      // that the proxy can assume the socket connect is 
      // ready to go.

      proxy->setInputConnected();

      // allow more clients
      return 0;
   };

   ACE_HANDLE get_handle(void) const
   {
      return this->peer_acceptor_.get_handle();
   };


 protected:
   void open(const string &port)
   {
      // Create an address on which to receive connections
      ACE_INET_Addr addr(port.c_str());
      
      int status = peer_acceptor_.open(addr, 1);  // enable reuse_addr 
         
      // REUSE_ADDR prevents socket TIME_WAIT problem.
      // i.e., allows immediate reconnect on the same port
      // by server & client rather than having to sit 
      // through the TCP TIME_WAIT timeout period.
      // Ref: Unix Network Programming by Stevens (2nd ed, sect 7.5).
      
      if (status != 0)
      {
         string proxyTypename = typeid(T_NssProxy).name();
         
         stringstream strm;
         strm << "NssAcceptHandler::open failure for "
              << proxyTypename << "."
              << " (port '" << port << "' is already in use or is invalid)."
              << endl;
         
         throw NssAcceptHandlerException(strm.str());
      }
      
   };

   void registerWithReactor() 
   {
      //Register the reactor to call back when incoming client connects
      int result =  ACE_Reactor::instance()->register_handler(
         this, ACE_Event_Handler::ACCEPT_MASK);

      if (result == -1)
      {
         string proxyTypename = typeid(T_NssProxy).name();
         
         throw NssAcceptHandlerException(
            proxyTypename + 
            " accept handler failed to register");
      }
      
   }
   
   void unregister() 
   {
      //unregister with the reactor 
      int result =  ACE_Reactor::instance()->remove_handler(
         this, ACE_Event_Handler::ACCEPT_MASK);
      
      if (result == -1) 
      {
         // TBD error handling.
         // This could fail if the ACE reactor has
         // already been shutdown, which is fine.
      }
   }

 private:

   ACE_SOCK_Acceptor peer_acceptor_;
   NssComponentManager<T_NssProxy>* component_manager_;

   // Disable copy construction & assignment.
   // Don't define these.
   NssAcceptHandler(const NssAcceptHandler& rhs);
   NssAcceptHandler& operator=(const NssAcceptHandler& rhs);
};

#endif