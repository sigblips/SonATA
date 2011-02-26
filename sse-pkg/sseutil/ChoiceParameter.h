/*******************************************************************************

 File:    ChoiceParameter.h
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


#ifndef ChoiceParameter_H
#define ChoiceParameter_H

#include "Parameter.h"
#include "AnyValueParameter.h"
#include <list>
#include <algorithm>
#include <sstream>


template<class T> class ChoiceParameter : public AnyValueParameter<T>
{

 public:

   typedef typename ChoiceParameter<T>::ParamCategory ChoiceParamCategory;

   ChoiceParameter(string parameterName, string parameterUnits, 
		   string helpMessage, T defaultValue)
      :
      AnyValueParameter<T>(parameterName, parameterUnits, 
			   helpMessage, defaultValue)
   {
      // automatically add in the default/current values.
      // other values must be added manually
      
      addChoice(this->getDefault());
   }
  
   // default copy constructor and assignment operator are safe

   virtual string getHelp() const 
   {
      std::stringstream strm;
      strm << " ";
      printChoices(strm);
      return AnyValueParameter<T>::getHelp() + strm.str();
   }
   
   virtual void addChoice(const T value);
   virtual void printChoices(ostream &strm) const;
   virtual int getNumberOfChoices()  const;

   virtual bool isValid() const;
   virtual bool isValid(T value) const;

   virtual void printValidSelections(ostream &strm, int width) const;

   virtual void sort();

   // override base class definition so that the choice values get printed

   virtual void print(ostream& os = cout) const
   {
      os << *this << std::endl;
   }

 private:

   std::list<T> choiceValues_;  
   typedef typename std::list<T>::const_iterator ListConstIterator;

};


template<class T>
void ChoiceParameter<T>::printValidSelections(ostream &strm, int width) const
{
   strm << std::setw(width);

   printChoices(strm);
}


template<class T>
bool ChoiceParameter<T>::isValid() const
{
   return isValid(this->getCurrent());
}

template<class T>
bool ChoiceParameter<T>::isValid(T value) const
{
   // make sure value is on the list
   return (find(choiceValues_.begin(), choiceValues_.end(), value) != 
           choiceValues_.end());
}

template<class T>
void ChoiceParameter<T>::printChoices(ostream &strm) const
{
   // print choices in this format:  (a|b|c)
   strm << "(";

   ListConstIterator it;
   for (it = choiceValues_.begin(); it != choiceValues_.end();)
   {
      strm << *it;
      ++it;

      // print separator if we're not at the end
      if (it != choiceValues_.end())
      {
	 strm << " | ";
      }
   }

   strm << ") ";
}

template<class T> 
void ChoiceParameter<T>::addChoice(const T value)
{
   if (!isValid(value)) {   // avoid adding duplicates
      choiceValues_.push_back(value);
   }
}


template<class T> 
int ChoiceParameter<T>::getNumberOfChoices() const
{
   return choiceValues_.size();
}


template<class T>
void ChoiceParameter<T>::sort()
{
   choiceValues_.sort();
}


#endif // ChoiceParameter_H