/*******************************************************************************

 File:    TestNssComponentManager.cpp
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


#include "ace/OS.h"
#include "TestRunner.h"
#include "TestNssComponentManager.h"
#include "Subscriber.h"
#include <iostream>

using namespace std;

class Publisher;
class DummySubscriber : public Subscriber
{
public:
    DummySubscriber() {};
    ~DummySubscriber() {};
    void update(Publisher *changedPublisher)
	{ cout << "update called " << endl; }
};


void TestNssComponentManager::setUp ()
{
    subscriber_ = new DummySubscriber();
    proxyManager_ = new NssComponentManager<TestProxy>(subscriber_);
    proxyManager_->setManagerName("TestProxyManager");

    proxyList_ = new TestProxyList();

    proxy_ = new TestProxy();
    
}

void TestNssComponentManager::tearDown()
{
    delete proxy_;
    delete proxyList_;
    delete proxyManager_;
    delete subscriber_;
}

void TestNssComponentManager::testGetProxy()
{
    cout << "testGetProxy" << endl;

    // ask for an empty list of proxies,
    // make sure it's really empty.

    cu_assert(proxyManager_->getNumberOfProxies() == 0);

    proxyManager_->getProxyList(proxyList_);
    cu_assert(proxyList_->size() == 0);


}

void TestNssComponentManager::testPrintProxyList()
{

    cout << "testPrintProxyLists" << endl;

    proxyManager_->printProxyList();

    //site_->printDetailedProxyList();

    // try to register it
    proxyManager_->registerProxy(proxy_);

    proxyManager_->printProxyList();

    //site_->printDetailedProxyList();

    cout << "proxy names: " << proxyManager_->getNamesOfProxies() << endl;

}


void TestNssComponentManager::testProxyStatus()
{
    cout << "testProxyStatus" << endl;

#if 0
// TBD add status to Proxy interface?

    cout << "DxId: " << proxy_->getDxId() << endl;
    
    proxy_->printStatus();

    DxStatus stat = proxy_->getCachedDxStatus();
    cout << stat;
#endif
}

void TestNssComponentManager::testRegisterProxy()
{
    cout << "testRegisterProxy" << endl;

    // test registering and retrieving a proxy

    // try to register 
    proxyManager_->registerProxy(proxy_);
    
    // try to get it back
    proxyManager_->getProxyList(proxyList_);

    cu_assert(proxyList_->size() == 1);  // should only get 1
    
    // make sure it's the same one we put in
    cu_assert(proxyList_->front() == proxy_);

    // now unregister it and make sure it's removed from the list
    proxyList_->clear();  // clean up the local list
    proxyManager_->unregisterProxy(proxy_);  // remove it from the manager

    // make sure list is empty
    proxyManager_->getProxyList(proxyList_);
    cu_assert(proxyList_->size() == 0); 

}

void TestNssComponentManager::testReceiveIntrinsics()
{
    cout << "testReceiveIntrinsics" << endl;


    // test registering and retrieving a proxy
    // with a mismatched interface version

    TestProxy *testProxy = 
       new TestProxyVersionMismatch();

    // try to register 
    proxyManager_->registerProxy(testProxy);

    // should have only 1 proxy
    cu_assert(proxyManager_->getNumberOfProxies() == 1);

    // this should generate a version mismatch error
    cout << "force an interface version mismatch error: " << endl;
    proxyManager_->receiveIntrinsics(testProxy);


    // should have been disconnected due to version mismatch
    // can we detect this somehow?

    // cu_assert(proxyManager_->getNumberOfProxies() == 0);

    // Don't delete testProxy.  The receiveIntrinsics method
    // eventually causes the ACE_Reactor::instance()->notify()
    // method to be called, which causes a segfault when the
    // Reactor destructor is eventually called.

    
}

void TestNssComponentManager::testAllocateProxy()
{
    cout << "testAllocateProxy" << endl;

    // test allocating a proxy
    // & releasing it back

    // try to register it
    proxyManager_->registerProxy(proxy_);
    proxyManager_->printProxyList();

    // now allocate it
    proxyManager_->allocateProxyList(proxyList_);
    cu_assert(proxyList_->size() == 1);  // should only get 1
    
    proxyManager_->printProxyList();

    // make sure it's the same one we put in
    cu_assert(proxyList_->front() == proxy_);

    // try to get another proxy, this should fail
    TestProxyList proxyList2;
    proxyManager_->allocateProxyList(&proxyList2);
    cu_assert(proxyList2.size() == 0);  // shouldn't get any

    // now try to release it back to the manager
    proxyManager_->releaseProxyList(proxyList_);
    proxyManager_->printProxyList();

    // try to get it again, this should succeed
    TestProxyList proxyList3;
    proxyManager_->allocateProxyList(&proxyList3);
    cu_assert(proxyList3.size() == 1);  // should get one

    // make sure it's the same one we put in
    cu_assert(proxyList3.front() == proxy_);


}


Test *TestNssComponentManager::suite ()
{
	TestSuite *testSuite = new TestSuite("TestNssComponentManager");

	testSuite->addTest (new TestCaller <TestNssComponentManager> ("testProxyStatus", &TestNssComponentManager::testProxyStatus));

	testSuite->addTest (new TestCaller <TestNssComponentManager> ("testRegisterProxy", &TestNssComponentManager::testRegisterProxy));

	testSuite->addTest (new TestCaller <TestNssComponentManager> ("testGetProxy", &TestNssComponentManager::testGetProxy));

	testSuite->addTest (new TestCaller <TestNssComponentManager> ("testAllocateProxy", &TestNssComponentManager::testAllocateProxy));

	testSuite->addTest (new TestCaller <TestNssComponentManager> ("testPrintProxyList", &TestNssComponentManager::testPrintProxyList));

	testSuite->addTest (new TestCaller <TestNssComponentManager> ("testReceiveIntrinsics", &TestNssComponentManager::testReceiveIntrinsics));

	return testSuite;
}
