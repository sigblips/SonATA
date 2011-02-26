/*******************************************************************************

 File:    SignalReport.cpp
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

#include "SignalReport.h" 
#include "SseUtil.h"
#include "SseMsg.h"
#include "SseDxMsg.h"
#include <iomanip>
#include "Assert.h"
#include <iostream>
#include <fstream>

using namespace std;

static const int defaultPageSize = 49;

SignalReport::SignalReport(
   const string &activityName,
   int activityId,
   const string &dxName,
   const string &reportType,
   const string &dxTuningInfo)
   : 
   activityName_(activityName),
   activityId_(activityId),
   dxName_(dxName),
   reportType_(reportType),
   dxTuningInfo_(dxTuningInfo),
   creationTime_(SseUtil::currentIsoDateTime()),
   pageSize_(defaultPageSize)
{
   // show all decimal places up to precision
   signalStrm_.setf(std::ios::fixed);  
}

SignalReport::~SignalReport()
{
}

void SignalReport::setPageSize(size_t pageSize)
{
   Assert(pageSize > 0);
   pageSize_ = pageSize;
}

size_t SignalReport::getPageSize()
{
   return pageSize_;
}


int SignalReport::getActivityId() const
{
   return activityId_;
}

stringstream & SignalReport::getSignalStrm()
{
   return signalStrm_;
}

void SignalReport::addText(const string &text)
{
   getSignalStrm() << text;
}

void SignalReport::printPreamble(ostream &strm) const
{
   strm 
      << "# ==================\n"
      << "# NSS Signal Reports\n"
      << "# ==================\n";

   strm
      << "# Activity Name: " << activityName_ << "\n"
      << "# Activity Id  : " << activityId_ << "\n"
      << "# Creation Time: " << creationTime_ << "\n"
      << "# Report Type  : " << reportType_ << "\n"
      << "# Dx Name     : " << dxName_ << "\n";

   strm
      << dxTuningInfo_ << endl;

// Disable this for now.  Most of this info can
// probably be documented elsewhere, freeing up some report space.
#if 0   
   strm
      << "# Signal report table header explanation:\n"
      << "# Freq     : Signal frequency (MHz)\n"
      << "# Typ      : Signal type: CwP = cw power, "
      << "Pul = pulse, CwC = cw coherent\n"
      << "# SigId    : Signal Id\n"
      << "# Sbnd     : subchannel # containing the signal\n"
      << "# Drift    : Drift rate (+- Hz/Sec)\n"
      << "# Width    : Signal width (Hz)\n"
      << "# Power    : Power (in Janskys?)\n"
      << "# Pol      : Polarization: L = left, R = right, B = both, "
      << "M = mixed\n"
      << "# BB       : Contains Bad Bands\n"
      << "# Clss     : Classification: Cand = candidate, RFI, TEST, " 
      << "Unkn = unknown\n"
      << "# Reason   : Reason for classification\n"
      << "# PFA      : Prob. of false alarm (e to the -x)\n"
      << "# SNR      : Signal to noise ratio in a 1 Hz channel\n"
      << endl;
#endif
}

void SignalReport::printCondensedSigDescripHeader(ostream &strm) const
{
   strm 
      << "#Freq           Sig Sub  Pol Signal Drift Wid  Pwr  BB Clss Reason  Orig PFA      SNR \n"
      << "#MHz            Typ band     Id     Hz/S  Hz                        Dx               \n"
      << "#-------------- --- ---- --- ------ ----- ---- ---- -- ---- ------- ---- -------- ----- " 
      << endl;
}


void SignalReport::printCondensedSigDescrip(
   ostream &strm, 
   const string &sigType, 
   const SignalDescription &sig)
{
   // rfFreq
   strm.precision(9);  // milliHz
   strm << setw(15) << sig.path.rfFreq << " ";

   // sigType
   strm << setw(3) << sigType.c_str() << " ";

   // subchannelNumber
   strm << setw(4) << sig.subchannelNumber << " ";

   // polarization (first letter only)
   char pol = SseMsg::polarizationToSingleUpperChar(sig.pol);
   strm << setw(3) << pol << " ";

    // signalId (number subfield only)
   strm << setw(6) << sig.signalId.number << " ";

   // drift
   strm.precision(2);
   strm << setw(5) << sig.path.drift << " ";

    // width
   strm.precision(2); 
   strm << setw(4) << sig.path.width << " ";

    // power
   strm.precision(0);
   strm << setw(4) << sig.path.power << " ";


    // contains bad bands
   char badBandChar = 'N';
   if (sig.containsBadBands)
   {
      badBandChar = 'Y';
   }
   strm << setw(2) << badBandChar << " ";


    // classification
   string classify = SseDxMsg::signalClassToString(sig.sigClass);
   int len = 4;
   strm << setw(len) << classify.substr(0, len).c_str() << " ";

   // reason for classification
   string reason = SseDxMsg::signalClassReasonToBriefString(sig.reason);
   len = 7;
   strm << setw(len) << reason.substr(0, len).c_str() << " ";

   // orig dx number
   strm << setw(4) << sig.origSignalId.dxNumber << " ";
}

void SignalReport::printCondensedConfirmStats(
   ostream &strm,
   const ConfirmationStats &stats)
{

   // PFA
   strm.precision(2); 
   strm << setw(8) << stats.pfa << " ";

   // SNR
   strm.precision(2);
   strm << setw(5) << stats.snr << " ";
}

static void printPageFooter(ostream &strm, int pageNum, int nPages,
			    int activityId)
{
   strm << endl
        << "#   -----  Page " 
        << pageNum << " of " << nPages << "."
        << "  Activity Id: " << activityId
        << " ------ " << endl;
}

void SignalReport::printPageHeader(ostream &strm) const
{
   // Do nothing.  Let subclasses override as desired
}


ostream& operator << (ostream &strm, const SignalReport &report)
{
   // Print the signal report, one line at at time.
   // Report already contains the preamble and first table header.
   // For each new page, repeat table header.

   const string & outStr = report.signalStrm_.str();
   vector<string> lines = SseUtil::splitByDelimiter(outStr, '\n');

   int pageNum = 1;
   int nPages = lines.size() / report.pageSize_ + 1;

   for (size_t i=0; i< lines.size(); ++i)
   {
      // output the report line
      strm << lines[i] << "\n"; 

	// start a new page as needed
      if (i % report.pageSize_ == (report.pageSize_ -1))
      {
         printPageFooter(strm, pageNum++, nPages, 
                         report.getActivityId());

         strm << "\f";  // form feed
         strm << endl << endl;  // blank lines at top of page

         report.printPageHeader(strm);

      }
   }

   // put the footer on the last page
   printPageFooter(strm, pageNum++, nPages, 
                   report.getActivityId());

   return strm;
}

bool SignalReport::saveToFile(const string &filename)
{
   // open an output text stream attached to a file
   ofstream strm;
   strm.open(filename.c_str(), (ios::out | ios::trunc));
   if (!strm.is_open()) {
      return false;
   }

   strm << *this;

   strm.close();

   return true;
}

