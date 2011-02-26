/*******************************************************************************

 File:    RotatingFileLogger.h
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


#ifndef RotatingFileLogger_H
#define RotatingFileLogger_H

#include <string>
#include <fstream>

using std::string;
using std::ofstream;
using std::ostream;

class RotatingFileLogger
{
 public:
    RotatingFileLogger(const string &logDir,
                       const string &baseLogname,
                       int maxFileSizeBytes,
                       int maxFiles);

    virtual ~RotatingFileLogger();

    template <class Type>
    ostream & operator << (Type const & data)
    { 
       checkForFileRotation();

       strm_ << data;
       strm_.flush();

       return strm_;
    }

 private:
    void openStream();
    void closeStream();
    void rotateFiles();
    void shuffleFiles();
    void checkForFileRotation();
    int fileSizeBytes();

    string logDir_;
    string baseLogname_;
    string baseLogFullPath_;
    int maxFileSizeBytes_;
    int maxFiles_;

    ofstream strm_;

    // Disable copy construction & assignment.
    // Don't define these.
    RotatingFileLogger(const RotatingFileLogger& rhs);
    RotatingFileLogger& operator=(const RotatingFileLogger& rhs);

};

#endif // RotatingFileLogger_H