/*******************************************************************************

 File:    BinTail.java
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

// tail a binary file, passing data to stdout.
// If file is at EOF, sleep until more input is available.
// If file is truncated, then exit.

import java.io.*;
import java.text.*;

public class BinTail {

    // Read file containing binary data.
    // Effectively does a 'tail -f' on the file, sleeping periodically
    // until new data appears.
    // If an EOFException is received, it means the file has been truncated
    // and it's time to exit.

   DataInputStream in;
   byte[] byteArray;

   BinTail() {
      int MAX_READBYTES = 512;

      byteArray = new byte[MAX_READBYTES];
   }

   private void openFile(String filename)
   {
      try {
	 in = new DataInputStream(
	    new BufferedInputStream(
	       new FileInputStream(filename)));
      }
      catch (IOException e)
      {
	 System.err.println(e);
	 System.exit(1);
      }
   }
   
   private void sleep()
   {
      try {
	 //System.err.println("sleeping");
	 int latency = 200;  //mS
	 
	 Thread.sleep(latency);
      }
      catch (InterruptedException e) {
	 // Interrupt may be thrown manually by stop()
      }
   }


   public void run() {

      boolean continueRunning = true;
      
      while (continueRunning) {
	 
	 try {

	    // Keep reading data, and passing it to stdout
		    
	    int x = 0;
	    while (in != null && in.available() != 0) {

	       // read it
	       int nread = in.read(byteArray);

	       System.err.println("read " + nread + " bytes");
	       if (nread == -1)
	       {
		  // file was truncated
		  System.err.println("file was truncated, exiting");
		  System.exit(0);
	       }

	       // write it
	       int offset = 0;
	       System.out.write(byteArray, offset, nread);

	    }
	 }
	 catch (EOFException e)
	 {
	    // Input file was truncated.  
	    // Reopen file to start over.

	    System.err.println(e);

	    System.exit(0);

	 }
	 catch (IOException e)
	 {
	    System.err.println(e);

	 }

	 //System.err.println("read " + count + " bytes");

	 // wait for more data to appear
	 //sleep();
      }
   }

   public static void main(String[] args) {

      if (args.length < 1)
      {
	 System.err.println("usage: bintail <filename>");
	 System.exit(1);
      }

      System.err.println("started reading...");
      
      BinTail binTail = new BinTail();

      binTail.openFile(args[0]);
      binTail.run();
      
   }
}