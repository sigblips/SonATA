/*******************************************************************************

 File:    SseComponentManager.h
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


#ifndef SseComponentManager_H
#define SseComponentManager_H

#include "ace/Synch.h"
#include "DebugLog.h"  // keep this early in the headers for VERBOSE macros
#include "NssComponentManager.h"
#include "SseArchive.h"
#include "SseMessage.h"
#include "Assert.h"
#include <list>
#include <map>
#include <sstream>


// This class handles storage of Sse components.
// Components are represented by proxy pointers.

class Subscriber;

template<typename Tproxy>

class SseComponentManager : public NssComponentManager<Tproxy>
{
 public:
    SseComponentManager(Subscriber *subscriber);
    virtual ~SseComponentManager();
    void logErrorMsg( NssMessage &nssMessage);

 protected:

    virtual void logErrorMsg(const string &msgText);
    virtual void logInfoMsg(const string &msgText);

 private:
    // Disable copy construction & assignment.
    // Don't define these.
    SseComponentManager(const SseComponentManager& rhs);
    SseComponentManager& operator=(const SseComponentManager& rhs);

};

//-------------------------------------------------------------------

// constructor
template<typename Tproxy>
SseComponentManager<Tproxy>::
SseComponentManager(Subscriber *subscriber)
    :
    NssComponentManager<Tproxy>(subscriber)
{

}

// destructor
template<typename Tproxy>
SseComponentManager<Tproxy>::~SseComponentManager()
{

}


template<typename Tproxy>
void SseComponentManager<Tproxy>::
logErrorMsg(const string &msgText)
{
    SseArchive::ErrorLog() << "SseComponentManager<Tproxy>::logErrorMsg"
			 << msgText;

    // send all error messages to the system log also
    SseArchive::SystemLog() << "SseComponentManager<Tproxy>::logErrorMsg"
			 << msgText;
}

template<typename Tproxy>
void SseComponentManager<Tproxy>::
logErrorMsg(NssMessage &nssMessage)
{
    SseArchive::ErrorLog() << nssMessage;

    // send all error messages to the system log also
    SseArchive::SystemLog() << nssMessage;
}


template<typename Tproxy>
void SseComponentManager<Tproxy>::
logInfoMsg(const string &msgText)
{
    SseArchive::SystemLog() << msgText;
}


#endif 