/*******************************************************************************

 File:    SseTscopeMsg.cpp
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


#include "SseTscopeMsg.h"
#include "ArrayLength.h"
#include "Assert.h"
#include "SseException.h"
#include "SseUtil.h"
#include "HostNetByteorder.h"
#include <iostream>
#include <string>

// names associated with SSE - TSCOPE message codes (see sseTscopeInterface.h)

static const char *tscopeCodeNames[] = {
   "tscope msg uninit",

   // outgoing commands to tscope
   "tscope allocate",
   "tscope deallocate",
   "tscope monitor",
   "tscope point subarray",
   "tscope request intrinsics",
   "tscope reset",
   "tscope tune",
   "tscope shutdown",
   "tscope request status",
   "tscope stop",
   "tscope stow",
   "tscope wrap",
   "tscope connect",
   "tscope disconnect",
   "tscope simulate",
   "tscope unsimulate",
   "tscope zfocus",
   "tscope lna on",
   "tscope pam set",
   "tscope request point check",
   "tscope send backend cmd",
   "tscope antgroup autoselect",
   "tscope begin sending command sequence",
   "tscope done sending command sequence",

   // beamformer commands
   "tscope bf reset",
   "tscope bf init",
   "tscope bf autoatten",
   "tscope bf set ants",
   "tscope bf clear ants",
   "tscope bf set attn",
   "tscope bf set obslen",
   "tscope bf set coords",
   "tscope bf clear coords",
   "tscope bf set null type",
   "tscope bf add null",
   "tscope bf clear nulls",
   "tscope bf cal",
   "tscope bf point",
   "tscope bf stop",
   "tscope bf dest",

   // -- incoming messages from tscope
   "tscope error",
   "tscope message",
   "tscope intrinsics",
   "tscope tracking on",
   "tscope tracking off",
   "tscope status multibeam",
   "tscope ready"
};

string SseTscopeMsg::getMessageCodeString(uint32_t code)
{
   // tscope message codes are offset from zero, so adjust the
   // code to fit the codeNames array.

   int index = static_cast<signed int>(code) - TSCOPE_CODE_RANGE_START;
   if (index > 0 && index < ARRAY_LENGTH(tscopeCodeNames))
   { 
      return tscopeCodeNames[index];
   }
   else
   {
      return "Error: message code out of range";
   }
}

static const char *tscopeBeamNames[] = {
   "BEAMXA1",
   "BEAMYA1",
   "BEAMXA2",
   "BEAMYA2",
   "BEAMXA3",
   "BEAMYA3",
   "BEAMXA4",
   "BEAMYA4",

   "BEAMXB1",
   "BEAMYB1",
   "BEAMXB2",
   "BEAMYB2",
   "BEAMXB3",
   "BEAMYB3",
   "BEAMXB4",
   "BEAMYB4",

   "BEAMXC1",
   "BEAMYC1",
   "BEAMXC2",
   "BEAMYC2",
   "BEAMXC3",
   "BEAMYC3",
   "BEAMXC4",
   "BEAMYC4",

   "BEAMXD1",
   "BEAMYD1",
   "BEAMXD2",
   "BEAMYD2",
   "BEAMXD3",
   "BEAMYD3",
   "BEAMXD4",
   "BEAMYD4"
};


string SseTscopeMsg::beamToName(TscopeBeam beam)
{
   Assert(ARRAY_LENGTH(tscopeBeamNames) == TSCOPE_N_BEAMS);

   if (beam >= 0 && beam < ARRAY_LENGTH(tscopeBeamNames))
   { 
      return tscopeBeamNames[beam];
   }

   return "SseTscopeMsg Error: invalid beam";
}

TscopeBeam SseTscopeMsg::nameToBeam(const string & name)
{
   Assert(ARRAY_LENGTH(tscopeBeamNames) == TSCOPE_N_BEAMS);

   string uppercaseName(SseUtil::strToUpper(name));
   for (int index = 0; index < ARRAY_LENGTH(tscopeBeamNames);
	++index)
   {
      if (tscopeBeamNames[index] == uppercaseName)
      {
	 return static_cast<TscopeBeam>(index);
      }
   }
   
   return TSCOPE_INVALID_BEAM;
}


static const char *tscopeTuningNames[] = {
   "TUNINGA",
   "TUNINGB",
   "TUNINGC",
   "TUNINGD"
};


string SseTscopeMsg::tuningToName(TscopeTuning tuning)
{
   Assert(ARRAY_LENGTH(tscopeTuningNames) == TSCOPE_N_TUNINGS);

   if (tuning >= 0 && tuning < ARRAY_LENGTH(tscopeTuningNames))
   { 
      return tscopeTuningNames[tuning];
   }
   
   return "SseTscopeMsg Error: invalid tuning";
}

TscopeTuning SseTscopeMsg::nameToTuning(const string & name)
{
   Assert(ARRAY_LENGTH(tscopeTuningNames) == TSCOPE_N_TUNINGS);

   string uppercaseName(SseUtil::strToUpper(name));
   for (int index = 0; index < ARRAY_LENGTH(tscopeTuningNames);
	++index)
   {
      if (tscopeTuningNames[index] == uppercaseName)
      {
	 return static_cast<TscopeTuning>(index);
      }
   }
   
   return TSCOPE_INVALID_TUNING;
}

void SseTscopeMsg::marshall(TscopeBeam &tscopeBeam)
{
   tscopeBeam = static_cast<TscopeBeam>(htonl(tscopeBeam));
}

void SseTscopeMsg::marshall(TscopeTuning &tuning)
{
   tuning = static_cast<TscopeTuning>(htonl(tuning));
}

void SseTscopeMsg::marshall(TscopePointing::CoordSys &coordSys)
{
   coordSys = static_cast<TscopePointing::CoordSys>(htonl(coordSys));
}


void SseTscopeMsg::marshall(TscopeCalRequest::CalType &calType)
{
   calType = static_cast<TscopeCalRequest::CalType>(htonl(calType));
}


void SseTscopeMsg::marshall(TscopeNullType::NullType &nullType)
{
   nullType = static_cast<TscopeNullType::NullType>(htonl(nullType));
}


//-------------------

static const char *tscopeCoordSysNames[] = {
   "AZEL",
   "J2000",
   "GAL",
   "UNINIT"
};

string SseTscopeMsg::coordSysToString(
   const TscopePointing::CoordSys & coordSys)
{
   if (coordSys >= 0 && coordSys < ARRAY_LENGTH(tscopeCoordSysNames))
   {
      return tscopeCoordSysNames[coordSys];
   }

   return "SseTscopeMsg Error: invalid coordSys";
}

TscopePointing::CoordSys SseTscopeMsg:: stringToCoordSys(
       const string & coordSysString)
{
   string uppercaseName(SseUtil::strToUpper(coordSysString));
   for (int index = 0; index < ARRAY_LENGTH(tscopeCoordSysNames);
	++index)
   {
      if (tscopeCoordSysNames[index] == uppercaseName)
      {
	 return static_cast<TscopePointing::CoordSys>(index);
      }
   }
   return TscopePointing::UNINIT;
}

//-------------------

static const char *tscopeCalTypeNames[] = {
   "delay",
   "phase",
   "freq",
   "uninit"
};

string SseTscopeMsg::calTypeToString(
   const TscopeCalRequest::CalType & calType)
{
   if (calType >= 0 && calType < ARRAY_LENGTH(tscopeCalTypeNames))
   {
      return tscopeCalTypeNames[calType];
   }

   return "SseTscopeMsg Error: invalid calType";
}

TscopeCalRequest::CalType SseTscopeMsg::stringToCalType(
       const string & calTypeString)
{
   string lowercaseName(SseUtil::strToLower(calTypeString));
   for (int index = 0; index < ARRAY_LENGTH(tscopeCalTypeNames);
	++index)
   {
      if (tscopeCalTypeNames[index] == lowercaseName)
      {
	 return static_cast<TscopeCalRequest::CalType>(index);
      }
   }
   return TscopeCalRequest::UNINIT;
}

//-------------------

static const char *tscopeNullTypeNames[] = {
   "axial",
   "projection",
   "none",
   "uninit"
};

string SseTscopeMsg::nullTypeToString(
   const TscopeNullType::NullType & nullType)
{
   if (nullType >= 0 && nullType < ARRAY_LENGTH(tscopeNullTypeNames))
   {
      return tscopeNullTypeNames[nullType];
   }

   return "SseTscopeMsg Error: invalid nullType";
}

TscopeNullType::NullType SseTscopeMsg::stringToNullType(
       const string & nullTypeString)
{
   string lowercaseName(SseUtil::strToLower(nullTypeString));
   for (int index = 0; index < ARRAY_LENGTH(tscopeNullTypeNames);
	++index)
   {
      if (tscopeNullTypeNames[index] == lowercaseName)
      {
	 return static_cast<TscopeNullType::NullType>(index);
      }
   }
   return TscopeNullType::UNINIT;
}
