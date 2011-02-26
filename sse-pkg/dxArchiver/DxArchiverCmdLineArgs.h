/*******************************************************************************

 File:    DxArchiverCmdLineArgs.h
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


#ifndef DxArchiverCmdLineArgs_H
#define DxArchiverCmdLineArgs_H

// processes (parses) command line arguments

#include <string>
using std::string;

class DxArchiverCmdLineArgs
{
 public:
    DxArchiverCmdLineArgs(const string &progName, 
			   const string &defaultSseHostname,
			   int defaultSsePort, 
			   int defaultDxPort, 
			   const bool defaultNoUi,
			   const string &defaultName);

    virtual ~DxArchiverCmdLineArgs();
    bool parseArgs(int argc, char *argv[]);
    void usage();

    int getSsePort();
    int getDxPort();
    const string &getSseHostname();
    bool getNoUi();
    const string &getName();

 private:
    // Disable copy construction & assignment.
    // Don't define these.
    DxArchiverCmdLineArgs(const DxArchiverCmdLineArgs& rhs);
    DxArchiverCmdLineArgs& operator=(const DxArchiverCmdLineArgs& rhs);

    bool getPortValue(const char *value, const string &keyword, int *port);
    bool getSseHostnameValue(const char *value);
    bool getNameValue(const char *value);

 private:
    string progName_;

    string ssePortKeyword_;
    int ssePort_;
    int defaultSsePort_;

    string dxPortKeyword_;
    int dxPort_;
    int defaultDxPort_;

    string sseHostnameKeyword_;
    string sseHostname_;
    string defaultSseHostname_;

    string noUiKeyword_;
    bool noUi_;
    bool defaultNoUi_;

    string nameKeyword_;
    string name_;
    string defaultName_;


};

#endif // DxArchiverCmdLineArgs_H