/*******************************************************************************

 File:    BandReport.h
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

/*
 * BandReport: report a bad band.
 *
 *  Created on: Mar 6, 2009
 *      Author: kes
 */

#ifndef _BandReportH
#define _BandReportH

#include "CwBadBandList.h"
#include "Report.h"

using dx::CwBadBandList;

class BandReport: public ReportBadBand
{
public:
	BandReport(Polarization pol_, dx::CwBadBandList *badBandList_): pol(pol_),
			badBandList(badBandList_) {}
	virtual void report(const DaddBand& band) {
		dx::CwBadBand badBand;
		badBand.band.bin = band.bin;
		badBand.band.width = band.width;
		badBand.band.pol = pol;
		badBand.paths = band.hits;
		badBand.maxPaths = band.limit;
		badBand.maxPath = band.maxPath;
		badBandList->record(badBand);
	}

private:
	Polarization pol;
	CwBadBandList *badBandList;
};


#endif /* _BandReportH */