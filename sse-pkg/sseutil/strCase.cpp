/*******************************************************************************

 File:    strCase.cpp
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

///////////////////////////////////////////////////////////////////////////////
//
// case-insensitive string compare
// based on Josuttis "C++ Std Library"
//
// The book by Josuttis is The C++ Standard Library - A Tutorial and Reference 
//
// According to the book's copyright, it is OK to use this code if the
// following copyright notice is included:
//
// Copyright 1999 by Addison Wesley Longman, Inc. and Nicolai M. Josuttis.
// All rights reserved.
// 
// Permission to use, copy, modify and distribute this software for personal and 
// educational use is hereby granted without fee, provided that the above 
// copyright notice appears in all copies and that both that copyright notice and 
// this permission notice appear in supporting documentation, and that the names 
// of Addison Wesley Longman or the author are not used in advertising or 
// publicity pertaining to distribution of the software without specific, written 
// prior permission. Addison Wesley Longman and the author make no 
// representations about the suitability of this software for any purpose. It is 
// provided "as is" without express or implied warranty.
// 
// ADDISON WESLEY LONGMAN AND THE AUTHOR DISCLAIM ALL WARRANTIES WITH REGARD TO 
// THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. 
// IN NO EVENT SHALL ADDISON WESLEY LONGMAN OR THE AUTHOR BE LIABLE FOR ANY 
// SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
// FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
// PERFORMANCE OF THIS SOFTWARE. 
//
///////////////////////////////////////////////////////////////////////////////

#include <SseUtil.h>
#include <algorithm>
#include <iostream>

using namespace std;

// std::tolower & toupper take and returns an int, not a char. 
// Use wrappers to make them more standard-conforming.

inline char charToLower(char c) { return tolower(c); }
inline char charToUpper(char c) { return toupper(c); }

static bool nocaseCompare(char c1, char c2)
{
    return charToLower(c1) == charToLower(c2);
}

bool SseUtil::strCaseEqual(const string &str1, const string &str2)
{
    if (str1.size() == str2.size() && 
	std::equal (str1.begin(), str1.end(),
		    str2.begin(), nocaseCompare))
    {
	return true;
    }
    
    return false;
}

string SseUtil::strToLower(const string &strValue)
{
    string outString(strValue);

    transform(outString.begin(), outString.end(), 
	      outString.begin(),
	      charToLower);

    return outString;
}

string SseUtil::strToUpper(const string &strValue)
{
    string outString(strValue);

    transform(outString.begin(), outString.end(), 
	      outString.begin(),
	      charToUpper);

    return outString;
}
