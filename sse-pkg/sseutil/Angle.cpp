/*******************************************************************************

 File:    Angle.cpp
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


#include "Angle.h"
#include "Assert.h"
#include <sstream>
#include <iostream>
#include <iomanip>

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

using namespace std;

BasicRadian::BasicRadian(double radian):
   radian_(radian)
{
}

double BasicRadian::getRadian() const
{
   return radian_;
}

void BasicRadian::setRadian(double radian)
{
   radian_ = radian;
}

const BasicRadian & BasicRadian::operator+=(const BasicRadian &addend)
{
   radian_ += addend.radian_;
   return *this;
}

const BasicRadian & BasicRadian::operator-=(const BasicRadian &addend)
{
   radian_ -= addend.radian_;
   return *this;
}

void BasicRadian::setPositive()
{
   while (radian_ < 0)
   {	/* convert to positive number */
      
      // Prevent this loop from going on (nearly) forever, if too small a
      // radian_ value is set.
      
      const double MIN_RADIAN_VALUE = -10000;
      AssertMsg(radian_ > MIN_RADIAN_VALUE, 
		"BasicRadian::setPositive() radian value is too small");
      
      radian_ += 2 * M_PI;
   }
   if (radian_ > 2 * M_PI)
   {
      radian_ -= 2 * M_PI;
   }
}


BasicRadian::operator double() const
{
   return getRadian();
}

bool operator==(const BasicRadian &a,
		const BasicRadian &b)
{
   return (a.radian_ == b.radian_);
}


// BasicDegree
// -----------

BasicDegree::BasicDegree(double degree):
   degree_(degree)
{
}

BasicDegree::BasicDegree(BasicRadian radian):
   degree_(radian.getRadian() / BasicDegree::UnitToRadian)
{
}

BasicDegree::operator BasicRadian()
{
   return BasicRadian(degree_ * BasicDegree::UnitToRadian);
}

double BasicDegree::getDegree() const
{
   return degree_;
}

double BasicDegree::getSeconds() const
{
   return degree_ * 60.0 * 60.0;
}

bool operator==(const BasicDegree &a,
		const BasicDegree &b)
{
   return (a.degree_ == b.degree_);
}



// BasicHour
BasicHour::BasicHour(double hour):
   hour_(hour)
{
}

BasicHour::BasicHour(BasicRadian radian):
   hour_(radian.getRadian() / BasicHour::UnitToRadian)
{
}

BasicHour::operator BasicRadian ()
{
   return BasicRadian(hour_ * BasicHour::UnitToRadian);
}

double BasicHour::getHour() const
{
   return hour_;
}
double BasicHour::getMinutes() const
{
   return hour_ * 60.0;
}
double BasicHour::getSeconds() const
{
   return hour_ * 60.0 * 60.0;
}

bool operator==(const BasicHour &a,
		const BasicHour &b)
{
   return (a.hour_ == b.hour_);
}



// BasicDm
// -------

BasicDm::BasicDm(BasicDegree degree):
   degree_(degree)
{
}

// prints angle in Degrees, Minutes, Seconds

ostream &operator<<(ostream& os, BasicDm dm)
{
   double totalMinutesFlt = dm.degree_.getSeconds() / 60.0;
   totalMinutesFlt += copysign(0.5, totalMinutesFlt);
   int totalMinutes = static_cast<int>(totalMinutesFlt);
   while (totalMinutes < 0) {	/* convert to positive number */
      totalMinutes += 360 * 60;
   }
   if (totalMinutes > 180 * 60){
      totalMinutes -= 360 * 60;
   }
  
   const char* const angleSign = (totalMinutes >= 0)?" ":"-";
   totalMinutes = abs(totalMinutes);
   int degrees = totalMinutes / 60;
   int minutes = totalMinutes % 60;
   stringstream dmText;
   dmText
      << angleSign
      ;
   dmText << degrees << ':';
   dmText << setw(2) << setfill('0') << minutes
      ;
   os << setw(7) << dmText.str().c_str();
   return os;
}


//  BasicDms
BasicDms::BasicDms(BasicDegree degree):
   degree_(degree)
{
}

// BasicHm
BasicHm::BasicHm(BasicHour hour):
   hour_(hour)
{
}

BasicHms::BasicHms(BasicHour hour):
   hour_(hour)
{
}



RaDec::RaDec(Radian raInput, Radian decInput):
   ra(raInput),
   dec(decInput)
{
}

ostream& operator<<(ostream& os, const RaDec &raDec)
{
   return os
      << Hms(raDec.ra)
      << Dms(raDec.dec)
      ;
}

ostream &operator<<(ostream& os, BasicDms dms)
{
   double totalSecondsFlt = dms.degree_.getSeconds();
   totalSecondsFlt += copysign(0.5, totalSecondsFlt);
   int totalSeconds = static_cast<int>(totalSecondsFlt);
  
   while (totalSeconds < 0) {	/* convert to positive number */
      totalSeconds += 360 * 60 * 60;
   }
   if (totalSeconds > 180 * 60 * 60){
      totalSeconds -= 360 * 60 * 60;
   }
  
   char angleSign = (totalSeconds >= 0)?' ':'-';
   totalSeconds = abs(totalSeconds);
   int degrees = totalSeconds / 60 / 60;
   int minutes = totalSeconds / 60 % 60;
   int seconds = totalSeconds % 60;
   stringstream dmsText;
   dmsText << angleSign
	   << degrees << ':'
	   << setw(2) << setfill('0') << minutes
	   << ':' << setw(2) << setfill('0') << seconds
      ;
   os << setw(10) << dmsText.str().c_str();
   return os;
}



ostream &operator<<(ostream& os, BasicHms hms)
{
   double totalSecondsFlt = hms.hour_.getSeconds();
   totalSecondsFlt += copysign(0.5, totalSecondsFlt);
   int totalSeconds = static_cast<int>(totalSecondsFlt);
   while (totalSeconds < 0) {	/* convert to positive number */
      totalSeconds += 24 * 60 * 60;
   }
   if (totalSeconds > 24 * 60 * 60){
      totalSeconds -= 24 * 60 * 60;
   }

   char angleSign = (totalSeconds >= 0)?' ':'-';
   totalSeconds = abs(totalSeconds);
   int hours = totalSeconds / 60 / 60;
   int minutes = totalSeconds / 60 % 60;
   int seconds = totalSeconds % 60;
   stringstream hmsText;
   hmsText << angleSign
	   << setw(2) << setfill('0') << hours << ':'
	   << setw(2) << setfill('0') << minutes
	   << ':' << setw(2) << setfill('0') << seconds
      //	  << ends
      ;
   os << setw(6) << hmsText.str().c_str();
   return os;
}


ostream &operator<<(ostream& os, BasicHm hm)
{
   double totalMinutesFlt = hm.hour_.getSeconds()/ 60.0;
   totalMinutesFlt += copysign(0.5, totalMinutesFlt);
   int totalMinutes = static_cast<int>(totalMinutesFlt);
   while (totalMinutes < 0) {	/* convert to positive number */
      totalMinutes += 24 * 60;
   }
   if (totalMinutes > 12 * 60){
      totalMinutes -= 24 * 60;
   }
  
   char angleSign = (totalMinutes >= 0)?' ':'-';
   totalMinutes = abs(totalMinutes);
   int hours = totalMinutes / 60;
   int minutes = totalMinutes % 60;
   stringstream hmText;
   hmText << angleSign
	  << hours << ':'
	  << setw(2) << setfill('0') << minutes
      //	 << ends;
      ;
  
   os << setw(6) << hmText.str().c_str();
   return os;
}