/*******************************************************************************

 File:    Timeout.h
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


#ifndef Timeout_H
#define Timeout_H

#include "ace/Reactor.h"
#include "Assert.h"
#include "Verbose.h"
#include <string>

using std::string;
using std::endl;
using std::cerr;

/*
  Create & start an ACE timer that calls the specified method on the
  given object after the waiting period, unless the timer is
  cancelled first.
*/

class TimeoutCommand
{
 public:
   virtual void execute() = 0;
   virtual ~TimeoutCommand() = 0;
};

inline TimeoutCommand::~TimeoutCommand() {}

/*
  The timer handler is created on the heap, and deletes itself 
  when its handle is closed.
*/

class TimerHandler: public ACE_Event_Handler
{
 public: 

   TimerHandler(const string &timerName, 
		TimeoutCommand *command,
		bool repeat,
		int verboseLevel
      )
      :
      timerName_(timerName),
      command_(command),
      repeat_(repeat),
      verboseLevel_(verboseLevel)
      {}
   
   //Method which is called back by the Reactor when timeout occurs. 
   virtual int handle_timeout (const ACE_Time_Value &tv, 
			       const void *arg)
   {
      VERBOSE2(verboseLevel_,  
	       timerName_ << " timeout handler called " << endl;);
      
      command_->execute();

      if (repeat_)
      {
	 return 0;  // stay registered with the reactor
      }

      return -1;  // unregister with reactor
   }
   
   virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
   {
      VERBOSE2(verboseLevel_,  
	       timerName_ << " TimerHandler::handle_close called " << endl;);

      delete this;
      
      return 0;
   }

 protected:
   
   virtual ~TimerHandler() {}  // force allocation on the heap
   
 private:
   
   // Disable copy construction & assignment.
   // Don't define these.
   TimerHandler(const TimerHandler& rhs);
   TimerHandler& operator=(const TimerHandler& rhs);
   
   string timerName_;
   TimeoutCommand * command_;
   bool repeat_;
   int verboseLevel_;
};


template <class Object>
class Timeout: public TimeoutCommand
{
 public:

   Timeout(const string &timerName)
      :
      timerName_(timerName),
      object_(0),
      methodToCallOnTimeout_(0),
      timerId_(IdNotInUse),
      state_(STATE_INIT),
      expectToBeDeletedAfterTimeoutMethodIsCalled_(false),
      repeatIntervalSecs_(0)
      {}

   virtual ~Timeout()
      {}

   virtual void startTimer(
      int waitDurationSecs,
      Object *object,
      void (Object::*methodToCallOnTimeout)(),
      int verboseLevel);

   virtual void cancelTimer();

   virtual string getName() const;

   virtual bool isTimerActive() const;

   virtual void setExpectToBeDeletedAfterTimeoutMethodIsCalled(bool option);

   virtual void setRepeatIntervalSecs(int repeatSecs);

 protected:
   
   friend class TimerHandler;

   virtual void execute();

 private:

   enum State { STATE_INIT, STATE_PENDING, STATE_EXECUTING, 
		STATE_CANCELLED, STATE_DONE };
   
   virtual void setState(State state);

   virtual void setTimerId(long timerId);
   virtual long getTimerId() const;

   static const long IdNotInUse = -1;

   // Disable copy construction & assignment.
   // Don't define these.
   Timeout(const Timeout& rhs);
   Timeout& operator=(const Timeout& rhs);

   string timerName_;
   Object *object_;
   void (Object::*methodToCallOnTimeout_)();
   long timerId_;
   int verboseLevel_;
   State state_;
   bool expectToBeDeletedAfterTimeoutMethodIsCalled_;
   int repeatIntervalSecs_;

   mutable ACE_Recursive_Thread_Mutex timerIdMutex_;
   mutable ACE_Recursive_Thread_Mutex nameMutex_;
   mutable ACE_Recursive_Thread_Mutex stateMutex_;

};


