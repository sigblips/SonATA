/*******************************************************************************

 File:    AttnBank.h
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


#ifndef AttnBank_H
#define AttnBank_H

// Control an attenuator bank via a
// switch control unit (SCU).
// Bank consists of attenutor steps in db
// and their associated switch ID.
// The strobe method is called after the
// setAttnLeveldb method to force the attn
// change.

#include <vector>

using std::vector;

class AT34970;  // SCU

class AttnBank
{
 public:
    AttnBank(AT34970 & scu);
    virtual ~AttnBank();

    // initialize bank:
    void addStepLevelDbAndSwitchId(int stepDb, int switchId);
    void addStrobeSwitchId(int switchId);

    // set and get current attn setting
    void setAttnLevelDb(int levelDb);
    int getAttnLevelDb();

 private:

    void strobe();
    
    struct AttnStep
    {
       int valueDb_;
       int switchId_;
       AttnStep(int valueDb, int switchId);
    };

    friend bool attnStepHigherDbValue(const AttnStep & attnStep1,
			       const AttnStep & attnStep2);
    
    vector<AttnStep> attnStepContainer_;
    vector<int> strobeSwitchIds_;
    AT34970 & scu_;

    // Disable copy construction & assignment.
    // Don't define these.
    AttnBank(const AttnBank& rhs);
    AttnBank& operator=(const AttnBank& rhs);

};

#endif // AttnBank_H