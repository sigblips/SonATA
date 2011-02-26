/*******************************************************************************

 File:    wrapperTest.java
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

import java.util.*; 
import java.io.*; 
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import java.awt.*;
import java.awt.event.*;

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
		//System.out.println(type + ">" + line); 
		//System.out.println(line); 
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

public class wrapperTest extends JApplet
{ 
    static PrintWriter cmdStream;
    static JTextArea seekerFeedbackArea;
    static JTextField cmdField;

    public void init()
    {
	Container cp = getContentPane();
	cp.setLayout(new FlowLayout());

	JButton helpButton = new JButton("help");
	cp.add(helpButton);
	helpButton.addActionListener(new ButtonL());

	JButton exitButton = new JButton("exit");
	cp.add(exitButton);
	exitButton.addActionListener(new BLexit());

	int rows = 20;
	int cols = 80;
	seekerFeedbackArea = new JTextArea(rows, cols);
	seekerFeedbackArea.setEditable(false);
	seekerFeedbackArea.setLineWrap(true);

	JScrollPane pane = new JScrollPane(seekerFeedbackArea);
	pane.setHorizontalScrollBarPolicy(
	    JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
	cp.add(pane);

	cmdField = new JTextField(80);
	cmdField.addActionListener(new CmdFieldActionListener());
	cp.add(cmdField);


    }

    class CmdFieldActionListener implements ActionListener {
	public void actionPerformed(ActionEvent e) 
	{
	    String text = cmdField.getText();
	    sendCommand(text);
	    cmdField.selectAll();
	}
    }


    class ButtonL implements ActionListener {
	public void actionPerformed(ActionEvent e){
	    String name = ((JButton)e.getSource()).getText();
	    sendCommand(name);
	}
    }

    class BLexit implements ActionListener {
	public void actionPerformed(ActionEvent e){

	    String name = ((JButton)e.getSource()).getText();
	    sendCommand(name);
	    System.exit(0);
	}
    }

    public void sendCommand(String cmd)
    {
	// echo cmd
	seekerFeedbackArea.append("> " + cmd + "\n");

	// send cmd on to seeker
        // for tcl, need puts to echo back the results
        cmdStream.println("puts [" + cmd + "]");

	if (cmd.equals("quit") || cmd.equals("exit")) 
	{
	    System.exit(0);
	}

    }


    public static void main(String args[]) 
    { 

	try 
	{ 
	    // create GUI
	    JApplet applet = new wrapperTest();
	    JFrame frame = new JFrame("wrapperTest");

	    frame.getContentPane().add(applet);
	    frame.setSize(900, 500);
	    applet.init();
	    applet.start();
	    frame.setVisible(true);

		
	    // start up seeker
	    String[] cmd = new String[3]; 
	    cmd[0] = "/bin/sh" ; 
	    cmd[1] = "-c" ; 
	    cmd[2] = "seeker";
	    Runtime rt = Runtime.getRuntime(); 

	    System.out.println("Running " + cmd[0] + " " + cmd[1] 
			       + " " + cmd[2]); 
	    Process proc = rt.exec(cmd); 

	    // catch the error & output streams
	    StreamGobbler errorGobbler = new 
		StreamGobbler(proc.getErrorStream(), "ERROR",
			      seekerFeedbackArea);
		
	    StreamGobbler outputGobbler = new 
		StreamGobbler(proc.getInputStream(), "OUTPUT",
			      seekerFeedbackArea);

	    // kick them off 
	    errorGobbler.start(); 
	    outputGobbler.start(); 

	    // prepare a stream for outgoing commands
	    cmdStream = new PrintWriter(proc.getOutputStream(),
					true);

	} catch (Throwable t) 
	{ 
	    t.printStackTrace(); 
	} 
    } 
} 
