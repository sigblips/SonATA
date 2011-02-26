/*******************************************************************************

 File:    SseTimer.cpp
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


#include "SseTimer.h" 

SseTimer::SseTimer()
{
        initial_time.tv_sec = 0;
	initial_time.tv_usec = 0;
        last_time.tv_sec = 0;
	last_time.tv_usec = 0;
        this_time.tv_sec = 0;
	this_time.tv_usec = 0;
	time_zone.tz_minuteswest = 0;
	time_zone.tz_dsttime = 0;
}
void SseTimer::start()
{
        gettimeofday( &initial_time, &time_zone );
	last_time = initial_time;
}

double SseTimer::elapsed_time()
{
        double seconds;

        gettimeofday( &this_time, &time_zone );
	seconds = (this_time.tv_sec - last_time.tv_sec) +
			(this_time.tv_usec - last_time.tv_usec)/1000000.;
	last_time = this_time;
	return(seconds);
}

double SseTimer::total_elapsed_time()
{
	double seconds;

        gettimeofday( &this_time, &time_zone );
	seconds = (this_time.tv_sec - initial_time.tv_sec) +
			(this_time.tv_usec - initial_time.tv_usec)/1000000.;
	last_time = this_time;
	return(seconds);
}