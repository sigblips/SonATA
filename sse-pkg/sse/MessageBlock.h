/*******************************************************************************

 File:    MessageBlock.h
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
 * MessageBlock.h - declaration of functions defined in MessageBlock.h
 * PURPOSE:  
 *****************************************************************/

#ifndef MESSAGEBLOCK_H
#define MESSAGEBLOCK_H

#include "DataBlock.h"

/*
   In this Tutorial, we derive from ACE_Data_Block for our special data.  With
   the possiblilty that our Task object may forward the unit of work on to
   another thread pool, we have to make sure that the data object doesn't go
   out of scope unexpectedly.  An ACE_Message_Block will be deleted as soon as
   it's release() method is called but the ACE_Data_Blocks it uses are
   reference counted and only delete when the last reference release()es the
   block.  We use that trait to simplify our object memory management.
 */
/*
   This simple derivative of ACE_Message_Block will construct our Data_Block
   object to contain a unit of work.
 */
template<class UnitOfWork>
class Message_Block : public ACE_Message_Block
{
public:
  typedef ACE_Message_Block inherited;

  Message_Block(UnitOfWork * _data);

protected:
  virtual ~Message_Block(void)
    { }

  MLD;
};


/*
   Store the unit of work in a Data_Block and initialize the baseclass with
   that data.
 */
template<class UnitOfWork>
Message_Block<UnitOfWork>::Message_Block(UnitOfWork * data)
{
    // The Data_Block must be allocated with one of the
    // ACE_NEW_MALLOC functions rather than using 'new',
    // Otherwise you'll get a PURIFY
    // 'Freeing mismatched memory' problem in the base class,
    // since it uses  'delete[]' rather than 'delete' to get rid
    // of the data block.
    // See the ACE Message_Block.cpp code
    // (ACE_Message_Block::init_i method) for an example of
    // how the library allocates the ACE_Data_Block. 

    ACE_Data_Block *db = 0;
    ACE_NEW_MALLOC (
	db,
	ACE_static_cast(
	    Data_Block<UnitOfWork> *,
	    ACE_Allocator::instance()->malloc (sizeof (Data_Block<UnitOfWork>))),
	Data_Block<UnitOfWork>(data));

    // set the new data block in the base class
    data_block(db);

  // ACE_DEBUG((LM_DEBUG, "(%P|%t) 0x%x Message_Block ctor, date:  0x%x\n", (void *) this, (void *) data));
}


#endif /* MESSAGEBLOCK_H */