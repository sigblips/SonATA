/*******************************************************************************

 File:    ObserveActivityStatus.h
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


#ifndef ObserveActivityStatus_H
#define ObserveActivityStatus_H

#include <ace/Synch.h>
#include <string>

using std::string;

class ObserveActivityStatus
{
 public:
    ObserveActivityStatus();
    virtual ~ObserveActivityStatus();

  enum StatusEnum {
      ACTIVITY_CREATED,
      ACTIVITY_STARTED,
      PENDING_DATA_COLLECTION,
      DATA_COLLECTION_STARTED,
      DATA_COLLECTION_COMPLETE,
      PENDING_SIGNAL_DETECTION,
      SIGNAL_DETECTION_STARTED,
      SIGNAL_DETECTION_COMPLETE,
      ACTIVITY_ABORTED,
      ACTIVITY_FAILED,
      ACTIVITY_STOPPING,
      ACTIVITY_STOPPED,
      ACTIVITY_COMPLETE,
      ACTIVITY_STATUS_MAXIMUM,
  };

  StatusEnum get() const;
  string getString() const;
  void set(StatusEnum status);

 private:

  mutable ACE_Recursive_Thread_Mutex objectMutex_;
  StatusEnum statusEnum_;

  // Disable copy construction & assignment.
  // Don't define these.
  ObserveActivityStatus(const ObserveActivityStatus& rhs);
  ObserveActivityStatus& operator=(const ObserveActivityStatus& rhs);

};

#endif // ObserveActivityStatus_H