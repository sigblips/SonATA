/*******************************************************************************

 File:    Condition.h
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
 * Condition.h - declaration of Condition
 * PURPOSE:  Condition is a template base class which provides some simple
 * Condition variables.
 *****************************************************************/

#ifndef CONDITION_H
#define CONDITION_H

#include <ace/Synch.h>


template<class TypeClass>
class BaseCondition
{
public:
  typedef TypeClass value_t;
  // Initialize the condition variable
  BaseCondition (value_t value):
    condition_(mutex_),
    value_(value)
  {
  }
  ~BaseCondition (void)
  {
  }
  // Set/Reset the condition variable's value
  BaseCondition &operator= (value_t value)
  {
    guard_t    guard(mutex_);

    value_ = value;

    condition_.broadcast();

    return *this;
  }
  /* These operators perform the actual waiting.

  This is the "typical" use for condition mutexes.  Each of the
  operators below behaves this way for their respective
  comparisons.

  To use one of these in code, you would simply do:

  Condition mycondition;
  ...
  // Wait until the condition variable has the value 42
  mycondition != true
  ...  */

  // As long as the condition variable is NOT EQUAL TO <value>, we wait
  // return -1 upon failure
  int waitWhileNe (value_t val)
  {
    guard_t    guard(mutex_);

    while( value() != val )
      condition_.wait();

    return 0;
  }

  // As long as the condition variable is EXACTLY EQUAL TO <value>, we
  // wait
  bool waitWhileEq (value_t val)
  {
    guard_t    guard(mutex_);

    while( value() == val )
      condition_.wait();

    return 0;
  }

  // Return the value of the condition variable
  operator value_t (void)
  {
    // Place a guard around the variable so that it won't change as
    // we're copying it back to the client.
    guard_t    guard(mutex_);
    return value();
  }

  // This particular accessor will make things much easier if we
  // decide that 'int' isn't the correct datatype for value_.  Note
  // that we keep this private and force clients of the class to use
  // the cast operator to get a copy of the value.
  const value_t &value (void) const
  {
    return this->value_;
  }

protected:
  typedef ACE_Thread_Mutex mutex_t;
  typedef ACE_Condition_Thread_Mutex condition_t;
  typedef ACE_Guard<mutex_t> guard_t;
  // The mutex that keeps the data safe
  mutable mutex_t mutex_;

  // The condition mutex that makes waiting on the condition easier.
  condition_t condition_;

  // The actual variable that embodies the condition we're waiting
  // for.
  value_t value_;
 private:
  // not implemented
  bool operator!= (value_t val);
  bool operator== (value_t val);

};

template<class TypeClass>
class NumericCondition : public BaseCondition<TypeClass>
{
 public:

  typedef typename NumericCondition<TypeClass>::value_t Numeric_value_t;
  typedef typename NumericCondition<TypeClass>::guard_t Numeric_guard_t;
  
  NumericCondition<TypeClass> (Numeric_value_t value):
    BaseCondition<TypeClass> (value)
    {
    }
  
  NumericCondition & operator++ (void)
    {
      Numeric_guard_t    guard(this->mutex_);

      ++(this->value_);

      this->condition_.broadcast();

      return *this;
    }

  NumericCondition & operator-- (void)
    {
      Numeric_guard_t    guard(this->mutex_);

      --(this->value_);

      this->condition_.broadcast();

      return *this;
    }

  NumericCondition & operator+= (int _inc)
    {
      Numeric_guard_t    guard(this->mutex_);

      this->value_ += _inc;

      this->condition_.broadcast();

      return *this;
    }

  NumericCondition & operator-= (int _inc)
    {
      Numeric_guard_t    guard(this->mutex_);

      this->value_ -= _inc;

      this->condition_.broadcast();

      return *this;
    }

  NumericCondition & operator*= (int _inc)
    {
      Numeric_guard_t    guard(this->mutex_);

      this->value_ *= _inc;

      this->condition_.broadcast();

      return *this;
    }

  NumericCondition & operator/= (int _inc)
    {
      Numeric_guard_t    guard(this->mutex_);

      this->value_ /= _inc;

      this->condition_.broadcast();

      return *this;
    }

  NumericCondition & operator%= (int _inc)
    {
      Numeric_guard_t    guard(this->mutex_);

      this->value_ %= _inc;

      this->condition_.broadcast();

      return *this;
    }


  // As long as the variable is less than or equal to value, we wait...
  int waitWhileLe ( Numeric_value_t val )
    {
      Numeric_guard_t    guard(this->mutex_);

      while( this->value() <= val )
        this->condition_.wait();

      return 0;
    }

  // As long as the variable is less than value, we wait...
  int waitWhileLt ( Numeric_value_t val )
    {
      Numeric_guard_t    guard(this->mutex_);

      while( this->value() < val )
        this->condition_.wait();

      return 0;
    }

  // As long as the variable is greater than or equal to value, we wait...
  int waitWhileGe ( Numeric_value_t val )
    {
      Numeric_guard_t    guard(this->mutex_);

      while( this->value() >= val )
        this->condition_.wait();

      return 0;
    }
  // As long as the variable is greater than value, we wait...
  int waitWhileGt ( Numeric_value_t val )
    {
      Numeric_guard_t    guard(this->mutex_);

      while( this->value() > val )
        this->condition_.wait();

      return 0;
    }
 private:
  // not implemented
  void operator<=(Numeric_value_t value);
  void operator>=(Numeric_value_t value);
  
};


 
#endif /* CONDITION_H */