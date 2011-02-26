/*******************************************************************************

 File:    checkSseTSigMsgSizes.cpp
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

// $ID

// checks all the struct sizes & field offsets
// in the sseIfcInterface messages, as a
// marshalling aid.

#include "sseTSigInterface.h"

#include <iostream>
#include <cstddef>

using std::cout;

#define SIZEOF(x) cout << "\nsizeof " << #x << ": " << sizeof(x) << endl

#define OFFSETOF(x,field) cout << "offsetof " << #field << ": " << offsetof(x,field) << endl

int main() {

  SIZEOF(TSigStatus);
  OFFSETOF(TSigStatus, name);
  OFFSETOF(TSigStatus, time_stamp);
  OFFSETOF(TSigStatus, testLO);
  OFFSETOF(TSigStatus, test_signal);
  OFFSETOF(TSigStatus, attn);
}