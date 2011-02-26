/*******************************************************************************

 File:    ObserveActivityOps.h
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


#ifndef OBSERVE_ACTIVITY_OPS_H
#define OBSERVE_ACTIVITY_OPS_H

#include <limits.h>
#include <bitset>

using std::bitset;

// Observation Activity Operations Bitset

// define all the operations an observation activity
// can perform:

enum ObserveActivityOperations
{
   USE_TSCOPE,
   POINT_AT_TARGETS, 
   RF_TUNE,  
   USE_IFC,
   USE_DX,
   TEST_SIGNAL_GEN,     // use test signal generator

   AUTOSELECT_ANTS,
   PREPARE_ANTS,
   FREE_ANTS,
   BEAMFORMER_RESET,
   BEAMFORMER_INIT,
   BEAMFORMER_AUTOATTEN,
   CALIBRATE,      
   POINT_ANTS_AND_WAIT,

   CREATE_RECENT_RFI_MASK,
   FOLLOW_UP_OBSERVATION,
   OFF_OBSERVATION,
   ON_OBSERVATION,
   GRID_WEST_OBSERVATION,
   GRID_SOUTH_OBSERVATION,
   GRID_ON_OBSERVATION,
   GRID_NORTH_OBSERVATION,
   GRID_EAST_OBSERVATION,
   CLASSIFY_ALL_SIGNALS_AS_RFI_SCAN,
   MULTITARGET_OBSERVATION,
   FORCE_ARCHIVING_AROUND_CENTER_TUNING,
   DO_NOT_REPORT_CONFIRMED_CANDIDATES_TO_SCHEDULER,
   TSCOPE_SETUP
};


// define a bitset datatype for the operations

const unsigned long OBSERVE_ACTIVITY_OPS_NBITS = 32;
typedef bitset<OBSERVE_ACTIVITY_OPS_NBITS> ObserveActivityOpsBitset;


#endif //  OBSERVE_ACTIVITY_OPS_H