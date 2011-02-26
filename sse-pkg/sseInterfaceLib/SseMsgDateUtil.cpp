/*******************************************************************************

 File:    SseMsgDateUtil.cpp
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


#include "SseUtil.h" 
#include "sseInterface.h"
#include "SseMsg.h"
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

// get the current date-time in NssDate format

NssDate SseMsg::currentNssDate()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);

    NssDate date;
    date.tv_sec = tp.tv_sec;
    date.tv_usec = tp.tv_usec;

    return date;
}

// convert NssDate to isoDateTime string
string SseMsg::isoDateTime(const NssDate &nssDate)
{
    // TBD fold in tv_usec

    return SseUtil::isoDateTime(static_cast<time_t>(nssDate.tv_sec));

}