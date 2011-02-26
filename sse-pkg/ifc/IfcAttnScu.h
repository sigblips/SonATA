/*******************************************************************************

 File:    IfcAttnScu.h
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


#ifndef IfcAttnScu_H
#define IfcAttnScu_H

#include "SCU.h"
#include "AttnBank.h"

class IfcAttnScu : public AT34970
{
 public:

    IfcAttnScu(int bufferSize = DefaultBufferSize); 
    virtual ~IfcAttnScu();

    virtual void setAttnLeft(int level);
    virtual int getAttnLeft();
    
    virtual void setAttnRight(int level);
    virtual int getAttnRight();

    virtual void setIfSource(IfSource ifSource);
    virtual IfSource getIfSource();

 private:

    void initAttnBankLeft();
    void initAttnBankRight();

    // hold simulated attn db levels
    int simAttnDbLeft_; 
    int simAttnDbRight_; 
    
    AttnBank attnBankLeft_;
    AttnBank attnBankRight_;

    // Disable copy construction & assignment.
    // Don't define these.
    IfcAttnScu(const IfcAttnScu& rhs);
    IfcAttnScu& operator=(const IfcAttnScu& rhs);

};

#endif // IfcAttnScu_H