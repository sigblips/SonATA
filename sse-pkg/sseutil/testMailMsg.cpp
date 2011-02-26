/*******************************************************************************

 File:    testMailMsg.cpp
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



// Test MailMsg routine

#include "SseUtil.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2) 
    {
	cerr << "usage: " << argv[0] << " <email addr to send message to>" << endl;
	exit(1);
    }

    string toList(argv[1]);
    string subject("SseUtil::mailMsg tester's subject line");
    string body("this is someone's text.\nanother line");

    cout << "sending mail:" << endl
	 << "subject: " << subject << endl
	 << "to: " << toList << endl
	 << "body: " << body << endl;

    SseUtil::mailMsg(subject, toList, body);

}