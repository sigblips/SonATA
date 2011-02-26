/*******************************************************************************

 File:    alarm.cpp
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

// $Id: alarm.cpp,v 1.1 2009/05/15 19:42:36 kes Exp $

#include "alarm.h"

Alarm::Alarm() {
   timer.it_value.tv_sec = 0;
   timer.it_value.tv_usec = 0;
   timer.it_interval.tv_sec = 0;
   timer.it_interval.tv_usec = 0;
}

void Alarm::set( void timerHandler(int), int useconds )
{
   signal ( SIGALRM, timerHandler );

   timer.it_value.tv_sec = 0;
   timer.it_value.tv_usec = useconds;
   timer.it_interval.tv_sec = 0;
   timer.it_interval.tv_usec = 0;

   setitimer( ITIMER_REAL, &timer, NULL ); 
}
void Alarm::set( void timerHandler(int), int useconds, int repInterval )
{
   signal ( SIGALRM, timerHandler );

   timer.it_value.tv_sec = 0;
   timer.it_value.tv_usec = useconds;
   timer.it_interval.tv_sec = 0;
   timer.it_interval.tv_usec = repInterval;

   setitimer( ITIMER_REAL, &timer, NULL ); 
}