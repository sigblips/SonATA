/*******************************************************************************

 File:    Angle.h
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


#ifndef ANGLE_H
#define ANGLE_H

#include <math.h>
#include <iosfwd>

using std::ostream;

class BasicRadian
{
 public:
   explicit BasicRadian(double radian = 0);
   double getRadian() const;
   void setRadian(double radian);
   operator double() const;	// returns radians
   friend bool operator== (const BasicRadian &a,
			   const BasicRadian &b);
   const BasicRadian & operator+=(const BasicRadian &addend);
   const BasicRadian & operator-=(const BasicRadian &addend);
   void setPositive();
   static const double UnitToRadian = 1.0;

 private:
   double radian_;
};


class BasicDegree
{
 public:
   explicit BasicDegree(double degree = 0);
   BasicDegree(BasicRadian radian);
   operator BasicRadian ();
   double getDegree() const;
   double getSeconds() const;
   friend bool operator== (const BasicDegree &a,
			   const BasicDegree &b);

   static const double UnitToRadian = M_PI / 180.0;;

 private:
   double degree_;
};


class BasicHour
{
 public:
   explicit BasicHour(double hour = 0);
   BasicHour(BasicRadian radian);
   operator BasicRadian ();
   double getHour() const;
   double getMinutes() const;
   double getSeconds() const;
   friend bool operator== (const BasicHour &a,
			   const BasicHour &b);
   static const double UnitToRadian = M_PI / 12.0;
 private:
   double hour_;
};



typedef BasicRadian Radian;
typedef BasicDegree Degree;
typedef BasicHour Hour;

// prints angle in Degrees, Minutes

class BasicDm
{
 public:
   BasicDm(BasicDegree degree);
   friend ostream &operator<<(ostream& os, BasicDm dm);
 private:
   BasicDegree degree_;
};



class BasicDms
{
 public:
   BasicDms(BasicDegree degree);
   friend ostream &operator<<(ostream& os, BasicDms dms);
 private:
   BasicDegree degree_;
};

class BasicHms
{
 public:
   BasicHms(BasicHour hour);
   friend ostream &operator<<(ostream& os, BasicHms hms);
 private:
   BasicHour hour_;
};


class BasicHm
{
 public:
   BasicHm(BasicHour hour);
   friend ostream &operator<<(ostream& os, BasicHm hm);
 private:
   BasicHour hour_;
};


typedef BasicDms Dms;
typedef BasicDm Dm;
typedef BasicHms Hms;
typedef BasicHm Hm;
     
class RaDec
{
 public:
   RaDec(Radian raInput = Radian(0), Radian decInput = Radian(0));
   friend ostream &operator<<(ostream& os, const RaDec &raDec);
   
   Radian ra;
   Radian dec;
};


#endif /* ANGLE_H */