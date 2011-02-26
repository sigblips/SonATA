/*******************************************************************************

 File:    NssComponentTree.cpp
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


#include "NssComponentTree.h" 
#include "ExpectedNssComponentsTree.h"
#include "Assert.h"
#include "IfcProxy.h"
#include "DxProxy.h"
#include "ChannelizerProxy.h"
#include "TscopeProxy.h"
#include "DebugLog.h"
#include <map>
#include <algorithm>
#include <sstream>

using namespace std;

typedef map<IfcProxy *, DxList *> DxListForIfcMap;

// Note: dx and ifcs lists are sorted by name, so that they are accessed
// in a predictable order (eg, when tune frequencies are assigned).

static bool CompareChansByName(ChannelizerProxy *proxy1, ChannelizerProxy *proxy2)
{
    return proxy1->getName() < proxy2->getName();
}

static bool CompareDxsByName(DxProxy *proxy1, DxProxy *proxy2)
{
    return proxy1->getName() < proxy2->getName();
}

static bool CompareIfcsByName(IfcProxy *proxy1, IfcProxy *proxy2)
{
    return proxy1->getName() < proxy2->getName();
}

struct NssComponentTreeInternal
{
    NssComponentTreeInternal();
    ~NssComponentTreeInternal();

    DxList & getDxListForIfc(IfcProxy *ifcProxy);

    void createDxListsForIfcs();
    void getDxsConnectedToIfc(IfcProxy *ifcProxy, DxList &outputDxList);

    // data

    IfcList ifcList_;
    DxList dxList_;
    ChannelizerList chanList_;
    TscopeList tscopeList_;
    TestSigList testSigList_;

    ExpectedNssComponentsTree *expectedNssComponentsTree_;

    DxListForIfcMap dxListForIfc_;

};

NssComponentTreeInternal::NssComponentTreeInternal()
{
}

NssComponentTreeInternal::~NssComponentTreeInternal()
{
    // delete DxLists from the DxListForIfcMap

    for (DxListForIfcMap::iterator it=dxListForIfc_.begin();
	 it != dxListForIfc_.end(); ++it)
    {
	DxList *dxList = it->second;
	delete dxList;
    }

}
void NssComponentTreeInternal::createDxListsForIfcs()
{
    // create a DxList for each ifc, based on the expectedNssComponentsTree
    // information

    for (IfcList::iterator it = ifcList_.begin(); 
	 it != ifcList_.end(); it++)
    {
	IfcProxy *ifcProxy = *it;
	DxList *dxList = new DxList;
	
	getDxsConnectedToIfc(ifcProxy, *dxList);

	dxList->sort(CompareDxsByName);

	// ifcProxy shouldn't already be in the map
	Assert(dxListForIfc_.find(ifcProxy) == dxListForIfc_.end());

	dxListForIfc_[ifcProxy] = dxList;
	
    }

}


// Find the dx proxies that are connected to the given
// ifc, and store them in the outputDxList

void NssComponentTreeInternal::getDxsConnectedToIfc(IfcProxy *ifcProxy,
						     DxList &outputDxList)
{

    // First, use the name of the ifcProxy to look up the
    // names of the dxs that are associated with it.

    Assert(ifcProxy);
    string ifcName = ifcProxy->getName();

    Assert(expectedNssComponentsTree_);
    vector<string> expectedDxNames =
	expectedNssComponentsTree_->getDxsForIfc(ifcName);

#if 0
    // debug: print the names
    cout << "dxs expected to be attached to ifc: " << ifcName << endl;
    for (vector<string>::iterator it = expectedDxNames.begin();
	 it != expectedDxNames.end(); ++it)
    {
	cout << *it << " ";
    }
    cout << endl;
#endif 

    // Now go through the dx proxy list.
    // For each proxy whose name is on the expectedDxNames list, add
    // it to the output proxy list.

    DxList & dxProxyList = dxList_;
    for (DxList::iterator it = dxProxyList.begin();
	 it != dxProxyList.end(); ++it)
    {
	DxProxy *dxProxy = *it;
	if (find(expectedDxNames.begin(), expectedDxNames.end(),
		 dxProxy->getName()) != expectedDxNames.end())
	{
	    outputDxList.push_back(dxProxy);
	}    
    }


}


DxList & NssComponentTreeInternal::getDxListForIfc(IfcProxy *ifcProxy)
{
   // make sure ifcProxy is in the map
   Assert(dxListForIfc_.find(ifcProxy) != dxListForIfc_.end());

   return *dxListForIfc_[ifcProxy];
}



// ---- end NssComponentTreeInternal ----
// ---- begin NssComponentTree ----

NssComponentTree::NssComponentTree(const DxList &dxList,
                                   const ChannelizerList & chanList,
				   const IfcList &ifcList,
				   const TscopeList &tscopeList,
				   const TestSigList &testSigList,
				   ExpectedNssComponentsTree *expectedNssComponentsTree)
    : internal_(new NssComponentTreeInternal())
{

    // copy in all the proxy pointers

    // DX
    internal_->dxList_ = dxList;
    internal_->dxList_.sort(CompareDxsByName);

    // CHANNELIZER
    internal_->chanList_ = chanList;
    internal_->chanList_.sort(CompareChansByName);

    // IFC
    internal_->ifcList_ = ifcList;
    internal_->ifcList_.sort(CompareIfcsByName);

    // tscope
    internal_->tscopeList_ = tscopeList;

    // testSig
    internal_->testSigList_ = testSigList;

    // store a pointer to the expectedNssComponentsTree_
    internal_->expectedNssComponentsTree_ = expectedNssComponentsTree; 

    // create a Dxlist for each ifc, based on the expectedNssComponentsTree
    // information
    internal_->createDxListsForIfcs();

}

NssComponentTree::~NssComponentTree()
{
    delete internal_;
}

//-- Dx ---
DxList & NssComponentTree::getDxs()
{
    return internal_->dxList_;
}

// Return a list of dx proxies that are on the same IF chain
// as the given ifcProxy

DxList & NssComponentTree::getDxsForIfc(IfcProxy *ifcProxy)
{
    return internal_->getDxListForIfc(ifcProxy);
}


ChannelizerList & NssComponentTree::getChans()
{
    return internal_->chanList_;
}


//-- Ifc ---
IfcList & NssComponentTree::getIfcs()
{
    return internal_->ifcList_;
}

// -- Tscope --
TscopeList & NssComponentTree::getTscopes()
{
    return internal_->tscopeList_;
}

// -- TestSig -- 

TestSigList & NssComponentTree::getTestSigs()
{
    return internal_->testSigList_;
}

ExpectedNssComponentsTree * NssComponentTree::getExpectedNssComponentsTree()
{
    return internal_->expectedNssComponentsTree_;
}

IfcList NssComponentTree::getIfcsForBeams(std::vector<std::string> beamNames)
{
    IfcList localIfcList;

    // For each ifc, get its beam(s).  If they're on the incoming
    // list, then add the ifc to the local list, eliminating any 
    // duplicates.  Return the ifc list by value.

    for (IfcList::iterator it = internal_->ifcList_.begin(); 
	 it != internal_->ifcList_.end(); ++it)
    {
	IfcProxy *ifcProxy = *it;

	vector<string> beamNamesForIfc(internal_->expectedNssComponentsTree_->getBeamsForIfc(ifcProxy->getName()));
				   
	for (vector<string>::iterator beamNamesForIfcIt = beamNamesForIfc.begin();
	     beamNamesForIfcIt != beamNamesForIfc.end(); ++beamNamesForIfcIt)
	{
	    string & beam = *beamNamesForIfcIt;

	    if (find(beamNames.begin(), beamNames.end(), beam)
		!= beamNames.end())
	    {
		localIfcList.push_back(ifcProxy);
		break;  // no need to look for any more beams on this ifc
	    }
	}
    }

    return localIfcList;

}


DxList NssComponentTree::getDxsForBeams(std::vector<std::string> beamNames)
{
    DxList localDxList;

    // Go through list of dx proxies.  For each one whose beam is
    // on the incoming beamNames argument, add it to the local list.

    for (DxList::iterator it = internal_->dxList_.begin();
	 it != internal_->dxList_.end(); ++it)
    {
	DxProxy *dxProxy = *it;

	string beamForDx = internal_->expectedNssComponentsTree_->getBeamForDx(dxProxy->getName());

	if (find(beamNames.begin(), beamNames.end(), beamForDx)
	    != beamNames.end())
	{
	    localDxList.push_back(dxProxy);
	}
    }

    return localDxList;
}




ChannelizerList NssComponentTree::getChansForBeams(std::vector<std::string> beamNames)
{
    ChannelizerList localChannelizerList;

    // Go through list of chan proxies.  For each one whose beam is 
    // on the incoming beamNames argument, add it to the local list
    for (ChannelizerList::iterator it = internal_->chanList_.begin();
	 it != internal_->chanList_.end(); ++it)
    {
	ChannelizerProxy *chanProxy = *it;
//cout << "Chan Name " << chanProxy->getName() << endl;
	string beamForChan = internal_->expectedNssComponentsTree_->getBeamForChan(chanProxy->getName());
//cout << "Beam " << beamForChan << endl;
	if (find(beamNames.begin(), beamNames.end(), beamForChan)
	    != beamNames.end())
	{
	    localChannelizerList.push_back(chanProxy);
	}
    }

    return localChannelizerList;
}


