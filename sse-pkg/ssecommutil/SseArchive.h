/*******************************************************************************

 File:    SseArchive.h
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


#ifndef SseArchive_H
#define SseArchive_H

#include <string>
#include "Log.h"

using std::string;

class SseArchive
{
 public:
    static string getArchiveDir();
    static string getArchiveTemplogsDir();
    static string getArchivePermlogsDir();
    static string getArchiveSystemlogsDir();
    static string getArchiveErrorlogsDir();
    static string getArchiveSystemDir();
    static string getConfirmationDataDir();
    static string prepareDataProductsDir(int actId);
    static void setup();

    // Define classes that can be used for logging.
    // example use:  SseArchive::ErrorLog() << "error message" << endl;

    class ErrorLog : public Log
    {
    public:
      ErrorLog();

    };

    class SystemLog : public Log
    {
    public:
      SystemLog();
    private:
    };

 private:
    // Disable copy construction & assignment.
    // Don't define these.
    SseArchive(const SseArchive& rhs);
    SseArchive& operator=(const SseArchive& rhs);

    SseArchive();
    virtual ~SseArchive();


};

#endif // SseArchive_H