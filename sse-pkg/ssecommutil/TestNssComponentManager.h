/*******************************************************************************

 File:    TestNssComponentManager.h
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


#ifndef TEST_NSS_COMPONENT_MANAGER_H
#define TEST_NSS_COMPONENT_MANAGER_H

#include <ace/OS.h>
#include "TestCase.h"
#include "TestSuite.h"
#include "TestCaller.h"

#include "NssComponentManager.h"
#include "NssProxy.h"

class Subscriber;

// create a generic do-nothing TestProxy for testing
//
class TestProxy : public NssProxy
{
 public:
    void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff) {}
    string getName() { return "testProxy"; }

    string expectedInterfaceVersion() { return "rev1"; }
    string receivedInterfaceVersion() { return "rev1"; }
	string getIntrinsics() { return "This is a test" ; };
};


class TestProxyVersionMismatch : public TestProxy
{
 public:
    void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff) {}
    string getName() { return "testProxyVersionMismatch"; }

    string expectedInterfaceVersion() { return "rev1.1"; }
    string receivedInterfaceVersion() { return "rev1.2"; }
};

// define a list for TextProxy
#include <list>
typedef std::list<TestProxy*> TestProxyList;


class TestNssComponentManager : public TestCase
{
 public:
    TestNssComponentManager (string name) : TestCase (name) {}

    void setUp ();
    void tearDown();
    static Test *suite ();

 protected:
    void testGetProxy();
    void testAllocateProxy();
    void testRegisterProxy();
    void testProxyStatus(); 
    void testPrintProxyList();
    void testReceiveIntrinsics();

    Subscriber *subscriber_;
    TestProxyList *proxyList_;
    TestProxy *proxy_;
    NssComponentManager<TestProxy> *proxyManager_;
};


#endif