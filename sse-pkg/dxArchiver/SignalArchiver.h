/*******************************************************************************

 File:    SignalArchiver.h
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


#ifndef SignalArchiver_H
#define SignalArchiver_H

#include "sseDxInterface.h"
#include <fstream>
#include <string>

class SignalArchiver
{
 public:
    SignalArchiver();
    virtual ~SignalArchiver();

    virtual void setDxHostname(const string &hostname);

    virtual void archiveSignal(const ArchiveDataHeader &archiveDataHeader);
    virtual void beginSendingArchiveComplexAmplitudes(const Count &count);
    virtual void sendArchiveComplexAmplitudes(const ComplexAmplitudeHeader &hdr,
                                     SubchannelCoef1KHz subchannelArray[]);
    virtual void doneSendingArchiveComplexAmplitudes();

    virtual void setVerboseLevel(int level);
    virtual int getVerboseLevel();

 private:
    // Disable copy construction & assignment.
    // Don't define these.
    SignalArchiver(const SignalArchiver& rhs);
    SignalArchiver& operator=(const SignalArchiver& rhs);

    ofstream outFileLeft_;
    ofstream outFileRight_;
    string dxHostname_;
    int verboseLevel_;

    void openArchiveFiles(const ArchiveDataHeader &hdr);
    void writeComplexAmplitudesToArchiveFile(
	ofstream &outstrm,
	const ComplexAmplitudeHeader &hdr,
	SubchannelCoef1KHz subchannelArray[]);
    void closeArchiveFiles();
};

#endif // SignalArchiver_H