/*******************************************************************************

 File:    DxArchiverProxy.h
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


#ifndef _DxArchiverProxy_h
#define _DxArchiverProxy_h

#include "sseDxInterface.h"
#include "NssProxy.h"

class Dx;

class DxArchiverProxy : public NssProxy
{

public:
    DxArchiverProxy(ACE_SOCK_STREAM &stream);
    virtual ~DxArchiverProxy();

    void setDx(Dx *dx);
    string getName();

    // outgoing messages to dxArchiver
    void sendDxMessage(const NssMessage &nssMessage, int activityId = -1);
    void sendIntrinsics(const DxIntrinsics &intrinsics);

    //void sendDxStatus(const DxStatus &status);

    void archiveSignal(const ArchiveDataHeader &hdr, int activityId);
    void beginSendingArchiveComplexAmplitudes(const Count &nHalfFrames, int activityId);
    void sendArchiveComplexAmplitudes(const ComplexAmplitudeHeader &hdr,
	SubchannelCoef1KHz subchannels[], int activityId);
    void doneSendingArchiveComplexAmplitudes(int activityId);


private:
    // Disable copy construction & assignment.
    // Don't define these.
    DxArchiverProxy(const DxArchiverProxy& rhs);
    DxArchiverProxy& operator=(const DxArchiverProxy& rhs);
    
    void sendMessage(int messageCode, int activityId = -1, int dataLength = 0,
			       const void *msgBody = 0);

    void handleIncomingMessage(SseInterfaceHeader *hdr, void *bodybuff);

    void notifyInputConnected();
    void notifyInputDisconnected();

    void sendComplexAmplitudesMsg(int messageCode, 
				  int activityId,
				  const ComplexAmplitudeHeader &hdr,
				  SubchannelCoef1KHz subchannels[]);

    Dx *dx_;

};



#endif 