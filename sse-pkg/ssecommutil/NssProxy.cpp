/*******************************************************************************

 File:    NssProxy.cpp
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



#include <ace/OS.h>
#include <ace/Synch.h>
#include <ace/Thread.h>
#include "NssProxy.h"
#include "sseInterface.h"
#include "SseMessage.h"
#include "Verbose.h"
#include "Assert.h"
#include <string>
#include <sstream>

using namespace std;

static const ACE_Time_Value SEND_TIMEOUT_SECS(30);  
static const ACE_Time_Value RECEIVE_TIMEOUT_SECS(30);  

static void setMaxTcpBufferSize(ACE_SOCK_STREAM &stream);

#define CheckForConnectedIoHandler(x) \
    if (!(x)) \
    { \
       cerr << __FILE__ << "(" << __LINE__ << "): "; \
       cerr << "No Proxy connected!" << endl; \
       return; \
    }

// define private data & methods
struct NssProxyInternal
{
    NssProxyInternal(NssProxy *nssProxy);
    NssProxyInternal(NssProxy *nssProxy, ACE_SOCK_STREAM &stream);
    ~NssProxyInternal();

    // define a max message size for sanity checking
    static const int maxBodySize_ = 1000000;

    static const int initialMessageNumber_ = 1;

    int processIncomingMessage(SseInterfaceHeader *hdr);
    bool checkReceivedMessageNumber(uint32_t hdrMessageNumber);
    void sendMsg(const void *msg, int len);

    uint32_t sendMessageNumber_;   // outgoing message number
    uint32_t expectedReceiveMessageNumber_;   
    NssProxy *nssProxy_;
    bool previousSendFailure_;

    ACE_Recursive_Thread_Mutex objectMutex_;
    ACE_Recursive_Thread_Mutex sendMutex_;
    ACE_Recursive_Thread_Mutex receiveMutex_;
    ACE_SOCK_Stream aceSockStream_;


};

NssProxyInternal::NssProxyInternal(NssProxy *nssProxy)
    :
    sendMessageNumber_(initialMessageNumber_),
    expectedReceiveMessageNumber_(initialMessageNumber_),
    nssProxy_(nssProxy),
    previousSendFailure_(false)
{
}

NssProxyInternal::NssProxyInternal(NssProxy *nssProxy, 
				   ACE_SOCK_STREAM &stream)
    :
    sendMessageNumber_(initialMessageNumber_),
    expectedReceiveMessageNumber_(initialMessageNumber_),
    nssProxy_(nssProxy),
    previousSendFailure_(false),
    aceSockStream_(stream)
{
    nssProxy_->endAceEventLoopOnClose(true);

    setMaxTcpBufferSize(stream);

}

NssProxyInternal::~NssProxyInternal()
{

}


// Read the message body (if there is one),
// then forward the message header & body.

int NssProxyInternal::processIncomingMessage(SseInterfaceHeader *hdr)
{
    // Assume caller provides mutex protection

    int status = 0; // 0 means ok, stay registered with the reactor

#if ENABLE_NUMCHECK

    if (! checkReceivedMessageNumber(hdr->messageNumber))
    {
	// invalid message number, so output the problem message
	cerr << *hdr << endl;
    }

#endif

    char *bodybuff = 0;
    int bodySize = hdr->dataLength;
    if (bodySize != 0)
    {
	// do a sanity check
	if (bodySize < 0 || bodySize > maxBodySize_)
	{
	    stringstream strm;

	    strm << "Received invalid message from"
		 << " component: " << nssProxy_->getName()
		 << " host: " << nssProxy_->getRemoteHostname() << endl
		 << *hdr
		 << "Invalid message body size: " << bodySize
		 << ".  Disconnecting component." 
		 << endl;

	    SseMessage::log(nssProxy_->getRemoteHostname(),
		    NSS_NO_ACTIVITY_ID, SSE_MSG_INVALID_MSG,
		    SEVERITY_ERROR, strm.str(),
		    __FILE__, __LINE__);

	    status = -1;  // unregister
	}
	else 
	{
	    bodybuff = new char[bodySize];    
	    
	    ssize_t result = aceSockStream_.recv_n(bodybuff, bodySize, 
						   &RECEIVE_TIMEOUT_SECS);
	    if (result <= 0)
	    {

		if (errno == ETIME) 
		{
		    stringstream strm;
		    strm  << "NssProxy timeout on incoming message body"
                          << " (errno is ETIME)\n"
		          << "Unregistering proxy input handler" << endl;

	           SseMessage::log(nssProxy_->getRemoteHostname(),
                                   NSS_NO_ACTIVITY_ID, SSE_MSG_PROXY_TIMEOUT,
                                   SEVERITY_ERROR, strm.str(),
                                   __FILE__, __LINE__);
		}

		// error, or connection has closed
		// unregister handler
		//ACE_DEBUG((LM_DEBUG,"unregistering input handler\n"));

		status = -1;  // unregister
	    }
	}
    }

    if (status == 0)
    {
	// Forward the received message.  It's OK if bodybuff is void.
	nssProxy_->handleIncomingMessage(hdr, bodybuff);
    }

    delete[] bodybuff;
	
    return status; 

}


bool NssProxyInternal::checkReceivedMessageNumber(uint32_t hdrMessageNumber)
{
    // Assume caller provides mutex protection

    bool status = false;

    // make sure received message number is in sequence
    if (hdrMessageNumber != expectedReceiveMessageNumber_)
    {
	cerr << "NssProxy Error: Received message number out of sequence.\n"
	     << "Expected " << expectedReceiveMessageNumber_ 
	     << " but received " << hdrMessageNumber << endl;

	// Try to get back in sequence.
	expectedReceiveMessageNumber_ = hdrMessageNumber + 1;

    }
    else
    {
	expectedReceiveMessageNumber_++;

	status = true;
    }

    return status;
}



// send a message across the socket to the physical nss device
void NssProxyInternal::sendMsg(const void *msg, int len)
{
    // Assume caller provides mutex protection.

    //cout << "NssProxy::sendMsg" << endl;

    if (! previousSendFailure_)
    {
	if (! nssProxy_->isConnectionOpen())
	{
	    stringstream strm;
	    strm << "failure sending message to "
		 << nssProxy_->getName()
		 << ", connection is not open." << endl;

	    SseMessage::log(nssProxy_->getModuleName(),
                            NSS_NO_ACTIVITY_ID, SSE_MSG_CONNECT_CLOSED,
                            SEVERITY_ERROR, strm.str(),
                            __FILE__, __LINE__);

	    previousSendFailure_ = true;
	    
	    return;
	}

	// send_n is supposed to return a status of -1
	// on an error (including timeout)
	// but this is apparently broken in ace5.1,  and
	// fixed in ace5.2.
	// Checking for <= 0  seems valid 
	// for both ace versions.

	// Note: using send_n with a timeout when the
	// connection is not open will cause a segfault,
	// at least with ace 5.1.

	int status = aceSockStream_.send_n(msg, len, &SEND_TIMEOUT_SECS);
	if (status <= 0)
	{ 
	    previousSendFailure_ = true;

	    stringstream strm;
	    strm << "NssProxy: failure sending message to: " 
		 << nssProxy_->getName() << endl;

	    if (errno == ETIME) 
	    {
		strm << "Message timeout (errno is ETIME)." << endl;
	    }

	    // If this send fails, then all further communications
	    // on this socket can become garbled, so disconnect the socket.

	    strm << "Resetting socket.\n";

	    SseMessage::log(
               nssProxy_->getName(),
               NSS_NO_ACTIVITY_ID, SSE_MSG_SOCKET_RESET,
               SEVERITY_ERROR, strm.str(),
               __FILE__, __LINE__);

	    nssProxy_->resetSocket();

	} 

    }

}

// ----- NssProxy methods ---------

NssProxy::NssProxy()
    :
    internal_(new NssProxyInternal(this))
{
}

NssProxy::NssProxy(ACE_SOCK_STREAM &stream)
    :
    internal_(new NssProxyInternal(this, stream))
{
}

NssProxy::~NssProxy()
{
    delete internal_;
}


//Called back to handle any input received
int NssProxy::handle_input(ACE_HANDLE)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->receiveMutex_);

    // read the message header
    SseInterfaceHeader header;
    SseInterfaceHeader *hdr = &header;

    ssize_t result = getAceSockStream().recv_n(hdr, sizeof(SseInterfaceHeader),
					       &RECEIVE_TIMEOUT_SECS);

    //VERBOSE2(getVerboseLevel(), "handle_input result=" << result << endl;);

    if (result <= 0)
    {
	if (errno == ETIME) 
	{
	    stringstream strm;
	    strm << "NssProxy timeout on incoming message header (errno is ETIME)"
	         << "Unregistering proxy input handler" << endl;

	    SseMessage::log(internal_->nssProxy_->getName(),
                            NSS_NO_ACTIVITY_ID, SSE_MSG_PROXY_TIMEOUT,
                            SEVERITY_ERROR, strm.str(),
                            __FILE__, __LINE__);
	}

	// error, or connection has closed
	// unregister handler
	//ACE_DEBUG((LM_DEBUG,"unregistering input handler\n"));

	return -1;
    }
    else
    {
	hdr->demarshall();

	int status = internal_->processIncomingMessage(hdr); 

	return status;
    }

}


// send a marshalled message to the connected target
void NssProxy::sendMessage(int messageCode, int activityId, 
			   int dataLength, const void *msgBody)
{
    SseInterfaceHeader hdr;

    ACE_Time_Value currentTime = ACE_OS::gettimeofday();

    // set message hdr fields
    hdr.code = messageCode;
    hdr.dataLength = dataLength;
    hdr.messageNumber = internal_->sendMessageNumber_++; 
    hdr.activityId = activityId;
    hdr.timestamp.tv_sec = currentTime.sec(); 
    hdr.timestamp.tv_usec = currentTime.usec();

    // TBD:
    fill_n(hdr.sender, SSE_MAX_HDR_ID_SIZE, '\0');
    fill_n(hdr.receiver, SSE_MAX_HDR_ID_SIZE, '\0');

    //hdr.sender[0] = '\0';
    //hdr.receiver[0] = '\0';

    //VERBOSE2(getVerboseLevel(), hdr);

    hdr.marshall();

    // TBD check if output stream is connected?
    //CheckForConnectedIoHandler(x);

    {
	// Make sure the header & body stay together
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->sendMutex_);
	
	// send header
	internal_->sendMsg(&hdr, sizeof(hdr));

	// Send the message body (if there is one).
	// Message body must already be marshalled.
	if (dataLength > 0)
	{
	    Assert(msgBody != 0);
	    internal_->sendMsg(msgBody, dataLength); 
	}
    }
}

//Used by the reactor to determine the underlying handle
ACE_HANDLE NssProxy::get_handle() const
{

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->objectMutex_);

    return internal_->aceSockStream_.get_handle();
}

//Returns a reference to the underlying stream.
ACE_SOCK_Stream & NssProxy::getAceSockStream()
{

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->objectMutex_);

    return internal_->aceSockStream_;
}


// set the maximum tcp send & receive buffer size
static void setMaxTcpBufferSize(ACE_SOCK_STREAM &stream)
{
    // Make the buffer large enough to handle a lot
    // of message traffic.
    // Appropriate size is still TBD

    const int buffsize = 65536;
    
    if (stream.set_option(SOL_SOCKET, SO_RCVBUF,
			  (void *) & buffsize,
			  sizeof(buffsize)) == -1)
    {
	cerr << "NssProxy: failed to set max TCP receive buffer" << endl;
    }
	
	
    if (stream.set_option(SOL_SOCKET, SO_SNDBUF,
			  (void *) & buffsize,
			  sizeof(buffsize)) == -1)
    {
	cerr << "NssProxy: failed to set max TCP send buffer" << endl;
    }
	
}