/*******************************************************************************

 File:    SseTscopeMsg.h
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


#ifndef _sse_tscope_msg_h
#define _sse_tscope_msg_h

#include "sseTscopeInterface.h"

using std::string;

class SseTscopeMsg
{
 public:

    static string getMessageCodeString(uint32_t code);

    static string beamToName(TscopeBeam beam);
    static TscopeBeam nameToBeam(const string & name);

    static string tuningToName(TscopeTuning tuning);
    static TscopeTuning nameToTuning(const string & name);

    static string coordSysToString(
       const TscopePointing::CoordSys & coordSys);

    static TscopePointing::CoordSys stringToCoordSys(
       const string & coordSysString);

    static string calTypeToString(
       const TscopeCalRequest::CalType & calType);

    static TscopeCalRequest::CalType stringToCalType(
       const string & calTypeString);

    static string nullTypeToString(
       const TscopeNullType::NullType & nullType);

    static TscopeNullType::NullType stringToNullType(
       const string & nullTypeString);

    static void marshall(TscopeBeam &tscopeBeam);
    static void marshall(TscopePointing::CoordSys &coordSys);
    static void marshall(TscopeCalRequest::CalType &calType);
    static void marshall(TscopeNullType::NullType &nullType);
    static void marshall(TscopeTuning &tuning);

 private:

    // prevent instantiation
    SseTscopeMsg();
    ~SseTscopeMsg();

};

#endif // _sse_tscope_msg_h