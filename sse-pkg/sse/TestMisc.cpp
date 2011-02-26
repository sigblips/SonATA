/*******************************************************************************

 File:    TestMisc.cpp
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
#include "TestRunner.h"
#include "TestMisc.h"
#include "ScienceDataArchive.h"
#include "ObserveActivityStatus.h"
#include "ObsSummaryStats.h"
#include "ExpectedNssComponentsTree.h"
#include "CondensedSignalReport.h"
#include "ExpandedSignalReport.h"
#include "MutexBool.h"
#include "MinMaxBandwidth.h"
#include "SseAstro.h"
#include "SseArchive.h"
#include "TargetPosition.h"
#include "Target.h"
#include "SharedTclProxy.h"
#include "ActUnitListMutexWrapper.h"
#include "AtaInformation.h"
#include <string>
#include <list>

void TestMisc::setUp ()
{

}

void TestMisc::tearDown()
{

}

void TestMisc::testScienceDataArchive()
{

}

void TestMisc::testObsSummary()
{
    cout << "testObsSummary" << endl;

    ObsSummaryStats stats;
    stats.confirmedCwCandidates = 12;

    ObsSummaryStats stats2;
    stats2.confirmedCwCandidates = 10;

    stats += stats2;
    cout << stats;

    cu_assert(stats.confirmedCwCandidates == 22);

    //test class reason counters

    // should be zero to start
    cu_assert(stats.getClassReasonCount(ZERO_DRIFT) == 0);

    // try an increment
    stats.incrementClassReasonCount(ZERO_DRIFT);
    cu_assert(stats.getClassReasonCount(ZERO_DRIFT) == 1);

    // try an increment
    stats.incrementClassReasonCount(BIRDIE_SCAN);
    stats.incrementClassReasonCount(BIRDIE_SCAN);
    cu_assert(stats.getClassReasonCount(BIRDIE_SCAN) == 2);

    // try an out of range reason value, should trigger an assert failure
    //cu_assert(stats.getClassReasonCount(static_cast<SignalClassReason>(9999)) == 0);

    stats.printInExpandedFormat(cout);

}

void TestMisc::testParamPrint()
{
    cout << "testParamPrint" << endl;

    // AP para; // TBD needs parameters

    // cout << para;


}

// load some dummy signal info for testing
static void setSignalDescription(SignalDescription &sig)
{
    sig.path.rfFreq = -1;  // MHz
    sig.subchannelNumber = 1024;
    sig.signalId.number = 777333; 
    sig.path.drift = 0.5;      // Hz/sec
    sig.path.width = 1.5;      // Hz
    sig.path.power = 100;;      // Janskys (?)
    sig.pol = POL_BOTH;
    sig.sigClass = CLASS_UNKNOWN; // signal classification
    sig.reason = SEEN_OFF;   // reason for signal classification
}

static void setConfirmStats(ConfirmationStats &stats)
{
    stats.pfa = -21.001;
    stats.snr = 55.2;
}

void TestMisc::testCondensedSignalReport()
{
    cout << "testCondensedSignalReport" << endl;

    string activityName("testAct");
    int activityId(1234);
    string dxName("dxsim1");
    string reportType("condensed candidates");
    string dxTuningInfo("# tune line1\n# tune line2\n");

    CondensedSignalReport report(activityName,
				 activityId,
				 dxName,
				 reportType,
				 dxTuningInfo);

    // add some signals to the report,
    // enough to force more than one page.

    int nSignals = 2 * report.getPageSize() + 10;
    //int nSignals = 12;
    int nSignalTypes = 3;
    int sigId = 0;
    for (int i =0; i< nSignals/nSignalTypes; ++i)
    {
	CwPowerSignal cwp;
	setSignalDescription(cwp.sig);
        cwp.sig.signalId.number = sigId++;
	cwp.sig.path.rfFreq = 1234.0;
	cwp.sig.reason = RFI_SCAN;
	report.addSignal(cwp);

	CwCoherentSignal cwCoherent;
	setSignalDescription(cwCoherent.sig);
        cwCoherent.sig.signalId.number = sigId++;
	cwCoherent.sig.path.rfFreq = 10010.558; 
	cwCoherent.sig.reason = SEEN_OFF;
	setConfirmStats(cwCoherent.cfm);
	report.addSignal(cwCoherent);

	PulseSignalHeader pulseHdr;  // pulse signal has a variable length
	setSignalDescription(pulseHdr.sig);
        pulseHdr.sig.signalId.number = sigId++;
	pulseHdr.sig.path.rfFreq = 1420.1234567876;
	pulseHdr.sig.reason = SNR_TOO_HIGH;
	setConfirmStats(pulseHdr.cfm);
	report.addSignal(pulseHdr);
    }

    cout << report;

    // test saveToFile

    string testFile("/tmp/condensedSignalReport.txt");
    remove(testFile.c_str());  // make sure not already there
    cu_assert(report.saveToFile(testFile));
    remove(testFile.c_str());  // clean up


}

void TestMisc::testExpandedSignalReport()
{
    cout << "testExpandedSignalReport" << endl;

    string activityName("testAct");
    int activityId(1234);
    string dxName("dxsim1");
    string reportType("expanded candidates");
    string dxTuningInfo("# tune line1\n# tune line2\n");


    ExpandedSignalReport report(activityName,
				activityId,
				dxName,
				reportType,
				dxTuningInfo);

    PulseSignalHeader pulseHdr;  // pulse signal has a variable length
    pulseHdr.sig.containsBadBands = SSE_TRUE;
    setSignalDescription(pulseHdr.sig);
    pulseHdr.sig.path.rfFreq = 1420.1234567837;
    pulseHdr.sig.reason = SNR_TOO_HIGH;
    setConfirmStats(pulseHdr.cfm);

    int nPulses = 3;

    // PulseTrainDescription
    pulseHdr.train.pulsePeriod = 0.15;
    pulseHdr.train.numberOfPulses = nPulses;
    pulseHdr.train.res = RES_2HZ;

    // define the pulses in the train

    Pulse pulses[nPulses];
    for (int i=0; i < nPulses; ++i)
    {
        pulses[i].rfFreq = 2002.5;
        pulses[i].power = 10.5;
        pulses[i].spectrumNumber = i;
        pulses[i].binNumber = 10+i;
    }

    int nSignals = 5;
    for (int i=0; i< nSignals; ++i)
    {
	report.addSignal(pulseHdr, pulses);
    }

    report.addText("------ This is an 'addedText' separator ------\n\n");
    report.addSignal(pulseHdr, pulses);

    report.addText("------ This is a last line 'addedText' separator ------\n");

    cout << report;

    // test saveToFile

    string testFile("/tmp/expandedSignalReport.txt");
    remove(testFile.c_str());  // make sure not already there
    cu_assert(report.saveToFile(testFile));
    remove(testFile.c_str());  // clean up


}



void TestMisc::testLogging()
{
    cout << "testLogging" << endl;

    SseArchive::ErrorLog() << "TestMisc: Error log test message" << endl;

    SseArchive::SystemLog() << "TestMisc: System log test message" << endl;

}

void TestMisc::testObserveActivityStatus()
{
    ObserveActivityStatus status;

    // verify initial value
    cu_assert(status.get() == ObserveActivityStatus::ACTIVITY_CREATED);

    // try a set & get
    status.set(ObserveActivityStatus::ACTIVITY_COMPLETE);
    cu_assert(status.get() == ObserveActivityStatus::ACTIVITY_COMPLETE);

    cu_assert(status.getString() == "Act Complete");
}


void TestMisc::testExpectedNssComponentsTree()
{
    cout << "testExpectedNssComponentsTree" << endl;

    // create a test config array (simulate a config file)
    vector<string> config;
    config.push_back("sonata expected components v1.0");
    config.push_back("# A comment");
    config.push_back("Site Site1 IfcList ifc1 ifc2 ifc3");
    config.push_back("Ifc ifc1 BeamList beam1");
    config.push_back("Ifc ifc2 BeamList beam2 beam3");
    config.push_back("Ifc ifc3 BeamList beam4");
    config.push_back("Beam beam1 DxList dx0 dx1 dx2");
    config.push_back("Beam beam2 DxList dx3");
    config.push_back("Beam beam3 DxList dx4");
    config.push_back("Beam beam4 DxList dx5 dx6 dx7");
    config.push_back("");
    config.push_back("BeamToAtaBeams beam1 beamxa1 beamya1");
    config.push_back("BeamToAtaBeams beam2 beamxa2 beamya2");
    config.push_back("BeamToAtaBeams beam3 beamxa3 beamya3");
    config.push_back("BeamToAtaBeams beam4 beamxd4");

    // put it into a config file
    string filename("/tmp/testExpectedConfigFile.txt");
    remove(filename.c_str());     // make sure it's not already there

    // open an output text stream attached to the file
    ofstream strm;
    strm.open(filename.c_str(), (ios::app));
    cu_assert(strm.is_open());

    // send text to the file
    for (size_t i=0; i<config.size(); ++i)
    {
        strm << config[i] << endl;
    }
    strm.close();

    // load in the config file, creating the components tree
    stringstream errorStrm;
    ExpectedNssComponentsTree tree(filename, errorStrm);

    // should be no errors, print any that appear
    cerr << errorStrm.str();  
    cu_assert(errorStrm.str() == "");

    cout << tree;

    // get all the components
    vector<string> componentsAll(tree.getAllComponents());
    cout << "componentsAll.size(): " << componentsAll.size() << endl;

    cu_assert (componentsAll.size() == 16);
    cu_assert (find(componentsAll.begin(), componentsAll.end(), "ifc1") != componentsAll.end());
    cu_assert (find(componentsAll.begin(), componentsAll.end(), "Site1") != componentsAll.end());

    // get the sites
    vector<string> sitesAll(tree.getSites());
    cu_assert (sitesAll.size() == 1);
    cu_assert (sitesAll[0] == "Site1");
    
    // get the ifcs
    vector<string> ifcsAll(tree.getIfcs());
    cu_assert (ifcsAll.size() == 3);
    cu_assert (find(ifcsAll.begin(), ifcsAll.end(), "ifc1") != ifcsAll.end());
    cu_assert (find(ifcsAll.begin(), ifcsAll.end(), "ifc2") != ifcsAll.end());
    cu_assert (find(ifcsAll.begin(), ifcsAll.end(), "ifc3") != ifcsAll.end());
    
    // get the dxs
    vector<string> dxsAll(tree.getDxs());
    cu_assert (dxsAll.size() == 8);
    cu_assert (find(dxsAll.begin(), dxsAll.end(), "dx0") != dxsAll.end());
    cu_assert (find(dxsAll.begin(), dxsAll.end(), "dx7") != dxsAll.end());
    
    // get the beams 
    vector<string> beams(tree.getBeams());
    cu_assert (beams.size() == 4);
    cu_assert (find(beams.begin(), beams.end(), "beam1") != beams.end());
    cu_assert (find(beams.begin(), beams.end(), "beam4") != beams.end());

    // get the dxsForBeam for a beam
    vector<string> dxsForBeam(tree.getDxsForBeam("beam1"));
    cu_assert (dxsForBeam.size() == 3);
    cu_assert (find(dxsForBeam.begin(), dxsForBeam.end(), "dx0") != dxsForBeam.end());
    cu_assert (find(dxsForBeam.begin(), dxsForBeam.end(), "dx2") != dxsForBeam.end());

    
    // get the dxs for an ifc
    vector<string> dxs(tree.getDxsForIfc("ifc1"));
    cu_assert (dxs.size() == 3);
    cu_assert (find(dxs.begin(), dxs.end(), "dx0") != dxs.end());
    cu_assert (find(dxs.begin(), dxs.end(), "dx2") != dxs.end());

    
    // get the beams for an ifc
    vector<string> beamsForIfc(tree.getBeamsForIfc("ifc2"));
    cu_assert (beamsForIfc.size() == 2);
    cu_assert (find(beamsForIfc.begin(), beamsForIfc.end(), "beam2") != beamsForIfc.end());
    cu_assert (find(beamsForIfc.begin(), beamsForIfc.end(), "beam3") != beamsForIfc.end());


    // get the ifcs for a site
    vector<string> ifcsForSite = tree.getIfcsForSite("Site1");
    cu_assert (ifcsForSite.size() == 3);
    cu_assert (find(ifcsForSite.begin(), ifcsForSite.end(), "ifc1") != ifcsForSite.end());
    cu_assert (find(ifcsForSite.begin(), ifcsForSite.end(), "ifc2") != ifcsForSite.end());
    cu_assert (find(ifcsForSite.begin(), ifcsForSite.end(), "ifc3") != ifcsForSite.end());

    // get the dxs for a site
    vector<string> dxsForSite(tree.getDxsForSite("Site1"));
    cu_assert (dxsForSite.size() == 8);
    cu_assert (find(dxsForSite.begin(), dxsForSite.end(), "dx0") != dxsForSite.end());
    cu_assert (find(dxsForSite.begin(), dxsForSite.end(), "dx4") != dxsForSite.end());

    // get the beam to atabeam associations
    vector<string> ataBeams(tree.getAtaBeamsForBeam("beam1"));
    cu_assert(ataBeams.size() == 2);
    cu_assert(find(ataBeams.begin(), ataBeams.end(), "beamxa1") != ataBeams.end());
    cu_assert(find(ataBeams.begin(), ataBeams.end(), "beamya1") != ataBeams.end());

    ataBeams = tree.getAtaBeamsForBeam("beam4");
    cu_assert(ataBeams.size() == 1);
    cu_assert(find(ataBeams.begin(), ataBeams.end(), "beamxd4") != ataBeams.end());

    // get all ata beams
    ataBeams = tree.getAtaBeams();
    cu_assert(ataBeams.size() == 7);
    cu_assert(find(ataBeams.begin(), ataBeams.end(), "beamxa1") != ataBeams.end());
    cu_assert(find(ataBeams.begin(), ataBeams.end(), "beamxd4") != ataBeams.end());

    // misc dx lookups
    cu_assert(tree.getBeamForDx("dx0") == "beam1");
    cu_assert(tree.getIfcForDx("dx0") == "ifc1");

    // clean up
    remove(filename.c_str());    
}



void TestMisc::testExpectedNssComponentsTreeErrors()
{
    cout << "testExpectedNssComponentsTreeErrors" << endl;

    // try a variety of incorrect configuration entries

    vector<string> expectedErrors;

    // create a test config array (simulate a config file)
    vector<string> config;
    config.push_back("   sonata expected components v1.0 # a test  ");  // extra white space & comment
    config.push_back("# A comment");
    config.push_back("#A comment with no space after the #.");

    // a component has a child listed (ifcNotHere)
    // but the child has no later entry of its own
    config.push_back("Site Site1 IfcList ifc0 ifc1 ifcNotHere");
    expectedErrors.push_back("Subcomponent 'ifcNotHere' was listed under component 'Site1' but it has no entry of its own.");


    // these should all be ok
    config.push_back("Ifc ifc0 BeamList beam0");
    config.push_back("Beam beam0 DxList dx0 dx1 dx2");
    config.push_back("Ifc ifc1 BeamList beam1 beam2");
    config.push_back("Beam beam1 DxList dx3 dx4 dx5");

    // some junk lines.
    config.push_back("This is junk.");
    expectedErrors.push_back("Invalid Component Type: 'This'");

    config.push_back("Another longer line of junk.");
    expectedErrors.push_back("Invalid Component Type: 'Another'");

    // a partial component line
    config.push_back("Ifc fred");
    expectedErrors.push_back("Missing component information: Ifc fred");

    // invalid subcomponent list type
    config.push_back("Ifc ifcfoo FooList beam9");
    expectedErrors.push_back("Invalid Subcomponent Type List: 'FooList'");

    // a component without a parent
    config.push_back("Ifc ifcNoParent BeamList beam5 beam6");
    expectedErrors.push_back("ExpectedNssComponents Error: SubComponent 'ifcNoParent' has no parent component.");

    // a component that is not of the expected subtype.
    // (it doesn't match the subtype for the parent it was listed in).
    // ie, beam2 should be of Beam type, not Ifc type

    config.push_back("Ifc beam2 BeamList beamFoo");
    expectedErrors.push_back("ExpectedNssComponents Error: Subcomponent 'beam2' was listed under component 'ifc1' but its type 'Ifc' is incompatible with its parent's type.");

    // a repeated component (ifc1)
    config.push_back("Ifc ifc1 BeamList beam14");
    expectedErrors.push_back("Duplicate component: ifc1");

    // a component with repeated children on its own list
    config.push_back("Ifc ifc2 BeamList beam8 beam8");
    expectedErrors.push_back("Duplicate subcomponent: beam8");

    // a component with the same children as another
    config.push_back("Ifc ifc2 BeamList beam0");
    expectedErrors.push_back("Duplicate subcomponent: beam0");

    // a component - subcomponent type mismatch
    // (ifc should have a BeamList)
    config.push_back("Ifc ifc3 DxList dx12");
    expectedErrors.push_back("Invalid Subcomponent Type List: 'DxList'");


    // beam to ata beam associations
    // -----------------------------
    // insufficient number of tokens
    config.push_back("BeamToAtaBeams beam5");
    expectedErrors.push_back("Incorrect number of tokens for BeamToAtaBeams: BeamToAtaBeams beam5");

    // repeated beam to atabeam association
    config.push_back("BeamToAtaBeams beam1 fred barney");
    config.push_back("BeamToAtaBeams beam1 fred barney");
    expectedErrors.push_back("Repeated beam for BeamToAtaBeams: beam1");

    // repeated ata beam
    config.push_back("BeamToAtaBeams beam2 fred");
    expectedErrors.push_back("Repeated ata beam for BeamToAtaBeams: fred");

    // ------
    // parse the configuration, check for expected errors
    stringstream errorStrm;
    ExpectedNssComponentsTree tree(config, errorStrm);
    cu_assert(errorStrm.str() != "");

    cout << tree;

    // check that each expected error made it into the
    // error stream

    // TBD check that no unexpected errors were
    // returned

    string returnedErrorText(errorStrm.str());
    cout << returnedErrorText << endl;

    for(vector<string>::iterator it = expectedErrors.begin();
	it != expectedErrors.end(); ++it)
    {
	const string & expectedErrorLine = *it;
	if (returnedErrorText.find(expectedErrorLine) == 
	    string::npos)
	{
	    cerr << "Expected error line not found: " 
		 << expectedErrorLine << endl;
	    cu_assert(0);
	}
    }


    // get the sites
    vector<string> sites(tree.getSites());
    cu_assert (sites.size() == 1);
    cu_assert (sites[0] == "Site1");

/*    
    // get the ifcs
    vector<string> ifcs(tree.getIfcs());
    cu_assert (ifcs.size() == 2);
    cu_assert (find(ifcs.begin(), ifcs.end(), "ifc0") != ifcs.end());
    cu_assert (find(ifcs.begin(), ifcs.end(), "ifc1") != ifcs.end());
*/  


}


