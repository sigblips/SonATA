/*******************************************************************************

 File:    RangeParameter.h
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


#ifndef RangeParameter_H
#define RangeParameter_H

#include "ParameterImpl.h"
#include <sstream>
#include <iomanip>

using std::string;
using std::endl;

template<class T> class RangeParameter : public ParameterImpl<T>
{

 public:

   typedef typename RangeParameter<T>::ParamCategory RangeParamCategory;

   RangeParameter(string parameterName, string parameterUnits, 
		  string helpMessage = "", 
		  T defaultValue = 0, T minimum = 0, T maximum = 0) :
      ParameterImpl<T>(parameterName, parameterUnits, helpMessage, 
		       defaultValue),
      min_(minimum), max_(maximum) {}

   // default copy constructor and assignment operator are safe

   const T& getValue(RangeParamCategory category = Parameter::CURRENT) const;
   bool setValue(T value, string & errorText, RangeParamCategory category = Parameter::CURRENT);

   const T& getMin() const {return min_;}
   bool setMin(T value)
   {
      string errorText;
      return(setValue(value, errorText, Parameter::MIN));
   }

   const T& getMax() const {return max_;}
   bool setMax(T value) 
   {
      string errorText;
      return(setValue(value, errorText, Parameter::MAX));
   }

   bool isValid() const;
   bool isValid(T value) const;

   void  writeValues(const string &command, ostream &os) const;

   virtual void printValidSelections(ostream &strm, int width) const;

   void print(ostream& os = cout) const
   {
      os << *this << endl;
   }
  
 private:
   T min_;
   T max_;

};


template<class T>
void RangeParameter<T>::printValidSelections(ostream &strm, int width) const
{
   strm << std::resetiosflags(std::ios::adjustfield)
	<< std::setiosflags(std::ios::right) 
	<< std::setw(width) << getMin() << " "
	<< std::setw(width) << getMax() << " "
	<< std::resetiosflags(std::ios::adjustfield);
}

template<class T> const T& RangeParameter<T>::getValue(
   RangeParamCategory category) const
{
   switch (category)
   {
   case Parameter::DEFAULT:
      return(this->getDefault());
      break;

   case Parameter::MIN:
      return(getMin());
      break;

   case Parameter::MAX:
      return(getMax());
      break;

   case Parameter::CURRENT:
      return(this->getCurrent());
      break;

   default:
      return(this->getCurrent());
      break;
   }

}


template<class T> bool RangeParameter<T>::setValue(T value,
						   string & errorText,
						   RangeParamCategory category)
{
   switch (category)
   {
   case Parameter::DEFAULT:
   case Parameter::CURRENT:
      if (isValid(value))
      {
	 switch (category)
	 {
	 case Parameter::DEFAULT:
	    this->setInternalDefault(value);
	    break;

	 case Parameter::CURRENT:
	    this->setInternalCurrent(value);
	    break;

	 default:
	    break;
	 }
	 return(true);

      }
      else
      {
	 std::stringstream strm;
	 this->printHeaderForInvalidParam(strm);
	 strm << "  value: " << value 
	      << " is not between: " << getMin() << " and " << getMax() << endl;

	 errorText = strm.str();
	 return(false);
      }
      break;

   case Parameter::MIN:
      if (value < getMax())
      {
	 min_ = value;
	 return(true);
      }
      else
      {
	 std::stringstream strm;
	 this->printHeaderForInvalidParam(strm);
	 strm << "  attempt to set min: " << value << endl;
	 strm << "  greater than max: " << getMax() << endl;
	 errorText = strm.str();
	 return(false);
      }
      break;

   case Parameter::MAX:
      if (value > getMin())
      {
	 max_ = value;
	 return(true);
      }
      else
      {
	 std::stringstream strm;
	 this->printHeaderForInvalidParam(strm);
	 strm << "  attempt to set max: " << value << endl;
	 strm << "  less than min: " << getMin() << endl;
	 errorText = strm.str();
	 return(false);
      }
      break;
   }
   return(true);
}


template<class T> bool RangeParameter<T>::isValid() const
{
   return isValid(this->getCurrent());
}

template<class T> bool RangeParameter<T>::isValid(T value) const
{
   if (value < getMin() || value > getMax())
   {
      return false;
   }

   return true;
}

template<class T>
void  RangeParameter<T>::writeValues(const string &command, ostream &os) const
{
   os << command << " set " << this->getName() << " " << this->getMin() 
      << " min " << endl;
   os << command << " set " << this->getName() << " " << this->getMax() 
      << " max " << endl;
   os << command << " set " << this->getName() << " " << this->getDefault() 
      << " default " << endl;
   os << command << " set " << this->getName() << " " << this->getCurrent() 
      << " current " << endl;
}

#endif // RangeParameter_H