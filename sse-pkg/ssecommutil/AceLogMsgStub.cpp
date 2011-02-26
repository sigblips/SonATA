/*******************************************************************************

 File:    AceLogMsgStub.cpp
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



#include <ace/OS.h>
#include <ace/Log_Msg.h>

// This method is undefined when linking against the stlport library.
// TBD: where is this being called from?
// Probably due to an ACE_DEBUG or ACE_ERROR_RETURN macro still being
// called somewhere in the sse-pkg.
// Use this temporary bypass for now to get the link to work.

void ACE_Log_Msg::set(char const *, int, int, int, int, std::ostream *, ACE_Log_Msg_Callback *)
{
    // do nothing
}