void TestMisc::testMutexBool()
{
    cout << "testMutexBool" << endl;

    MutexBool bool1;

    // check the default
    cu_assert(bool1.get() == false);

    // check set/get
    bool1.set(true);
    cu_assert(bool1.get() == true);

}

class BandwidthComponent
{
public:

    BandwidthComponent(double bandwidthMhz)
	: bandwidthMhz_(bandwidthMhz)
    {
    }

    double getBandwidthInMHz() const
    {
	return bandwidthMhz_;
    }

private:
    
    double bandwidthMhz_;
};

void TestMisc::testMinMaxBandwidth()
{
    cout << "testMinMaxBandwidth" << endl;

    BandwidthComponent bw1(10), bw2(20);
    typedef list<BandwidthComponent *> Bwlist;

    Bwlist bwlist;

    bwlist.push_back(&bw1);
    bwlist.push_back(&bw2);

    double difftol = 0.1;

    double expectedminbw = 10;
    double minbw = MinBandwidth<Bwlist::const_iterator, BandwidthComponent>
	(bwlist.begin(), bwlist.end());
    assertDoublesEqual(expectedminbw, minbw, difftol);

    double expectedmaxbw = 20;
    double maxbw = MaxBandwidth<Bwlist::const_iterator, BandwidthComponent>
	(bwlist.begin(), bwlist.end());
    assertDoublesEqual(expectedmaxbw, maxbw, difftol);

    
    // try an empty list, make sure bw comes back as zero
    Bwlist emptylist;

    expectedminbw = 0;
    minbw =  MinBandwidth<Bwlist::const_iterator, BandwidthComponent>
	(emptylist.begin(), emptylist.end());
    assertDoublesEqual(expectedminbw, minbw, difftol);

    expectedmaxbw = 0;
    maxbw =  MaxBandwidth<Bwlist::const_iterator, BandwidthComponent>
	(emptylist.begin(), emptylist.end());
    assertDoublesEqual(expectedmaxbw, maxbw, difftol);

}


