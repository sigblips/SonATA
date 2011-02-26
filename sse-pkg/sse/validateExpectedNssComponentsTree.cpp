/*******************************************************************************

 File:    validateExpectedNssComponentsTree.cpp
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


#include <iostream>
#include <SseUtil.h>
#include <ExpectedNssComponentsTree.h>
#include <sstream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
	cerr << "Validates an ExpectedNssComponentsTree"
	     << " configuration file." << endl;
	cerr << "usage: " << argv[0] << " <config filename>" << endl;
	exit (1);
    }

    string configFilename(argv[1]);

    // make sure the file is there
    if (! SseUtil::fileIsReadable(configFilename))
    {
        cerr << "Warning: Can't read config file " << configFilename << endl;

	exit(1);
    } 
    else
    {
        ExpectedNssComponentsTree *tree;

	// load & parse the tree.  any errors or warnings will
	// be printed
	stringstream errorStrm;

        tree = new ExpectedNssComponentsTree(configFilename,
					     errorStrm);

	// print out the parsed tree
	cout << *tree;

	// print out any errors
	cout << errorStrm.str();

	delete tree;

	exit(0);
    }

}