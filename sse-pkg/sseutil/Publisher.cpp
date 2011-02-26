/*******************************************************************************

 File:    Publisher.cpp
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



#include "Publisher.h" 
#include "Subscriber.h"
#include "Assert.h"
#include <algorithm>

using std::list;

Publisher::Publisher()
{
}

Publisher::~Publisher()
{
}

void Publisher::attach(Subscriber *subscriber)
{
    subscribers_.push_back(subscriber);
}

void Publisher::detach(Subscriber *subscriber)
{
    // make sure the subscriber is in the list
    Assert(find (subscribers_.begin(), subscribers_.end(), subscriber)
	   != subscribers_.end());

    subscribers_.remove(subscriber);
}

void Publisher::notify()
{
    list<Subscriber *>::iterator it;
    for (it=subscribers_.begin(); it != subscribers_.end(); ++it)
    {
	(*it)->update(this);
    }
}