template <class Object>
void Timeout<Object>::startTimer(
   int waitDurationSecs,
   Object *object,
   void (Object::*methodToCallOnTimeout)(),
   int verboseLevel)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(stateMutex_);

   // Don't try to start if a cancel request came in first
   if (state_ == STATE_CANCELLED)
   {
      return;
   }

   VERBOSE2(verboseLevel, 
	    "starting " << waitDurationSecs << " second timer for "
	    << getName() << endl;);

   object_ = object;
   methodToCallOnTimeout_ = methodToCallOnTimeout;
   
   bool repeat = (repeatIntervalSecs_ > 0);

   TimerHandler *timerHandler = 
      new TimerHandler(getName(), this, repeat, verboseLevel); 
   
   setTimerId(ACE_Reactor::instance()->schedule_timer(
      timerHandler, 
      0, // timer arg, unused
      ACE_Time_Value (waitDurationSecs),
      ACE_Time_Value (repeatIntervalSecs_)
      ));
   
   verboseLevel_ = verboseLevel;
   
   VERBOSE2(verboseLevel,  
	    "timer id is " << getTimerId()
	    << " for " << getName() << endl;);

   state_ = STATE_PENDING;
   
}

/*
  Request that the timer be cancelled.  This will only happen if the
  timer is currently pending. It's ok for this method to be called
  multiple times -- the ACE reactor cancel_timer will only be called
  once at most.

  Note that the ACE Reactor cancel_timer call
  is NOT protected by the stateMutex_ in order to prevent
  a possible deadlock if this method is called indirectly
  by the Reactor.
*/
template <class Object>
void Timeout<Object>::cancelTimer()
{
   long savedTimerId(getTimerId());
   bool okToCancelTimer(false);
   {
      ACE_Guard<ACE_Recursive_Thread_Mutex> guard(stateMutex_);

      if (state_ == STATE_PENDING)
      {
	 VERBOSE2(verboseLevel_, 
		  "cancelling " << getName()
		  << " timer for id " << savedTimerId << endl;);
	 
	 // reset the id
	 setTimerId(IdNotInUse);

	 okToCancelTimer = true;
      }
      state_ = STATE_CANCELLED;
   }
    
   if (okToCancelTimer)
   {
      /*
	Cancel the timer, requesting that the handler's 
	handle_close method be called for cleanup.
      */
      const void *cancelArg = 0;  // no arg
      int dontCallHandleClose = 0;   	 
      
      if (ACE_Reactor::instance()->cancel_timer(
	 savedTimerId, &cancelArg, dontCallHandleClose))
      {
	 //cout << "cancelling timer" << endl; 
      }
      else 
      {
	 VERBOSE2(verboseLevel_, 
		  "error cancelling " << getName()
		  << " timer for id " << savedTimerId << endl;);
      }
   }

}

template <class Object>
string Timeout<Object>::getName() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(nameMutex_);

   return timerName_;
}

template <class Object>
void Timeout<Object>::execute()
{
   /* Don't use the stateMutex_ guard here, in case
      the methodToCallOnTimeout_ takes some time
      to complete. That way other classes/threads can
      still check the current state.
    */

   Assert(object_);
   Assert(methodToCallOnTimeout_);

   setState(STATE_EXECUTING);

   if (expectToBeDeletedAfterTimeoutMethodIsCalled_)
   {
      (object_->*methodToCallOnTimeout_)();

      // Don't do anything after this point
   }
   else
   {
      (object_->*methodToCallOnTimeout_)(); 

      if (repeatIntervalSecs_ > 0)
      {
         // reset for repeat
         setState(STATE_PENDING);
      }
      else
      {
         setTimerId(IdNotInUse);
         setState(STATE_DONE);
      }
   }
}



/*
  Returns true if a timout is pending, or
  has triggered and is currently excuting.
 */

template <class Object>
bool Timeout<Object>::isTimerActive() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(stateMutex_);

   return (state_ == STATE_PENDING) ||
      (state_ == STATE_EXECUTING);
}

template <class Object>
void Timeout<Object>::setTimerId(long timerId)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(timerIdMutex_);

   timerId_ = timerId;
}

template <class Object>
long Timeout<Object>::getTimerId() const
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(timerIdMutex_);

   return timerId_;
}

template <class Object>
void Timeout<Object>::setState(State state) 
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(stateMutex_);

   state_ = state;
}

template <class Object>
void Timeout<Object>::setExpectToBeDeletedAfterTimeoutMethodIsCalled(bool option)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(stateMutex_);

   expectToBeDeletedAfterTimeoutMethodIsCalled_ = option;
}

template <class Object>
void Timeout<Object>::setRepeatIntervalSecs(int repeatSecs)
{
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(stateMutex_);

   repeatIntervalSecs_ = repeatSecs;
}


#endif // Timeout_H