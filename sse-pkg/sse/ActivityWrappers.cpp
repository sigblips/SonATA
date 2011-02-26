/*******************************************************************************

 File:    ActivityWrappers.cpp
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


#include "ActivityWrappers.h"
#include "ObserveActivityImp.h"
#include "NssParameters.h"

static void setPulseDetectionOpsBit(DxOpsBitset &dxOpsBitset)
{
   dxOpsBitset.set(PULSE_DETECTION);
}

static void setDxStandardOpsBits(DxOpsBitset &dxOpsBitset)
{
   dxOpsBitset.set(DATA_COLLECTION);
   dxOpsBitset.set(BASELINING);

   //Note: reject zero drift bit also controls high drift rate
   //rejection
   dxOpsBitset.set(REJECT_ZERO_DRIFT_SIGNALS);

   dxOpsBitset.set(APPLY_BIRDIE_MASK);
   dxOpsBitset.set(APPLY_TEST_SIGNAL_MASK);

   setPulseDetectionOpsBit(dxOpsBitset);

   dxOpsBitset.set(POWER_CWD);
   dxOpsBitset.set(COHERENT_CWD);
   dxOpsBitset.set(CANDIDATE_SELECTION);
};

Activity
*NewBirdieScanActWrapper(ActivityId_t id,
			 ActivityStrategy* activityStrategy,
			 NssComponentTree *nssComponentTree,
			 const NssParameters& nssParameters,
			 int verboseLevel)
{
   string actName("BirdieScan");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // enable test gen control, so it can be forced off
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   dxOpsBitset.set(DATA_COLLECTION);
   dxOpsBitset.set(BASELINING);
   dxOpsBitset.set(POWER_CWD);

   setPulseDetectionOpsBit(dxOpsBitset);


   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}




Activity
*NewRfBirdieScanActWrapper(ActivityId_t id,
			   ActivityStrategy* activityStrategy,
			   NssComponentTree *nssComponentTree,
			   const NssParameters& nssParameters,
			   int verboseLevel)
{
   string actName("RfBirdieScan");


   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // enable test gen control, so it can be forced off
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   dxOpsBitset.set(DATA_COLLECTION);
   dxOpsBitset.set(BASELINING);
   dxOpsBitset.set(POWER_CWD);

   // apply the IF birdie mask so that only RF birdies are found
   dxOpsBitset.set(APPLY_BIRDIE_MASK);  

   setPulseDetectionOpsBit(dxOpsBitset);


   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}



Activity
*NewDataCollectActWrapper(ActivityId_t id,
			  ActivityStrategy* activityStrategy,
			  NssComponentTree *nssComponentTree,
			  const NssParameters& nssParameters,
			  int verboseLevel)  
{
   string actName("DataCollect");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(TEST_SIGNAL_GEN); 
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   dxOpsBitset.set(DATA_COLLECTION);
   dxOpsBitset.set(BASELINING);

   return new ObserveActivityImp(id,
				 activityStrategy,
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);

}


Activity
*NewIfTestActWrapper(ActivityId_t id,
		     ActivityStrategy* activityStrategy,
		     NssComponentTree *nssComponentTree,
		     const NssParameters& nssParameters,
		     int verboseLevel)
{
   string actName("IfTest");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewIfTestZeroDriftActWrapper(ActivityId_t id,
			      ActivityStrategy* activityStrategy,
			      NssComponentTree *nssComponentTree,
			      const NssParameters& nssParameters,
			      int verboseLevel)
{
   string actName("IfTestZeroDrift");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);

   // don't reject zero drift signals (clear the bit)
   dxOpsBitset.reset(REJECT_ZERO_DRIFT_SIGNALS);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}


Activity
*NewIfTestFollowUpActWrapper(ActivityId_t id,
			     ActivityStrategy* activityStrategy,
			     NssComponentTree *nssComponentTree,
			     const NssParameters& nssParameters,
			     int verboseLevel)
{
   string actName("IfTestFollowUp");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewIfTestFollowUpOnActWrapper(ActivityId_t id,
			       ActivityStrategy* activityStrategy,
			       NssComponentTree *nssComponentTree,
			       const NssParameters& nssParameters,
			       int verboseLevel)
{
   string actName("IfTestFollowUpOn");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(ON_OBSERVATION); 

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewIfTestFollowUpOffActWrapper(ActivityId_t id,
				ActivityStrategy* activityStrategy,
				NssComponentTree *nssComponentTree,
				const NssParameters& nssParameters,
				int verboseLevel)
{
   string actName("IfTestFollowUpOff");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(OFF_OBSERVATION); 

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   // OFFs don't use zero drift reject
   // so disable it
   dxOpsBitset.reset(REJECT_ZERO_DRIFT_SIGNALS);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}



Activity
*NewDxTestActWrapper(ActivityId_t id,
		      ActivityStrategy* activityStrategy,
		      NssComponentTree *nssComponentTree,
		      const NssParameters& nssParameters,
		      int verboseLevel)
{
   string actName("DxTest");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}


Activity
*NewRfTestActWrapper(ActivityId_t id,
		     ActivityStrategy* activityStrategy,
		     NssComponentTree *nssComponentTree,
		     const NssParameters& nssParameters,
		     int verboseLevel)
{
   string actName("RfTest");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewRfTestFollowupActWrapper(ActivityId_t id,
			     ActivityStrategy* activityStrategy,
			     NssComponentTree *nssComponentTree,
			     const NssParameters& nssParameters,
			     int verboseLevel)
{
   string actName("RfTestFollowup");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(ON_OBSERVATION); 

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);

   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}


Activity
*NewRfTestZeroDriftActWrapper(ActivityId_t id,
			      ActivityStrategy* activityStrategy,
			      NssComponentTree *nssComponentTree,
			      const NssParameters& nssParameters,
			      int verboseLevel)
{
   string actName("RfTestZeroDrift");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);

   // don't reject zero drift signals (clear the bit)
   dxOpsBitset.reset(REJECT_ZERO_DRIFT_SIGNALS);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}


Activity
*NewRfTestForcedArchiveActWrapper(ActivityId_t id,
				  ActivityStrategy* activityStrategy,
				  NssComponentTree *nssComponentTree,
				  const NssParameters& nssParameters,
				  int verboseLevel)
{
   string actName("RfTestForcedArchive");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(FORCE_ARCHIVING_AROUND_CENTER_TUNING);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}


Activity
*NewRfiScanActWrapper(ActivityId_t id,
		      ActivityStrategy* activityStrategy,
		      NssComponentTree *nssComponentTree,
		      const NssParameters& nssParameters,
		      int verboseLevel)
{
   string actName("RfiScan");

   // set the targetId so emailed status messages 
   // identify the correct activity type
   //const int RFI_SCAN_TARGET_ID = 210;

   // Using a local copy of the nssParameters used to cause the 
   // seeker to core dump when the activity was run,
   // but it seems to be working now.
   NssParameters localNssParameters(nssParameters);

   // TBD fix for multibeam
   //localNssParameters.act_->setStarNumber(RFI_SCAN_TARGET_ID);

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD set telescope coords to point to zenith?

   // let user set telescope manually for now
   //actOpsBitset.set(POINT_AT_TARGETS);  

   // enable test gen control, so it can be forced off
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(CLASSIFY_ALL_SIGNALS_AS_RFI_SCAN);
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   DxOpsBitset dxOpsBitset;
   dxOpsBitset.set(DATA_COLLECTION);
   dxOpsBitset.set(BASELINING);
   dxOpsBitset.set(APPLY_BIRDIE_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);

   setPulseDetectionOpsBit(dxOpsBitset);

   dxOpsBitset.set(POWER_CWD);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 localNssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}




Activity
*NewTargetActWrapper(ActivityId_t id,
			  ActivityStrategy* activityStrategy,
			  NssComponentTree *nssComponentTree,
			  const NssParameters& nssParameters,
			  int verboseLevel)
{
   string actName("Target");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(CREATE_RECENT_RFI_MASK);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_PERMANENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RECENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewTargetOnActWrapper(ActivityId_t id,
			    ActivityStrategy* activityStrategy,
			    NssComponentTree *nssComponentTree,
			    const NssParameters& nssParameters,
			    int verboseLevel)
{
   string actName("TargetOn");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(ON_OBSERVATION); 

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(CREATE_RECENT_RFI_MASK);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_PERMANENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RECENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewTargetOffActWrapper(ActivityId_t id,
			     ActivityStrategy* activityStrategy,
			     NssComponentTree *nssComponentTree,
			     const NssParameters& nssParameters,
			     int verboseLevel)
{
   string actName("TargetOff");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(OFF_OBSERVATION); 

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);

   // Note that recent RFI mask is deliberately 
   // not enabled.

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_PERMANENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   // Don't mark zero drift signals as RFI, let them
   // be detected.
   dxOpsBitset.reset(REJECT_ZERO_DRIFT_SIGNALS);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}


// This is meant to be the final ON in an extended on/off
// sequence.  It will not report any Confirmed Candidates to
// the scheduler, causing the scheduler to discontinue
// any more followups.

Activity
*NewTargetOnNoFollowupActWrapper(ActivityId_t id,
                                 ActivityStrategy* activityStrategy,
                                 NssComponentTree *nssComponentTree,
                                 const NssParameters& nssParameters,
                                 int verboseLevel)
{
   string actName("TargetOnNoFollowup");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(ON_OBSERVATION); 
   actOpsBitset.set(DO_NOT_REPORT_CONFIRMED_CANDIDATES_TO_SCHEDULER); 

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(CREATE_RECENT_RFI_MASK);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_PERMANENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RECENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewCalibrateActWrapper(ActivityId_t id,
			  ActivityStrategy* activityStrategy,
			  NssComponentTree *nssComponentTree,
			  const NssParameters& nssParameters,
			  int verboseLevel)
{
   string actName("Calibration");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN);  // So can turn them off

   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(CALIBRATE);  

   DxOpsBitset dxOpsBitset;
   // Dxs are not used.

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewAutoselectAntsActWrapper(ActivityId_t id,
                             ActivityStrategy* activityStrategy,
                             NssComponentTree *nssComponentTree,
                             const NssParameters& nssParameters,
                             int verboseLevel)
{
   string actName("AutoselectAnts");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(AUTOSELECT_ANTS);  

   DxOpsBitset dxOpsBitset;
   // Dxs are not used.

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

//JR - Implement Tscope setup
Activity
*NewTscopeSetupActWrapper(ActivityId_t id,
		                  ActivityStrategy* activityStrategy,
					      NssComponentTree *nssComponentTree,
					      const NssParameters& nssParameters,
					      int verboseLevel)
{
   string actName("TscopeSetup");

   // set the operations bits for the activity/activityUnit & dxs
   //Tscope not required to be connected in order to connect!
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(TSCOPE_SETUP);  
 
   DxOpsBitset dxOpsBitset;
   // Dxs are not used.

   return new ObserveActivityImp(id,
                 activityStrategy,
                 actName,
                 nssComponentTree,
                 nssParameters,
                 actOpsBitset,
                 dxOpsBitset,
                 verboseLevel);
}
		  
		  
Activity
*NewPrepAntsActWrapper(ActivityId_t id,
                       ActivityStrategy* activityStrategy,
                       NssComponentTree *nssComponentTree,
                       const NssParameters& nssParameters,
                       int verboseLevel)
{
   string actName("PrepareAnts");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(PREPARE_ANTS);  

   DxOpsBitset dxOpsBitset;
   // Dxs are not used.

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewFreeAntsActWrapper(ActivityId_t id,
                       ActivityStrategy* activityStrategy,
                       NssComponentTree *nssComponentTree,
                       const NssParameters& nssParameters,
                       int verboseLevel)
{
   string actName("FreeAnts");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(FREE_ANTS);  

   DxOpsBitset dxOpsBitset;
   // Dxs are not used.

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewBeamformerResetActWrapper(ActivityId_t id,
                              ActivityStrategy* activityStrategy,
                              NssComponentTree *nssComponentTree,
                              const NssParameters& nssParameters,
                              int verboseLevel)
{
   string actName("BeamformerReset");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(BEAMFORMER_RESET);  

   DxOpsBitset dxOpsBitset;
   // Dxs are not used.

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewBeamformerInitActWrapper(ActivityId_t id,
                              ActivityStrategy* activityStrategy,
                              NssComponentTree *nssComponentTree,
                              const NssParameters& nssParameters,
                              int verboseLevel)
{
   string actName("BeamformerInit");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(BEAMFORMER_INIT);  

   DxOpsBitset dxOpsBitset;
   // Dxs are not used.

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewBeamformerAutoattenActWrapper(ActivityId_t id,
                                  ActivityStrategy* activityStrategy,
                                  NssComponentTree *nssComponentTree,
                                  const NssParameters& nssParameters,
                                  int verboseLevel)
{
   string actName("BeamformerAutoatten");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(BEAMFORMER_AUTOATTEN);  

   DxOpsBitset dxOpsBitset;
   // Dxs are not used.

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewPointAntsAndWaitActWrapper(ActivityId_t id,
                               ActivityStrategy* activityStrategy,
                               NssComponentTree *nssComponentTree,
                               const NssParameters& nssParameters,
                               int verboseLevel)
{
   string actName("PointAntsAndWait");
   
   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;
   actOpsBitset.set(USE_TSCOPE);  
   actOpsBitset.set(POINT_ANTS_AND_WAIT);  

   DxOpsBitset dxOpsBitset;
   // Dxs are not used.

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewGridWestStarActWrapper(ActivityId_t id,
			   ActivityStrategy* activityStrategy,
			   NssComponentTree *nssComponentTree,
			   const NssParameters& nssParameters,
			   int verboseLevel)
{
   string actName("GridWestStar");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(GRID_WEST_OBSERVATION);

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(CREATE_RECENT_RFI_MASK);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_PERMANENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RECENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewGridSouthStarActWrapper(ActivityId_t id,
			    ActivityStrategy* activityStrategy,
			    NssComponentTree *nssComponentTree,
			    const NssParameters& nssParameters,
			    int verboseLevel)
{
   string actName("GridSouthStar");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(GRID_SOUTH_OBSERVATION);

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(CREATE_RECENT_RFI_MASK);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_PERMANENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RECENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewGridOnStarActWrapper(ActivityId_t id,
			 ActivityStrategy* activityStrategy,
			 NssComponentTree *nssComponentTree,
			 const NssParameters& nssParameters,
			 int verboseLevel)
{
   string actName("GridOnStar");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(GRID_ON_OBSERVATION);

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(CREATE_RECENT_RFI_MASK);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_PERMANENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RECENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewGridNorthStarActWrapper(ActivityId_t id,
			    ActivityStrategy* activityStrategy,
			    NssComponentTree *nssComponentTree,
			    const NssParameters& nssParameters,
			    int verboseLevel)
{
   string actName("GridNorthStar");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(GRID_NORTH_OBSERVATION);

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(CREATE_RECENT_RFI_MASK);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_PERMANENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RECENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

Activity
*NewGridEastStarActWrapper(ActivityId_t id,
			   ActivityStrategy* activityStrategy,
			   NssComponentTree *nssComponentTree,
			   const NssParameters& nssParameters,
			   int verboseLevel)
{
   string actName("GridEastStar");

   // set the operations bits for the activity/activityUnit & dxs
   ObserveActivityOpsBitset actOpsBitset;

   actOpsBitset.set(FOLLOW_UP_OBSERVATION); 
   actOpsBitset.set(GRID_EAST_OBSERVATION);

   // TBD remove this? get from ui param?
   actOpsBitset.set(TEST_SIGNAL_GEN); 

   actOpsBitset.set(POINT_AT_TARGETS);  
   actOpsBitset.set(RF_TUNE);  
   actOpsBitset.set(USE_TSCOPE);
   actOpsBitset.set(USE_IFC);
   actOpsBitset.set(USE_DX);
   actOpsBitset.set(CREATE_RECENT_RFI_MASK);

   DxOpsBitset dxOpsBitset;
   setDxStandardOpsBits(dxOpsBitset);
   dxOpsBitset.set(APPLY_PERMANENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RECENT_RFI_MASK);
   dxOpsBitset.set(APPLY_RCVR_BIRDIE_MASK);
   dxOpsBitset.set(FOLLOW_UP_CANDIDATES);

   return new ObserveActivityImp(id,
				 activityStrategy,  
				 actName,
				 nssComponentTree,
				 nssParameters,
				 actOpsBitset,
				 dxOpsBitset,
				 verboseLevel);
}

