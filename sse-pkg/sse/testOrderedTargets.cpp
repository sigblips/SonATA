/*******************************************************************************

 File:    testOrderedTargets.cpp
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
#include "AtaInformation.h"
#include "Assert.h"
#include "SseUtil.h"
#include "SseAstro.h"
#include "MysqlQuery.h"
#include <iostream>
#include <mysql.h>
#include <sstream>
#include <vector>
#include "time.h"

using namespace std;

// primary beamsize
const double PrimaryBeamsizeAtOneGhzDeg(3.5);
const double ArcSecPerDeg(3600);

const double PrimaryBeamsizeAtOneGhzArcSec(PrimaryBeamsizeAtOneGhzDeg * 
                                           ArcSecPerDeg);

/*
  Synth beam size:

  Use worse case synth beamsize for determining beam separation.
  For ata42, Peter says it's approx 13.7 x 3.2 arcmin at dec=-33 deg
  at 1420 Mhz.
  13.7 arcmin = 822 arcsec at 1420 Ghz,  which is ~ 1167 arcsec at one ghz.
*/    
const double SynthBeamsizeAtOneGhzArcSec(1167);


MYSQL* connectToDatabase(const string &databaseHost, 
			 const string &databaseName);
ActivityId_t getNextActId(MYSQL *db);

OrderedTargets * prepareOrderedTargets(MYSQL *db);

void getTargets(ActivityId_t actId, 
		OrderedTargets *orderedTargets, time_t obsDate, 
		TargetId & firstTargetId, 
                TargetIdSet & additionalTargetIds,
		ObsRange & chosenObsRange,
		TargetId & primaryTargetId);

void writeSimulatedObsHistoryInDatabase(MYSQL *db, 
					ActivityId_t activityId,
					TargetId firstTargetId,
					const TargetIdSet & additionalTargetIds,
					TargetId primaryTargetId,
					const string &dataCollStartTime,
					const ObsRange & primaryTargetObsRange);

void recordObsHistoryInDatabase(MYSQL *db,
				ActivityId_t activityId,
				TargetId targetId,
				TargetId primaryTargetId,
				int beamNumber,
				int dxNumber,
				double dxLowFreqMhz,
				double dxHighFreqMhz,
				const string &dataCollStartTime);

void updateActivityTable(MYSQL *db,
			 ActivityId_t activityId,
			 const string & dataCollStartTime);

time_t getLastStartTimeFromDb(MYSQL *db);

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

   double minRemainingTimeRads = SseAstro::hoursToRadians(minRemainingTimeHours);

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

      cout << "dec: " << SseAstro::radiansToDegrees(dec)
           << " merit: " << merit << endl;

   }
}

