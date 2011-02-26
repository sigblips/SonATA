/*******************************************************************************

 File:    DumpBaselines.java
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

package opensonata.dataDisplays;

// Convert binary baselines (stored as C structs)
// on the input file to ascii on stdout.
// Assumes the baseline structs are in network byte
// order.
// Structs & enums are defined in ssePdmInterface.h and
// sseInterface.h.

import java.io.*;

public class DumpBaselines {

    String filename;
    DataInputStream in;

    DumpBaselines(String filename) {
	this.filename = filename;

	openFile();
    }

    // see ssePdmInterface.h for C++ version of this struct
    // (BaselineHeader and BaselineValue structs)
    class NssBaseline {

	int maxSubbands = 10000;  // sanity check

	// header
	double rfCenterFrequency;
	double bandWidth;
	int halfFrameNumber;
	int numberOfSubbands;
	int polarization;  // defined as Polarization enum.  Assumed to be 32bit.
	int alignPad;

	// variable length body
	float[] baselineValue;

	public void read(DataInputStream in) {
	    try {
		// read the header
		rfCenterFrequency = in.readDouble();
		bandWidth = in.readDouble();
		halfFrameNumber = in.readInt();
		numberOfSubbands = in.readInt();
		polarization = in.readInt();
		alignPad = in.readInt();

		// read the variable length baseline value array
		if (numberOfSubbands > maxSubbands)
		{
		    System.out.println(
			"ERROR: subbands value " + numberOfSubbands +
			" exceeds max subbands value  " + maxSubbands);
		    System.exit(1);
		}

		baselineValue = new float[numberOfSubbands];
		for (int i=0; i<numberOfSubbands; ++i)
		{
		    baselineValue[i] = in.readFloat();
		}

	    }
	    catch (IOException ioe) {
		System.out.println(ioe);
	    }
	}

	private String polIntToString(int pol) {
	    String value;
	    switch (pol) {
	    case 0: value ="R";  // right circular
		break;
	    case 1: value = "L"; // left circular
		break;
	    case 2: value = "B"; // both
		break;
	    case 3: value = "M"; // mixed
		break;
	    default: value = "?";
		break;
	    }

	    return value;
	}


	public void printHeader() {
	    System.out.println(
		"rfcenterFreq " + rfCenterFrequency + " MHz," +
		" bandWidth " + bandWidth + " MHz," +
		" halfFrameNumber " + halfFrameNumber + "," +
		" #subbands " +  numberOfSubbands + "," +
		" pol " + polIntToString(polarization)
		);
		// alignPad
	}

	public void printBody() {
	    int maxToPrint = 5;
	    System.out.print("baselineValues: ");
	    for (int i=0; i<maxToPrint && i<numberOfSubbands; ++i)
	    {
		System.out.print(baselineValue[i] + " ");
	    }
	    System.out.println("");
	}

    }

    private void openFile()
    {
	if (filename != "") {
	    try {
		in = new DataInputStream(
		    new BufferedInputStream(
			new FileInputStream(filename)));
	    }
	    catch (IOException e)
	    {
		System.out.println(e);
	    }
	}
    }



    public void execute() {

	NssBaseline nssBaseline = new NssBaseline();

	if (in != null) {

	    try {

		while (in.available() != 0) {
		    
		    nssBaseline.read(in);
		    nssBaseline.printHeader();
		    nssBaseline.printBody();
		}
	    }
	    catch (IOException ioe) {
		System.out.println(ioe);
	    }
	}

    }

    public static void main(String[] args) {

	// get optional filename
	String filename = "";
	if (args.length != 1)
	{
	    System.out.println("args: [<baseline data filename>]");
	    System.exit(1);
	}

	filename = args[0];
	System.out.println("file: " + filename);

	DumpBaselines dumpBaselines = new DumpBaselines(filename);
	dumpBaselines.execute();
	
    }


}