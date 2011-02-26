/*******************************************************************************

 File:    DxArchiverProxy.cpp
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


#include <ace/OS.h>
#include "DxArchiverProxy.h"
#include <iostream>
#include "SseDxMsg.h"
#include "Dx.h"
#include "HostNetByteorder.h"
#include "Assert.h"

using namespace std;

//#define TEST_SEND_BAD_MESSAGE_SIZE 1


DxArchiverProxy::DxArchiverProxy(ACE_SOCK_STREAM &stream)
    : NssProxy(stream),
      dx_(0)
{
    NssProxy::setVerboseLevel(2);   // 2 = maximum verbose output

    // don't end the ace event loop if this proxy disconnects
    endAceEventLoopOnClose(false);
}

DxArchiverProxy::~DxArchiverProxy()
{
}

void DxArchiverProxy::setDx(Dx *dx)
{
    dx_ = dx;
}

string DxArchiverProxy::getName()
{
    return "dx-archiver";
}


void DxArchiverProxy::notifyInputDisconnected()
{
    dx_->notifyDxArchiverDisconnected();
}

void DxArchiverProxy::notifyInputConnected()
{
    dx_->notifyDxArchiverConnected();
}


// outgoing messages to dxArchiver

void DxArchiverProxy::sendDxMessage(const NssMessage &nssMessage, int activityId)
{
    cout << "DxArchiverProxy: sendDxMessage" << endl;

    DxMessageCode code = SEND_DX_MESSAGE;
    int dataLength = sizeof(nssMessage);

    NssMessage marshall = nssMessage;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);

}

void DxArchiverProxy::sendIntrinsics(const DxIntrinsics &intrinsics)
{
    cout << "DxArchiverProxy: sendIntrinsics" << endl;

    DxMessageCode code = SEND_INTRINSICS;
    int dataLength = sizeof(intrinsics);

    // marshall a copy of the intrinsics
    DxIntrinsics marshall = intrinsics;
    marshall.marshall();
    int activityId = NSS_NO_ACTIVITY_ID;
    sendMessage(code, activityId, dataLength, &marshall);

}


#if 0

void DxArchiverProxy::sendDxStatus(const DxStatus &status)
{
    cout << "DxArchiverProxy: sendStatus" << endl;


    DxMessageCode code = SEND_DX_STATUS;
    int dataLength = sizeof(status);

    // marshall a copy of the status
    DxStatus marshall = status;
    marshall.marshall();

    int activityId = NSS_NO_ACTIVITY_ID;
    sendMessage(code, activityId, dataLength, &marshall);

}

#endif



void DxArchiverProxy::archiveSignal(const ArchiveDataHeader &hdr,
			     int activityId)
{
    cout << "DxArchiverProxy: archiveSignal" << endl;

    DxMessageCode messageCode = ARCHIVE_SIGNAL;
    int dataLength = sizeof(hdr);

    // marshall a copy of the data
    ArchiveDataHeader marshall = hdr;
    marshall.marshall(); 
    sendMessage(messageCode, activityId, dataLength, &marshall);

}

void DxArchiverProxy::beginSendingArchiveComplexAmplitudes(
    const Count &nHalfFrames, int activityId)
{
    cout << "DxArchiverProxy: beginSendingArchiveComplexAmplitudes" << endl;

    DxMessageCode code = BEGIN_SENDING_ARCHIVE_COMPLEX_AMPLITUDES;
    int dataLength = sizeof(Count);

    Count marshall = nHalfFrames;
    marshall.marshall();
    sendMessage(code, activityId, dataLength, &marshall);
}

void DxArchiverProxy::sendArchiveComplexAmplitudes(
    const ComplexAmplitudeHeader &hdr,
    SubchannelCoef1KHz subchannels[], int activityId)
{
    cout << "DxArchiverProxy: sendArchiveComplexAmplitudes" << endl;

    DxMessageCode code = SEND_ARCHIVE_COMPLEX_AMPLITUDES;
    sendComplexAmplitudesMsg(code, activityId, hdr, subchannels);
}


void DxArchiverProxy::doneSendingArchiveComplexAmplitudes(int activityId)
{
    cout << "DxArchiverProxy:doneSendingArchiveComplexAmplitudes" << endl;

    DxMessageCode code =  DONE_SENDING_ARCHIVE_COMPLEX_AMPLITUDES;
    sendMessage(code, activityId);
}



// --- utilities -----
// send a marshalled message to the dxArchiver
void DxArchiverProxy::sendMessage(int messageCode, int activityId, 
			   int dataLength, const void *msgBody)
{
    cout << "Send: " << SseDxMsg::messageCodeToString(messageCode) << endl;

    NssProxy::sendMessage(messageCode, activityId, dataLength, msgBody);

}


void DxArchiverProxy::sendComplexAmplitudesMsg(int messageCode,
					int activityId,
					const ComplexAmplitudeHeader &hdr,
					SubchannelCoef1KHz subchannels[])
{
    int dataLength;

    char *msgBody = SseDxMsg::encodeComplexAmplitudesIntoMsg(
	hdr, subchannels, &dataLength);

#ifdef  TEST_SEND_BAD_MESSAGE_SIZE
    // force bad message length for testing
    dataLength = 2000000;
#endif

    sendMessage(messageCode, activityId, dataLength, msgBody);

    delete[] msgBody;
}


//--------------------------------------------------

// dispatch incoming dxArchiver message to appropriate
// dx method
void DxArchiverProxy::handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff)
{

  cout << "DxArchiverProxy Recv: " << SseDxMsg::messageCodeToString(hdr->code) << endl;
  cout << *hdr;

  Assert(dx_);

  // dispatch to Dx.
  // process message body, if there is one
  switch (hdr->code)
  {
  case REQUEST_INTRINSICS:
      dx_->requestIntrinsics(this);
      break;

#if 0
  case REQUEST_DX_STATUS:
      dx_->requestDxStatus();
      break;
#endif

// TBD handle incoming sendDxMessage(NssMessage)

  default: 
      cout << "DxArchiverProxy::handleSseMsg: ";
      cout << "unexpected message code received:" << hdr->code << endl;
      break;
  };  


}
