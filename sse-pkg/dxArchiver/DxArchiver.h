/*******************************************************************************

 File:    DxArchiver.h
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


#ifndef _dx_archiver_h
#define _dx_archiver_h

#include "sseDxInterface.h"
#include "sseDxArchiverInterface.h"
#include <vector>
#include "DxOpsBitset.h"

using std::string;

class SseProxy;
class DxProxy;
class DxArchiverSubscriber;

template<typename Tproxy> class NssComponentManager;
template<typename Tproxy> class NssAcceptHandler;

class DxArchiver {

public:
    DxArchiver(SseProxy *sseProxy, const string &name, int dxPort);
    virtual ~DxArchiver();

    // --- incoming messages from SSE ------
    virtual void requestArchiverIntrinsics();
    virtual void requestArchiverStatus();
    virtual void shutdownArchiver();

    // ----- public utility methods ------------------
    virtual void printIntrinsics();
    virtual void printStatus();
    virtual void printDxNames();

    virtual void setVerboseLevel(int level);
    virtual int getVerboseLevel();

    virtual void updateDxArchiverStatus();

    virtual void resetDxSocket(const string & dxName);

    // test methods
    virtual void sendErrorMsgToSse(const string &text);

protected:
    virtual void setup();
    virtual SseProxy *getSseProxy();
    virtual void initIntrinsics();

    // Data
    SseProxy *sseProxy_;
    DxArchiverIntrinsics archiverIntrinsics_;
    DxArchiverStatus archiverStatus_;
    string name_;
    int dxPort_;
    int verboseLevel_;
    DxArchiverSubscriber *subscriber_;
    NssComponentManager<DxProxy> *dxComponentManager_;
    NssAcceptHandler<DxProxy> *dxAcceptHandler_;
    
private:
    // Disable copy construction & assignment.
    // Don't define these.
    DxArchiver(const DxArchiver& rhs);
    DxArchiver& operator=(const DxArchiver& rhs);

};



#endif