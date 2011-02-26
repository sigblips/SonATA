/*******************************************************************************

 File:    mld.h
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


// mld.h,v 1.5 1998/11/15 17:05:29 jcej Exp

#ifndef MLD_H
#define MLD_H

#include "ace/Synch.h"
#include <ace/Version.h>
#if (ACE_MAJOR_VERSION >= 5) && (ACE_MINOR_VERSION >= 2) && (ACE_BETA_VERSION >=4)
#include <ace/Atomic_Op.h>
#endif

// tbd make sure this is defined for ace 5.3,
#if (ACE_MAJOR_VERSION >= 5) && (ACE_MINOR_VERSION >= 3)
#include <ace/Atomic_Op_T.h>
#endif


#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Singleton.h"

/*
   This is a cheap memory leak detector.  Each class I want to watch over
   contains an mld object.  The mld object's ctor increments a global counter
   while the dtor decrements it.  If the counter is non-zero when the program
   is ready to exit then there may be a leak.
 */

class mld
{
public:
    mld (void);
    ~mld (void);

    static int value (void);

protected:
    static ACE_Atomic_Op < ACE_Mutex, int >counter_;
};

// ================================================

/*
   Just drop 'MLD' anywhere in your class definition to get cheap memory leak
   detection for your class.
 */
#define MLD            mld mld_

/*
   Use 'MLD_COUNTER' in main() to see if things are OK.
 */
#define MLD_COUNTER    mld::value()

// ================================================

#endif