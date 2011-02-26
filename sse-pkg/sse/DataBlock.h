/*******************************************************************************

 File:    DataBlock.h
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
 * DataBlock.h - declaration of functions defined in DataBlock.h
 * PURPOSE:  
 *****************************************************************/

#ifndef DATABLOCK_H
#define DATABLOCK_H

#include "mld.h"

template<class UnitOfWork> 
class Data_Block : public ACE_Data_Block
{
public:
  typedef ACE_Data_Block inherited;


/*
   Construct a Data_Block to contain a unit of work.  Note the careful
   construction of the baseclass to set the block type and the locking
   strategy.
 */

  Data_Block(UnitOfWork * _data)
        : ACE_Data_Block(0, ACE_Message_Block::MB_DATA, 0, 0,
			  new Lock(), 0, 0),
	  data_(_data)
  {
	    // ACE_DEBUG((LM_DEBUG, "(%P|%t) 0x%x Data_Block ctor for 0x%x\n", (void *) this, (void *) data_));
  }

/*
   The Lock object created in the constructor is stored in the baseclass and
   available through the locking_strategy() method.  We can cast it's value to
   our Lock object and invoke the destroy() to indicate that we want it to go
   away when the lock is released.
 */

  virtual ~Data_Block(void)
  {
      // ACE_DEBUG((LM_DEBUG, "(%P|%t) 0x%x Data_Block dtor for 0x%x\n", (void *) this, (void *) data_));
    ((Lock *) locking_strategy())->destroy();
    delete data_;
  }

  // Returns the work pointer
  UnitOfWork *data(void)
  {
    return this->data_;
  }

protected:
  UnitOfWork * data_;
  MLD;                        // Our memory leak detector

  // The ACE_Data_Block allows us to choose a locking strategy
  // for making the reference counting thread-safe.  The
  // ACE_Lock_Adaptor<> template adapts the interface of a
  // number of lock objects so that the ACE_Message_Block will
  // have an interface it can use.
  class Lock : public ACE_Lock_Adapter < ACE_Mutex >
  {
  public:
    typedef ACE_Lock_Adapter < ACE_Mutex > inherited;

    Lock(void)
    { }
    // destroy() will be called to explicitly delete the
    // lock when we no longer need it.  The method will then
    // cleanup to prevent any memory leaks.

    /*
      Delete ourselves to prevent any memory leak
    */
     int destroy(void)
    {
	delete this;
	return(0);
    }

  protected:
    // destructor should not be called directly
    // It should only be called through destroy
    virtual ~Lock(void)
    { }

    MLD;
  };
};

/*
   Construct a Data_Block to contain a unit of work.  Note the careful
   construction of the baseclass to set the block type and the locking
   strategy.
 */


#endif /* DATABLOCK_H */