class TestSharedTclProxy : public SharedTclProxy
{
protected:
   void processIndividualMessage(const string & message);

   vector<string> getStoredMessages();
   void clearStoredMessages();

private:   
   friend class TestMisc;
   vector<string> individualMessages_;
};

vector<string> TestSharedTclProxy::getStoredMessages()
{
   return individualMessages_;
}

void TestSharedTclProxy::clearStoredMessages()
{
   individualMessages_.clear();
}

void TestSharedTclProxy::processIndividualMessage(const string & message)
{
   individualMessages_.push_back(message);
}

void TestMisc::testSharedTclProxy()
{
   cout << "testSharedTclProxy" << endl;
   const string terminator("<end>");
   
   TestSharedTclProxy * proxy = new TestSharedTclProxy();

   // single message, all in once piece
   const string msg1 = "the rain in spain";
   proxy->handleIncomingMessage(msg1 + terminator);

   vector<string> msg1Result = proxy->getStoredMessages();
   cu_assert(msg1Result.size() == 1);
   cu_assert(msg1Result.front() == msg1);
   proxy->clearStoredMessages();

   // single message, split into two incoming parts
   const string msg2part1 = "the longest ";
   proxy->handleIncomingMessage(msg2part1);
   const string msg2part2 = "journey";
   proxy->handleIncomingMessage(msg2part2 + terminator);

   vector<string> msg2Result = proxy->getStoredMessages();
   cu_assert(msg2Result.size() == 1);
   cu_assert(msg2Result.front() == msg2part1 + msg2part2);
   proxy->clearStoredMessages();

   // two messages at once (multiple terminators in
   // a single message).  Also include a third blank message,
   // which should be ignored
   const string msg3part1 = "the first message";
   const string msg3part2 = "the second message";
   const string msg3part3 = "  "; // blank message;
   stringstream msg3strm;
   msg3strm << msg3part1 << terminator
	    << msg3part2 << terminator
	    << msg3part3 << terminator;
   proxy->handleIncomingMessage(msg3strm.str());

   vector<string> msg3Result = proxy->getStoredMessages();
   cu_assert(msg3Result.size() == 2);
   cu_assert(msg3Result[0] == msg3part1);
   cu_assert(msg3Result[1] == msg3part2);
   proxy->clearStoredMessages();

   // one and a half messages, then the remainder of the second message
   const string msg4part1 = "red sky at night";
   const string msg4part2 = "sailor's ";

   stringstream msg4strm;
   msg4strm << msg4part1 << terminator
	    << msg4part2;
   proxy->handleIncomingMessage(msg4strm.str());
   
   const string msg4part3 = "delight";
   proxy->handleIncomingMessage(msg4part3 + terminator);
   
   vector<string> msg4Result = proxy->getStoredMessages();
   cu_assert(msg4Result.size() == 2);
   cu_assert(msg4Result[0] == msg4part1);
   cu_assert(msg4Result[1] == msg4part2 + msg4part3);
   proxy->clearStoredMessages();

   delete proxy;
}

