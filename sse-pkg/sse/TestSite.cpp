/*******************************************************************************

 File:    TestSite.cpp
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
#include "TestSite.h"
#include "TestRunner.h"
#include "Site.h"
#include "DxProxy.h"
#include "IfcProxy.h"
#include "TscopeProxy.h"
#include "SseDxMsg.h"
#include "NssComponentTree.h"
#include <algorithm>

// number of dxs & ifcs to create
static const unsigned int DxCount = 3;
static const unsigned int IfcCount = 3;
static const unsigned int TscopeCount = 2;

static void loadExpectedComponentsConfig(vector<string> &config)
{
    // create a test 'expectedComponentsTree' config vector
    // containing 2 IF chains

    config.push_back("sonata expected components v1.0");
    config.push_back("Site site1 IfcList ifctest0 ifctest1");
    config.push_back("Ifc ifctest0 BeamList beam0");
    config.push_back("Beam beam0 DxList dxtest0 dxtest1");
    config.push_back("Ifc ifctest1 BeamList beam1");
    config.push_back("Beam beam1 DxList dxtest2");

}


void TestSite::setUp ()
{
    vector<string> expectedComponentsConfig;
    loadExpectedComponentsConfig(expectedComponentsConfig);

    site_ = new Site(expectedComponentsConfig);
    site_->setVerbose(0);

    // attach some components to the site
    setUpDxs();
    setUpIfcs();
    setUpTscopes();

}


void TestSite::setUpDxs()
{
    // Put some dxs on the list.
    // Their names should be the same as the
    // ones in the expected components configuration info.

    for (unsigned int i=0; i<DxCount; i++)
    {
	DxProxy *proxy;

	// create proxies
	proxy = new DxProxy(site_->dxManager());

	// give the dx a name
	stringstream name;
	name << "dxtest" << i;
	proxy->setName(name.str());

	cu_assert(proxy->getName() == name.str());

	// register with the site
	proxy->notifyInputConnected();
	
	dxList_.push_back(proxy);
    }

}


void TestSite::setUpIfcs()
{
    // Put some ifcs on the list.
    // These names should match the ones in the
    // expected components info, plus one more.

    for (unsigned int i=0; i<IfcCount; i++)
    {
	IfcProxy *proxy;

	// create proxies
	proxy = new IfcProxy(site_->ifcManager()); 

	// give the ifc a name
	stringstream name;
	name << "ifctest" << i;
	proxy->setName(name.str());

	cu_assert(proxy->getName() == name.str());

	// register with the site
	proxy->notifyInputConnected();
	
	ifcList_.push_back(proxy);
    }

}


void TestSite::setUpTscopes()
{
    // Put some tscopes on the list.
    // These names should match the ones in the
    // expected components info.

    for (unsigned int i=0; i<TscopeCount; i++)
    {
	TscopeProxy *proxy;

	// create proxies
	proxy = new TscopeProxy(site_->tscopeManager()); 

	// give the tscope a name
	stringstream name;
	name << "tscopetest" << i;
	proxy->setName(name.str());

	cu_assert(proxy->getName() == name.str());

	// register with the site
	proxy->notifyInputConnected();
	
	tscopeList_.push_back(proxy);
    }

}



void TestSite::tearDown()
{
    // clean up the dxs
    for (DxList::iterator it = dxList_.begin(); 
	 it != dxList_.end(); ++it)
    {
	DxProxy *proxy = *it;

	// unregister from the site
	proxy->notifyInputDisconnected();	    
	delete proxy;
    }

    // clean up the ifcs
    for (IfcList::iterator it = ifcList_.begin(); 
	 it != ifcList_.end(); ++it)
    {
	IfcProxy *proxy = *it;

	// unregister from the site
	proxy->notifyInputDisconnected();	    
	delete proxy;
    }

    // clean up the tscopes
    for (TscopeList::iterator it = tscopeList_.begin(); 
	 it != tscopeList_.end(); ++it)
    {
	TscopeProxy *proxy = *it;

	// unregister from the site
	proxy->notifyInputDisconnected();	    
	delete proxy;
    }

    delete site_;
}

void TestSite::testDxStatus()
{
    cout << "testDxStatus" << endl;

    DxProxy *proxy = dxList_.front();

    cout << "DxName: " << proxy->getName() << endl;
    cout << proxy->getCachedDxStatus() << endl;
    cout << proxy->getConfiguration();
    cout << proxy->getIntrinsics();
    cout << "Dx bandwidth: " << proxy->getBandwidthInMHz() << endl;

}

void TestSite::testGetNssComponentTree()
{
    cout << "testGetNssComponentTree" << endl;

    NssComponentTree * tree = site_->getAllComponents();

    DxList &dxList = tree->getDxs();
    cu_assert(dxList.size() == DxCount);

    // make sure all the dxs in the component tree are
    // on the original list used to create it
    for(DxList::iterator it = dxList.begin();
	it != dxList.end(); ++it)
    {
	DxProxy *proxy = *it;

	cout << proxy->getName() << endl;

	cu_assert(find(dxList_.begin(), dxList_.end(), proxy)
		  != dxList_.end());
    }


    IfcList &ifcList = tree->getIfcs();
    cu_assert(ifcList.size() == IfcCount);

    // make sure all the ifcs in the component tree are 
    // on the original list used to create it
    for(IfcList::iterator it = ifcList.begin();
	it != ifcList.end(); ++it)
    {
	IfcProxy *proxy = *it;

	cout << proxy->getName() << endl;

	cu_assert(find(ifcList_.begin(), ifcList_.end(), proxy)
		  != ifcList_.end());
    }

    vector<string> beamNames;
    beamNames.push_back("beam0");
    beamNames.push_back("beam1");

    IfcList ifcsForBeams(tree->getIfcsForBeams(beamNames));
    cu_assert(ifcsForBeams.size() == 2);

    for (IfcList::iterator it = ifcsForBeams.begin();
	it != ifcsForBeams.end(); ++it)
    {
       IfcProxy *proxy = *it;
       cu_assert(proxy->getName() == "ifctest0" ||
	      proxy->getName() == "ifctest1"); 
    }

    DxList dxsForBeams(tree->getDxsForBeams(beamNames));
    cu_assert(dxsForBeams.size() == 3);
    for (DxList::iterator it = dxsForBeams.begin();
	 it != dxsForBeams.end(); ++it)
    {
       DxProxy *proxy = *it;
       cu_assert(proxy->getName() == "dxtest0"
		 || proxy->getName() == "dxtest1"
		 || proxy->getName() == "dxtest2");
    }

    TscopeList &tscopeList = tree->getTscopes();
    cu_assert(tscopeList.size() == TscopeCount);

    TestSigList &testSigList = tree->getTestSigs();
    cu_assert(testSigList.size() == 0);

    delete tree;
}


void TestSite::testGetDxsForIfc()
{
    cout << "testGetDxsForIfc" << endl;

    // Test that the NssComponentTree correctly
    // uses the expectedNssComponentsTree information
    // to return the dx proxies that
    // are attached to each of the IFs.

    // expected ifc/dx configuration:
    // ifctest0: dxtest0 dxtest1
    // ifctest1: dxtest2

    // Note:
    // a proxy for ifctest2 exists, but it is not listed
    // in the configuration info, and so should have no
    // dxs attached.

    NssComponentTree * tree = site_->getAllComponents();

    IfcList &ifcList = tree->getIfcs();
    cu_assert(ifcList.size() == IfcCount);

    // Get the dx proxies for each of the ifcs
    // and make sure they are the right ones.

    unsigned int nIfcsPassed = 0;
    for(IfcList::iterator it = ifcList.begin();
	it != ifcList.end(); ++it)
    {
	IfcProxy *ifcProxy = *it;

	if (ifcProxy->getName() == "ifctest0")
	{
	    DxList & dxList = tree->getDxsForIfc(ifcProxy);
	    cu_assert(dxList.size() == 2);

	    // make sure the names match
	    DxList::iterator it;
	    for (it = dxList.begin(); it != dxList.end(); ++it)
	    {
		DxProxy *proxy = *it;
		cu_assert(proxy->getName() == "dxtest0" ||
			  proxy->getName() == "dxtest1");
	    }

	    nIfcsPassed++;
	}
	else if (ifcProxy->getName() == "ifctest1")
	{
	    DxList & dxList = tree->getDxsForIfc(ifcProxy);
	    cu_assert(dxList.size() == 1);

	    nIfcsPassed++;
	}
	else if (ifcProxy->getName() == "ifctest2")
	{
	    // this one should not have any dxs attached to it

	    DxList & dxList = tree->getDxsForIfc(ifcProxy);
	    cu_assert(dxList.size() == 0);

	    nIfcsPassed++;
	}
	else {

	    // unexpected ifc is on the list
	    cout << "error: unexpected ifc on the list: " 
		 << ifcProxy->getName() << endl;

	    cu_assert(0);
	}

    }
    cu_assert(nIfcsPassed == IfcCount);

    delete tree;
}



void TestSite::testCopy()
{
    // verify that copy constructor & assignment op are disabled
    // these should cause compilation errors

#if 0
    // copy constructor
    Site siteCopy(*site_);
 
    // assignment op
    Site siteAssign;
    siteAssign = *site_;

#endif
}





Test *TestSite::suite ()
{
	TestSuite *testSuite = new TestSuite("TestSite");

	testSuite->addTest (new TestCaller <TestSite> ("testDxStatus", &TestSite::testDxStatus));
	testSuite->addTest (new TestCaller <TestSite> ("testGetNssComponentTree", &TestSite::testGetNssComponentTree));
	testSuite->addTest (new TestCaller <TestSite> ("testGetDxsForIfc", &TestSite::testGetDxsForIfc));
	testSuite->addTest (new TestCaller <TestSite> ("testCopy", &TestSite::testCopy));

	return testSuite;
}