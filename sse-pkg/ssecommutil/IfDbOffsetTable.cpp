/*******************************************************************************

 File:    IfDbOffsetTable.cpp
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



#include "IfDbOffsetTable.h" 
#include "SseUtil.h"
#include "Interpolate.h"
#include <iostream>

using namespace std;


// Creates a table of attenuator db offset settings,
// indexed by sky frequency.
// Table entries are read from an ascii file of this form:
// <skyfreqMhz> <leftAttnOffsetDb> <rightAttnOffsetDb>

// # comment lines
// 2010 5 -2
// 2020 9 4
// etc.


struct IfDbOffsetTableInternal
{
   IfDbOffsetTableInternal(const string & filename);

   void loadTableFromFile(const string &filename);

   void parseLine(const string & line, const string &filename);

   int lookUpDbOffset(const InterpolateLinear &offsetInterp,
                      double skyFreqMhz);

   void printTable(ostream & strm);

   InterpolateLinear dbOffsetTableLeft_;
   InterpolateLinear dbOffsetTableRight_;
};


IfDbOffsetTableInternal::IfDbOffsetTableInternal(
   const string & filename)
{
   // load the offset table file & parse it
   loadTableFromFile(filename);
}

void IfDbOffsetTableInternal::parseLine(
   const string & line,
   const string &filename)
{
   // break the line into tokens
   vector<string> tokens = SseUtil::tokenize(line, " \t");
   unsigned int nExpectedTokensPerLine = 3;
   if (tokens.size() > 0)  // ignore blank lines
   {
      if (tokens.size() != nExpectedTokensPerLine)
      {
         cerr << "Warning: IfDbOffsetTable " << filename <<
            ": line is not of the form "
              << "<skyFreqMHz> <dbOffsetLeft> <dbOffsetRight>" << endl
              << "(invalid number of tokens)" << endl
              << "'" << line << "'" << endl;

         return; 
      }

      double skyFreqMhz(0);
      int dbOffsetLeft(0);
      int dbOffsetRight(0);
      try {

         // break out the <skyFreqMhz> <dboffsetLeft> <dboffsetRight>
         skyFreqMhz = SseUtil::strToDouble(tokens[0]);
         dbOffsetLeft = SseUtil::strToInt(tokens[1]);
         dbOffsetRight = SseUtil::strToInt(tokens[2]);

// debug
#if 0	    
         cout << "skyFreqMhz: " << skyFreqMhz
              << " dbOffsetLeft: " << dbOffsetLeft 
              << " dbOffsetRight: " << dbOffsetRight << endl;
#endif

      }
      catch (...)
      {

         cerr << "Warning: IfDbOffsetTable " << filename <<
            ": error parsing <skyfreqMHz> "
              << "<dbOffsetLeft> <dbOffsetRight> values" << endl
              << "(one or more tokens is not a number)" << endl
              << "'" << line << "'" << endl;

         return;
      }

      dbOffsetTableLeft_.addValues(skyFreqMhz,
                                   static_cast<double>(dbOffsetLeft));

      dbOffsetTableRight_.addValues(skyFreqMhz,
                                    static_cast<double>(dbOffsetRight));
	    
   }

} 


void IfDbOffsetTableInternal::loadTableFromFile(const string &filename)
{
   // TBD file access error handling
   vector<string> textVector = 
      SseUtil::loadFileIntoStringVector(filename);

   for (vector<string>::iterator i = textVector.begin();
        i != textVector.end(); ++i)
   {
      string line = *i;

      // erase any trailing comments
      string::size_type pos = line.find("#");
      if (pos != string::npos)
      {
         line.erase(pos);
      }

      parseLine(line, filename);
   }

}

/*
Lookup the dbOffset in the interpolation for the
given skyfreq. Value is truncated to int.
If not found, return 0.
*/

int IfDbOffsetTableInternal::lookUpDbOffset(
   const InterpolateLinear &offsetInterp,
   double skyFreqMhz)
{
   int dbOffset(0);
   double lookupValue(0);
   if (offsetInterp.inter(skyFreqMhz, lookupValue))
   {
      dbOffset = static_cast<int>(lookupValue);
   }

   return dbOffset;
}

void IfDbOffsetTableInternal::printTable(ostream & strm)
{
   strm << "IfDbOffsetTables: " << endl
        << "-----------------" << endl
        << "Left:" << endl
        << dbOffsetTableLeft_ << endl
        << "Right:" << endl
        << dbOffsetTableRight_ << endl;
}

// ------------------------------------------------

IfDbOffsetTable::IfDbOffsetTable(const string &filename)
{
   internal_ = new IfDbOffsetTableInternal(filename);
}

IfDbOffsetTable::~IfDbOffsetTable()
{
   delete internal_;
}

int IfDbOffsetTable::getDbOffsetLeft(double skyFreqMhz)
{
   return internal_->lookUpDbOffset(
      internal_->dbOffsetTableLeft_, skyFreqMhz);
}

int IfDbOffsetTable::getDbOffsetRight(double skyFreqMhz)
{
   return internal_->lookUpDbOffset(
      internal_->dbOffsetTableRight_, skyFreqMhz);
}


ostream& operator << (ostream &strm, 
		      const IfDbOffsetTable &table)
{
   table.internal_->printTable(strm);

   return strm;
}