void TestMisc::testActUnitListMutexWrapper()
{
   cout << "testActUnitListMutexWrapper" << endl;

   ActUnitListMutexWrapper wrapper1;

   ActivityUnit *actUnit1 = reinterpret_cast<ActivityUnit *>(0x01);
   ActivityUnit *actUnit2 = reinterpret_cast<ActivityUnit *>(0x02);

   wrapper1.addToList(actUnit1);
   wrapper1.addToList(actUnit2);

   cu_assert(wrapper1.getListCopy().size() == 2);
   cu_assert(wrapper1.listSize() == 2);
   
   // test copy ctor
   ActUnitListMutexWrapper wrapper2(wrapper1);
   cu_assert(wrapper2.getListCopy() == wrapper1.getListCopy());

   // test assign op
   ActUnitListMutexWrapper wrapper3 = wrapper1;
   cu_assert(wrapper3.getListCopy() == wrapper1.getListCopy());

   wrapper1.removeFromList(actUnit1);
   cu_assert(wrapper1.getListCopy().size() == 1);
   cu_assert(wrapper1.getListCopy().front() == actUnit2);

   // overwrite with actUnitList
   ActUnitListMutexWrapper wrapper4;
   wrapper4 = wrapper1.getListCopy();
   cu_assert(wrapper4.getListCopy() == wrapper1.getListCopy());


   
}

