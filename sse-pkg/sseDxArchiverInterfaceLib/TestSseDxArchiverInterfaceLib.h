/*******************************************************************************

 File:    TestSseDxArchiverInterfaceLib.h
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


#ifndef TEST_SSE_DX_ARCHIVER_INTERFACE_LIB_H
#define TEST_SSE_DX_ARCHIVER_INTERFACE_LIB_H

#include "TestCase.h"
#include "TestSuite.h"
#include "TestCaller.h"
#include "sseDxArchiverInterface.h"

class TestSseDxArchiverInterfaceLib : public TestCase
{
 public:
    TestSseDxArchiverInterfaceLib (std::string name) : TestCase (name) {}

    void setUp ();
    void tearDown();
    static Test *suite ();

 protected:
    void testFoo();
    void testDxArchiverIntrinsics();
    void testPrint();
    void initSseInterfaceHdr(SseInterfaceHeader &hdr,
				unsigned int code);
};


#endif
