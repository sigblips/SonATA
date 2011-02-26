/*******************************************************************************

 File:    SseException.h
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
 * SseException.h - declaration of top level Sse exception
 * PURPOSE:  
 *****************************************************************/

#ifndef SSEEXCEPTION_H
#define SSEEXCEPTION_H

#include <iostream>
#include <string>
#include "sseInterface.h"
#include "SseMessage.h"

using std::string;
using std::ostream;

class SseException
{
public:

  SseException(const string& descrip,
	const string &sourceFilename,
	int lineNumber, 
        SseMsgCode code = SSE_MSG_UNINIT, 
	NssMessageSeverity severity = SEVERITY_ERROR) throw()
     : 
     description_(descrip), 
     sourceFilename_(sourceFilename),
     lineNumber_(lineNumber),
     code_(code), 
     severity_(severity) 
     {
     }
	
  SseException(const string& descrip) throw()
     :
     description_(descrip),
     sourceFilename_(""),
     lineNumber_(-1),
     code_(SSE_MSG_UNINIT),
     severity_(SEVERITY_ERROR)
  {
  }

  virtual ~SseException() throw()
  {
  }

  virtual const char* descrip() const throw()
  { 
      return description_.c_str();
  }

  virtual void newDescription(const string & newDescrip)
  {
      description_ = newDescrip;
  }

  virtual void newCode(SseMsgCode code)
  {
      code_ = code;
  }

  virtual const char* sourceFilename() const throw()
  { 
      return sourceFilename_.c_str();
  }

  virtual const int lineNumber() const throw()
  { 
      return lineNumber_;
  }

  virtual const SseMsgCode code() const throw()
  { 
      return code_;
  }

  virtual const NssMessageSeverity severity() const throw()
  { 
      return severity_;
  }

  friend ostream& operator<< (ostream& strm, const SseException& err)
  {
      strm << "SseException: " << err.sourceFilename_ 
	   << "[" << err.lineNumber_ << "] " << err.descrip() ;
      return(strm);
  }

 protected:

  SseException() throw() { }
  
 private:

  string description_;
  string sourceFilename_;
  int lineNumber_;
  SseMsgCode code_;
  NssMessageSeverity severity_;

};



#endif /* SSEEXCEPTION_H */