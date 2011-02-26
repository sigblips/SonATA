/*******************************************************************************

 File:    FileSender.java
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

/*
Programs to send & receive binary files across sockets:

FileSender: (client)

- Given a list of input filenames in a specified directory,
then for each one:
- open the file. 
- Make a socket connection to the FileReceiver server.
- Send the filename as a preliminary header message.
- Read the file continuously, passing data to the socket
(essentially a tail -f).
- If no data is available, then sleep and try again later.
- If file is truncated, close socket 
and restart monitoring file as in the beginning (open a socket, 
send filename, etc).

Note: the full directory path is not sent to the filereceiver,
only the filename.
*/

import java.io.*;
import java.net.*;
import java.text.*;
import com.martiansoftware.jsap.*;
import com.martiansoftware.jsap.stringparsers.*;

public class FileSender extends Thread {

   String serverName;
   int serverPort;
   String dir;
   String filename;
   boolean verbose;
   int sleepTimeMs = 200;
   
   FileSender(String host, int port, String dir, String filename,
	      boolean verbose) {
      this.serverName = host;
      this.serverPort = port;
      this.dir = dir;
      this.filename = filename;
      this.verbose = verbose;
   }

   public void run() {

      // keep rereading the file forever
      while (true) {
	 SingleFileSender singleFileSender = new SingleFileSender();
	 
	 if (verbose) {
	    System.err.println(filename +  ": started reading");
	 }
	 singleFileSender.run();
      }

   }

   public class SingleFileSender {

      DataInputStream in;
      DataOutputStream out;
      byte[] byteArray;
      Socket socket = null;
      boolean alreadyTruncated = true;

      SingleFileSender() {

	 int MAX_READBYTES = 20000;
	 byteArray = new byte[MAX_READBYTES];

	 openSocket();
      }

      private void openSocket() {

	 try {
	    socket = new Socket(serverName, serverPort);
	    out = new DataOutputStream(socket.getOutputStream());

	    // send the filename to the receiver
	    out.writeUTF(filename);

	 } catch (UnknownHostException e) {
	    System.err.println("Unknown host: " + serverName + " (" + e + ")");
	    System.exit(1);
	 } catch (IOException e) {
	    System.err.println("Failed to open socket to host: "
			       + serverName + " (" + e + ")");
	    System.exit(1);
	 }
      }

      private void closeConnections() {

	 try {
	    in.close();
	    out.close();
	    socket.close();
	 
	    socket = null;
	    out = null;
	    in = null;

	 } catch (IOException e) {
	    System.err.println(e);
	    System.exit(1);
	 }

      }

      private void openFile() {
	 try {

	    String path = dir + "/" + filename;

	    in = new DataInputStream(
	       new BufferedInputStream(
		  new FileInputStream(path)));
	 }
	 catch (IOException e)
	 {
	    System.err.println(e);
	    System.exit(1);
	 }
      }
   
      private void sleep() {
	 try {
	    //System.err.println("sleeping");
	    Thread.sleep(sleepTimeMs);
	 }
	 catch (InterruptedException e) {
	    // Interrupt may be thrown manually by stop()
	 }
      }

      public void run() {

	 openFile();

	 boolean continueRunning = true;
      
	 while (continueRunning) {
	 
	    try {

	       // Keep reading data, and passing it to the output socket
	       while (in != null && in.available() != 0) {

		  // read it
		  int nread = in.read(byteArray);

		  if (verbose) {
		     System.err.println(filename + ": read " + nread + " bytes");
		  }
		  if (nread == -1)
		  {
		     // input file was truncated
		     // only process file truncation once when it's detected
		     if (! alreadyTruncated)
		     {
			if (verbose) {
			   System.err.println("file " + filename 
					      + ": was truncated, restarting");
			}
			alreadyTruncated = true;
			continueRunning = false;
		     
			break; 
		     }
		  } 
		  else {

		     alreadyTruncated = false;
		  
		     // write data to the socket
		     int offset = 0;
		     out.write(byteArray, offset, nread);
		  }
	       }
	       out.flush();
	    }
	    catch (EOFException e)
	    {
	       if (verbose) {
		  System.err.println(filename + ": caught EOFException");
		  System.err.println(e);
	       }
	       // this is ok, do nothing

	    }
	    catch (IOException e)
	    {
	       System.err.println(filename + ": " + e);

	    }

	    // wait for more data to appear
	    sleep();
	 }

	 closeConnections();

      }
   }

   public static void main(String[] args) throws IOException, Exception {
  
      JSAP jsap = new JSAP();  // arg parser
    
      // get the server host, port, and filename arguments

      String hostArgName = "host";
      FlaggedOption hostOpt = new FlaggedOption(hostArgName)
	 .setStringParser(new StringStringParser())
	 .setRequired(true)
	 .setLongFlag(hostArgName);
      hostOpt.setHelp("filereceiver server host");
      jsap.registerParameter(hostOpt);

      String portArgName = "port";
      String defaultServerPort = "8840";
      FlaggedOption portOpt = new FlaggedOption(portArgName)
	 .setStringParser(new IntegerStringParser())
	 .setRequired(false)
	 .setDefault(defaultServerPort)
	 .setLongFlag(portArgName);
      portOpt.setHelp("filereceiver server port (default=" 
		      + defaultServerPort +")");
      jsap.registerParameter(portOpt);

      String verboseArgName = "verbose";
      Switch verboseOpt = new Switch(verboseArgName)
	 .setLongFlag(verboseArgName);
      verboseOpt.setHelp("turn on verbose output");
      jsap.registerParameter(verboseOpt);


      String dirArgName = "dir";
      String defaultDir = ".";
      FlaggedOption dirOpt = new FlaggedOption(dirArgName)
	 .setStringParser(new StringStringParser())
	 .setRequired(false)
	 .setDefault(defaultDir)
	 .setLongFlag(dirArgName);
      dirOpt.setHelp("directory containing files to be sent. default="
		     + defaultDir);
      jsap.registerParameter(dirOpt);
        
      String unflaggedArgFilename = "filename";
      UnflaggedOption filenameOpt = new UnflaggedOption(unflaggedArgFilename)
	 .setStringParser(new StringStringParser())
	 .setRequired(true)
	 .setGreedy(true);   // greedy allows any number of arguments
      jsap.registerParameter(filenameOpt);


      JSAPResult config = jsap.parse(args);
      if (!config.success()) {

	 for (java.util.Iterator errs = config.getErrorMessageIterator();
	      errs.hasNext();) {
	    System.err.println("Error: " + errs.next());
	 }

	 System.err.print("usage: ");
	 System.err.println(jsap.getUsage());
	 System.err.println(jsap.getHelp());
	 System.exit(1);
      }

      String host = config.getString(hostArgName);
      int port = config.getInt(portArgName);
      String dir = config.getString(dirArgName);
      boolean verbose = config.getBoolean(verboseArgName);

      // loop through filenames, start a FileSender in it's own thread
      // for each one
      String[] filenameList = config.getStringArray(unflaggedArgFilename);
      for (int i=0; i< filenameList.length; ++i) {
	 //System.out.println("filename: " + filenameList[i]);
	 new FileSender(host, port, dir, filenameList[i], verbose).start();
      }

      // wait for all the threads to run to completion
   }
}