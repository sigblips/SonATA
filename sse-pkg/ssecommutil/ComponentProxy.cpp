/*******************************************************************************

 File:    ComponentProxy.cpp
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


#include "ComponentProxy.h"
#include "Assert.h"
#include "Verbose.h"
#include <iostream>
#include <sstream>

using namespace std;

struct ComponentProxyInternal
{
    ComponentProxyInternal() 
	: verboseLevel_(0),
	  endAceEventLoopOnClose_(false),
	  isAlive_(false),
	  remoteHostName_("unknown"),
	  name_("unknown"),
	  moduleName_("NssProxy"),
	  alreadyClosed_(false)
    {};

    int verboseLevel_;
    bool endAceEventLoopOnClose_;
    bool isAlive_;  // indicate proxy is alive & connected
    string remoteHostName_;
    string name_;
    string moduleName_;
    bool alreadyClosed_;
    ACE_Recursive_Thread_Mutex aceMutex_;
    ACE_Thread_Mutex verboseMutex_;
    ACE_SOCK_Stream aceSockStream_;
};

ComponentProxy::ComponentProxy()
    : cpInternal_(new ComponentProxyInternal()) 
{};

ComponentProxy::ComponentProxy(ACE_SOCK_STREAM& aceSockStream)
    : cpInternal_(new ComponentProxyInternal())
{
    cpInternal_->aceSockStream_ = aceSockStream;
};

ComponentProxy::~ComponentProxy()
{ 
    delete cpInternal_;
}

int ComponentProxy::handle_close(ACE_HANDLE fd, ACE_Reactor_Mask close_mask)
{

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    // only close once
    if (! cpInternal_->alreadyClosed_)
    {
	cpInternal_->alreadyClosed_ = true;

	//VERBOSE2(getVerboseLevel(), "handle_close " << getName() 
	//	 << " mask=" << close_mask << endl);

	ACE_OS::close(fd);

	setInputDisconnected();


	/* Normally a handle_close would indicate that
	   this handler is being removed from the reactor,
	   but apparently this may not always be the case,
	   so try to remove it again.
	   Use the DONT_CALL flag to prevent
	   handle_close() from being called again,
	   to prevent recursion */

	//int status = 
        ACE_Reactor::instance()->remove_handler (
	    this->get_handle(),
	    ACE_Event_Handler::READ_MASK 
	    | ACE_Event_Handler::EXCEPT_MASK
	    | ACE_Event_Handler::DONT_CALL);

	//VERBOSE2(getVerboseLevel(), "handle_close " << getName() 
	//	 << " remove_handler status=" << status << endl);

	// TBD check for bad status from remove_handler


	// Need to explicitly close the stream to
	// make the socket connection break.
	// This must be done *after* the remove_handler call above
	// because the aceSockStream is the value returned by
	// the get_handle() method used as an argument to
	// that call.

	getAceSockStream().close();


	if (cpInternal_->endAceEventLoopOnClose_) 
	{
	    ACE_Reactor::end_event_loop();
	}
    }

    return 0;
}

ACE_HANDLE ComponentProxy::get_handle() const
{
    // TBD this will change the state of aceMutex_, not quite const
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    return(cpInternal_->aceSockStream_.get_handle());
}

ACE_SOCK_Stream& ComponentProxy::getAceSockStream()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);
    return(cpInternal_->aceSockStream_);
}

void ComponentProxy::setInputConnected()
{

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    cpInternal_->isAlive_ = true;

    ACE_INET_Addr remote_addr;
    if (getAceSockStream().get_remote_addr(remote_addr) == -1) 
    {
	//cerr << "ComponentProxy::get_remote_addr() ACE Error" << endl;
    }
    else
    {
	cpInternal_->remoteHostName_ = remote_addr.get_host_name();
    }

    notifyInputConnected();  // hook for sub class
}

bool ComponentProxy::isConnectionOpen()
{

    // check to see if the other end of the socket can be accessed
    ACE_INET_Addr remote_addr;
    if (getAceSockStream().get_remote_addr(remote_addr) == 0)
    {
	return true;
    }

    return false;
}

void ComponentProxy::setInputDisconnected()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    cpInternal_->isAlive_ = false;
    notifyInputDisconnected();  // hook for sub class
}

void ComponentProxy::notifyInputConnected()
{
    // default is: do nothing
}

void ComponentProxy::notifyInputDisconnected()
{
    // default is: do nothing
}

void ComponentProxy::requestIntrinsics()
{
    // no op, for sub class to override.
}

void ComponentProxy::requestStatusUpdate()
{
    // no op, for sub class to override.
}

bool ComponentProxy::isAlive()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    return(cpInternal_->isAlive_);
}

void ComponentProxy::setVerboseLevel(int level)
{
    ACE_Guard<ACE_Thread_Mutex> guard(cpInternal_->verboseMutex_);

    cpInternal_->verboseLevel_ = level;
}

int ComponentProxy::getVerboseLevel()
{
    ACE_Guard<ACE_Thread_Mutex> guard(cpInternal_->verboseMutex_);

    return(cpInternal_->verboseLevel_);
}

void ComponentProxy::setModuleName(string mName)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    cpInternal_->moduleName_ = mName;
}
string ComponentProxy::getModuleName()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    return(cpInternal_->moduleName_);
}
string ComponentProxy::getName()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    return(cpInternal_->name_);
}

const string& ComponentProxy::getRemoteHostname()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    return(cpInternal_->remoteHostName_);
}


int ComponentProxy::handle_exception(ACE_HANDLE)
{
   //VERBOSE2(getVerboseLevel(), "handle_exception " << getName() << endl);

    // unregister with the reactor (causing handle_close to be called)
    return -1;
}


void ComponentProxy::shutdown()
{
   // do nothing, let subclasses override
}


void ComponentProxy::resetSocket()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    //VERBOSE2(getVerboseLevel(), "resetSocket " << getName() << endl);

    // Schedule an exception event that will call back to ::handle_exception(),
    // which will unregister this handler from the Reactor
    // (and thereby disconnect the socket).
    // Don't unregister here directly, since the handle_input method may currently
    // be active

    ACE_Reactor::instance()->notify(this, ACE_Event_Handler::EXCEPT_MASK);

}

bool ComponentProxy::validInterfaceVersion()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    // This method assumes that the subclassed proxy stores the
    // interface version in itself before this method is called.

    if (expectedInterfaceVersion() != receivedInterfaceVersion())
    {
	stringstream strm;
	strm << "Interface mismatch from proxy: " << getName() 
	     << " (host: " << getRemoteHostname() << ")" << " \n"
	     << "Expected version: '" << expectedInterfaceVersion() << "'\n"
	     << "Received version: '" << receivedInterfaceVersion() << "'"
	     << endl;
	logError(strm.str());
	return(false);
    }

    return(true);
}

string ComponentProxy::receivedInterfaceVersion()
{
    return("no-received-interface-version-set");
}

string ComponentProxy::expectedInterfaceVersion()
{
    return("no-expected-interface-version-set");
}

void ComponentProxy::logError(const string& errorMsg)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    cerr << errorMsg << endl;
}

void ComponentProxy::endAceEventLoopOnClose(bool flag)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(cpInternal_->aceMutex_);

    cpInternal_->endAceEventLoopOnClose_ = flag;
}