int main(int argc, char *argv[])
{
#if 0
   //testTimeMerit();
   testDecMerit();

   exit(0);
#endif

   string databaseHost("sol");
   string databaseName("tom_iftest");
   //string databaseName("tom_ordered_targets15");
     
   // start at a known time so that the results are reproducible.
   // gal grid should be visible shortly after start
   //time_t baseStartTime(1150177810);   // 2006-06-13 05:50:10 UTC
   //time_t baseStartTime(1248473144);   //  2009-07-24 22:05:44 UTC
   time_t baseStartTime(1254870056); // 2009-10-06 23:00:56 UTC

   int obsLenSec(98);
   int actHouseKeepingLenSec(30);
   int simulatedActLengthSec(obsLenSec + actHouseKeepingLenSec);
   //int nSimActivities(20000000);
   int nSimActivities(50000);
   //int nSimActivities(500000);
   //int nSimActivities(1000);


   // Run simulated observing session.  
   // Do a specified number of activities.  For each,
   // choose targets, and update the database tables just as the
   // real activity would have to record a valid observation.
   // Skip ahead by the simulatedCurrentTime after each observation
   // to approximate time passage during observing.

   int primaryIdRotatePeriodSecs(3600);
   time_t primaryIdRotateTime;
   bool rotatePrimaryIds(true);

   try {

      MYSQL* db = connectToDatabase(databaseHost, databaseName);

      // To allow for interruptions, try to use the
      // last start time available in the db.  If
      // not there (ie, this is the first time) then use the base time.

      time_t simulatedCurrentTime = getLastStartTimeFromDb(db);
      if (simulatedCurrentTime <= 0)
      {
         simulatedCurrentTime = baseStartTime;
      }
      simulatedCurrentTime += simulatedActLengthSec;

      cout << "start time is: " 
           << SseUtil::isoDateTime(simulatedCurrentTime)
           << endl;
      primaryIdRotateTime = simulatedCurrentTime + primaryIdRotatePeriodSecs;

      OrderedTargets *orderedTargets = prepareOrderedTargets(db);

      list<TargetId> primaryTargetsObservedList;
      for (int actCount = 0; actCount < nSimActivities; actCount++)
      {
         // create entry in Activities table
	 ActivityId_t actId = getNextActId(db);  

	 cout << "act: " << actId << endl;

	 TargetId firstTargetId;
	 TargetIdSet additionalTargetIds;
	 ObsRange chosenObsRange;
	 TargetId primaryTargetId;

	 //cout << "**getTargets" << endl;
	 getTargets(actId, orderedTargets, simulatedCurrentTime, 
		    firstTargetId, additionalTargetIds, chosenObsRange,
		    primaryTargetId);

	 string dataCollStartTime(SseUtil::isoDateTimeWithoutTimezone(simulatedCurrentTime));

	 //cout << "**writesimobshist" << endl;
	 writeSimulatedObsHistoryInDatabase(db, actId, firstTargetId,
					    additionalTargetIds,
					    primaryTargetId,
					    dataCollStartTime,
					    chosenObsRange);

	 //cout << "**updateacttab" << endl;
	 updateActivityTable(db, actId, dataCollStartTime);

	 //cout << "**updateObservedFreqs" << endl;
	 orderedTargets->updateObservedFreqsForTargetsFromDbObsHistory(actId);

	 simulatedCurrentTime += simulatedActLengthSec;

         if (rotatePrimaryIds &&
             (simulatedCurrentTime >= primaryIdRotateTime))
         {
            orderedTargets->rotatePrimaryTargetIds();
            primaryIdRotateTime = simulatedCurrentTime +
               primaryIdRotatePeriodSecs;
         }
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
   int verboseLevel(2);
   int minNumberReservedFollowupObs(12);
   //int minNumberReservedFollowupObs(0);
   const string antenna("ATA");
   double bandwidthOfSmallestDxMhz(2.1);
   //double minAcceptableRemainingBandMhz(7.5);
   double minAcceptableRemainingBandMhz(2.5);
   double maxDxTuningSpreadMhz(50);
   bool autorise(false);
   double obsLenSec(98);
   const double maxDistLightYears = 225;    

   double sunAvoidAngleDeg(60);
   double moonAvoidAngleDeg(10);
   double geosatAvoidAngleDeg(5);
   double zenithAvoidAngleDeg(3);

   //sunAvoidAngleDeg = 0;
   //moonAvoidAngleDeg = 0;
   //geosatAvoidAngleDeg = 0;
   //zenithAvoidAngleDeg = 0;

   double autoRiseTimeCutoffMinutes(10);
   bool waitTargetComplete(false);
   double decLowerLimitDeg(-34);
   double decUpperLimitDeg(90);
   int primaryTargetIdCountCutoff(120);
   int priorityCatalogSizeCutoff(20000);
   //int priorityCatalogSizeCutoff(100);

   string highPriorityCatalogs("galsurvey,nearest,habcat"); 
   //string highPriorityCatalogs("habcat,nearest,exoplanets"); 
   //string highPriorityCatalogs("exoplanets,nearest,habcat"); 

   string lowPriorityCatalogs("tycho2subset,tycho2remainder");    
   vector<FrequencyBand> permRfiBands;

   cout << "preparing ordered targets..." << endl;

   Range freqRangeLimitsMhz(1410.0, 1730.0); 
   //Range freqRangeLimitsMhz(2840.0, 3440.0); 

   vector<TargetMerit::MeritFactor> meritFactors;

#if 1
// galsurvey - wide sky coverage, primary id rotation
   meritFactors.push_back(TargetMerit::Catalog);
   meritFactors.push_back(TargetMerit::PrimaryId);
   meritFactors.push_back(TargetMerit::Meridian);
   meritFactors.push_back(TargetMerit::CompletelyObs);
   meritFactors.push_back(TargetMerit::TimeLeft);
   
   // use instead of meridian for galcen to emphasize low decs
   //  meritFactors.push_back(TargetMerit::Dec);
#else

// habcat et al.
   meritFactors.push_back(TargetMerit::Catalog);
   meritFactors.push_back(TargetMerit::Meridian);
   meritFactors.push_back(TargetMerit::CompletelyObs);
   meritFactors.push_back(TargetMerit::TimeLeft);

#endif

   OrderedTargets *orderedTargets = new OrderedTargets(
      db,
      verboseLevel,
      minNumberReservedFollowupObs,
      AtaInformation::AtaLongWestDeg,
      AtaInformation::AtaLatNorthDeg,
      AtaInformation::AtaHorizonDeg,
      bandwidthOfSmallestDxMhz,
      minAcceptableRemainingBandMhz, 
      maxDxTuningSpreadMhz,
      autorise, obsLenSec, maxDistLightYears,
      sunAvoidAngleDeg, moonAvoidAngleDeg,
      geosatAvoidAngleDeg,
      zenithAvoidAngleDeg,
      autoRiseTimeCutoffMinutes,
      waitTargetComplete, 
      decLowerLimitDeg, decUpperLimitDeg,
      primaryTargetIdCountCutoff,
      priorityCatalogSizeCutoff,
      highPriorityCatalogs,
      lowPriorityCatalogs,
      meritFactors,
      freqRangeLimitsMhz,
      permRfiBands,
      PrimaryBeamsizeAtOneGhzArcSec,
      SynthBeamsizeAtOneGhzArcSec);

   return orderedTargets;
}

void getTargets(ActivityId_t actId,
		OrderedTargets *orderedTargets, time_t obsDate, 
		TargetId & firstTargetId, 
		TargetIdSet & additionalTargetIds,
		ObsRange & chosenObsRange,
		TargetId & primaryTargetId)
{   
   //cout << "choose first target" <<endl;

   int nTargetsToChoose = 2;
   double minTargetSepBeamsizes(2);

   bool areActsRunning(false); 
   orderedTargets->chooseTargets(
      nTargetsToChoose,
      obsDate,
      minTargetSepBeamsizes,
      areActsRunning,
      firstTargetId,   // returned
      chosenObsRange, // returned
      primaryTargetId, // returned
      additionalTargetIds);  // returned

   cout << "primaryTargetId: " << primaryTargetId << endl;

   Assert(static_cast<int>(additionalTargetIds.size()) == nTargetsToChoose-1);
   
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
				TargetId primaryTargetId,
				int beamNumber,
				int dxNumber,
				double dxLowFreqMhz,
				double dxHighFreqMhz,
				const string &dataCollStartTime)
{
   stringstream sqlstmt;

   int HzPrecision(6);
   sqlstmt.precision(HzPrecision); 
   sqlstmt.setf(std::ios::fixed);      // show all decimal places up to precision
   double dxTuneFreqMhz((dxHighFreqMhz + dxLowFreqMhz) /2.0);

   sqlstmt << "insert into ActivityUnits set "
	   << " activityId = " << activityId
	   << ", targetId = " << targetId
	   << ", primaryTargetId = " << primaryTargetId
	   << ", beamNumber = " << beamNumber
	   << ", dxNumber =  " << dxNumber
	   << ", dxTuneFreq = " << dxTuneFreqMhz
	   << ", dxLowFreqMhz = " << dxLowFreqMhz
	   << ", dxHighFreqMhz = " <<  dxHighFreqMhz
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

   string actType("target");  
   sqlstmt << "update Activities set "
	   << " startOfDataCollection = '" 
	   << dataCollStartTime
	   << "'"
	   << ", validObservation = 'Yes'"
	   << ", type = '" << actType << "'"
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
					TargetId firstTargetId,
					const TargetIdSet & additionalTargetIds,
					TargetId primaryTargetId,
					const string &dataCollStartTime,
					const ObsRange & primaryTargetObsRange)
{
   //int nSimDxsPerBeam = 1;
   int beamNumber = 1;

   // use lowest available freq in the obs range as the dx start freq
   double dxLowFreqMhz(primaryTargetObsRange.minValue());

   double dxBandwidthMhz = 20.0;
   double dxHighFreqMhz(dxLowFreqMhz+dxBandwidthMhz);
   int dxNumber = 1001;

   // write ObsHistory
   // one entry per dx, per beam

   // primary target
   cout << "writing " << dxLowFreqMhz << " mhz - "
	<< dxHighFreqMhz <<  " mhz" << endl;

   recordObsHistoryInDatabase(db, activityId,
			      firstTargetId,
			      primaryTargetId,
			      beamNumber++,
			      dxNumber++,
			      dxLowFreqMhz,
			      dxHighFreqMhz,
			      dataCollStartTime);


   // additional targets
   for (TargetIdSet::const_iterator it = additionalTargetIds.begin();
	it != additionalTargetIds.end(); ++it)
   {
      const TargetId &targetId = *it;

      cout << "additional target: " << targetId << endl;

      recordObsHistoryInDatabase(db, activityId,
				 targetId,
				 primaryTargetId,
				 beamNumber++,
				 dxNumber++,
				 dxLowFreqMhz,
				 dxHighFreqMhz,
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

/*
  Get last good act start time.  If none, return zero.
 */
time_t getLastStartTimeFromDb(MYSQL *db)
{
   stringstream sqlstmt;
   stringstream errorMsg;

   time_t startTime(0);

   enum colIndices { startTimeCol, numCols };

   sqlstmt << "select max(UNIX_TIMESTAMP(startOfDataCollection)) "
           << "FROM Activities "
           << "where validObservation = 'Yes'";

   MysqlQuery query(db);
   query.execute(sqlstmt.str(), numCols, __FILE__, __LINE__);

   MYSQL_ROW row = mysql_fetch_row(query.getResultSet());
   if (mysql_num_rows(query.getResultSet()) > 1)
   {
      errorMsg << " found multiple rows of start time: ";

      throw SseException(errorMsg.str(), __FILE__, __LINE__, 
			 SSE_MSG_DBERR, SEVERITY_ERROR);
   }
   else if (mysql_num_rows(query.getResultSet()) == 1)
   {
      if (row[startTimeCol])
      {
         startTime = query.getInt(row, startTimeCol, __FILE__, __LINE__);
      }
   }

   return startTime;

}

