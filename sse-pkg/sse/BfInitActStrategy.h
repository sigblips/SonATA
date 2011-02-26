/*******************************************************************************

 File:    BfInitActStrategy.h
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


#ifndef BfInitActStrategy_H
#define BfInitActStrategy_H

#include "BfActStrategy.h"

class BfInitActStrategy : public BfActStrategy
{
 public:
   BfInitActStrategy(Scheduler *scheduler,
                     Site *site,
                     const NssParameters &nssParameters,
                     int verboseLevel);

   virtual ~BfInitActStrategy();
   
 protected:
   // base class overrides
   virtual Activity *createNewActivity(NssComponentTree *nssComponentTree);

 private:

   // Disable copy construction & assignment.
   // Don't define these.
   BfInitActStrategy(const BfInitActStrategy& rhs);
   BfInitActStrategy& operator=(const BfInitActStrategy& rhs);

};

#endif // BfInitActStrategy_H