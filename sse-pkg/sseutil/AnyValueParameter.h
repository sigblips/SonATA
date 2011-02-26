/*******************************************************************************

 File:    AnyValueParameter.h
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


// parameter may be set to any value which the template allows

#ifndef AnyValueParameter_H
#define AnyValueParameter_H

#include "ParameterImpl.h"
#include <algorithm>
#include <sstream>

using std::string;

template<class T> class AnyValueParameter : public ParameterImpl<T>
{
 public:

   typedef typename AnyValueParameter<T>::ParamCategory AnyValueParamCategory;

   AnyValueParameter(string parameterName, string parameterUnits, 
		     string helpMessage, T defaultValue):
      ParameterImpl<T>(parameterName, parameterUnits, helpMessage, 
		       defaultValue)
   {
   }

   virtual const T& getValue(AnyValueParamCategory category = Parameter::CURRENT) const;
   virtual bool setValue(T value, string &errorText, 
			 AnyValueParamCategory category = Parameter::CURRENT);

   virtual bool isValid() const;
   virtual bool isValid(T value) const;

   // default copy constructor and assignment operator are safe

   virtual void print(ostream& os = cout) const
   {
      os << *this << std::endl;
   }
};

template<class T> const T& AnyValueParameter<T>::getValue(
   AnyValueParamCategory category) const
{
   switch (category)
   {
   case Parameter::DEFAULT:
      return(this->getDefault());
      break;
   case Parameter::MIN:
      break;
   case Parameter::MAX:
      break;
   case Parameter::CURRENT:
      return(this->getCurrent());
      break;
   }
   return(this->getCurrent());
}

template<class T> bool AnyValueParameter<T>::setValue(
   T value, string & errorText, AnyValueParamCategory category)
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
	 errorText = strm.str();
	 return(false);
      }
      break;

   case Parameter::MIN:
      errorText = "Not allowed to set min on this parameter\n";
      return(false);
      break;

   case Parameter::MAX:
      errorText = "Not allowed to set max on this parameter\n";
      return(false);
      break;
   }
   return(true);
}

template<class T>
bool AnyValueParameter<T>::isValid() const 
{
   // Any value is true
   return true;
}

template<class T>
bool AnyValueParameter<T>::isValid(T value) const
{
   // Any value is valid
   return true;
}

#endif // AnyValueParameter_H
