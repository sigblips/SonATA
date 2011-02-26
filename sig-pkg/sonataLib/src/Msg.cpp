/*******************************************************************************

 File:    Msg.cpp
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

//
// Message base class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/src/Msg.cpp,v 1.4 2009/05/24 23:29:05 kes Exp $
//
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <sseInterface.h>
#include "Sonata.h"
#include "Err.h"
//#include "MemList.h"
#include "Msg.h"
#include "Util.h"

namespace sonata_lib {

Msg::Msg(DxMessageCode code_): allocFlag(false), unit(UnitNone),
		dataType(FIXED_BLOCK), blk(0), data(0)
{
	timeval time;

	hdr.code = code_;
	gettimeofday(&time, NULL);
	hdr.timestamp.tv_sec = time.tv_sec;
	hdr.timestamp.tv_usec = time.tv_usec;
	hdr.dataLength = 0;
	hdr.messageNumber = 0;
}

Msg::~Msg()
{
	freeData();
}

//
// forward: forward a message
//
// Description:
//		Copies the header and data areas from this message
//		to the new message, then resets the data and length members
//		of this message.
// Notes:
//		This is used to avoid copying a data area when a message
//		must be duplicated to send it on.
//
void
Msg::forward(Msg *msg_, Unit unit_)
{
	msg_->setHeader(hdr);
	if (unit_ == UnitNone)
		unit_ = unit;
	msg_->setUnit(unit_);
	msg_->setData(data, hdr.dataLength, blk, dataType);
	resetData();
}

//
// setData: set the data
//
void
Msg::setData(void *data_, int32_t len_, MemBlk *blk_,
		MsgDataType dataType_)
{
	data = data_;
	hdr.dataLength = len_;
	blk = blk_;
	dataType = dataType_;
}

int32_t
Msg::getActivityId()
{
	return (hdr.activityId);
}


void
Msg::marshall()
{
	marshallData();
	marshallHeader();
}

void
Msg::demarshall()
{
	demarshallHeader();
	demarshallData();
}

void
Msg::marshallHeader()
{
	hdr.marshall();
}

void
Msg::demarshallHeader()
{
	hdr.demarshall();
}

void
Msg::marshallData()
{
}

void
Msg::demarshallData()
{
}

//
// freeData: release the data associated with the message
//
// Notes:
//		There are three different types of data associated with
//		messages: fixed-length blocks from the PartitionSet,
//		structure memory allocated with new, and array
//		memory allocated with new.  Where possible, the first
//		should be used, since they are much faster.
//		The Static type indicates that ownership is global,
//		and that the object should not be freed.
//
void
Msg::freeData()
{
	if (data && hdr.dataLength) {
		switch (dataType) {
		case FIXED_BLOCK:
			Assert(blk);
			blk->free();
			break;
		case NEW:
			delete static_cast<uint8_t *> (data);
			break;
		case NEW_ARRAY:
			delete [] static_cast<uint8_t *> (data);
		case STATIC:
		case IMMEDIATE:
		case USER:
			break;
		default:
			Fatal(ERR_IMBT);
			break;
		}
	}
	data = 0;
	hdr.dataLength = 0;
}

/**
 * Msg list
*/

MsgList *MsgList::instance = 0;

MsgList *
MsgList::getInstance(string name_, int messages_)
{
	static Lock l;
	l.lock();
	if (!instance)
		instance = new MsgList(name_, messages_);
	l.unlock();
	return (instance);
}

MsgList::MsgList(string name_, int messages_): allocs(0), frees(0),
		mlName(name_), messages(messages_), array(0), llock("ml" + name_)
{
	array = new Msg[messages_];
	if (!array)
		Fatal(ERR_MAF);

	for (int i = 0; i < messages; ++i)
		msgList.push_back(&array[i]);
}

MsgList::~MsgList()
{
	if (array) {
		msgList.clear();
		delete [] array;
	}
}

Msg *
MsgList::alloc(DxMessageCode code_, int32_t activityId_, void *data_,
		int32_t len_, MemBlk *blk_, MsgDataType dataType_)
{
	Msg *msg;
	SseInterfaceHeader hdr;

	lock();
	if (msgList.empty()) {
		Debug(DEBUG_MSGLIST, allocs, "allocs");
		Debug(DEBUG_MSGLIST, frees, "frees");
		unlock();
		Fatal(ERR_NMA);
	}

	msg = msgList.front();
	msgList.pop_front();

	Assert(!msg->allocated());
	msg->setAllocated();

	hdr.code = code_;
	hdr.activityId = activityId_;
	GetNssDate(hdr.timestamp);
	hdr.messageNumber = GetNextMsg();
	msg->setHeader(hdr);
	msg->setData(data_, len_, blk_, dataType_);

	++allocs;
	unlock();
	return (msg);
}

//
// free: add the msg to the end of the free list
//
// Notes:
//		If the free list is empty, both head and tail are assigned
//		the message.
//
bool
MsgList::free(Msg *msg_)
{
	void *data;

	if (msg_->getDataLength() && (data = msg_->getData()))
		msg_->freeData();
	Assert(msg_->allocated());
	msg_->setAllocated(false);

	lock();
	msgList.push_back(msg_);
	++frees;
	unlock();
	return (true);
}



}