/*******************************************************************************

 File:    RunExecWrapper.java
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

// A wrapper that starts up an executable in 
// a separate process.  The output & error streams
// are captured and put into a JTextArea widget.
// Commands can be send to the input stream.

import java.util.*; 
import java.io.*; 
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;

public class RunExecWrapper
{ 
    private PrintWriter cmdStream;
    private JTextArea feedbackArea;

    public RunExecWrapper(String progName, JTextArea textArea)
    {
	feedbackArea = textArea;
	spawnExec(progName, textArea);
    }

    public void sendCommand(String cmd)
    {
	// echo cmd
	feedbackArea.append("> " + cmd + "\n");

	cmdStream.println(cmd);
    }

    private void spawnExec(String progName, JTextArea area)
    {

	try
	{
	    // start up the program
	    String[] cmd = new String[3]; 
	    cmd[0] = "/bin/sh" ; 
	    cmd[1] = "-c" ; 
	    cmd[2] = progName;
	    Runtime rt = Runtime.getRuntime(); 

	    System.out.println("Running " + cmd[0] + " " + cmd[1] 
			       + " " + cmd[2]); 
	    Process proc = rt.exec(cmd); 

	    // catch the error & output streams
	    StreamGobbler errorGobbler = new 
		StreamGobbler(proc.getErrorStream(), "stderr",
			      feedbackArea);
	    errorGobbler.start(); 
		
	    StreamGobbler outputGobbler = new 
		StreamGobbler(proc.getInputStream(), "stdout",
			      feedbackArea);
	    outputGobbler.start(); 

	    // prepare a stream for the outgoing commands
	    cmdStream = new PrintWriter(proc.getOutputStream(),
					true);

	} catch (Throwable t) 
	{ 
	    t.printStackTrace(); 
	} 

    }


    // Handles stream output, putting it into the specified
    // JTextArea.

    class StreamGobbler extends Thread 
    { 
	InputStream is; 
	String type; 
	JTextArea area;

	StreamGobbler(InputStream is, String type, JTextArea area)
	{ 
	    this.is = is; 
	    this.type = type;
	    this.area = area;
	} 

	public void run() 
	{ 
	    try 
	    { 
		InputStreamReader isr = new InputStreamReader(is); 
		BufferedReader br = new BufferedReader(isr); 

		String line=null; 
		while ( (line = br.readLine()) != null)
		{ 
		    area.append(line + "\n");

		    // scroll to the bottom
		    area.setCaretPosition(area.getDocument().getLength());
		}

	    } 
	    catch (IOException ioe) 
	    { 
		ioe.printStackTrace(); 
	    } 
	} 
    } 



} 