void TestMisc::testAtaBeamsize()
{
    cout << "testAtaBeamsize" << endl;

    // Test beamsize calculation for ATA350

    static const double Ata350BeamsizeAt1GhzArcSec = 110.76;
    static const double ArcSecPerDeg = 3600;

    // Try exactly 1 GHz
    double reqFreqMhz = 1000;
    double beamsizeRads = AtaInformation::ataBeamsizeRadians(
        reqFreqMhz, Ata350BeamsizeAt1GhzArcSec);

    double expectedBeamsizeRads = 
       SseAstro::degreesToRadians(Ata350BeamsizeAt1GhzArcSec / 
				 ArcSecPerDeg);

#if 0
    cout << "freqMhz=" << reqFreqMhz 
         << " expectedBeamsizeRads=" << expectedBeamsizeRads
         << " beamsizeRads=" << beamsizeRads << endl;
#endif

    double tolRadians = 0.000001;
    assertDoublesEqual(expectedBeamsizeRads, beamsizeRads, tolRadians); 

    // -----------------------------------------

    // try the high end of the band (11.12Ghz)
    reqFreqMhz = 11120;
    const double MhzPerGhz(1000.0);
    double reqFreqGhz(reqFreqMhz / MhzPerGhz);

    static const double expectedAta350BeamsizeAt11120MhzArcSec(
       Ata350BeamsizeAt1GhzArcSec / reqFreqGhz);

    beamsizeRads = AtaInformation::ataBeamsizeRadians(
         reqFreqMhz, Ata350BeamsizeAt1GhzArcSec);
    
    expectedBeamsizeRads = SseAstro::degreesToRadians(
       expectedAta350BeamsizeAt11120MhzArcSec / ArcSecPerDeg);

#if 0    
    cout <<  "freqMhz=" << reqFreqMhz 
         << " expectedBeamsizeRads=" << expectedBeamsizeRads
         << " beamsizeRads=" << beamsizeRads << endl;
#endif

    assertDoublesEqual(expectedBeamsizeRads, beamsizeRads, tolRadians); 

}


