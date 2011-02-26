/*******************************************************************************

 File:    CmdExec.java
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


import java.io.*;
import java.text.*;
import java.util.*;

// Execute a system command, printing the results
// of both the output and error streams.
// Based on code from
// http://www.javaworld.com/javaworld/jw-12-2000/jw-1229-traps.html

class CmdExec {

   class StreamGobbler extends Thread
   {
      InputStream istream;
      PrintWriter out;

      StreamGobbler (InputStream istream, PrintWriter out) {
	 this.istream = istream;
	 this.out = out;
      }

      public void run() {

	 try {
	    BufferedReader input = 
	       new BufferedReader
		  (new InputStreamReader(istream));
	    String line;
	    while ((line = input.readLine()) != null) {
	       out.println(line);
	       out.println("<br>");
	    }
	    input.close();
	 } 
	 catch (Exception err) {
	    out.println("caught exception <br>");
	    err.printStackTrace();
	 }
      }
   }

   public void execute(PrintWriter out, String cmdline) {
      try {
	 String[] cmdArray = {"/bin/sh", "-c", cmdline};

	 Process proc = Runtime.getRuntime().exec(cmdArray);

	 // catch the error & output streams
	 StreamGobbler errorGobbler = new 
	    StreamGobbler(proc.getErrorStream(), out);

	 StreamGobbler outputGobbler = new 
	    StreamGobbler(proc.getInputStream(), out);

	 errorGobbler.start(); 
	 outputGobbler.start(); 

	 // wait for the threads to complete
	 int exitVal = proc.waitFor();

      } 
      catch (Exception err) {
	 err.printStackTrace();
      }
   }

   public void executeWithEcho(PrintWriter out, String cmdline) {
      	 out.println("% " + cmdline + "<br>");
	 execute(out, cmdline);
   }

   CmdExec() {}

   CmdExec(PrintWriter out, String cmdline) {
      executeWithEcho(out, cmdline);
   }

}