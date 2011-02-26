/*******************************************************************************

 File:    CmdLineParser.h
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


#ifndef CmdLineParser_H
#define CmdLineParser_H

#include <string>
#include <vector>
#include <sstream>

using std::string;
using std::vector;
using std::stringstream;

class CmdLineOption;

class CmdLineParser
{
 public:
    CmdLineParser();

    string getUsage();

    void addFlagOption(const string &name,
                       const string &helpText = "");
    bool getFlagOption(const string &name);

    void addStringOption(const string &name,
                         const string &defaultValue,
                         const string &helpText = "");

    string getStringOption(const string &name);

    void addDoubleOption(const string &name,
                         double defaultValue,
                         const string &helpText = "");

    double getDoubleOption(const string &name);

    void addIntOption(const string &name,
                      int defaultValue,
                      const string &helpText = "");
    
    int getIntOption(const string &name);

    bool parse(int argc, char *argv[]);
    string getErrorText();

    virtual ~CmdLineParser();

 private:

    string indent();

    typedef vector<CmdLineOption*> OptionList;
    OptionList options_;

    stringstream errorStrm_;
    stringstream commandSummaryStrm_;
    stringstream usageHelpStrm_;
    string progName_;

    // Disable copy construction & assignment.
    // Don't define these.
    CmdLineParser(const CmdLineParser& rhs);
    CmdLineParser& operator=(const CmdLineParser& rhs);

};

#endif // CmdLineParser_H