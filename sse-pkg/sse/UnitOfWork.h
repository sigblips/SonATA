/*******************************************************************************

 File:    UnitOfWork.h
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

/*****************************************************************
 * UnitOfWork.h - declaration of functions defined in UnitOfWork.h
 * PURPOSE:  
 *****************************************************************/

#ifndef UNITOFWORK_H
#define UNITOFWORK_H


/*
   Our specialized message queue and thread pool will know how to do
   "work" on our Unit_Of_Work baseclass.
 */
template<class CallArgument>
class Unit_Of_Work
{
public:
  Unit_Of_Work(int message);
  virtual ~ Unit_Of_Work(void)
    {}

  // This is where you do application level logic.  It will be
  // called once for each thread pool it passes through.  It
  // would typically implement a state machine and execute a
  // different state on each call.

  // Evaluate the synchronization constraint
  virtual bool can_run() const 
  {
    return true;
  }
  // Execute the method
  // each Unit_Of_Work should have a call method,
  // the arguments vary
  //  virtual int call(*) = 0;

  // returns true if active object should quit
  virtual bool quit() const
  {
    return false;
  }
 
  // This is called by the last Task in the series (see task.h)
  // in case our process() didn't get through all of it's states.
  virtual int fini(void);
  virtual int call(CallArgument* scheduler) = 0;

protected:
  ACE_Atomic_Op < ACE_Mutex, int >state_;
  MLD;
  int message_;
};


template<class CallArgument>
Unit_Of_Work<CallArgument>::Unit_Of_Work(int message)
  : message_(message)
{
  // ACE_DEBUG((LM_DEBUG, "(%P|%t) Unit_Of_Work ctor 0x%x for message %d\n", (void *) this, message_));
}


/*
   ditto
 */
template<class CallArgument>
int Unit_Of_Work<CallArgument>::fini(void)
{
    return -1;
}
 


#endif /* UNITOFWORK_H */