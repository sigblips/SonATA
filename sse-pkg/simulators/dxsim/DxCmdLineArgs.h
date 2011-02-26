/*******************************************************************************

 File:    DxCmdLineArgs.h
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


#ifndef DxCmdLineArgs_H
#define DxCmdLineArgs_H

// processes (parses) command line arguments

#include <string>
using std::string;

class DxCmdLineArgs
{
 public:
    DxCmdLineArgs(const string &progName, 
		   int defaultMainPort, 
		   int defaultRemotePort, 
		   const string &defaultHost,
		   const string &defaultSciDataDir,
		   const string &defaultSciDataPrefix,
		   const bool defaultBroadcast,
		   const bool defaultNoUi,
		   const bool varyOutputData,
		   const string &defaultDxName,
		   const bool defaultRemoteMode);

    virtual ~DxCmdLineArgs();
    bool parseArgs(int argc, char *argv[]);
    void usage();

    int getMainPort();
    int getRemotePort();
    const string &getHostName();
    const string &getSciDataDir();
    const string &getSciDataPrefix();
    bool getBroadcast();
    bool getNoUi();
    bool getVaryOutputData();
    bool getRemoteMode();
    const string &getDxName();

 private:
    // Disable copy construction & assignment.
    // Don't define these.
    DxCmdLineArgs(const DxCmdLineArgs& rhs);
    DxCmdLineArgs& operator=(const DxCmdLineArgs& rhs);

    bool getPortValue(const char *value, const string &keyword, int *port);
    bool getHostnameValue(const char *value);
    bool getSciDataDirValue(const char *value);
    bool getSciDataPrefixValue(const char *value);
    bool getDxNameValue(const char *value);

 private:
    string progName_;

    string mainPortKeyword_;
    int mainPort_;
    int defaultMainPort_;

    string remotePortKeyword_;
    int remotePort_;
    int defaultRemotePort_;

    string hostNameKeyword_;
    string hostName_;
    string defaultHostName_;

    string sciDataDirKeyword_;
    string sciDataDir_;
    string defaultSciDataDir_;

    string sciDataPrefixKeyword_;
    string sciDataPrefix_;
    string defaultSciDataPrefix_;

    string broadcastKeyword_;
    bool broadcast_;
    bool defaultBroadcast_;

    string noUiKeyword_;
    bool noUi_;
    bool defaultNoUi_;

    string varyOutputDataKeyword_;
    bool varyOutputData_;
    bool defaultVaryOutputData_;

    string remoteModeKeyword_;
    bool remoteMode_;
    bool defaultRemoteMode_;

    string dxNameKeyword_;
    string dxName_;
    string defaultDxName_;


};

#endif // DxCmdLineArgs_H