/*******************************************************************************

 File:    ActivitySummary.java
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

// ==========================================================
// Filename:    ActivitySummary.java
// Description: generates activity summary
//
// Authors:     L.R. McFarland
// Language:    java
//
// Created:     2002-11-15
// ==========================================================

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;

import javax.swing.*;
import javax.swing.text.*;

import java.text.*;

import java.io.*;
import java.util.*;

import java.util.regex.*;

import java.sql.*;

public class ActivitySummary implements DBApplicationInterface {

    JFrame             asFrame;
    DBConnectionDialog dbcd;

    JLabel             hostValue;
    JLabel             dbnameValue;

    // ----- parameters -----

    JTextField    startActivityTF;
    JTextField    stopActivityTF;

    JRadioButton  mainRB;
    static String mainST = new String("main");

    JRadioButton  remoteRB;
    static String remoteST = new String("remote");

    JRadioButton  mainAndRemoteRB;
    static String mainAndRemoteST = new String("both");

    JTextField    whereTF;
    

    // ----- report area -----

    JTextArea   summaryReportTA;
    String      saveAsFilename;
  
  
    // -------------------
    // ----- methods -----
    // -------------------

    public ActivitySummary (DBConnectionDialog dbConnectionDialog) {

        dbcd = dbConnectionDialog;
        dbcd.setDBAI(this);

        asFrame = new JFrame("Activity Summary");

        Container cp = asFrame.getContentPane();

        // ----- grid bag -----

	GridBagLayout      asPanelGB  = new GridBagLayout();
	cp.setLayout(asPanelGB);

	GridBagConstraints asPanelGBC = new GridBagConstraints();
	asPanelGBC.fill      = GridBagConstraints.HORIZONTAL;
	asPanelGBC.insets    = new Insets(5, 5, 5, 5);


	// ----------------------------------
	// ----- query parameters panel -----
	// ---------------------------------

	int tfWidth = 8;

        hostValue = new JLabel("TBD");
        dbnameValue = new JLabel("TBD");


	// ----- main remote radio buttons -----

	JLabel pdmModeLabel = new JLabel("PDM Mode: ");

	mainRB = new JRadioButton(mainST);
	mainRB.setActionCommand(mainST);
        mainRB.setSelected(true);

	remoteRB = new JRadioButton(remoteST);
	remoteRB.setActionCommand(remoteST);

	mainAndRemoteRB = new JRadioButton(mainAndRemoteST);
	mainAndRemoteRB.setActionCommand(mainAndRemoteST);

        ButtonGroup mrBG = new ButtonGroup();
        mrBG.add(mainRB);
        mrBG.add(remoteRB);
        mrBG.add(mainAndRemoteRB);

	JPanel mrPanel = new JPanel();

	mrPanel.add(pdmModeLabel);
	mrPanel.add(mainRB);
	mrPanel.add(remoteRB);
	mrPanel.add(mainAndRemoteRB);

	asPanelGBC.gridwidth = 4;
	asPanelGBC.gridx = 0;
	asPanelGBC.gridy = 0;
	asPanelGB.setConstraints(mrPanel, asPanelGBC);
	cp.add(mrPanel);
	asPanelGBC.gridwidth = 1; // restore default
	
	// ----- start/stop activity -----

	JPanel ssPanel = new JPanel();

	JLabel startActivityLabel = new JLabel("Start Activity");
	ssPanel.add(startActivityLabel);

	startActivityTF = new JTextField(tfWidth);
	ssPanel.add(startActivityTF);

	JLabel stopActivityLabel = new JLabel("Stop Activity");
	ssPanel.add(stopActivityLabel);

	stopActivityTF = new JTextField(tfWidth);
	ssPanel.add(stopActivityTF);


	asPanelGBC.gridwidth = 4;
	asPanelGBC.gridx = 0;
	asPanelGBC.gridy = 1;
	asPanelGB.setConstraints(ssPanel, asPanelGBC);
	cp.add(ssPanel);
	asPanelGBC.gridwidth = 1; // restore default



	// ----- where clause -----

	JLabel whereLabel = new JLabel("where:");
	asPanelGBC.gridx = 0;
	asPanelGBC.gridy = 2;
	asPanelGB.setConstraints(whereLabel, asPanelGBC);
	cp.add(whereLabel);

	asPanelGBC.gridwidth = 3;
	whereTF = new JTextField(tfWidth);
	asPanelGBC.gridx = 1;
	asPanelGBC.gridy = 2;
	asPanelGB.setConstraints(whereTF, asPanelGBC);
	cp.add(whereTF);
	asPanelGBC.gridwidth = 1; // restore default

	// ------------------------
	// ----- report panel -----
	// ------------------------
	
	summaryReportTA             = new JTextArea();
	JScrollPane summaryReportSP = new JScrollPane(summaryReportTA);

	summaryReportSP.setPreferredSize(new Dimension(550, 500));
	summaryReportSP.setBorder(BorderFactory.createTitledBorder(
				 BorderFactory.createLoweredBevelBorder(), 
				 "Summary Report"));

	asPanelGBC.gridwidth = 4;
	
	asPanelGBC.gridx = 0;
	asPanelGBC.gridy = 3;
	asPanelGB.setConstraints(summaryReportSP, asPanelGBC);
	cp.add(summaryReportSP);

	asPanelGBC.gridwidth = 1; // restore default

	// -------------------------
	// ----- control panel -----
	// -------------------------

	JPanel controlPanel = new JPanel();

        // ----- dbConnection button -----

        JButton dbConnectionBT = new JButton("Connection");
        dbConnectionBT.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    dbcd.setVisible(true);
                }
            });

        controlPanel.add(dbConnectionBT);

	// ----- update button -----

	JButton updateBT = new JButton("Update");
	updateBT.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    updateReport();
		}
	    });

	controlPanel.add(updateBT);

	// ----- saveAs button -----

	final JFileChooser saveAsFileChooser = new JFileChooser();
	JButton            saveAsBT          = new JButton("Save As");

	saveAsBT.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {

		    int returnVal = 
			saveAsFileChooser.showDialog(asFrame, "Save");
          
		    if (returnVal == JFileChooser.APPROVE_OPTION) {
            
			saveAsFilename =
			    saveAsFileChooser.getSelectedFile().getPath();
			
			saveAs(saveAsFilename);
			
		    }
		}
	    });

	controlPanel.add(saveAsBT);

	// ----- exit button -----

	JButton exitBT = new JButton("Exit");
	exitBT.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    System.exit(0);
		}
	    });

	controlPanel.add(exitBT);

	asPanelGBC.gridwidth = 4;
	asPanelGBC.gridx = 0;
	asPanelGBC.gridy = 4;
	asPanelGB.setConstraints(controlPanel, asPanelGBC);
	cp.add(controlPanel);
	asPanelGBC.gridwidth = 1; // restore default


        // ----- add to frame -----

        asFrame.pack();

        asFrame.setVisible(true);

        asFrame.addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    System.exit(0);
                }
            });

    }

    public void saveAs (String filename) {

	try {

	    BufferedOutputStream os = 
		new BufferedOutputStream(new FileOutputStream(saveAsFilename));
	    
	    String sOut = summaryReportTA.getText();

	    os.write(sOut.getBytes());
	    os.flush();

	} catch (IOException e) {
	    JOptionPane.showMessageDialog(asFrame, "Unexpected IO Error: " 
					  + e);
	}

    }  

    public void updateReport () {

	if (startActivityTF.getText().length() == 0) {
	    setFirst();
	}

	if (stopActivityTF.getText().length() == 0) {
	    setLast();
	}

	summaryReportTA.setText(""); // clear text
	StringBuffer reportSB = new StringBuffer();

	reportSB.append("Host:\t" + dbcd.hostnameTF.getText());
	reportSB.append("\n");

	reportSB.append("Database:\t" + dbcd.dbnameTF.getText());
	reportSB.append("\n");

	reportSB.append("PDM Mode:\t");

	if (mainRB.isSelected()) {
	    reportSB.append("main or main_only");
	}

	if (remoteRB.isSelected()) {
	    reportSB.append("remote");
	}

	if (mainAndRemoteRB.isSelected()) {
	    reportSB.append("main and remote");
	}

	reportSB.append("\n");

	reportSB.append("Where:\t");
	reportSB.append(whereTF.getText());
	reportSB.append("\n");

	reportSB.append("Activity Range:\t");
	reportSB.append(startActivityTF.getText());
	reportSB.append(" - ");
	reportSB.append(stopActivityTF.getText());
	reportSB.append("\n");

	reportSB.append("Date Range:\t");
	reportActivityDate(reportSB, startActivityTF.getText());
	reportSB.append(" \t ");
	reportActivityDate(reportSB, stopActivityTF.getText());
	reportSB.append("\n");

	reportSB.append("Freq. Range:\t");
	reportActivityFrequencies(reportSB);
	reportSB.append("\n");

	reportSB.append("==================================");
	reportSB.append("==================================");
	reportSB.append("\n");

	reportSB.append("Clsss \tReason \t\tCwP \tCwC \tPulse \n");

	reportSB.append("---------------------------------------");
	reportSB.append("---------------------------------------");
	reportSB.append("---------------------------------------");
	reportSB.append("\n");


	// ----------------------
	// ----- candidates -----
	// ----------------------

	reportSB.append("Candidates:\n");


	// ----- confirmed -----

	reportSB.append("\tConfirmed: \t\t");

	reportConfirmed("Candidate", "CwP", reportSB);
	reportSB.append("\t");

	reportConfirmed("Candidate", "CwC", reportSB);
	reportSB.append("\t");

	reportConfirmed("Candidate", "Pul", reportSB);
	reportSB.append("\n");

	// ----- not confirmed -----

	reportSB.append("\tNot Confirmed: \t\t");

	reportNotConfirmed("Candidate", "CwP", reportSB);
	reportSB.append("\t");

	reportNotConfirmed("Candidate", "CwC", reportSB);
	reportSB.append("\t");

	reportNotConfirmed("Candidate", "Pul", reportSB);
	reportSB.append("\n");

	// ----- passed power -----

	reportSB.append("\tPassed Power Threshold: \t");

	reportPassedPower("Candidate", "CwP", reportSB);
	reportSB.append("\t");

	reportPassedPower("Candidate", "CwC", reportSB);
	reportSB.append("\t");

	reportPassedPower("Candidate", "Pul", reportSB);
	reportSB.append("\n");

	// ----- seen off -----

	reportSB.append("\tSeen off: \t\t");

	reportSeenOff("Candidate", "CwP", reportSB);
	reportSB.append("\t");

	reportSeenOff("Candidate", "CwC", reportSB);
	reportSB.append("\t");

	reportSeenOff("Candidate", "Pul", reportSB);
	reportSB.append("\n");

	// ----- seen off -----

	reportSB.append("\tNot Seen off: \t\t");

	reportNotSeenOff("Candidate", "CwP", reportSB);
	reportSB.append("\t");

	reportNotSeenOff("Candidate", "CwC", reportSB);
	reportSB.append("\t");

	reportNotSeenOff("Candidate", "Pul", reportSB);
	reportSB.append("\n");

	// ----- total -----

	reportSB.append("\tTotal: \t\t");

	reportTotal("Candidate", "CwP", reportSB);
	reportSB.append("\t");

	reportTotal("Candidate", "CwC", reportSB);
	reportSB.append("\t");

	reportTotal("Candidate", "Pul", reportSB);
	reportSB.append("\n");

	reportSB.append("\n");

	// ---------------
	// ----- RFI -----
	// ---------------

	reportSB.append("RFI:\n");

	// ----- failed coherent -----

	reportSB.append("\tFailed Coherent Detection: \t");

	reportFailCoherent("RFI", "CwP", reportSB);
	reportSB.append("\t");

	reportFailCoherent("RFI", "CwC", reportSB);
	reportSB.append("\t");

	reportFailCoherent("RFI", "Pul", reportSB);
	reportSB.append("\n");

	// ----- RFI match -----

	reportSB.append("\tRecent Match: \t\t");

	reportRFIMatch("RFI", "CwP", reportSB);
	reportSB.append("\t");

	reportRFIMatch("RFI", "CwC", reportSB);
	reportSB.append("\t");

	reportRFIMatch("RFI", "Pul", reportSB);
	reportSB.append("\n");

	// ----- RFI scan -----

	reportSB.append("\tRFI Scan: \t\t");

	reportRFIScan("RFI", "CwP", reportSB);
	reportSB.append("\t");

	reportRFIScan("RFI", "CwC", reportSB);
	reportSB.append("\t");

	reportRFIScan("RFI", "Pul", reportSB);
	reportSB.append("\n");

	// ----- SNR to low -----

	reportSB.append("\tSNR to low: \t\t");

	reportSNRlow("RFI", "CwP", reportSB);
	reportSB.append("\t");

	reportSNRlow("RFI", "CwC", reportSB);
	reportSB.append("\t");

	reportSNRlow("RFI", "Pul", reportSB);
	reportSB.append("\n");

	// ----- SNR to high

	reportSB.append("\tSNR to high: \t\t");

	reportSNRhigh("RFI", "CwP", reportSB);
	reportSB.append("\t");

	reportSNRhigh("RFI", "CwC", reportSB);
	reportSB.append("\t");

	reportSNRhigh("RFI", "Pul", reportSB);
	reportSB.append("\n");

	// ----- zero drift -----

	reportSB.append("\tZero Drift: \t\t");

	reportZeroDrift("RFI", "CwP", reportSB);
	reportSB.append("\t");

	reportZeroDrift("RFI", "CwC", reportSB);
	reportSB.append("\t");

	reportZeroDrift("RFI", "Pul", reportSB);
	reportSB.append("\n");

	// ----- total -----

	reportSB.append("\tTotal: \t\t");

	reportTotal("RFI", "CwP", reportSB);
	reportSB.append("\t");

	reportTotal("RFI", "CwC", reportSB);
	reportSB.append("\t");

	reportTotal("RFI", "Pul", reportSB);
	reportSB.append("\n");


	reportSB.append("\n");

	// -------------------
	// ----- unknown -----
	// -------------------

	reportSB.append("Unknown:\n");

	// ----- passed power -----

	reportSB.append("\tPassed Power Threshold: \t");

	reportPassedPower("Unknown", "CwP", reportSB);
	reportSB.append("\t");

	reportPassedPower("Unknown", "CwC", reportSB);
	reportSB.append("\t");

	reportPassedPower("Unknown", "Pul", reportSB);
	reportSB.append("\n");

	// ----- zero drift -----

	reportSB.append("\tZero Drift: \t\t");

	reportZeroDrift("Unknown", "CwP", reportSB);
	reportSB.append("\t");

	reportZeroDrift("Unknown", "CwC", reportSB);
	reportSB.append("\t");

	reportZeroDrift("Unknown", "Pul", reportSB);
	reportSB.append("\n");

	// ----- total -----

	reportSB.append("\tTotal: \t\t");

	reportTotal("Unknown", "CwP", reportSB);
	reportSB.append("\t");

	reportTotal("Unknown", "CwC", reportSB);
	reportSB.append("\t");

	reportTotal("Unknown", "Pul", reportSB);
	reportSB.append("\n");

	// ----------------
	// ----- test -----
	// ----------------

	reportSB.append("Test:\n");

	// ----- total -----

	reportSB.append("\tTotal: \t\t");

	reportTotal("Test", "CwP", reportSB);
	reportSB.append("\t");

	reportTotal("Test", "CwC", reportSB);
	reportSB.append("\t");

	reportTotal("Test", "Pul", reportSB);
	reportSB.append("\n");

	// --------------------------
	// ----- display report -----
	// --------------------------

	summaryReportTA.setText(reportSB.toString());

    }

    // -------------------
    // ----- reports -----
    // -------------------

    void reportActivityDate(StringBuffer reportSB, String activityId) {

        try {

	    String    dbquery;
	    ResultSet rs;

	    dbquery = new String("SELECT startOfDataCollection " +
				 "from Activities " +
				 "where " +
				 " id = " + activityId +
				 ";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getString(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportActivityFrequencies(StringBuffer reportSB) {

        try {

	    String    dbquery;
	    ResultSet rs;

	    dbquery = new String("SELECT min(rfFreq), max(rfFreq) " +
				 "from SignalDescription " +
				 "where " +

				 " activityId >= " +
				 startActivityTF.getText() +

				 " and activityId <= " + 
				 stopActivityTF.getText() +

				 ";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getString(1));
	    reportSB.append(" MHz \t");
	    reportSB.append(rs.getString(2));
	    reportSB.append(" MHz");

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportTotal(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportConfirmed(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and " + 
			   "(SignalDescription.reason = \'MAIN_CONFIRM\' " +
			   " or " + 
			   " SignalDescription.reason = \'REMOTE_CONFIRM\') ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));


	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportNotConfirmed(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and " + 
			   "(SignalDescription.reason = \'NO_MAIN_CONFIRM\' " +
			   " or " + 
			   " SignalDescription.reason = \'NO_REMOTE_CONFIRM\') ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));


	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportPassedPower(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and " + 
			   "(SignalDescription.reason = \'PASSED_MAIN_POWER_THRESH\' " +
			   " or " + 
			   " SignalDescription.reason = \'PASSED_REMOTE_POWER_THRESH\') ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportSeenOff(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and SignalDescription.reason = \'SEEN_OFF\' ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportNotSeenOff(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and SignalDescription.reason = \'NOT_SEEN_OFF\' ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportZeroDrift(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and SignalDescription.reason = \'ZERO_DRIFT\' ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportFailCoherent(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and SignalDescription.reason = \'FAILED_MAIN_COHERENT_DETECT\' ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportRFIScan(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and SignalDescription.reason = \'RFI_SCAN\' ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportSNRlow(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and SignalDescription.reason = \'REMOTE_SNR_TOO_LOW\' ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportSNRhigh(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and SignalDescription.reason = \'REMOTE_SNR_TOO_HIGH\' ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    void reportRFIMatch(String sigClass, String type, StringBuffer reportSB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT count(SignalDescription.id) " +
			   "from Activities, SignalDescription " +
			   "where ");

	    dbquery.append(" SignalDescription.type = \'" + type + "\' ");

	    dbquery.append(" and SignalDescription.sigClass = \'" + sigClass + "\' ");

	    dbquery.append(" and (SignalDescription.reason = \'MAIN_RECENT_RFI_MATCH\' " +
			   " or SignalDescription.reason = \'REMOTE_RECENT_RFI_MATCH\') ");

            if (mainRB.isSelected()) {
		dbquery.append(" and ( SignalDescription.pdmMode = \'main\'" +
			       " or " +
			       "SignalDescription.pdmMode = \'main_only\') ");
	    }
	    
            if (remoteRB.isSelected()) {
		dbquery.append(" and SignalDescription.pdmMode = \'remote\' ");
	    }

	    dbquery.append(" and Activities.id = " + 
			   " SignalDescription.activityId ");

	    dbquery.append(" and Activities.id >= " +
			   startActivityTF.getText() +
			   " and Activities.id <= " + 
			   stopActivityTF.getText() +
			   " ");

	    if (whereTF.getText().length() > 0) {
		dbquery.append(" and " + whereTF.getText());
	    }

	    dbquery.append(";");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next();  // only one, yada, yada, yada
	    reportSB.append(rs.getInt(1));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(asFrame, 
					  "SQLException: " + e.getMessage() +
					  "\n" + 
					  "SQLState:     " + e.getSQLState() +
					  "\n" +
					  "VendorError:  " + e.getErrorCode(),
					  "SQL Error",
					  JOptionPane.ERROR_MESSAGE
					  );
        }

	catch (Exception e) {
            e.printStackTrace();
        }

    }

    // -----------------
    // ----- Misc. -----
    // -----------------

    public void setFirst () {

        try {

            String dbquery = new String("SELECT min(id) " +
                                        "from Activities " +
                                        ";");
            
            
            ResultSet rs = dbcd.executeQuery(dbquery.toString());
            
            if (!rs.next()) {
                JOptionPane.showMessageDialog(asFrame, 
                                              "Empty result for min(id)? ");
		rs.close();
		return;

            } else {

		startActivityTF.setText(rs.getString(1));

            }
            
            // There can be only one.
            if (rs.next()) {
                JOptionPane.showMessageDialog(asFrame, 
                                              "Multiple results found for " +
                                              "min(id)?");
		rs.close();
		return;
            }

            rs.close();

            
        }

        catch (SQLException e) {
            JOptionPane.showMessageDialog(asFrame, 
                                          "SQLException: " + e.getMessage() +
                                          "\n" + 
                                          "SQLState:     " + e.getSQLState() +
                                          "\n" +
                                          "VendorError:  " + e.getErrorCode(),
                                          "SQL Error",
                                          JOptionPane.ERROR_MESSAGE
                                          );
        }

        catch (Exception e) {
            JOptionPane.showMessageDialog(asFrame, "Unexpected IO Error: " 
                                          + e);
        }




    }


    public void setLast () {

        try {

            String dbquery = new String("SELECT max(id) " +
                                        "from Activities " +
                                        ";");
            
            
            ResultSet rs = dbcd.executeQuery(dbquery.toString());
            
            if (!rs.next()) {
                JOptionPane.showMessageDialog(asFrame, 
                                              "Empty result for max(id)?");
		rs.close();
		return;

            } else {

		stopActivityTF.setText(rs.getString(1));

            }
            
            // There can be only one.
            if (rs.next()) {
                JOptionPane.showMessageDialog(asFrame, 
                                              "Multiple results found for " +
                                              "max(id)?");
		rs.close();
		return;
            }

            rs.close();

            
        }

        catch (SQLException e) {
            JOptionPane.showMessageDialog(asFrame, 
                                          "SQLException: " + e.getMessage() +
                                          "\n" + 
                                          "SQLState:     " + e.getSQLState() +
                                          "\n" +
                                          "VendorError:  " + e.getErrorCode(),
                                          "SQL Error",
                                          JOptionPane.ERROR_MESSAGE
                                          );
        }

        catch (Exception e) {
            JOptionPane.showMessageDialog(asFrame, "Unexpected IO Error: " 
                                          + e);
        }




    }

    public void initRecord() {
        setFirst();
        setLast();
    }

    public JLabel getHost() {
        return(hostValue);
    }

    public JLabel getDBName() {
        return(dbnameValue);
    }

    public static void main(String[] args) {

	try {
	    Class.forName("com.mysql.jdbc.Driver").newInstance();
	}
	catch (Exception e) {
            e.printStackTrace();
        }


        DBConnectionDialog dbcd = new DBConnectionDialog();

	ActivitySummary activitySummary =  new ActivitySummary(dbcd);

	// set default vaules



    }

}
