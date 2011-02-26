/*******************************************************************************

 File:    DxComponentManager.h
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


#ifndef DxComponentManager_H
#define DxComponentManager_H

// subclass of NssComponentManager template for DxProxy class

#include "DxProxy.h"
#include "SseComponentManager.h"
class ExpectedNssComponentsTree;

class DxComponentManager : public SseComponentManager<DxProxy>
{
public:

    DxComponentManager(Subscriber *subscriber, 
			const string &archiver1Hostname,
			const string &archiver1Port,
			const string &archiver2Hostname,
			const string &archiver2Port,
			const string &archiver3Hostname,
			const string &archiver3Port,
			const string &permRfiMaskFilename) 
	:
	SseComponentManager<DxProxy>(subscriber),
	archiver1Hostname_(archiver1Hostname),
	archiver1Port_(archiver1Port),
	archiver2Hostname_(archiver2Hostname),
	archiver2Port_(archiver2Port),
	archiver3Hostname_(archiver3Hostname),
	archiver3Port_(archiver3Port),
	permRfiMaskFilename_(permRfiMaskFilename),
	expectedNssComponentsTree_(0)
    {}

    void additionalReceiveIntrinsicsProcessing(DxProxy *proxy);
    void setExpectedNssComponentsTree(ExpectedNssComponentsTree *tree);

private:
    
    string getIfcNameForDx(const string &dxName);

    string archiver1Hostname_;
    string archiver1Port_;
    string archiver2Hostname_;
    string archiver2Port_;
    string archiver3Hostname_;
    string archiver3Port_;
    string birdieMaskFilename_;
    string permRfiMaskFilename_;

    ExpectedNssComponentsTree *expectedNssComponentsTree_;
};



#endif 