/*******************************************************************************

 File:    TestTscope.cpp
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
#include "ace/SOCK_Connector.h" 
#include "TestTscope.h"
#include "TestRunner.h"
#include "Tscope.h"
#include "SseProxy.h"
#include "TscopeCmdLineArgs.h"
#include "ArrayLength.h"
#include "SseTscopeMsg.h"
#include "SseException.h"
#include <sstream>

void TestTscope::setUp()
{
   dummyAceStream_ = new ACE_SOCK_STREAM;
   sseProxy_ = new SseProxy(*dummyAceStream_);

   const string antControlServerName("localhost");
   const int controlPort = 1083;  //Prelude is 1081
   const int monitorPort = 1084;  //Prelude is 1082

   tscope_ = new Tscope(*sseProxy_, antControlServerName,
			controlPort, monitorPort);
}

void TestTscope::tearDown()
{
   delete dummyAceStream_;
   delete sseProxy_;
   delete tscope_;
}

void TestTscope::testFoo()
{
   cout << "testFoo" << endl;
   //cu_assert(false);  // force failure  
}


// verify we can print intrinsics info
void TestTscope::testIntrinsics()
{
   cout << tscope_->getIntrinsics() << endl;
}


void TestTscope::testTscopeCmdLineArgs()
{
   const string progName("TestTscopeCmdLineArgs");
   const string defaultSseHostName("testHost");
   int defaultSsePort(9999);
   const string defaultAntControlServerName("ant-host");
   int defaultControlPort(7);
   int defaultMonitorPort(8);
   bool defaultNoUi(true);
   string defaultName("tscopesim33");
   bool defaultSimulate(true);
   int defaultVerboseLevel(2);

   TscopeCmdLineArgs cmdArgs(progName, 
			     defaultSseHostName,
			     defaultSsePort,
			     defaultAntControlServerName,
			     defaultControlPort,
			     defaultMonitorPort,
			     defaultNoUi,
			     defaultName,
			     defaultSimulate,
			     defaultVerboseLevel);

   // test usage message output
   cmdArgs.usage();
   cerr << endl;

   // check the defaults
   cu_assert(cmdArgs.getProgName() == progName);
   cu_assert(cmdArgs.getSseHostName() == defaultSseHostName);
   cu_assert(cmdArgs.getSsePort() == defaultSsePort);
   cu_assert(cmdArgs.getAntControlServerName() == defaultAntControlServerName);
   cu_assert(cmdArgs.getControlPort() == defaultControlPort);
   cu_assert(cmdArgs.getMonitorPort() == defaultMonitorPort);
   cu_assert(cmdArgs.getNoUi());
   cu_assert(cmdArgs.getName() == defaultName);
   cu_assert(cmdArgs.getSimulate());
   cu_assert(cmdArgs.getVerboseLevel() == defaultVerboseLevel);


   // try setting good parameters
   const char *argv[] = 
   { "ProgName",
     "-host", "host1",
     "-sseport", "8888",
     "-antserver", "abc123",
     "-controlport", "1111",
     "-monitorport", "2222",
     "-noui",
     "-name", "tscope1",
     "-sim",
     "-verbose", "1"
   };
   const int argc = ARRAY_LENGTH(argv);

   // verify that everything parses
   cu_assert(cmdArgs.parseArgs(argc, const_cast<char **>(argv)));

   // check the values
   cu_assert(cmdArgs.getProgName() == progName);
   cu_assert(cmdArgs.getSseHostName() == "host1");
   cu_assert(cmdArgs.getSsePort() == 8888);
   cu_assert(cmdArgs.getAntControlServerName() == "abc123");
   cu_assert(cmdArgs.getControlPort() == 1111);
   cu_assert(cmdArgs.getMonitorPort() == 2222);
   cu_assert(cmdArgs.getNoUi());
   cu_assert(cmdArgs.getName() == "tscope1");
   cu_assert(cmdArgs.getSimulate());
   cu_assert(cmdArgs.getVerboseLevel() == 1);


   cout << "Test a bad port number:" << endl;
   const char *argvBadPort[] = 
   { "ProgName",
     "-host",
     "host1",
     "-sseport",
     "badportnumber",
   };
   const int argcBadPort = ARRAY_LENGTH(argvBadPort);
   cu_assert(! cmdArgs.parseArgs(argcBadPort, const_cast<char **>(argvBadPort)));

}

void TestTscope::testStatusParser()
{
   cout << "testStatusParser" << endl;

   stringstream strm;

   // create a representative subset of the monitor status

   strm
      << "ARRAY: 10:21:03 UTC"
      << "\r\n"

      << "BEAMXA1: SUBARRAY: NANTS 42 NSHAREDPOINTING 39 NTRACK 39 NSLEW 0 NSTOP 0 NOFFLINE 3 NERROR 1 WRAP 1 ZFOCUS 2205.1 MHz GCERROR 0.123456 deg"
      << "\r\n"
      << "BEAMXA1: PRIMARY: AZEL AZ 120.123456 deg EL 30.123456 deg RA 20.123456 hr "
      << "DEC -10.234567 deg GLONG 45.123456 deg GLAT 10.98765 deg"
      << "\r\n"
      << "BEAMXA1: SYNTH: J2000 AZ 120.123456 deg EL 30.123456 deg RA 20.123456 hr "
      << "DEC -10.234567 deg GLONG 45.123456 deg GLAT 10.123456 deg"
      << "\r\n"
      << "BEAMXA1: IF: SKYFREQ 1229.010000 MHz ATTN 0.0 DBD"
      << "\r\n"

      << "BEAMYD4: SUBARRAY: NANTS 42 NSHAREDPOINTING 39 NTRACK 39 NSLEW 0 NSTOP 0 NOFFLINE 3 NERROR 0 WRAP 1 ZFOCUS 2205.1 MHz GCERROR 0.123456 deg"
      << "\r\n"
      << "BEAMYD4: PRIMARY: GAL AZ 120.123456 deg EL 30.123456 deg RA 20.123456 hr "
      << "DEC -10.234567 deg GLONG 45.123456 deg GLAT 10.98765 deg"
      << "\r\n"
      << "BEAMYD4: SYNTH: GAL AZ 120.123456 deg EL 30.123456 deg RA 20.123456 hr "
      << "DEC -10.234567 deg GLONG 45.123456 deg GLAT 10.123456 deg"
      << "\r\n"
      << "BEAMYD4: IF: SKYFREQ 3000.200000 MHz ATTN 0.0 DBD "
      << "\r\n"

      // some invalid beam status fields

      << "BAD-BEAM: SUBARRAY: TRACK WRAP 1 CAL OFF ZFOCUS 2205.1 MHz NSCOPES 40 GCERROR 0.123456 deg"
      << "\r\n"

      << "BEAMYD4: BAD-SECONDARY-KEYWORD: TRACK WRAP 1 CAL OFF ZFOCUS 2205.1 MHz NSCOPES 40 GCERROR 0.123456 deg"
      << "\r\n"

      << "TUNINGA: SKYFREQ 1420.123456 MHz"
      << "\r\n"
      << "TUNINGD: SKYFREQ 10520.123456 MHz"
      << "\r\n"

      // invalid tuning fields

      << "BAD-TUNING: SKYFREQ 10520.123456"
      << "\r\n";

   cout << "parse input: " << strm.str() << endl;

   try 
   {
      tscope_->parseStatus(strm.str());
    
      TscopeStatusMultibeam status = 
	 tscope_->getStatusMultibeam();

      cout << status << endl;

      // check ARRAY line results
      string expectedTime("10:21:03 UTC");
      cu_assert(status.time == expectedTime);

      cu_assert(status.subarray[TSCOPE_BEAMXA1].wrap == 1);
      cu_assert(status.subarray[TSCOPE_BEAMXA1].numTotal == 42);
      cu_assert(status.subarray[TSCOPE_BEAMXA1].numSharedPointing == 39);
      cu_assert(status.subarray[TSCOPE_BEAMXA1].numDriveError == 1);
   
      double expectedZfocusMhz(2205.1);
      double tolMhz(0.001);
      assertDoublesEqual(expectedZfocusMhz, status.subarray[TSCOPE_BEAMXA1].zfocusMhz, tolMhz);

      double expectedGcErrorDeg(0.123456);
      double gcErrorTolDeg(0.000001);
      assertDoublesEqual(expectedGcErrorDeg, status.subarray[TSCOPE_BEAMXA1].gcErrorDeg,
			 gcErrorTolDeg);


      // check PRIMARY line results
      TscopePointing::CoordSys expectedPrimaryCommandedCoordSys(TscopePointing::AZEL);
      cu_assert(status.primaryPointing[TSCOPE_BEAMXA1].coordSys ==
		expectedPrimaryCommandedCoordSys);

      double pointingTol(0.000001);
      double expectedAzDeg(120.123456);
      assertDoublesEqual(expectedAzDeg,
			 status.primaryPointing[TSCOPE_BEAMXA1].azDeg, 
			 pointingTol); 

      double expectedElDeg(30.123456);
      assertDoublesEqual(expectedElDeg,
			 status.primaryPointing[TSCOPE_BEAMXA1].elDeg, 
			 pointingTol); 

      double expectedRaHours(20.123456);
      assertDoublesEqual(expectedRaHours,
			 status.primaryPointing[TSCOPE_BEAMXA1].raHours, 
			 pointingTol); 

      double expectedDecDeg(-10.234567);
      assertDoublesEqual(expectedDecDeg,
			 status.primaryPointing[TSCOPE_BEAMXA1].decDeg, 
			 pointingTol); 

      double expectedGalLongDeg(45.123456);
      assertDoublesEqual(expectedGalLongDeg,
			 status.primaryPointing[TSCOPE_BEAMXA1].galLongDeg, 
			 pointingTol); 

      double expectedGalLatDeg(10.98765);
      assertDoublesEqual(expectedGalLatDeg,
			 status.primaryPointing[TSCOPE_BEAMXA1].galLatDeg, 
			 pointingTol); 

      // check BEAMXA1 synth beam results
      cu_assert(status.synthPointing[TSCOPE_BEAMXA1].coordSys ==
		TscopePointing::J2000);

      // check BEAMYD4 synth beam results
      cu_assert(status.synthPointing[TSCOPE_BEAMYD4].coordSys == 
		TscopePointing::GAL);

      double expectedTuningFreqMhz(0);
      double tuningTolMhz(0.000001);


      // check IF chain status
      expectedTuningFreqMhz = 1229.01;
      assertDoublesEqual(expectedTuningFreqMhz,
			 status.ifChain[TSCOPE_TUNINGA].skyFreqMhz, 
			 tuningTolMhz); 

      // TUNINGA
      expectedTuningFreqMhz = 1420.123456;
      assertDoublesEqual(expectedTuningFreqMhz,
			 status.tuning[TSCOPE_TUNINGA].skyFreqMhz, 
			 tuningTolMhz); 

      // TUNINGD
      expectedTuningFreqMhz = 10520.123456;
      assertDoublesEqual(expectedTuningFreqMhz,
			 status.tuning[TSCOPE_TUNINGD].skyFreqMhz, 
			 tuningTolMhz); 


#if 0
// TBD IF path status
      cu_assert(status.tuning[TSCOPE_TUNINGA].yAttnDb == 25);
#endif


   }
   catch (SseException &except)
   {
      cout << except << endl;

      // shouldn't get here
      cu_assert(false);
   }
}



Test *TestTscope::suite()
{
   TestSuite *testSuite = new TestSuite("TestTscope");
   
   testSuite->addTest(new TestCaller<TestTscope>("testFoo", &TestTscope::testFoo));
   testSuite->addTest(new TestCaller<TestTscope>("testIntrinsics", &TestTscope::testIntrinsics));
   testSuite->addTest(new TestCaller<TestTscope>("testTscopeCmdLineArgs", &TestTscope::testTscopeCmdLineArgs));
   testSuite->addTest(new TestCaller<TestTscope>("testStatusParser", &TestTscope::testStatusParser));
   
   return testSuite;
}