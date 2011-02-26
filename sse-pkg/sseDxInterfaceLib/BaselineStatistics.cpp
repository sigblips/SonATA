/*******************************************************************************

 File:    BaselineStatistics.cpp
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
#include <sstream>

using std::stringstream;

BaselineLimits::BaselineLimits()
    :
    meanUpperBound(0.0),
    meanLowerBound(0.0),
    stdDevPercent(0.0),
    maxRange(0.0)
{
}

void BaselineLimits::marshall()
{
    HTONF(meanUpperBound);
    HTONF(meanLowerBound);
    HTONF(stdDevPercent);
    HTONF(maxRange);
}

void BaselineLimits::demarshall()
{
    marshall();
}

ostream& operator << (ostream &strm, const BaselineLimits &limits)
{
    strm << "Baseline Limits" << endl
	 << "---------------" << endl
	 << "meanUpperBound: " << limits.meanUpperBound << endl
	 << "meanLowerBound: " << limits.meanLowerBound << endl
	 << "stdDevPercent:  " << limits.stdDevPercent << endl
	 << "maxRange:       " << limits.maxRange << endl;

    return strm;
}


//----------- BaselineStatus -------------------

static const char *BaselineStatusStrings[] =
{
   "uninit", "good", "warning", "error"
};

string SseDxMsg::baselineStatusToString(BaselineStatus status)
{
    if (status < 0 || status >= ARRAY_LENGTH(BaselineStatusStrings))
    {
        return "SseDxMsg Error: invalid BaselineStatus";
    }

    return BaselineStatusStrings[status];
}

static void marshallBaselineStatus(BaselineStatus &status)
{
    status = static_cast<BaselineStatus>(htonl(status));
}


// -----  BaselineStatistics ----


BaselineStatistics::BaselineStatistics()
    : 
    mean(0.0),
    stdDev(0.0),
    range(0.0),
    halfFrameNumber(-1),
    rfCenterFreqMhz(0.0),
    bandwidthMhz(0.0),
    pol(POL_UNINIT),
    status(BASELINE_STATUS_UNINIT)
{
}

void BaselineStatistics::marshall()
{
    HTONF(mean);
    HTONF(stdDev);
    HTONF(range);
    HTONL(halfFrameNumber);
    HTOND(rfCenterFreqMhz);
    HTOND(bandwidthMhz);
    SseMsg::marshall(pol);     // Polarization
    marshallBaselineStatus(status);  // BaselineStatus
}

void BaselineStatistics::demarshall()
{
    marshall();
}

ostream& operator << (ostream &strm, const BaselineStatistics &stats)
{
    strm << "BaselineStatistics" << endl
	 << "------------------" << endl
	 << "mean:            " << stats.mean << endl
	 << "stdDev:          " << stats.stdDev << endl
	 << "range:           " << stats.range << endl
	 << "halfFrameNumber: " << stats.halfFrameNumber << endl
	 << "rfCenterFreqMhz: " << stats.rfCenterFreqMhz << endl
	 << "bandwidthMhz:    " << stats.bandwidthMhz << endl
	 << "Pol:             " << SseMsg::polarizationToString(stats.pol) << endl
	 << "Status:          " << SseDxMsg::baselineStatusToString(stats.status) << endl;

    return strm;
}

// ---- BaselineLimitsExceededDetails ------

BaselineLimitsExceededDetails::BaselineLimitsExceededDetails()
    :
    pol(POL_UNINIT)    
{
    description[0] = '\0';
}

void BaselineLimitsExceededDetails::marshall()
{
    SseMsg::marshall(pol);     // Polarization

    // no need to marshall char array
    // description
}

void BaselineLimitsExceededDetails::demarshall()
{
    marshall();
}

ostream& operator << (ostream &strm, const BaselineLimitsExceededDetails &details)
{
    strm << "BaselineLimitsExceededDetails" << endl
	 << "-----------------------------" << endl
	 << "Pol: " 
	 << SseMsg::polarizationToString(details.pol) << endl
	 << "Descrip: " << details.description << endl;

    return strm;
}
