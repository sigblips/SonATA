/*******************************************************************************

 File:    DxArchiver.cpp
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


#include <ace/Timer_Queue.h>
#include <ace/Reactor.h>
#include "DxArchiver.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "SseProxy.h"
#include "DxProxy.h"
#include "SseDxMsg.h"
#include "SseMsg.h"
#include "Assert.h"
#include "SseUtil.h"
#include "Subscriber.h"
#include "NssComponentManager.h"
#include "NssAcceptHandler.h"
#include "Verbose.h"
#include <list>

using namespace std;

class DxProxy;
typedef list<DxProxy*> DxList;


class TestProxy : public NssProxy
{
 public:
    void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff) {};
    string getName() { return "testProxy"; }
	string getIntrinsics() { return "This is a test"; };
};



class Publisher;
class DxArchiverSubscriber : public Subscriber 
{
public:
    DxArchiverSubscriber(DxArchiver *dxArchiver);
    ~DxArchiverSubscriber();
    virtual void update(Publisher *changedPublisher);
private:
    DxArchiver *dxArchiver_;
};

DxArchiverSubscriber::DxArchiverSubscriber(DxArchiver *dxArchiver) : 
   dxArchiver_(dxArchiver) {}

DxArchiverSubscriber::~DxArchiverSubscriber() {}
void DxArchiverSubscriber::update(Publisher *changedPublisher)
{
    dxArchiver_->updateDxArchiverStatus();
}




DxArchiver::DxArchiver(SseProxy *sseProxy, const string &name, int dxPort)
    : sseProxy_(sseProxy),
      name_(name),
      dxPort_(dxPort),
      verboseLevel_(0),
      subscriber_(0),
      dxComponentManager_(0),
      dxAcceptHandler_(0)
{
    setup();
    sseProxy_->setModuleName(name_);
}


void DxArchiver::setup()
{
    sseProxy_->setDxArchiver(this);

    initIntrinsics();


    // more inits here for all local data
    // TBD

    subscriber_ = new DxArchiverSubscriber(this);

    // create a component manager to keep track of & control
    // the dxs that connect
    dxComponentManager_ = new NssComponentManager<DxProxy>(
       subscriber_,
       NssComponentManager<DxProxy>::DiscardOldProxyWithDuplicateName);
    dxComponentManager_->setManagerName("DxArchiver-DxManager");

    string portString = SseUtil::intToStr(dxPort_);

    // put up a listener to accept new dx connections
    dxAcceptHandler_ = new NssAcceptHandler<DxProxy>(portString.c_str(),
						       dxComponentManager_); 

}


DxArchiver::~DxArchiver()
{
    cout << "DxArchiver destructor" << endl;

    delete dxAcceptHandler_;
    delete dxComponentManager_;
    delete subscriber_;
}

SseProxy * DxArchiver::getSseProxy()
{
    return (sseProxy_);
}

void DxArchiver::setVerboseLevel(int level)
{
    verboseLevel_ = level;

    sseProxy_->setVerboseLevel(level);

    // set verbose mode on all the dx proxies
    dxComponentManager_->setVerbose(level);
}

int DxArchiver::getVerboseLevel()
{
    return verboseLevel_;
}



// SSE has requested the archiver's intrinsics

void DxArchiver::requestArchiverIntrinsics()
{
    VERBOSE2(getVerboseLevel(), "dxArchiver::requestArchiverIntrinsics\n");

#if 0

    for (int i=0; i<3; ++i)
    {
    // debug test
    NssMessage dxMessage;
    dxMessage.code = 0;
    dxMessage.severity = SEVERITY_INFO;
    SseUtil::strMaxCpy(dxMessage.description,
		      "test of DxMessage, sent before intrinsics",
		      MAX_NSS_MESSAGE_STRING);
    int activityId = NSS_NO_ACTIVITY_ID;
    sseProxy_->sendDxMessage(dxMessage, activityId);
    }
#endif	


    // send intrinsics info to SSE
    sseProxy_->sendIntrinsics(archiverIntrinsics_);   

    VERBOSE2(getVerboseLevel(), 
	     archiverIntrinsics_ << "\n" << "dx port: " << dxPort_ << endl;);

}

void DxArchiver::requestArchiverStatus()
{
    // send overall archiver status info to SSE

    cout << "DxArchiver::requestArchiverStatus" << endl;

    updateDxArchiverStatus();

}

void DxArchiver::updateDxArchiverStatus()
{ 
    archiverStatus_.timestamp = SseMsg::currentNssDate();

    archiverStatus_.numberOfConnectedDxs = 
	dxComponentManager_->getNumberOfProxies();
    
    SseUtil::strMaxCpy(archiverStatus_.namesOfConnectedDxs,
		       dxComponentManager_->getNamesOfProxies().c_str(),
		       MAX_TEXT_STRING); 
    
    sseProxy_->sendArchiverStatus(archiverStatus_);   

}

void DxArchiver::printDxNames()
{
    DxList dxList;
    dxComponentManager_->getProxyList(&dxList);

    cout << "Connected dxs: " << endl;

    for (DxList::iterator index = dxList.begin();
         index != dxList.end(); ++index)
    {
        DxProxy *dxProxy = *index;
        Assert(dxProxy);

        cout << dxProxy->getName() << " ";

    }

    cout << endl;
}

void DxArchiver::resetDxSocket(const string & dxName)
{
    DxList dxList;
    dxComponentManager_->getProxyList(&dxList);

    bool found = false;
    for (DxList::iterator index = dxList.begin();
         index != dxList.end(); ++index)
    {
        DxProxy *dxProxy = *index;
        Assert(dxProxy);

	if (dxProxy->getName() == dxName)
	{
	    found = true;
	    dxProxy->resetSocket();

	    cout << "reset socket on dx " << dxName << endl;
	    break;
	}
    }

    if (!found)
    {
	cout << "dx '" << dxName
	     << "' not found, can't reset socket" << endl;
    }

}


void DxArchiver::sendErrorMsgToSse(const string &text)
{
    cout << "sendErrorMsgToSse: text is '" << text << "'" <<endl;

    NssMessage dxMessage;
    dxMessage.code = 0;   // TBD add arg to set code
    dxMessage.severity = SEVERITY_ERROR;

    // Don't overrun the destination buffer.
    // need cast to prevent compiler warning about 
    // comparison between signed and unsigned
    
    Assert (static_cast<signed int>(text.length())
	    < MAX_NSS_MESSAGE_STRING); 

    SseUtil::strMaxCpy(dxMessage.description, text.c_str(),
		       MAX_NSS_MESSAGE_STRING); 

    int activityId = NSS_NO_ACTIVITY_ID;
    sseProxy_->sendNssMessage(dxMessage, activityId);
}


void DxArchiver::shutdownArchiver()
{
    cout <<"DxArchiver::shutdownArchiver" << endl;

    ACE_Reactor::end_event_loop();

}


//---- private  utilities -------------


void DxArchiver::printIntrinsics()
{
    cout << archiverIntrinsics_;

}

void DxArchiver::printStatus()
{
    updateDxArchiverStatus();

    cout << archiverStatus_ << endl;

//    dxComponentManager_->printProxyList();

    
}


// initialize the intrinsics information
void DxArchiver::initIntrinsics() 
{
    // use fill_n to zero out all the char arrays 
    // (to appease the memory checking tools)

    // interface version number
    fill_n(archiverIntrinsics_.interfaceVersionNumber, MAX_TEXT_STRING, '\0');
    SseUtil::strMaxCpy(archiverIntrinsics_.interfaceVersionNumber,
		       SSE_DX_ARCHIVER_INTERFACE_VERSION, MAX_TEXT_STRING);

    // debug
    // strcpy(intrinsics_.interfaceVersionNumber, "bad version number test");

    // hostname
    fill_n(archiverIntrinsics_.hostname, MAX_TEXT_STRING, '\0'); 
    SseUtil::strMaxCpy(archiverIntrinsics_.hostname, SseUtil::getHostname().c_str(),
		       MAX_TEXT_STRING);

    // server name
    fill_n(archiverIntrinsics_.name, MAX_TEXT_STRING, '\0'); 
    SseUtil::strMaxCpy(archiverIntrinsics_.name, name_.c_str(),
		       MAX_TEXT_STRING);

    // dxPort
    archiverIntrinsics_.port = dxPort_;

}

