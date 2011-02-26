/*******************************************************************************

 File:    ActivityWrappers.h
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


#ifndef ACTIVITY_WRAPPERS_H
#define ACTIVITY_WRAPPERS_H

#include "ActivityId.h"

class Activity;
class ActivityStrategy;
class NssComponentTree;
class NssParameters;

Activity *NewBirdieScanActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewRfBirdieScanActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);


Activity *NewDataCollectActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);  

Activity *NewIfTestActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewIfTestZeroDriftActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewIfTestFollowUpActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewIfTestFollowUpOnActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewIfTestFollowUpOffActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewDxTestActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewRfTestActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewRfTestFollowupActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewRfTestZeroDriftActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewRfTestForcedArchiveActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);


Activity *NewRfiScanActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);


Activity *NewTargetActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewTargetOnActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewTargetOffActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewTargetOnNoFollowupActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewGridWestStarActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewGridSouthStarActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewGridOnStarActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewGridNorthStarActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewGridEastStarActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewCalibrateActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewAutoselectAntsActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

//JR - Implement Tscope setup
Activity *NewTscopeSetupActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);


Activity *NewPrepAntsActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewFreeAntsActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewBeamformerResetActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewBeamformerInitActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewBeamformerAutoattenActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

Activity *NewPointAntsAndWaitActWrapper(
   ActivityId_t id,
   ActivityStrategy* activityStrategy,
   NssComponentTree *nssComponentTree,
   const NssParameters& nssParameters,
   int verboseLevel);

#endif 