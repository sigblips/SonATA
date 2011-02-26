/*******************************************************************************

 File:    testOrderedTargetsEclip.cpp
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

// Test harness for OrderedTargets target selections,
// using simulated observations.

#include <ace/OS.h>
#include "SseMessage.h"
#include "SseException.h"
#include "sseDxInterface.h"
#include "OrderedTargets.h"
#include "IdNumberFactory.h"
#include "Assert.h"
#include "SseUtil.h"
#include <iostream>
#include <mysql.h>
#include <sstream>
#include <vector>
#include "time.h"

using namespace std;


MYSQL* connectToDatabase(const string &databaseHost, 
			 const string &databaseName);
ActivityId_t getNextActId(MYSQL *db);

OrderedTargets * prepareOrderedTargets(MYSQL *db);

void getTargets(ActivityId_t actId, 
		OrderedTargets *orderedTargets, time_t obsDate, int nTargetsToChoose,
		TargetId & primaryTargetId, TargetIdSet & chosenTargetIds);

void writeSimulatedObsHistoryInDatabase(MYSQL *db, 
					ActivityId_t activityId,
					TargetId primaryTargetId,
					const TargetIdSet & secondaryTargetIds,
					const string &dataCollStartTime);

void recordObsHistoryInDatabase(MYSQL *db,
				ActivityId_t activityId,
				TargetId targetId,
				int beamNumber,
				int dxNumber,
				double dxLowFreqMhz,
				double dxHighFreqMhz,
				bool isPrimaryTarget,
				const string &dataCollStartTime);

void updateActivityTable(MYSQL *db,
			 ActivityId_t activityId,
			 const string & dataCollStartTime);



double computeMinRemainingTimeOnTargetRads() 
{
   // assume overall time for an activity is 2 times the observation length
   // TBD: take into account other activity startup overhead.

   double minNumberReservedFollowupObs_ = 12;
   int obsLengthSec_ = 98;

   double SecsPerHour = 3600;

   // allow for data collect & signal detect:
   int totalActivityLengthSec = 2 * static_cast<int>(obsLengthSec_); 
   double minRemainingTimeHours = totalActivityLengthSec/SecsPerHour * 
      minNumberReservedFollowupObs_;

   double minRemainingTimeRads = SseUtil::hoursToRadians(minRemainingTimeHours);

   return minRemainingTimeRads;
}


void testTimeMerit()
{
   double minRemainingTimeOnTargetRads_ = computeMinRemainingTimeOnTargetRads();
   cout << "minRemainingTimeOnTargetRads_=" << minRemainingTimeOnTargetRads_ << endl;

   double decrRads = 0.1;
   for(double timeUntilSetRads_ = 3.14; timeUntilSetRads_ > 0.0; timeUntilSetRads_ -= decrRads) 
   {
      
      double localTimeLeftMerit = (timeUntilSetRads_ - minRemainingTimeOnTargetRads_) ;
	 // / timeUntilSetRads_;
      
      if (localTimeLeftMerit < 0)
      {
	 localTimeLeftMerit = 0;
      }
      else 
      {
	 double scaleFactor = 20;
	 localTimeLeftMerit += scaleFactor;
	 localTimeLeftMerit /= scaleFactor;
      }

      double timeLeftMerit_ = localTimeLeftMerit; // * localTimeLeftMerit;
      // * localTimeLeftMerit * localTimeLeftMerit;
      
      cout << "timeUntilSetRads_ " << timeUntilSetRads_ << 
	 " time left merit " << timeLeftMerit_ << endl;
      
   }
}

void testDecMerit() 
{
   for (double dec = M_PI/2; dec > -M_PI/2; dec-=0.1)
   {
      double maxDecRads = M_PI/2;
      double merit = (maxDecRads - dec + 1.0);

      merit *= merit * merit;

      cout << "dec: " << dec << " merit: " << merit << endl;

   }
}

int main(int argc, char *argv[])
{
#if 0
   testTimeMerit();
   testDecMerit();

   exit(0);
#endif

   string databaseHost("sol");
   //string databaseName("tom_target_test6");
   string databaseName("tom_iftest");
     
   // start at a known time so that the results are reproducible.
   time_t baseStartTime(1132180483);   // Wed Nov 16 14:34:40 PST 2005

   baseStartTime -= (3600 * 5);  // subtrack some time to start earlier in the day

   int obsLenSec(98);
   int actHouseKeepingLenSec(25);
   int simulatedActLengthSec(obsLenSec + actHouseKeepingLenSec);
   int nTargetsPerObs(2);
   int nSimActivities(600);


   // Run simulated observing session.  
   // Do a specified number of activities.  For each,
   // choose targets, and update the database tables just as the
   // real activity would have to record a valid observation.
   // Skip ahead by the simulatedCurrentTime after each observation
   // so that target selections will be more realistic.

   try {

      MYSQL* db = connectToDatabase(databaseHost, databaseName);

      OrderedTargets *orderedTargets = prepareOrderedTargets(db);

      time_t simulatedCurrentTime(baseStartTime);
      list<TargetId> primaryTargetsObservedList;

      for (int actCount = 0; actCount < nSimActivities; actCount++)
      {
         // create entry in Activities table
	 ActivityId_t actId = getNextActId(db);  

	 cout << "act: " << actId << endl;

	 TargetId primaryTargetId;
	 TargetIdSet secondaryTargetIds;

	 //cout << "**getTargets" << endl;
	 getTargets(actId, orderedTargets, simulatedCurrentTime, nTargetsPerObs,
		    primaryTargetId, secondaryTargetIds);

#if 0
	 // look for repeated primary targets
	 list<TargetId>::iterator it;
	 it = find(primaryTargetsObservedList.begin(),
		   primaryTargetsObservedList.end(),
		   primaryTargetId);
	 if (it == primaryTargetsObservedList.end())
	 {
	    primaryTargetsObservedList.push_back(primaryTargetId);
	 }
	 else {
	    cout << "act: " << actId 
		 << " repeated primary target: " 
		 << primaryTargetId << endl;

	    cout << "N primary targets observed: " 
		 << primaryTargetsObservedList.size() 
		 << endl;
	    break;
	 }
#endif

	 string dataCollStartTime(SseUtil::isoDateTimeWithoutTimezone(simulatedCurrentTime));

	 //cout << "**writesimobshist" << endl;
	 writeSimulatedObsHistoryInDatabase(db, actId, primaryTargetId,
					    secondaryTargetIds,
					    dataCollStartTime);

	 //cout << "**updateacttab" << endl;
	 updateActivityTable(db, actId, dataCollStartTime);

	 //cout << "**updateObservedFreqs" << endl;
	 //orderedTargets->updateObservedFreqsForTargetsFromDbObsHistory(actId);

	 simulatedCurrentTime += simulatedActLengthSec;
      }

      delete orderedTargets;

      mysql_close(db);

   }
   catch (SseException &except)
   {
      cout << except << endl;
   }

   // don't catch any other (unexpected) exceptions, let the program 
   // core dump so the error can be traced.

}


OrderedTargets * prepareOrderedTargets(MYSQL *db)
{
   int verboseLevel(0);
   int minNumberReservedFollowupObs(12);
   const string antenna("ATA");
   double bandwidthOfSmallestDxMhz(2.1);
   //double minAcceptableRemainingBandMhz(7.5);
   double minAcceptableRemainingBandMhz(2.5);
   bool autorise(false);
   double obsLenSec(98);
   const double maxDistLightYears = 225;    

   //double sunAvoidAngleDeg(60);
   double sunAvoidAngleDeg(0);

   double autoRiseTimeCutoffMinutes(10);
   bool waitTargetComplete(false);
   bool useRaHourAngleLimits(false);
   double hourAngleRiseLimitHours(1);
   double hourAngleSetLimitHours(1);
   double decLowerLimitDeg(-34);
   double decUpperLimitDeg(90);
   vector<FrequencyBand> permRfiBands;

   cout << "preparing ordered targets..." << endl;

   Range freqRangeLimitsMhz(1414.0, 1424.0);  // for gal/sky survey

   OrderedTargets *orderedTargets = new OrderedTargets(
      db, verboseLevel, minNumberReservedFollowupObs,
      antenna,
      bandwidthOfSmallestDxMhz, minAcceptableRemainingBandMhz,
      autorise, obsLenSec, maxDistLightYears,
      sunAvoidAngleDeg, autoRiseTimeCutoffMinutes,
      waitTargetComplete, useRaHourAngleLimits,
      hourAngleRiseLimitHours, hourAngleSetLimitHours,
      decLowerLimitDeg, decUpperLimitDeg,
      freqRangeLimitsMhz,
      permRfiBands);

   return orderedTargets;
}

void getTargets(ActivityId_t actId,
		OrderedTargets *orderedTargets, time_t obsDate, 
		int nTargetsToChoose,
		TargetId & primaryTargetId, 
		TargetIdSet & secondaryTargetIds)
{   
   // choose first target:
   TargetId bestTargetId;
   ObsRange bestTargetObsRange;
   ActivityIdList actIdsInProgress;  // empty list

   //cout << "choose first target" <<endl;

   static bool firstTime = true;
   if (firstTime)
   {
      firstTime = false;
   }
   else 
   {
      actIdsInProgress.push_back(actId -1);
   }

   orderedTargets->chooseFirstTarget(
      obsDate,
      actIdsInProgress,
      bestTargetId,
      bestTargetObsRange);

   primaryTargetId = bestTargetId;

   // choose additional targets

   //cout << "choose addtl targets" << endl;

   int nAdditionalTargets = nTargetsToChoose - 1;
   if (nAdditionalTargets > 0)
   {
      //double AtaBeamsizeAtLbandRads(0.0430794);  // single dish beamsize at 1.4Ghz

      double AtaBeamsizeAtSbandRads(SseUtil::degreesToRadians(1.527051)); // single dish beamsize at 2.9 Ghz

      //double AtaBeamsizeAtLbandRads(SseUtil::degreesToRadians(0.029630)); // ata42 @ 1.575 ghz (based on the smaller dimension of 345 x 168 arcsec at 1.0 GHz)

      double minTargetSepBeamsizes(5);
      
      // pretend "field of view" is xx degrees for purposes
      // of selecting targets within the "primary"
      double primaryFieldOfViewRads(SseUtil::degreesToRadians(30.0)); 

      // use real ATA primary FOV
      //double primaryFieldOfViewRads(SseUtil::degreesToRadians(2.2222));  // @ 1.575 ghz

      double beamsizeRads(AtaBeamsizeAtSbandRads);
      
      orderedTargets->chooseAdditionalTargets(
	 nAdditionalTargets,
	 beamsizeRads,
	 minTargetSepBeamsizes,
	 primaryFieldOfViewRads,
	 secondaryTargetIds  // returned
	 );
   }

   Assert(static_cast<int>(secondaryTargetIds.size()) == nAdditionalTargets);
   
}

ActivityId_t getNextActId(MYSQL *db)
{
  // get the next activity Id from the database
  if (mysql_query(db, "insert into Activities () values();") != 0)
  {
    throw SseException("failed to get next ActivityId from db");
  }

  ActivityId_t nextIdValue = mysql_insert_id(db);

  return nextIdValue;
}


void recordObsHistoryInDatabase(MYSQL *db,
				ActivityId_t activityId,
				TargetId targetId,
				int beamNumber,
				int dxNumber,
				double dxLowFreqMhz,
				double dxHighFreqMhz,
				bool isPrimaryTarget,
				const string &dataCollStartTime)
{
   stringstream sqlstmt;

   int HzPrecision(6);
   sqlstmt.precision(HzPrecision); 
   sqlstmt.setf(std::ios::fixed);      // show all decimal places up to precision

   string primaryTarget("No");
   if (isPrimaryTarget)
   {
      primaryTarget = "Yes";
   }

   sqlstmt << "insert into ActivityUnits set "
	   << " activityId = " << activityId
	   << ", targetId = " << targetId
	   << ", beamNumber = " << beamNumber
	   << ", dxNumber =  " << dxNumber
	   << ", dxLowFreqMhz = " << dxLowFreqMhz
	   << ", dxHighFreqMhz = " <<  dxHighFreqMhz
	   << ", primaryTarget = '" << primaryTarget << "'"
	   << ", validObservation = 'Yes'"
	   << ", startOfDataCollection = '" 
	   << dataCollStartTime
	   << "'"
	   << " ";

   if (mysql_query(db, sqlstmt.str().c_str()) != 0)
   {	
      stringstream strm;
      strm << "::recordObsHistoryInDatabase() MySQL error: " 
	   << mysql_error(db)  << endl;
      
      throw SseException(strm.str(), __FILE__, __LINE__);
   }
}

void updateActivityTable(MYSQL *db,
			 ActivityId_t activityId,
			 const string & dataCollStartTime)
{
   
   stringstream sqlstmt;
   
   sqlstmt << "update Activities set "
	   << " startOfDataCollection = '" 
	   << dataCollStartTime
	   << "'"
	   << ", validObservation = 'Yes'"
	   << " where id = " << activityId
	   << " ";
   
   if (mysql_query(db, sqlstmt.str().c_str()) != 0)
   {
      stringstream strm;
      strm << mysql_error(db) << endl;
      throw SseException(strm.str(), __FILE__, __LINE__,
			 SSE_MSG_DBERR, SEVERITY_ERROR);
   }
   
}

void writeSimulatedObsHistoryInDatabase(MYSQL *db, 
					ActivityId_t activityId,
					TargetId primaryTargetId,
					const TargetIdSet & secondaryTargetIds,
					const string &dataCollStartTime)
{
   //int nSimDxsPerBeam = 1;
   int beamNumber = 1;
   /* static */ double dxLowFreqMhz(1414);
   double dxBandwidthMhz = 10.0;
   /* static */ double dxHighFreqMhz(dxLowFreqMhz+dxBandwidthMhz);
   bool isPrimaryTarget(true);
   int dxNumber = 1001;

   //dxLowFreqMhz += dxBandwidthMhz;
   //dxHighFreqMhz += dxBandwidthMhz;

   // write ObsHistory
   // one entry per dx, per beam

   // primary target
   cout << "writing " << dxLowFreqMhz << " mhz "
	<< dxHighFreqMhz <<  " mhz" << endl;

   recordObsHistoryInDatabase(db, activityId,
			      primaryTargetId, beamNumber++,
			      dxNumber++,
			      dxLowFreqMhz,
			      dxHighFreqMhz,
			      isPrimaryTarget,
			      dataCollStartTime);


   // secondary targets
   isPrimaryTarget = false;
   for (TargetIdSet::iterator it = secondaryTargetIds.begin();
	it != secondaryTargetIds.end(); ++it)
   {
      const TargetId &targetId = *it;

      recordObsHistoryInDatabase(db, activityId,
				 targetId, beamNumber++,
				 dxNumber++,
				 dxLowFreqMhz,
				 dxHighFreqMhz,
				 isPrimaryTarget,
				 dataCollStartTime);
   }

}


MYSQL* connectToDatabase(const string &databaseHost, 
			 const string &databaseName)
{
   MYSQL * db = mysql_init(NULL);
   if (!db)
   {
      throw SseException(
	 "::connect() mysql_init() failed. Cannot allocate new handler.",
	 __FILE__,  __LINE__, SSE_MSG_DBERR, SEVERITY_ERROR);
   }
   
   if (!mysql_real_connect(
      db, 
      databaseHost.c_str(), 
      "",  // user
      "",  // password 
      databaseName.c_str(),
      0,     // port number
      NULL,  // socket name
      0))    // flags
   {
      stringstream strm;
      strm << "::connect() MySQL error: " << mysql_error(db) << endl;
       throw SseException(strm.str(), __FILE__,  __LINE__, 
			  SSE_MSG_DBERR, SEVERITY_ERROR);
   }
   
   return db;
   
}



