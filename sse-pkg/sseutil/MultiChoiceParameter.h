/*******************************************************************************

 File:    MultiChoiceParameter.h
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


#ifndef MultiChoiceParameter_H
#define MultiChoiceParameter_H

// Comma separarated list of choices

#include "ChoiceParameter.h"
#include "SseUtil.h"
//#include <list>
//#include <algorithm>
//#include <sstream>

template<class T> class MultiChoiceParameter : public ChoiceParameter<T>
{
 public:

   MultiChoiceParameter(string parameterName, string parameterUnits, 
                        string helpMessage, T defaultValue)
      :
      ChoiceParameter<T>(parameterName, parameterUnits, 
                         helpMessage, defaultValue)
      {
      }

   virtual bool isValid() const;
   virtual bool isValid(T value) const;
  
   // default copy constructor and assignment operator are safe
};

template<class T>
bool MultiChoiceParameter<T>::isValid() const
{
   return ChoiceParameter<T>::isValid();
}

template<class T>
bool MultiChoiceParameter<T>::isValid(T value) const
{
   // split value by commas, make sure all elements are on the choice list
   // TBD only works with strings
   vector<string> words(SseUtil::tokenize(value, ","));

   for (unsigned int i=0; i<words.size(); ++i)
   {
      if (! ChoiceParameter<T>::isValid(words[i]))
      {
         return false;
      }
   }

   return true;
}



#endif 