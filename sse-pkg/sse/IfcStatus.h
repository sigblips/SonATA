/*******************************************************************************

 File:    IfcStatus.h
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


#ifndef IfcStatus_H
#define IfcStatus_H

#include "sseInterface.h"
#include "Signals.h"
#include <string>
#include <iosfwd>

using std::string;
using std::ostream;

struct IfcStatus
{
   string name;
   NssDate timeStamp;

   int32_t attnDbLeft;
   int32_t attnDbRight;

   string ifSource;
   string stxPol;
   int32_t stxStatus;
   bool goodStxStatus;

   int32_t stxCountLeft;
   double stxMeanLeft;
   double stxVarLeft;

   int32_t stxCountRight;
   double stxMeanRight;
   double stxVarRight;

   IfcStatus();
   friend ostream& operator << (ostream& strm, const IfcStatus& status);

};




#endif // IfcStatus_H