Test *TestMisc::suite ()
{
	TestSuite *testSuite = new TestSuite("TestMisc");

	testSuite->addTest (new TestCaller <TestMisc> ("testScienceDataArchive", &TestMisc::testScienceDataArchive));
	testSuite->addTest (new TestCaller <TestMisc> ("testObsSummary", &TestMisc::testObsSummary));
	testSuite->addTest (new TestCaller <TestMisc> ("testParamPrint", &TestMisc::testParamPrint));
	testSuite->addTest (new TestCaller <TestMisc> ("testLogging", &TestMisc::testLogging));
	testSuite->addTest (new TestCaller <TestMisc> ("testObserveActivityStatus", &TestMisc::testObserveActivityStatus));
        testSuite->addTest (new TestCaller <TestMisc> ("testExpectedNssComponentsTree", &TestMisc::testExpectedNssComponentsTree));
        testSuite->addTest (new TestCaller <TestMisc> ("testExpectedNssComponentsTreeErrors", &TestMisc::testExpectedNssComponentsTreeErrors));
        testSuite->addTest (new TestCaller <TestMisc> ("testExpectedNssComponentsTreeErrors", &TestMisc::testCondensedSignalReport));
        testSuite->addTest (new TestCaller <TestMisc> ("testExpectedNssComponentsTreeErrors", &TestMisc::testExpandedSignalReport));
        testSuite->addTest (new TestCaller <TestMisc> ("testMutexBool", &TestMisc::testMutexBool));
        testSuite->addTest (new TestCaller <TestMisc> ("testMinMaxBandwidth", &TestMisc::testMinMaxBandwidth));
        testSuite->addTest (new TestCaller <TestMisc> ("testSharedTclProxy", &TestMisc::testSharedTclProxy));
        testSuite->addTest (new TestCaller <TestMisc> ("testActUnitListMutexWrapper", &TestMisc::testActUnitListMutexWrapper));
        testSuite->addTest (new TestCaller <TestMisc> ("testAtaBeamsize", &TestMisc::testAtaBeamsize));

	return testSuite;
}







