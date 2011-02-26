/*******************************************************************************

 File:    DxActivityStatus.cpp
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


#include "sseDxInterfaceLib.h"

// DX activity states
static const char *DxActStateNames[] =
{
    "No Activity",        // DX_ACT_NONE
    "Init",               // DX_ACT_INIT
    "Dx Tuned",          // DX_ACT_TUNED
    "Pend Base Accum",    // DX_ACT_PEND_BASE_ACCUM
    "Base Accum",         // DX_ACT_RUN_BASE_ACCUM
    "Base Accum Complete",// DX_ACT_BASE_ACCUM_COMPLETE
    "Pend Data Coll",     // DX_ACT_PEND_DC (start time received)
    "Data Coll",          // DX_ACT_RUN_DC
    "Data Coll Complete", // DX_ACT_DC_COMPLETE
    "Pend Sig Det",       // DX_ACT_PEND_SD
    "Sig Det",            // DX_ACT_RUN_SD
    "Sig Det Complete",   // DX_ACT_SD_COMPLETE
    "Activity Complete",  // DX_ACT_COMPLETE
    "Stopping",           // DX_ACT_STOPPING
    "Stopped",            // DX_ACT_STOPPED
    "Error",              // DX_ACT_ERROR

};

// dx activity status
static string getDxActStateName(DxActivityState state)
{
    string stateName("?");
    
    if (state >= 0 && state < ARRAY_LENGTH(DxActStateNames)) 
    { 
	stateName =  DxActStateNames[state];
    }
    else
    {
	// error handling TBD
    }
    return stateName;

}



ostream& operator << (ostream &strm, const DxActivityStatus &actStat)
{
    strm << "Act ";
    strm << actStat.activityId;

    strm << ": "; 
    strm << getDxActStateName(actStat.currentState);

    return strm;

}

DxActivityStatus::DxActivityStatus()
    : activityId(0),
      currentState(DX_ACT_NONE) 
{
}


void DxActivityStatus::marshall()
{
    HTONL(activityId);
    SseDxMsg::marshall(currentState); // DxActivityState
}

void DxActivityStatus::demarshall()
{
    marshall();
}
