/*******************************************************************************

 File:    tokenize.cpp
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


/*
Split a line into tokens, based on the specified delimeter 
list.  The delimiter is discarded.

Note: Multiple delimiters in a row will be treated as a single delimiter.
eg, "abc,,def" will be split into 2 fields: 'abc' and 'def'.

Based on code from Usenet:

From: Chris Newton (chrisnewton@no.junk.please.btinternet.com)
Subject: Re: tokenize a string 
Newsgroups: alt.comp.lang.learn.c-c++, comp.lang.c++
Date: 2000-08-23 12:37:26 PST 

*/

#include <SseUtil.h>

vector<string> SseUtil::tokenize(const string & source, const string &
			delimiters)
{
  vector<string> tokens;

  string::size_type start = 0;
  string::size_type end = 0;

  while ((start = source.find_first_not_of(delimiters, start)) !=
	 string::npos) {

    end = source.find_first_of(delimiters, start);
    if (end == string::npos)  // don't go off the end
    {
	end = source.size();  // get the last token
    }
    tokens.push_back(source.substr(start, end - start));

    start = end;

  }

  return tokens;
}