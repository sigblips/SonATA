/*******************************************************************************

 File:    SignalArchiver.cpp
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



#include "Verbose.h"  // include this first so ACE header is defined first
#include "SignalArchiver.h" 
#include "SseArchive.h"
#include "SseUtil.h"
#include "SseDxMsg.h"
#include <iostream>
#include <string>

using namespace std;

static void openFile(ofstream &outstrm, const string &filename);

SignalArchiver::SignalArchiver()
    : dxHostname_("host-unknown"),
      verboseLevel_(0)
{
}

SignalArchiver::~SignalArchiver()
{
}

void SignalArchiver::setVerboseLevel(int level)
{
    verboseLevel_ = level;
}

int SignalArchiver::getVerboseLevel()
{
    return verboseLevel_;
}



void SignalArchiver::setDxHostname(const string &hostname)
{
    dxHostname_ = hostname;
}

void SignalArchiver::archiveSignal(const ArchiveDataHeader &archiveDataHeader)
{
    VERBOSE1(getVerboseLevel(), "archiving signal: dx " <<
	     archiveDataHeader.signalId.dxNumber << ", act: " <<
	     archiveDataHeader.signalId.activityId << ", number: " <<
	     archiveDataHeader.signalId.number << endl;);

    VERBOSE2(getVerboseLevel(), "SignalArchiver::archiveSignal" <<
	     archiveDataHeader; );

    // open both left & right pol files for this signal Id.
    openArchiveFiles(archiveDataHeader);
}

void SignalArchiver::beginSendingArchiveComplexAmplitudes(const Count &count)
{
    VERBOSE2(getVerboseLevel(),
	     "SignalArchiver::beginSendingArchiveComplexAmplitudes" << endl
	     << "count is: " << count;);
}

void SignalArchiver::sendArchiveComplexAmplitudes(
    const ComplexAmplitudeHeader &hdr,
    SubchannelCoef1KHz subchannelArray[])
{
    VERBOSE2(getVerboseLevel(),
	     "SignalArchiver::sendArchiveComplexAmplitudes()" << endl;);

    if (hdr.pol == POL_LEFTCIRCULAR)
    {
	writeComplexAmplitudesToArchiveFile(outFileLeft_, hdr, subchannelArray);
    }
    else if (hdr.pol == POL_RIGHTCIRCULAR)
    {
	writeComplexAmplitudesToArchiveFile(outFileRight_, hdr, subchannelArray);
    } 
    else
    {
	// invalid pol
	// TBD better error handling 

	cerr << "Error: invalid pol received" << endl;
    }



}

void SignalArchiver::doneSendingArchiveComplexAmplitudes()
{
    VERBOSE2(getVerboseLevel(),
	     "SignalArchiver::doneSendingArchiveComplexAmplitudes()" << endl;);

    closeArchiveFiles();
}


    
void SignalArchiver::openArchiveFiles(const ArchiveDataHeader &hdr)
{
/*
     Create filename in this form:
    
      <sonata_archive_dir>/YYYY-MM-DD/actNNNN/
          actNNNN.<date/time>.dxX.id-Y.archive-compamp
    
     containing the activity number, dx name, and
     the signal number within that activity for that dx.

     TBD: Get the isoDate from the signal id info,
     and use that to determine the archive subdir?
     Except for a day changeover during an observation
     this should be the same as the current date.
     The drawback to using the current
     date is that some of the files can appear in
     the next day's directory, but it
     is theoretically more robust than trusting the date
     in the signal Id.
 */

    string actId(SseUtil::intToStr(hdr.signalId.activityId));

    // Prepare the <sonata_archive_dir>/YYYY-MM-DD/act<actid>
    // directory.

    string sseArchiveDir(SseArchive::prepareDataProductsDir(
                             hdr.signalId.activityId));

    // For now, use the dx hostname as the dx identifier.
    // TBD use the dx number from the identifier?
    //string dxId =  "dx" + SseUtil::intToStr(hdr.signalId.dxNumber);
    string & dxId(dxHostname_);

    string signalNumber(SseUtil::intToStr(hdr.signalId.number));

    // create the output file, with the signal id info in the filename
    string filePrefix = sseArchiveDir
        + SseUtil::currentIsoDateTimeSuitableForFilename() +
	+ ".act" + actId + "." + dxId + "."
	+ "id-" + signalNumber;

    string fileSuffix = ".archive-compamp";

    string filenameLeft = filePrefix + ".L" + fileSuffix;
    openFile(outFileLeft_, filenameLeft);

    string filenameRight = filePrefix + ".R" + fileSuffix;
    openFile(outFileRight_, filenameRight);
    
}

static void openFile(ofstream &outstrm, const string &filename)
{
    outstrm.open(filename.c_str(), (ios::out | ios::binary ));
    if (!outstrm.is_open())
    {
	cerr << "File Open failed on " << filename << endl;
	
	// tbd better error handling
    }
    
}

// Write the compamp data, in marshalled format

void SignalArchiver::writeComplexAmplitudesToArchiveFile(
    ofstream &outstrm,
    const ComplexAmplitudeHeader &hdr,
    SubchannelCoef1KHz subchannelArray[])
{
    // save this off since it can be invalidated by marshalling
    int32_t nSubchannels = hdr.numberOfSubchannels;

    ComplexAmplitudeHeader marshalledHdr(hdr);
    marshalledHdr.marshall();
    outstrm.write((const char*) &marshalledHdr, sizeof(marshalledHdr));

    // Write the data array.
    // It does not require marshalling, since it's already
    // platform independent.

    outstrm.write((const char*) subchannelArray, sizeof(SubchannelCoef1KHz) * nSubchannels);
}



void SignalArchiver::closeArchiveFiles()
{
    outFileLeft_.close();
    outFileRight_.close();
}