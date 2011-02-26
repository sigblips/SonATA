/*******************************************************************************

 File:    FileReceiver.java
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

- Wait for socket connections from FileSender(s).  
- When connection is made, read header message for filename
and open output file with truncation.
- Receive data on socket and write to file.
- After a specified period of inactivity on the connection,
the socket and file are closed.
- If a new connection is made that has the same filename as
an old connection, then the old connection is closed, and the
new connection takes its place.

*/

import java.io.*;
import java.net.*;
import java.text.*;
import java.util.*;
import com.martiansoftware.jsap.*;
import com.martiansoftware.jsap.stringparsers.*;


public class FileReceiver {

   boolean verbose = false;
   ConnectionManager connectionManager = null;

   FileReceiver() {

      connectionManager = new ConnectionManager();
   }

   public void handleConnections(String[] args) throws Exception {
      JSAP jsap = new JSAP();  // arg parser

      String portArgName = "port";
      String defaultServerPort = "8840";
      FlaggedOption portOpt = new FlaggedOption(portArgName)
	 .setStringParser(new IntegerStringParser())
	 .setRequired(false)
	 .setDefault(defaultServerPort)
	 .setLongFlag(portArgName);
      portOpt.setHelp("server port (default=" 
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
      dirOpt.setHelp("directory where files are to be written. default="
		     + defaultDir);
      jsap.registerParameter(dirOpt);


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

      int port = config.getInt(portArgName);
      String dir = config.getString(dirArgName);
      verbose = config.getBoolean(verboseArgName);

      ServerSocket serverSocket = null;
      boolean listening = true;

      try {
	 serverSocket = new ServerSocket(port);
      } catch (IOException e) {
	 System.err.println("Could not listen on port: " + port + " " + e);
	 System.exit(-1);
      }

      while (listening) {
	 new SingleFileReceiver(serverSocket.accept(),dir,verbose).start();
      }

      serverSocket.close();
   }


   // Manage the connections. 
   // Store a FileReceiver (ie, a connection thread) in a map, 
   // indexed by the filename.  If there's already a FileReceiver
   // associated with that file, then send a message to it telling 
   // it to terminate itself, and put the new FileReceiver in the map
   // in its place.   FileReceivers may also disconnnect themselves 
   // if they timeout.

   private class ConnectionManager {

      Map map;  // key=String filename, data = SingleFileReceiver

      public ConnectionManager() {
	 map = new HashMap();
      }

      public synchronized void add(String filename,
		      SingleFileReceiver singleFileReceiver) {

	 if (verbose) {
	    System.err.println("connMgr: add " 
			       + singleFileReceiver.getConnectionId());
	 }
	 
	 if (map.containsKey(filename)) {

	    SingleFileReceiver oldReceiver = (SingleFileReceiver) map.get(filename);
	    map.remove(filename);

	    if (verbose) {
	       System.err.println("connMgr: 'add method' removed & terminated old "
			       + oldReceiver.getConnectionId() + " connection ");
	    }

	    oldReceiver.terminate();

	 }
	 map.put(filename, singleFileReceiver);

      }
 
      public synchronized void remove(String filename, 
				      SingleFileReceiver singleFileReceiver) {

	 if (verbose) {
	    System.err.println("connMgr: called remove " + 
			       singleFileReceiver.getConnectionId());
	 }

	 if (map.containsKey(filename)) {
	    
	    // may have already been removed by the 'add'
	    // method, so make sure there's no attempt to remove it twice

	    SingleFileReceiver receiver = (SingleFileReceiver) map.get(filename);
	    if (receiver == singleFileReceiver) {
	       map.remove(filename);
	    } else {
	       if (verbose) {
		  System.err.println("connMgr: old " + filename + " " 
				     + singleFileReceiver.getConnectionId() 
				     + " connection already terminated");
	       }

	    }

	 }

      }

   }


   public class SingleFileReceiver extends Thread {

      DataInputStream in;
      DataOutputStream out;
      byte[] byteArray;
      Socket socket = null;
      String dir;
      String filename;
      String connectionId;
      boolean verbose;
      int sleepTimeMs = 200;
      int connectionTimeoutMs = 120000;
      int inactiveConnectionTimeMs = 0;
      volatile boolean continueRunning = true;

      SingleFileReceiver(Socket socket, String dir, boolean verbose) {
	 super("SingleFileReceiver");

	 int MAX_READBYTES = 20000;
	 byteArray = new byte[MAX_READBYTES];
	 
	 this.socket = socket;
	 this.dir = dir;
	 this.verbose = verbose;

	 openSocket();
	 openFile();

	 connectionManager.add(filename, this);

      }

      public String getConnectionId()
      {
	 return connectionId;
      }

      private void setConnectionId()
      {
	 connectionId = filename + " (id: " + this.hashCode() + ")";
      }

      private void openSocket() {

	 try {

	    in = new DataInputStream(
	       socket.getInputStream());

	    this.filename = in.readUTF();
	    setConnectionId();

	    if (verbose) {
	       System.err.println("socket opened: incoming filename: "
				  + connectionId);
	    }


	 } catch (Exception e) {
	    System.err.println(e);
	    System.exit(1);
	 }

      }

      private void closeConnections() {
      
	 try {

	    if (verbose) {
	       System.err.println(connectionId + ": close socket & streams");
	    }

	    in.close();
	    out.close();
	    socket.close();
	 
	    socket = null;
	    out = null;
	    in = null;
	 
	    connectionManager.remove(filename, this);

	 } catch (IOException e) {
	    System.err.println(e);
	    System.exit(1);
	 }
      
      }

      private void openFile() {
	 try {

	    String path = dir + "/" + filename;

	    out = new DataOutputStream(
	       new BufferedOutputStream(
		  new FileOutputStream(path)));
	 }
	 catch (IOException e) {
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

      public void terminate() {
	 
	 continueRunning = false;
      }

      public void run() {
      
	 while (continueRunning) {
	 
	    try {

	       // Keep reading data, and passing it to the output socket
	       while (in != null && in.available() != 0) {

		  // read it
		  int nread = in.read(byteArray);

		  if (verbose) {
		     System.err.println(connectionId + ": read " + nread + " bytes" +
					" from the socket");
		  }
		  if (nread == -1)
		  {
		     // TBD add different error handling?
		     System.err.println(connectionId + ": input was truncated, exiting");
		     System.exit(0);
		  }

		  // write it
		  int offset = 0;
		  out.write(byteArray, offset, nread);

		  inactiveConnectionTimeMs = 0;

	       }
	       out.flush();
	    }
	    catch (EOFException e) {
	       System.err.println(connectionId + ": caught EOFException from the socket");
	       System.err.println(e);

	       // do nothing

	    }
	    catch (Exception e) {
	       System.err.println(e);

	    }

	    // wait for more data to appear
	    sleep();

	    inactiveConnectionTimeMs += sleepTimeMs;
	    if (inactiveConnectionTimeMs >= connectionTimeoutMs)
	    {
	       if (verbose) {
		  System.out.println(connectionId + ": connection timeout");
	       }
	       continueRunning = false;
	    }
	 }

	 closeConnections();

      }

   }

   public static void main(String[] args) throws IOException, Exception {

      FileReceiver fileReceiver = new FileReceiver();

      fileReceiver.handleConnections(args);

   }
}