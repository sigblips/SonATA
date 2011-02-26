/*******************************************************************************

 File:    splitByDelimiter.cpp
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


#include <SseUtil.h>

/*
 Split the string into chunks by breaking on the
 delimiter.  There is always one implied delimiter
 at the end.

 Example inputs & outputs:

 ["abc,def"] -> 2 tokens: ["abc"] ["def"]
 ["a,,c"] -> 3 tokens: ["a"] [""] ["c"]
 ["abc,"] -> two tokens:  ["abc"] [""]
 ["abc"] -> one token: ["abc"]
 [""] -> one token: [""]

*/

vector<string> SseUtil::splitByDelimiter(
   const string & source, char delimiter)
{
  vector<string> tokens;
  string::size_type start(0);
  string::size_type end(0);

  // Add a final delimiter to the end to make it easy
  // to get the last token.
  string localSource(source + delimiter);

  while ((end = localSource.find_first_of(delimiter, start)) !=
         string::npos)
  {
     string::size_type len(end-start);
     if (len < 1)
     {
        tokens.push_back(""); 
     }
     else
     {
        tokens.push_back(localSource.substr(start, end - start)); 
     }
     start = end + 1;
  }

  return tokens;
}