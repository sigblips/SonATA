/*******************************************************************************

 File:    Msg.h
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
// Message base clas
//
// Lists of these messages are created by the DxMsgList class
//
// $Header: /home/cvs/nss/sonata-pkg/sonataLib/include/Msg.h,v 1.5 2009/02/12 18:06:24 kes Exp $
//
#ifndef _MsgH
#define _MsgH

#include <deque>
#include <string>
#include <sseInterface.h>
#include <sseDxInterface.h>
#include "Sonata.h"
#include "Lock.h"
#include "Partition.h"
#include "Types.h"

using std::string;

namespace sonata_lib {

class Msg {
public:
	Msg(DxMessageCode code_ = MESSAGE_CODE_UNINIT);
	virtual ~Msg();

	void forward(Msg *msg_, Unit unit_ = UnitNone);

	void setUnit(Unit unit_) { unit = unit_; }
	void setCode(uint32_t code_) { hdr.code = code_; }
	void setMsgNumber(uint32_t msgNum_) { hdr.messageNumber = msgNum_; }
	void setActivityId(int32_t activity_) { hdr.activityId = activity_; }
	void setTime(NssDate& timestamp_) { hdr.timestamp = timestamp_; }
	void setSender(string sender_);
	void setReceiver(string receiver_);

	void setHeader(SseInterfaceHeader& hdr_) { hdr = hdr_; }
	void setDataType(MsgDataType dataType_) { dataType = dataType_; }
	void setData(void *data_, int32_t len_, MemBlk *blk_,
			MsgDataType dataType_ = FIXED_BLOCK);
	void resetData() { hdr.dataLength = 0; data = 0; blk = 0; }

	void setAllocated(bool flag = true) { allocFlag = flag; }
	bool allocated() { return (allocFlag); }

	Unit getUnit() { return (unit); }
	uint32_t getCode() { return (hdr.code); }
	int32_t getMsgNumber() { return (hdr.messageNumber); }
	int32_t getActivityId();
	SseInterfaceHeader& getHeader() { return (hdr); }
	int32_t getDataLength() { return (hdr.dataLength); }
	MsgDataType getDataType() { return (dataType); }
	void *getData() { return (data); }

	virtual void marshall();
	virtual void demarshall();

	virtual void marshallHeader();
	virtual void demarshallHeader();

	virtual void marshallData();
	virtual void demarshallData();

	virtual void freeData();

protected:
	bool allocFlag;					// allocated indicator
	Unit unit;						// sending unit
	SseInterfaceHeader hdr;			// message header
	MsgDataType dataType;			// memory block allocation type
	MemBlk *blk;					// memory block pointer
	void *data;						// ptr to actual data

private:
	// forbidden
	Msg(const Msg&);
	Msg& operator=(const Msg&);
};

/**
 * Message list (singleton)
 *
 * Description:
 * 	This is the master list of available messages.
*/

class MsgList {
public:
	static MsgList *getInstance(string name_ = "MsgList",
			int messages_ = DEFAULT_MESSAGES);

	~MsgList();

	Msg *alloc(DxMessageCode code_ = MESSAGE_CODE_UNINIT,
			int32_t activityId_ = -1, void *data_ = 0, int32_t len_ = 0,
			MemBlk *blk_ = 0, MsgDataType dataType_ = FIXED_BLOCK);
	bool free(Msg *msg_);
	int getFreeMessages() { return (msgList.size()); }
	int32_t getAllocs() { return (allocs); }
	int32_t getFrees() { return (frees); }

protected:
	static MsgList *instance;

	int allocs, frees;
	string mlName;
	int messages;
	std::deque<Msg *> msgList;
	Msg *array;

	Lock llock;

	void lock() { llock.lock(); }
	void unlock() { llock.unlock(); }

private:
	MsgList(string name_, int messages_);

	// forbidden
	MsgList(const MsgList&);
	MsgList& operator=(const MsgList&);
};

}

#endif