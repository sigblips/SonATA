/*******************************************************************************

 File:    TclProxy.cpp
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


#include "TclProxy.h"

#include <ace/Synch.h>
#include <iostream>
#include <sstream>
#include "SseUtil.h"
#include "SseMessage.h"
#include "Assert.h"

using namespace std;

static const int READ_BUFFER_SIZE = 5000;

struct TclProxyInternal
{
    TclProxyInternal();
    ~TclProxyInternal();

    char readBuffer_[READ_BUFFER_SIZE];
    ACE_Recursive_Thread_Mutex aceProxyMutex_;
};

TclProxyInternal::TclProxyInternal()
{
}

TclProxyInternal::~TclProxyInternal()
{
}


TclProxy::TclProxy() 
    : internal_(new TclProxyInternal())
{
}

TclProxy::TclProxy(ACE_SOCK_STREAM& aceSockStream) 
    : ComponentProxy(aceSockStream),
      internal_(new TclProxyInternal())
{
}

TclProxy::~TclProxy()
{
    delete internal_;
}


int TclProxy::handle_input(ACE_HANDLE fd) 
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->aceProxyMutex_);
    string message;
    int nBytesRead = receive(message);
    if (nBytesRead > 0)
    {
	handleIncomingMessage(message);
    }
    else
    {
	return(-1);
    }

    return(0);
}


int TclProxy::send(const string & message)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->aceProxyMutex_);

    return(getAceSockStream().send((void*) message.c_str(), message.size()));
}

int TclProxy::receive(string& message)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(internal_->aceProxyMutex_);

    ssize_t maxBytesToRead = READ_BUFFER_SIZE - 1;  // save room for trailing null
    ssize_t nBytesRead = getAceSockStream().recv(internal_->readBuffer_,
						 maxBytesToRead);
    if (nBytesRead <= 0)
    {
	stringstream errmsg;
	errmsg << getName() << ": ComponentProxy::receive() "
		      "nBytesRead <= 0\n" << endl;
	SseMessage::log(getName(), 0, SSE_MSG_CONNECT_CLOSED,
                        SEVERITY_ERROR, errmsg.str());
	return(-1); // unregister
    }

    Assert(nBytesRead <= maxBytesToRead);
    internal_->readBuffer_[nBytesRead] = '\0'; // null terminate the string
    message = internal_->readBuffer_;

    return(nBytesRead);
}

// Extract the value in 'message' that follows 'key' and precedes the
// delimiters.  Any white space between the key and the start of the
// element is skipped.  Throws const string exception if
// key is not found or the value is missing.

string TclProxy::parseElement(const string& message, const string& key,
			      const string& delimit)
{
    // cout << "message is:'" << message << "'"<<endl;

    string::size_type keyStartPos = message.find(key);
    if (keyStartPos == string::npos)
    {
	stringstream msg;
	msg << "parseElement: key not found: '" << key << "'" << endl;
	throw msg.str();
    }
    string::size_type valueStartPos = keyStartPos + key.size();

    // skip over any leading whitespace
    valueStartPos = message.find_first_not_of(" \t", valueStartPos);

    // find the delimiter
    string::size_type valueEndPos = message.find_first_of(delimit, valueStartPos);
    if (valueEndPos == string::npos)
    {
	stringstream msg;
	msg << "parseElement: delimiter not found for key: '" << key << "'" << endl;
	throw msg.str();
    }

    //cout << "valueStartPos = " << valueStartPos << endl;
    //cout << "valueEndPos = " << valueEndPos << endl;

    unsigned int valueLength = valueEndPos - valueStartPos;
    if (valueLength <= 0)
    {
	stringstream msg;
	msg << "parseElement: no value found for key: '" << key << "'" << endl;
	throw msg.str();
    }

    return(message.substr(valueStartPos, valueLength));
}

double TclProxy::parseDouble(const string& message, const string& key,
			     const string& delimit) 
{
    string temp(parseElement(message, key, delimit));

    // will throw on error
    return(SseUtil::strToDouble(temp));
}

int TclProxy::parseInt(const string& message, const string& key,
		       const string& delimit)
{
    string temp(parseElement(message, key, delimit));

    // will throw on error
    return(SseUtil::strToInt(temp));
}