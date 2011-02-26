/*******************************************************************************

 File:    DxTypes.h
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

//
// Typedefs and enums
//
// $Header: /home/cvs/nss/sonata-pkg/dx/include/DxTypes.h,v 1.4 2009/03/06 21:48:02 kes Exp $
//
#ifndef _DxTypesH
#define _DxTypesH

#include <complex>
#include <sseDxInterface.h>
#include "Types.h"

#ifdef notdef
typedef std::complex<int16_t> ComplexInt16;
typedef std::complex<float32_t> ComplexFloat32;
typedef std::complex<float64_t> ComplexFloat64;
#endif

namespace dx {

enum Unit {
	UnitNone = 0,
	UnitSse,
	UnitSseCmd,
	UnitCollect,
	UnitDetect,
	UnitConfirm,
	UnitArchive,
	UnitArchiver,
	UnitLCDInput,
	UnitRCDInput,
	UnitLCwInput,
	UnitRCwInput,
	UnitLPulseInput,
	UnitRPulseInput,
	UnitLBaselineInput,
	UnitRBaselineInput,
	UnitLCDWrite,
	UnitRCDWrite,
	UnitLCwWrite,
	UnitRCwWrite,
	UnitLPulseWrite,
	UnitRPulseWrite,
	UnitLBaselineWrite,
	UnitRBaselineWrite,
	UnitCDRead,
	UnitCwRead,
	UnitPulseRead,
	UnitCw,
	UnitLCwDetect,
	UnitRCwDetect,
	UnitPulse,
	UnitCwConfirmation,
	UnitPulseConfirmation,
	UnitReceiver,
	UnitWorker,
	UnitInput
};

#ifdef notdef
enum ConnectionType {
	ActiveTcpConnection = 0,
	PassiveTcpConnection,
	UdpConnection,
	SerialConnection,
	KeyboardConnection,
	DisplayConnection,
	ConsoleConnection,
	FileConnection
};
#endif

enum InternalMessageCode {
	InitiateConnection = DX_MESSAGE_CODE_END,
	InputPacket,
	Tune,
	StartCollection,
	DspCollectionComplete,
	StartDetection,
	CheckSliceBuf,
	CwComplete,
	PulseComplete,
	DetectionComplete,
	StartConfirmation,
	ConfirmCandidate,
	CwConfirmationComplete,
	PulseConfirmationComplete,
	ConfirmationComplete,
	StartArchive,
	ArchiveComplete,
	PollHitBuffer,
	SliceComplete,
	ResolutionComplete,
	ActivityStopped,
	HalfFrameReady,
	DfbProcess
};

enum SystemType {
	PRIMARY,
	SECONDARY
};

enum SystemState {
	Idle = 0,
	Initialized,
	SelfTest
};

enum SignalType {
	CW_POWER,
	PULSE,
	ANY_TYPE
};

enum SignalState {
	DETECTED,
	CONFIRMED,
	ARCHIVED,
	ANY_STATE
};

#ifdef notdef
// CWD slope types
enum CwSlope {
	POSITIVE = 0,
	NEGATIVE
};
#endif

// activity enums
enum ActivityType {
	NormalActivity,
	BaselineActivity
};

enum SpecState {
	NoChange = 0,					// no change in state
	BLStarted,						// baselining has started
	BLComplete,						// baselining is complete
	DCStarted,						// data collection has started
	DCComplete						// data collection is complete
};

#ifdef notdef
enum ActivityIdValue {
	Actual,						// use actual activity id
	Effective					// use effective activity id (id < 0 ? -id : id)
};
#endif

}

#endif