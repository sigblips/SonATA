/*******************************************************************************

 File:    SseMsgDoc.h
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


// ============================================================
// Filename:    SseMsgDoc.h
// Description: message documentation for SSE 
// Authors:     Jordan
// Created:     30 Nov 2004
// ============================================================

#ifndef _sse_message_doc
#define _sse_message_doc


#include "machine-dependent.h"

// forward declare ostream
#include <iosfwd>
#include <string>
#include "sseInterface.h"
#include "SseMessage.h"

using std::string;
using std::ostream;


struct SseMsgDocumentation {
    SseMsgCode code; 
    NssMessageSeverity severity;
    string name; 
    string description;
    string userAction;
    };

SseMsgDocumentation msgInfo[] = {
    {SSE_MSG_INFO, SEVERITY_INFO, "SSE_MSG_INFO", "Information only.",
	"No User Action."},
    {SSE_MSG_DBERR, SEVERITY_ERROR,  "SSE_MSG_DBERR", 
	"Error from MySQL query or invalid results set.",
	"See Software Team."},
    {SSE_MSG_INVALID_PARMS,  SEVERITY_ERROR, "SSE_MSG_INVALID_PARMS", 
	"Invalid parameter.",
	"Change parameter value or adjust limits."},
    {SSE_MSG_STX_INVALID_STAT, SEVERITY_ERROR, "SSE_MSG_STX_INVALID_STAT", 
	"Invalid STX Status.",
	"Reset STX or Power cycle IFC."},
    {SSE_MSG_STX_RVAR_WUP, SEVERITY_WARNING, "SSE_MSG_STX_RVAR_WUP", 
	"Rgt STX Variance exceeds upper Warning limit.",
	"Increase attenuator setting or variance limit."},
    {SSE_MSG_STX_LVAR_WUP, SEVERITY_WARNING, "SSE_MSG_STX_LVAR_WUP", 
	"Left STX Variance exceeds upper Warning limit.",
	"Increase attenuator setting or variance limit."},
    {SSE_MSG_STX_RVAR_WLW, SEVERITY_WARNING, "SSE_MSG_STX_RVAR_WLW",
	"Rgt STX Variance exceeds lower Warning limit.",
	"Decrease attenuator settings or variance limits."},
    {SSE_MSG_STX_LVAR_WLW, SEVERITY_WARNING, "SSE_MSG_STX_LVAR_WLW", 
	"Left STX Variance exceeds lower Warning limit.",
	"Decrease attenuator settings or variance limits."},
    {SSE_MSG_STX_RVAR_EUP, SEVERITY_ERROR, "SSE_MSG_STX_RVAR_EUP",	
	"Rgt STX Variance exceeds upper Error limit.",
	"Increase attenuator settings or variance limits."},
    {SSE_MSG_STX_LVAR_EUP, SEVERITY_ERROR, "SSE_MSG_STX_LVAR_EUP",
	"Left STX Variance exceeds upper Error limit.",
	"Increase attenuator settings or variance limits."},
    {SSE_MSG_STX_RVAR_ELW, SEVERITY_ERROR, "SSE_MSG_STX_RVAR_ELW",
	"Rgt STX Variance exceeds lower Error limit.",
	"Decrease attenuator settings or variance limits."},
    {SSE_MSG_STX_LVAR_ELW, SEVERITY_ERROR, "SSE_MSG_STX_LVAR_ELW",
	"Left STX Variance exceeds lower Error limit.",
	"Decrease attenuator settings or variance limits."},
    {SSE_MSG_BASELINE_RW, SEVERITY_WARNING, "SSE_MSG_BASELINE_RW",
	"Rgt Baseline statistic exceeds Warning limit.",
	"Adjust attenuator settings, baseline limits or \n\t\t\tPermanent RFI Mask. \n\t\t\tInspect Blue Wave and DDC installation."},
    {SSE_MSG_BASELINE_LW, SEVERITY_WARNING, "SSE_MSG_BASELINE_LW",
	"Left Baseline statistic exceeds Warning limit.",
	"Adjust attenuator settings, baseline limits or \n\t\t\tPermanent RFI Mask. \n\t\t\tInspect Blue Wave and DDC installation."},
    {SSE_MSG_BASELINE_RE, SEVERITY_ERROR, "SSE_MSG_BASELINE_RE",
	"Rgt Baseline statistic exceeds Error limit.",
	"Adjust attenuator settings, baseline limits or \n\t\t\tPermanent RFI Mask. \n\t\t\tInspect Blue Wave and DDC installation."},
    {SSE_MSG_BASELINE_LE, SEVERITY_ERROR, "SSE_MSG_BASELINE_LE",
	"Left Baseline statistic exceeds Error limit.",
	"Adjust attenuator settings, baseline limits or \n\t\t\tPermanent RFI Mask. \n\t\t\tInspect Blue Wave and DDC installation."},
    {SSE_MSG_BAD_SIG_CLASS, SEVERITY_WARNING, "SSE_MSG_BAD_SIG_CLASS",
	"Invalid Signal Classification for FollowUp.",
	"Possible Database corruption. See DA Administrator."},
    {SSE_MSG_DX_DISCONNECT, SEVERITY_ERROR, "SSE_MSG_DX_DISCONNECT",
	"DX closed socket connection. ",
	"Kill and restart DX from controldxs."},
    {SSE_MSG_UNINIT_DB_ID, SEVERITY_WARNING, "SSE_MSG_UNINIT_DB_ID",
	"Uninit Database Id (Db Version mismatch).",
	"Use a database with compatible schema."},
    {SSE_MSG_DX_TUNE_DIFF, SEVERITY_ERROR, "SSE_MSG_DX_TUNE_DIFF",
	"Diff of Dx Tuned vs. Requested Freq too big. ",
	"See DX Software Team."},
    {SSE_MSG_DISK_FULL_ERR, SEVERITY_ERROR, "SSE_MSG_DISK_FULL_ERR", 
	"Used space on Archive Disk exceeds Error Limit.",
	"Check that the disk rotation succeeded.\n\t\t\t Delete unnecessary files."},
    {SSE_MSG_DISK_FULL_WARN, SEVERITY_WARNING, "SSE_MSG_DISK_FULL_WARN", 
	"Used space on Archive Disk exceeds Warning Limit.",
	"Check that the disk rotation succeeded.\n\t\t\t Delete unnecessary files."},
    {SSE_MSG_INVALID_SKY_FREQ, SEVERITY_ERROR, "SSE_MSG_INVALID_SKY_FREQ", 
	"Tuning Spread exceeds input bandwidth.",
	"Reassign Dx Frequencies so they all lie within \n\t\t\tIF input bandwidth. Verify that the sched frequency \n\t\t\tparameters are within the receiver range."},
    {SSE_MSG_ACT_FAILED, SEVERITY_ERROR, "SSE_MSG_ACT_FAILED",	
	"Activity Failed.",
	"See message text."},
    {SSE_MSG_RD_TARG_EPHEM, SEVERITY_ERROR, "SSE_MSG_RD_TARG_EPHEM",
	"Error Reading Spacecraft Ephemeris File.",
	"Verify that the Spacecraft Ephemeris File exists \n\t\t\tand covers the current date/time."},
    {SSE_MSG_RD_EARTH_EPHEM, SEVERITY_ERROR, "SSE_MSG_RD_EARTH_EPHEM",
	"Error Reading Earth Ephemeris File.",
	"Verify that the Earth Ephemeris File exists and \n\t\t\tcovers the current date/time."},
    {SSE_MSG_SPACECRAFT_POS, SEVERITY_ERROR, "SSE_MSG_SPACECRAFT_POS",
	"Error Calculating Spacecraft Position.",
	"Verify that the Ephemeris Files exist \n\t\t\tand cover the current date/time.\n\t\t\tSee seeker window for additional information."},
    {SSE_MSG_OUT_OF_BAND_FREQ, SEVERITY_ERROR, "SSE_MSG_OUT_OF_BAND_FREQ",
	"Dx tune freq outside IF bandwidth.",
	"Reassign Dx Frequencies so they all lie within \n\t\t\tIF input bandwidth."},
    {SSE_MSG_MISSING_TSCOPE, SEVERITY_ERROR, "SSE_MSG_MISSING_TSCOPE",
	"No Telescopes available for actiivty.",
	"Exit and Restart seeker."},
    {SSE_MSG_MISSING_TSIG, SEVERITY_ERROR, "SSE_MSG_MISSING_TSIG", 
	"No TSigs available for activity.",
	"Exit and Restart the seeker."},
    {SSE_MSG_MISSING_IFC, SEVERITY_ERROR, "SSE_MSG_MISSING_IFC",
	"No ifcs available for activity.",
	"Exit and Restart the seeker."},
    {SSE_MSG_MISSING_DX, SEVERITY_ERROR, "SSE_MSG_MISSING_DX",
	"No Dxs available for activity.",
	"Start dxs with controldxs."},
    {SSE_MSG_MISSING_BEAM, SEVERITY_ERROR, "SSE_MSG_MISSING_BEAM",
	"No (or too few) beams available for activity.",
	"Enable more beams."},
    {SSE_MSG_NO_DB, SEVERITY_ERROR, "SSE_MSG_NO_DB",
	"Database not enabled or not available.",
	"Enable database: db set usedb on. \n\t\t\tOr select different database."},
    {SSE_MSG_EXCEPTION, SEVERITY_ERROR, "SSE_MSG_EXCEPTION",
	"Unexpected exception.",
	"See software team."},
    {SSE_MSG_NO_SEC_SIG, SEVERITY_WARNING, "SSE_MSG_NO_SEC_SIG",
	"No Counterpart Signals available.",
	"Counterpart dx may have disconnected."},
    {SSE_MSG_RFI_MASK_2LONG, SEVERITY_ERROR, "SSE_MSG_RFI_MASK_2LONG",
	"# of FreqBands > 4096.",
	"See software team."},
    {SSE_MSG_ACT_UNIT_FAILED, SEVERITY_ERROR, "SSE_MSG_ACT_UNIT_FAILED",
	"Activity Unit Failed, terminating ...",
	"See previous error the caused the failure."},
    {SSE_MSG_AUTO_TARG_FAILED, SEVERITY_ERROR, "SSE_MSG_AUTO_TARG_FAILED",
	"Automatic Target Selection Failed.",
	"Enable database/Verify Star Number/Check Database."},
    {SSE_MSG_ACT_STRAT_FAILED, SEVERITY_ERROR, "SSE_MSG_ACT_STRAT_FAILED", 
	"Failure in the Strategy Active Object Code.",
	"See software team."},
    {SSE_MSG_START_ACT_FAILED, SEVERITY_ERROR, "SSE_MSG_START_ACT_FAILED",
	"Failure Starting Next Activity.",
	"See message text."},
    {SSE_MSG_FILE_ERROR, SEVERITY_ERROR, "SSE_MSG_FILE_ERROR",
	"Error Reading or Writing a disk file.",
	"See software team."},
    {SSE_MSG_IFC_PROXY_ERROR, SEVERITY_ERROR, "SSE_MSG_IFC_PROXY_ERROR",
	"Errors received from IFC.",
	"See message text."},
    {SSE_MSG_SCHED_FAILED, SEVERITY_ERROR, "SSE_MSG_SCHED_FAILED",
	"Scheduler failed to Start.",
	"See software team/Restart seeker."},
    {SSE_MSG_2MANY_ACT, SEVERITY_ERROR, "SSE_MSG_2MANY_ACT",
	"Too many dx activities.", 
	"Shutdown and restart the dx in question."},
    {SSE_MSG_INVALID_MSG, SEVERITY_ERROR, "SSE_MSG_INVALID_MSG",
	"Invalid Message received from dx.",
	"Shutdown and restart the dx in question."},
    {SSE_MSG_ALREADY_ATTACH, SEVERITY_ERROR, "SSE_MSG_ALREADY_ATTACH",
	"Dx Proxy already attached to ActivityUnit.",
	"Shutdown and restart the dx in question."},
    {SSE_MSG_ERROR_DETACH, SEVERITY_ERROR, "SSE_MSG_ERROR_DETACH",
	"Error from dxProxy detaching from ActivityUnit.",
	"Shutdown and restart the dx in question."},
    {SSE_MSG_INTERFACE_MISMATCH, SEVERITY_ERROR, "SSE_MSG_INTERFACE_MISMATCH",
	"Mismatch between the SSE-DX Interface versions.",
	"See Software Team."},
    {SSE_MSG_INVALID_PORT, SEVERITY_ERROR, "SSE_MSG_INVALID_PORT",
	"Invalid Archiver Port Number.",
	"See Software team. Check runsse script."},
    {SSE_MSG_PROXY_NOT_FOUND, SEVERITY_WARNING, "SSE_MSG_PROXY_NOT_FOUND",
	"Error releasing a Proxy.",
	"Restart the component."},
    {SSE_MSG_UNKNOWN_ERROR, SEVERITY_ERROR, "SSE_MSG_UNKNOWN_ERROR",
	"Text Message received from Subsystem.",
	"Read Text of Message."},
    {SSE_MSG_PROXY_TIMEOUT, SEVERITY_ERROR, "SSE_MSG_PROXY_TIMEOUT",
	"Timeout on reading Subsystem Message.",
	"Restart subsystem component or seeker."},
    {SSE_MSG_CONNECT_CLOSED, SEVERITY_ERROR, "SSE_MSG_CONNECT_CLOSED",
	"Socket connection is closed.",
	"Restart subsystem component or seeker."},
    {SSE_MSG_SOCKET_RESET, SEVERITY_ERROR, "SSE_MSG_SOCKET_RESET",
	"Socket error necessitates reseting the socket.",
	"Restart subsystem component or seeker."},
    {SSE_MSG_START_OBS_ERROR, SEVERITY_ERROR, "SSE_MSG_START_OBS_ERROR",
	"Dx has been shutdown because of start obs error.",
	"Check time on dx and restart."},
    {SSE_MSG_CAND_LOOKUP_FAILED, SEVERITY_ERROR, "SSE_MSG_CAND_LOOKUP_FAILED",
	"Database errors looking up Followup Candidates.",
	"See software team." }
    };

#endif /*_sse_message_doc */