/*******************************************************************************

 File:    SharedTclProxy.cpp
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



#include "SharedTclProxy.h" 
#include "DebugLog.h"

SharedTclProxy::SharedTclProxy()
{
}

SharedTclProxy::~SharedTclProxy()
{
}

// Process incoming messages.
// Each message is assumed to end with the terminator "<end>".
// It's possible for this method to be called with partial messages,
// so wait until the terminator is the last non-white-space
// text in the accumulated message buffer before final processing
// of the messages.

void SharedTclProxy::handleIncomingMessage(const string & message)
{

#if 0
    VERBOSE2(getVerboseLevel(),
	     "SharedTclProxy::handleIncomingMessage(): \n"
	     << getName() << ": raw message read\n"
	     << "=======\n "
	     << message  
	     << "=======\n" << endl);
#endif

    // store (possibly partial) message
    messageBuffer_ += message;
    
    string terminator("<end>");

    // To avoid processing a partial message prematurely,
    // see if there's a message terminator in the buffer.

    string::size_type lastTerminatorStartPos = 
	messageBuffer_.rfind(terminator);

    if (lastTerminatorStartPos == string::npos)
    {
        // no terminator found at all, so this is an
	// incomplete message. Wait for the rest of it.

#if 0
	VERBOSE2(getVerboseLevel(),
		 "SharedTclProxy::handleIncomingMessage(): \n"
		 << getName() << ": partial message read, "
		 << "no terminator at all: \n "<< messageBuffer_ << endl);
#endif

	return; 
    }

    // There is a terminator, but make sure it's the last thing
    // in the buffer, other than any trailing white space.
    // Ie, we might have read one full message
    // and one partial message.  In that case, wait for the
    // final terminator so that the buffer only contains 
    // complete messages.

    string::size_type lastTerminatorEndPos = lastTerminatorStartPos 
	+ terminator.length();

    if (lastTerminatorEndPos < messageBuffer_.length())
    {
	if (messageBuffer_.find_first_not_of(
	    "\t\n \r", lastTerminatorEndPos) != string::npos)
	{

#if 0
	    VERBOSE2(getVerboseLevel(),
		     "SharedTclProxy::handleIncomingMessage(): \n"
		     << getName() << ": partial message read, at least one "
		     << "terminator found,\nbut it's not the last non-white-space "
		     << "token in the buffer:\n "
		     << messageBuffer_ << endl);
#endif
	    
	    // incomplete message. Wait for the rest of it.
	    
	    return;
	}
    }


    // There may be multiple messages in the buffer,
    // so process each one in turn.  

    string::size_type subMessageStartPos = 0;
    string::size_type subMessageEndPos = 0;
    while (subMessageStartPos < messageBuffer_.size())
    {
	subMessageEndPos = messageBuffer_.find(terminator,
					       subMessageStartPos);
	if (subMessageEndPos == string::npos)
	{
	    break;
	}
	else
	{
	    string::size_type subMessageLen = 
		subMessageEndPos - subMessageStartPos;

	    string subMessage(messageBuffer_.substr(subMessageStartPos, 
						    subMessageLen));

	    // if it's not just white space, pass it on for processing
	    if (subMessage.find_first_not_of("\t\n \r") != string::npos)
	    {
	       processIndividualMessage(subMessage);
	    }
	}
	  
	// skip over the terminator
	subMessageStartPos = subMessageEndPos + terminator.length();

    }

    messageBuffer_.erase(); 
      
}