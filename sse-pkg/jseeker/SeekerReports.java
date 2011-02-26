/*******************************************************************************

 File:    SeekerReports.java
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
// Filename:    SeekerReports.java
// Description: generates activity summary
//
// Authors:     L.R. McFarland
// Language:    java
//
// Created:     26 Mar 2003
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

class SRPreferencesDialog {

    JFrame     srpdFrame;

    JTextField toleranceTF;
    JTextField maskwidthTF;

    JTextField birdieThresholdTF;
    JTextField birdieToleranceTF;

    JTextField minimumSeenTF;
    JTextField percentSeenTF;

    JTextField starIdTF;

    JTextField extraWhereTF;

    // ----- last query -----

    JTextArea   lastQueryTA;



    public void setVisible (boolean visible) {
        srpdFrame.setVisible(visible);
    }

    public SRPreferencesDialog () {

	srpdFrame = new JFrame("Seeker Report Preferences");

        Container cp = srpdFrame.getContentPane();

        GridBagLayout srpdPanelGB  = new GridBagLayout();
        cp.setLayout(srpdPanelGB);

        GridBagConstraints srpdPanelGBC = new GridBagConstraints();
        srpdPanelGBC.fill      = GridBagConstraints.HORIZONTAL;
        srpdPanelGBC.insets    = new Insets(2,2,2,2);

	// --------------------------------------------
        // ----- seeker reports preferences panel -----
        // --------------------------------------------

        final int tfWidth = 8;

        // ----- general -----

        JLabel generalLabel = new JLabel("General");
        srpdPanelGBC.gridx = 0;
        srpdPanelGBC.gridy = 1;
        srpdPanelGB.setConstraints(generalLabel, srpdPanelGBC);
        cp.add(generalLabel);

        JLabel toleranceLabel = new JLabel("Tolerance:");
        srpdPanelGBC.gridx = 1;
        srpdPanelGBC.gridy = 2;
        srpdPanelGB.setConstraints(toleranceLabel, srpdPanelGBC);
        cp.add(toleranceLabel);

        toleranceTF = new JTextField(tfWidth);
        srpdPanelGBC.gridx = 2;
        srpdPanelGBC.gridy = 2;
        srpdPanelGB.setConstraints(toleranceTF, srpdPanelGBC);
        cp.add(toleranceTF);
        toleranceTF.setText("3");

        JLabel toleranceUnitsLabel = new JLabel("MHz (a.k.a. precision)");
        srpdPanelGBC.gridx = 3;
        srpdPanelGBC.gridy = 2;
        srpdPanelGB.setConstraints(toleranceUnitsLabel, srpdPanelGBC);
        cp.add(toleranceUnitsLabel);

        JLabel maskwidthLabel = new JLabel("Bandwidth");
        srpdPanelGBC.gridx = 1;
        srpdPanelGBC.gridy = 3;
        srpdPanelGB.setConstraints(maskwidthLabel, srpdPanelGBC);
        cp.add(maskwidthLabel);

        maskwidthTF = new JTextField(tfWidth);
        srpdPanelGBC.gridx = 2;
        srpdPanelGBC.gridy = 3;
        srpdPanelGB.setConstraints(maskwidthTF, srpdPanelGBC);
        cp.add(maskwidthTF);
        maskwidthTF.setText("0.000643");

        JLabel bandwidthUnitsLabel = new JLabel("MHz");
        srpdPanelGBC.gridx = 3;
        srpdPanelGBC.gridy = 3;
        srpdPanelGB.setConstraints(bandwidthUnitsLabel, srpdPanelGBC);
        cp.add(bandwidthUnitsLabel);

        // ----- birdie preferences -----

        JLabel birdieMaskLabel = new JLabel("Birdie Mask:");
        srpdPanelGBC.gridx = 0;
        srpdPanelGBC.gridy = 4;
        srpdPanelGB.setConstraints(birdieMaskLabel, srpdPanelGBC);
        cp.add(birdieMaskLabel);

        JLabel birdieThresholdLabel = new JLabel("Threshold");
        srpdPanelGBC.gridx = 1;
        srpdPanelGBC.gridy = 5;
        srpdPanelGB.setConstraints(birdieThresholdLabel, srpdPanelGBC);
        cp.add(birdieThresholdLabel);

        birdieThresholdTF = new JTextField(tfWidth);
        srpdPanelGBC.gridx = 2;
        srpdPanelGBC.gridy = 5;
        srpdPanelGB.setConstraints(birdieThresholdTF, srpdPanelGBC);
        cp.add(birdieThresholdTF);
        birdieThresholdTF.setText("30");

        JLabel thresholdUnitsLabel = new JLabel("counts");
        srpdPanelGBC.gridx = 3;
        srpdPanelGBC.gridy = 5;
        srpdPanelGB.setConstraints(thresholdUnitsLabel, srpdPanelGBC);
        cp.add(thresholdUnitsLabel);

        // ----- permanent rfi preferences -----

        JLabel permanentRFILabel = new JLabel("Permanent RFI Mask:");
        srpdPanelGBC.gridx = 0;
        srpdPanelGBC.gridy = 6;
        srpdPanelGB.setConstraints(permanentRFILabel, srpdPanelGBC);
        cp.add(permanentRFILabel);

        JLabel minimumSeenLabel = new JLabel("Minimum Seen");
        srpdPanelGBC.gridx = 1;
        srpdPanelGBC.gridy = 7;
        srpdPanelGB.setConstraints(minimumSeenLabel, srpdPanelGBC);
        cp.add(minimumSeenLabel);

        minimumSeenTF = new JTextField(tfWidth);
        srpdPanelGBC.gridx = 2;
        srpdPanelGBC.gridy = 7;
        srpdPanelGB.setConstraints(minimumSeenTF, srpdPanelGBC);
        cp.add(minimumSeenTF);
        minimumSeenTF.setText("10");

        JLabel minimumSeenUnitsLabel = new JLabel("counts");
        srpdPanelGBC.gridx = 3;
        srpdPanelGBC.gridy = 7;
        srpdPanelGB.setConstraints(minimumSeenUnitsLabel, srpdPanelGBC);
        cp.add(minimumSeenUnitsLabel);

        JLabel percentSeenLabel = new JLabel("Percent Seen");
        srpdPanelGBC.gridx = 1;
        srpdPanelGBC.gridy = 8;
        srpdPanelGB.setConstraints(percentSeenLabel, srpdPanelGBC);
        cp.add(percentSeenLabel);

        percentSeenTF = new JTextField(tfWidth);
        srpdPanelGBC.gridx = 2;
        srpdPanelGBC.gridy = 8;
        srpdPanelGB.setConstraints(percentSeenTF, srpdPanelGBC);
        cp.add(percentSeenTF);
        percentSeenTF.setText("75");

        JLabel percentSeenUnitsLabel = new JLabel("%");
        srpdPanelGBC.gridx = 3;
        srpdPanelGBC.gridy = 8;
        srpdPanelGB.setConstraints(percentSeenUnitsLabel, srpdPanelGBC);
        cp.add(percentSeenUnitsLabel);

        // ----- rfi preferences -----

        JLabel recentRFILabel = new JLabel("Recent RFI Mask:");
        srpdPanelGBC.gridx = 0;
        srpdPanelGBC.gridy = 11;
        srpdPanelGB.setConstraints(recentRFILabel, srpdPanelGBC);
        cp.add(recentRFILabel);

        JLabel starIdLabel = new JLabel("Star ID");
        srpdPanelGBC.gridx = 1;
        srpdPanelGBC.gridy = 12;
        srpdPanelGB.setConstraints(starIdLabel, srpdPanelGBC);
        cp.add(starIdLabel);

        starIdTF = new JTextField(tfWidth);
        srpdPanelGBC.gridx = 2;
        srpdPanelGBC.gridy = 12;
        srpdPanelGB.setConstraints(starIdTF, srpdPanelGBC);
        cp.add(starIdTF);
        starIdTF.setText("0");


        // ----- extra where -----

        JLabel extraWhereLabel = new JLabel("Extra Where Clause:");
	srpdPanelGBC.gridwidth = 2;
        srpdPanelGBC.gridx = 0;
        srpdPanelGBC.gridy = 20;
        srpdPanelGB.setConstraints(extraWhereLabel, srpdPanelGBC);
        cp.add(extraWhereLabel);
	srpdPanelGBC.gridwidth = 1; // restore default

        srpdPanelGBC.gridwidth = 4;
        extraWhereTF = new JTextField(tfWidth*4);
        srpdPanelGBC.gridx = 0;
        srpdPanelGBC.gridy = 21;
        srpdPanelGB.setConstraints(extraWhereTF, srpdPanelGBC);

        cp.add(extraWhereTF);
	srpdPanelGBC.gridwidth = 1; // restore default

        extraWhereTF.setText("");

	// ----------------------------
	// ----- last query panel -----
	// ----------------------------

	lastQueryTA             = new JTextArea();
	JScrollPane lastQuerySP = new JScrollPane(lastQueryTA);

	lastQuerySP.setPreferredSize(new Dimension(350, 150));
	lastQuerySP.setBorder(BorderFactory.createTitledBorder(
				 BorderFactory.createLoweredBevelBorder(), 
				 "Last Query"));

	srpdPanelGBC.gridwidth = 4;
	
	srpdPanelGBC.gridx = 0;
	srpdPanelGBC.gridy = 30;
	srpdPanelGB.setConstraints(lastQuerySP, srpdPanelGBC);
	cp.add(lastQuerySP);

	srpdPanelGBC.gridwidth = 1; // restore default


        // -------------------------
        // ----- control panel -----
        // -------------------------

        // ----- close button -----

        JButton closeBT = new JButton("Close");
        closeBT.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    srpdFrame.setVisible(false);
                }
            });


        srpdPanelGBC.gridwidth = 4;
        srpdPanelGBC.gridx = 0;
        srpdPanelGBC.gridy = 40;
        srpdPanelGB.setConstraints(closeBT, srpdPanelGBC);
        cp.add(closeBT);
        srpdPanelGBC.gridwidth = 1; // restore default

        srpdFrame.pack();

	srpdFrame.addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    System.exit(0);
                }
            });


    }


}


public class SeekerReports implements DBApplicationInterface {

    JFrame              srFrame;
    DBConnectionDialog  dbcd;
    SRPreferencesDialog srpd;


    StringBuffer        saveAsSB;
    String              saveAsFilename;

    JLabel              hostValue;
    JLabel              dbnameValue;

    String              reportType = new String("Activity Summary");


    // ----- parameters -----

    JTextField    startActivityTF;
    JTextField    stopActivityTF;

    JTextField    startTimeTF;
    JTextField    stopTimeTF;

    JTextField    minFreqTF;
    JTextField    maxFreqTF;



    // ----- report area -----

    JTextArea   summaryReportTA;
  
  
    // -------------------
    // ----- methods -----
    // -------------------

    public SeekerReports (DBConnectionDialog dbConnectionDialog,
			  SRPreferencesDialog srPreferenceDialog) {

        dbcd = dbConnectionDialog;
        dbcd.setDBAI(this);

	srpd = srPreferenceDialog;

        srFrame = new JFrame("Seeker Reports");

        Container cp = srFrame.getContentPane();

        saveAsSB  = new StringBuffer("IOU 1 print method.");

        final int tfWidth = 12;

        // ----- grid bag -----

	GridBagLayout      srPanelGB  = new GridBagLayout();
	cp.setLayout(srPanelGB);

	GridBagConstraints srPanelGBC = new GridBagConstraints();
	srPanelGBC.fill      = GridBagConstraints.HORIZONTAL;
	srPanelGBC.insets    = new Insets(2, 4, 2, 4);

	srPanelGBC.weightx = 0.1; // TBD needed on suns but not PCs?

        // ---------------------
        // ----- file menu -----
        // ---------------------

        String[]           fileCtrls = {"File", "Open", "Preferences", 
					"Save As", "Exit"};
        JComboBox          fileCB    = new JComboBox(fileCtrls);

        final JFileChooser saveAsFileChooser = new JFileChooser();

        fileCB.addActionListener(new ActionListener() {

                public void actionPerformed(ActionEvent e) {

                    JComboBox cb = (JComboBox) e.getSource();

                    if (cb.getSelectedItem() == "Open") {
                        dbcd.setVisible(true);
                    }

                    if (cb.getSelectedItem() == "Preferences") {
                        srpd.setVisible(true);
                    }

                    if (cb.getSelectedItem() == "Save As") {

                        int returnVal =
                            saveAsFileChooser.showDialog(srFrame, "Save");

                        if (returnVal == JFileChooser.APPROVE_OPTION) {
                            saveAsFilename =
                                saveAsFileChooser.getSelectedFile().getPath();
                            saveAs(saveAsFilename);
                        }

                    }


                    if (cb.getSelectedItem() == "Exit") {
                        System.exit(0);
                    }

                    cb.setSelectedIndex(0); // restore index

                }
            });


        srPanelGBC.gridx = 0;
        srPanelGBC.gridy = 0;
        srPanelGB.setConstraints(fileCB, srPanelGBC);
        cp.add(fileCB);


        // -----------------------
        // ----- report menu -----
        // -----------------------

        String[]  reportCtrls = {"Activity Summary",
		       		 "Birdie Mask, Main",
				 "Birdie Mask, Remote",
				 "Observation Summary",
				 "Permanent RFI Mask, Main",
				 "Permanent RFI Mask, Remote",
				 "Recent RFI Mask, Main",
				 "Recent RFI Mask, Remote",
				 "Signal Summary"};

        JComboBox reportCB    = new JComboBox(reportCtrls);

        reportCB.addActionListener(new ActionListener() {

                public void actionPerformed(ActionEvent e) {
                    JComboBox cb = (JComboBox) e.getSource();
		    reportType = new String(cb.getSelectedItem().toString());
                }
            });


	reportCB.setSelectedIndex(0);

        srPanelGBC.gridx = 3;
        srPanelGBC.gridy = 0;
        srPanelGB.setConstraints(reportCB, srPanelGBC);
        cp.add(reportCB);


	// ----------------------------------
	// ----- query parameters panel -----
	// ----------------------------------

	// ----- spacer -----
	JLabel spacer1 = new JLabel(" ");
        srPanelGBC.gridx = 0;
        srPanelGBC.gridy = 1;
        srPanelGB.setConstraints(spacer1, srPanelGBC);
        cp.add(spacer1);

        // ----- host -----
        JLabel hostLabel = new JLabel("Host:");
        srPanelGBC.gridx = 0;
        srPanelGBC.gridy = 2;
        srPanelGB.setConstraints(hostLabel, srPanelGBC);
        cp.add(hostLabel);

        hostValue = new JLabel("TBD");
        srPanelGBC.gridx = 1;
        srPanelGBC.gridy = 2;
        srPanelGB.setConstraints(hostValue, srPanelGBC);
        cp.add(hostValue);
        hostValue.setForeground(Color.red);

        // ----- dbname -----
        JLabel dbnameLabel = new JLabel("Database:");
        srPanelGBC.gridx = 2;
        srPanelGBC.gridy = 2;
        srPanelGB.setConstraints(dbnameLabel, srPanelGBC);
        cp.add(dbnameLabel);

        dbnameValue = new JLabel("TBD");
        srPanelGBC.gridx = 3;
        srPanelGBC.gridy = 2;
        srPanelGB.setConstraints(dbnameValue, srPanelGBC);
        cp.add(dbnameValue);
        dbnameValue.setForeground(Color.red);


	// ----- start/stop activity -----

	JLabel startActivityLabel = new JLabel("Start Activity:");
        srPanelGBC.gridx = 0;
        srPanelGBC.gridy = 3;
        srPanelGB.setConstraints(startActivityLabel, srPanelGBC);
        cp.add(startActivityLabel);

	startActivityTF = new JTextField(tfWidth);
        srPanelGBC.gridx = 1;
        srPanelGBC.gridy = 3;
        srPanelGB.setConstraints(startActivityTF, srPanelGBC);
        cp.add(startActivityTF);
        startActivityTF.setForeground(Color.blue);

	JLabel stopActivityLabel = new JLabel("Stop Activity:");
        srPanelGBC.gridx = 2;
        srPanelGBC.gridy = 3;
        srPanelGB.setConstraints(stopActivityLabel, srPanelGBC);
        cp.add(stopActivityLabel);

	stopActivityTF = new JTextField(tfWidth);
        srPanelGBC.gridx = 3;
        srPanelGBC.gridy = 3;
        srPanelGB.setConstraints(stopActivityTF, srPanelGBC);
        cp.add(stopActivityTF);
        stopActivityTF.setForeground(Color.blue);

	// ----- start/stop date -----

	JLabel startTimeLabel = new JLabel("Start Date-Time:");
        srPanelGBC.gridx = 0;
        srPanelGBC.gridy = 4;
        srPanelGB.setConstraints(startTimeLabel, srPanelGBC);
        cp.add(startTimeLabel);

	startTimeTF = new JTextField(tfWidth);
        srPanelGBC.gridx = 1;
        srPanelGBC.gridy = 4;
        srPanelGB.setConstraints(startTimeTF, srPanelGBC);
        cp.add(startTimeTF);
        startTimeTF.setForeground(Color.blue);

	JLabel stopTimeLabel = new JLabel("Stop Date-Time:");
        srPanelGBC.gridx = 2;
        srPanelGBC.gridy = 4;
        srPanelGB.setConstraints(stopTimeLabel, srPanelGBC);
        cp.add(stopTimeLabel);

	stopTimeTF = new JTextField(tfWidth);
        srPanelGBC.gridx = 3;
        srPanelGBC.gridy = 4;
        srPanelGB.setConstraints(stopTimeTF, srPanelGBC);
        cp.add(stopTimeTF);
        stopTimeTF.setForeground(Color.blue);


	// ----- min/max frequency -----

	JLabel minFreqLabel = new JLabel("Min. Freq. (MHz):");
        srPanelGBC.gridx = 0;
        srPanelGBC.gridy = 5;
        srPanelGB.setConstraints(minFreqLabel, srPanelGBC);
        cp.add(minFreqLabel);

	minFreqTF = new JTextField(tfWidth);
        srPanelGBC.gridx = 1;
        srPanelGBC.gridy = 5;
        srPanelGB.setConstraints(minFreqTF, srPanelGBC);
        cp.add(minFreqTF);
        minFreqTF.setForeground(Color.blue);

	JLabel maxFreqLabel = new JLabel("Max. Freq. (MHz):");
        srPanelGBC.gridx = 2;
        srPanelGBC.gridy = 5;
        srPanelGB.setConstraints(maxFreqLabel, srPanelGBC);
        cp.add(maxFreqLabel);

	maxFreqTF = new JTextField(tfWidth);
        srPanelGBC.gridx = 3;
        srPanelGBC.gridy = 5;
        srPanelGB.setConstraints(maxFreqTF, srPanelGBC);
        cp.add(maxFreqTF);
        maxFreqTF.setForeground(Color.blue);

	// ----- spacer -----
	JLabel spacer2 = new JLabel(" ");
        srPanelGBC.gridx = 0;
        srPanelGBC.gridy = 6;
        srPanelGB.setConstraints(spacer2, srPanelGBC);
        cp.add(spacer2);


	// ------------------------
	// ----- report panel -----
	// ------------------------
	
	summaryReportTA             = new JTextArea();
	JScrollPane summaryReportSP = new JScrollPane(summaryReportTA);

	summaryReportSP.setPreferredSize(new Dimension(550, 400));
	summaryReportSP.setBorder(BorderFactory.createTitledBorder(
				 BorderFactory.createLoweredBevelBorder(), 
				 "Report"));

	srPanelGBC.gridwidth = 4;
	
	srPanelGBC.gridx = 0;
	srPanelGBC.gridy = 9;
	srPanelGB.setConstraints(summaryReportSP, srPanelGBC);
	cp.add(summaryReportSP);

	srPanelGBC.gridwidth = 1; // restore default

	// -------------------------
	// ----- control panel -----
	// -------------------------

	// ----- update button -----

	JButton updateBT = new JButton("Update");
	updateBT.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    updateReport();
		}
	    });

	srPanelGBC.gridwidth = 4; // restore default
	srPanelGBC.gridx = 0;
	srPanelGBC.gridy = 10;
	srPanelGB.setConstraints(updateBT, srPanelGBC);
	cp.add(updateBT);
	srPanelGBC.gridwidth = 1; // restore default


        // ----- add to frame -----

        srFrame.pack();

        srFrame.setVisible(true);

        srFrame.addWindowListener(new WindowAdapter() {
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
	    JOptionPane.showMessageDialog(srFrame, "Unexpected IO Error: " 
					  + e);
	}

    }  

    public static void main(String[] args) {

	try {
	    Class.forName("com.mysql.jdbc.Driver").newInstance();
	}
	catch (Exception e) {
            e.printStackTrace();
        }


        DBConnectionDialog  dbcd = new DBConnectionDialog();
	SRPreferencesDialog srpd = new SRPreferencesDialog();

	SeekerReports seekerReports =  new SeekerReports(dbcd, srpd);

	// set default vaules


    }

    // ----- DB Application Interface -----

    public void setFirst () {

        try {

            String dbquery = new String("SELECT min(id) " +
                                        "from Activities " +
                                        ";");
            
            
            ResultSet rs = dbcd.executeQuery(dbquery.toString());
            
            if (!rs.next()) {
                JOptionPane.showMessageDialog(srFrame, 
                                              "Empty result for min(id)? ");
		rs.close();
		return;

            } else {

		startActivityTF.setText(rs.getString(1));

            }
            
            // There can be only one.
            if (rs.next()) {
                JOptionPane.showMessageDialog(srFrame, 
                                              "Multiple results found for " +
                                              "min(id)?");
		rs.close();
		return;
            }

            rs.close();

            
        }

        catch (SQLException e) {
            JOptionPane.showMessageDialog(srFrame, 
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
            JOptionPane.showMessageDialog(srFrame, "Unexpected IO Error: " 
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
                JOptionPane.showMessageDialog(srFrame, 
                                              "Empty result for max(id)?");
		rs.close();
		return;

            } else {
		stopActivityTF.setText(rs.getString(1));
            }
            
            // There can be only one.
            if (rs.next()) {
                JOptionPane.showMessageDialog(srFrame, 
                                              "Multiple results found for " +
                                              "max(id)?");
		rs.close();
		return;
            }
            rs.close();
        }

        catch (SQLException e) {
            JOptionPane.showMessageDialog(srFrame, 
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
            JOptionPane.showMessageDialog(srFrame, "Unexpected IO Error: " 
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

    public boolean appendWhereClauseToActivityQuery (StringBuffer dbquery) {

	StringBuffer temp = new StringBuffer(" where ");
	boolean empty = true;

	if (startActivityTF.getText().length() > 0) {
	    if(!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" id >= " + startActivityTF.getText());
	}

	if (stopActivityTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" id <= " + stopActivityTF.getText());
	}

	if (startTimeTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" startOfDataCollection >= \'" + 
			startTimeTF.getText() + "\' ");
	}

	if (stopTimeTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" startOfDataCollection <= \'" + 
			stopTimeTF.getText() + "\' ");
	}

	if (minFreqTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" mainRfcSkyFrequency >= " + minFreqTF.getText());
	}

	if (maxFreqTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" mainRfcSkyFrequency <= " + maxFreqTF.getText());
	}

	if (srpd.extraWhereTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(srpd.extraWhereTF.getText());
	}

	// copy over
	if (!empty) {
	    dbquery.append(temp);
	}

	return(empty);
    }

    public boolean appendWhereClauseToSignalQuery (StringBuffer dbquery) {

	StringBuffer temp = new StringBuffer(" where ");
	boolean empty = true;

	if (startActivityTF.getText().length() > 0) {
	    if(!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" activityId >= " + startActivityTF.getText());
	}

	if (stopActivityTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" activityId <= " + stopActivityTF.getText());
	}

	if (startTimeTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" activityStartTime >= \'" + 
			startTimeTF.getText() + "\' ");
	}

	if (stopTimeTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" activityStartTime <= \'" + 
			stopTimeTF.getText() + "\' ");
	}

	if (minFreqTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" rfFreq >= " + minFreqTF.getText());
	}

	if (maxFreqTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" rfFreq <= " + maxFreqTF.getText());
	}

	if (srpd.extraWhereTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(srpd.extraWhereTF.getText());
	}

	// copy over
	if (!empty) {
	    dbquery.append(temp);
	}

	return(empty);
    }

    public boolean appendWhereClauseToActivitySignalJoin (StringBuffer dbquery) {

	StringBuffer temp = new StringBuffer(" where ");
	boolean empty = true;

	if (startActivityTF.getText().length() > 0) {
	    if(!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" Activities.id >= " + startActivityTF.getText());
	}

	if (stopActivityTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" Activities.id <= " + stopActivityTF.getText());
	}

	if (startTimeTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" Activities.startOfDataCollection >= \'" + 
			startTimeTF.getText() + "\' ");
	}

	if (stopTimeTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" Activities.startOfDataCollection <= \'" + 
			stopTimeTF.getText() + "\' ");
	}

	if (minFreqTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" SignalDescription.rfFreq >= " + 
			minFreqTF.getText());
	}

	if (maxFreqTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(" SignalDescription.rfFreq <= " + 
			maxFreqTF.getText());
	}

	if (srpd.extraWhereTF.getText().length() > 0) {
	    if (!empty) {
		temp.append(" and ");
	    } else {
		empty = false;
	    }
	    temp.append(srpd.extraWhereTF.getText());
	}

	// copy over
	if (!empty) {
	    dbquery.append(temp);
	}

	return(empty);
    }

    // ===================
    // ----- reports -----
    // ===================

    public void updateReport () {

	if (reportType.compareTo("Activity Summary") == 0) {
	    activitySummaryReport();
	}

	if (reportType.compareTo("Birdie Mask, Main") == 0) {
	    birdieMaskReport(true);
	}

	if (reportType.compareTo("Birdie Mask, Remote") == 0) {
	    birdieMaskReport(false);
	}

	if (reportType.compareTo("Observation Summary") == 0) {
	    observationSummaryReport(true);
	}

	if (reportType.compareTo("Permanent RFI Mask, Main") == 0) {
	    permanentRFIReport(true);
	}

	if (reportType.compareTo("Permanent RFI Mask, Remote") == 0) {
	    permanentRFIReport(false);
	}

	if (reportType.compareTo("Recent RFI Mask, Main") == 0) {
	    recentRFIReport(true);
	}

	if (reportType.compareTo("Recent RFI Mask, Remote") == 0) {
	    recentRFIReport(false);
	}

	if (reportType.compareTo("Signal Summary") == 0) {
	    signalSummaryReport();
	}

    }

    // ---------------------------------
    // ----- Signal Summary Report -----
    // ---------------------------------

    public void activitySummaryReport () {

	summaryReportTA.setText(""); // clear text
	StringBuffer reportSB    = new StringBuffer();

	srpd.lastQueryTA.setText(""); // clear text
	StringBuffer lastQuerySB = new StringBuffer();

	reportSB.append("Host:\t" + dbcd.hostnameTF.getText());
	reportSB.append("\n");

	reportSB.append("Database:\t" + dbcd.dbnameTF.getText());
	reportSB.append("\n");

	reportSB.append("Activity Range:\t");
	reportActivities(reportSB, lastQuerySB);
	reportSB.append("\n");

	reportSB.append("Date Range:\t");
	reportDates(reportSB, lastQuerySB);
	reportSB.append("\n");

	// TBD use main rfc sky frequency

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT " +
			   "distinct type, " +
			   "count(*) as count " +
			   "from Activities ");
	    appendWhereClauseToActivityQuery(dbquery);

	    dbquery.append(" group by type ");
	    dbquery.append(" order by type ");

	    dbquery.append(";");

	    lastQuerySB.append(dbquery.toString() + "\n");

	    rs = dbcd.executeQuery(dbquery.toString());

	    reportSB.append("\n");
	    reportSB.append("Activity\tCount");
	    reportSB.append("\n");

	    while (rs.next()) {
		reportSB.append(rs.getString("type")  + "\t" + 
				rs.getString("count") +	"\n");
	    }

	    rs.close();

	    reportSB.append("\n");

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(srFrame, 
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


	// --------------------------
	// ----- display report -----
	// --------------------------

	srpd.lastQueryTA.setText(lastQuerySB.toString());
	summaryReportTA.setText(reportSB.toString());

    }


    void reportActivities(StringBuffer reportSB, StringBuffer lastQuerySB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT min(Activities.id), " +
			   "max(Activities.id) " +
			   "from Activities ");
	    appendWhereClauseToActivityQuery(dbquery);
	    dbquery.append(";");

	    lastQuerySB.append(dbquery.toString() + "\n");

	    rs = dbcd.executeQuery(dbquery.toString());
	    rs.next(); 
	    reportSB.append(rs.getString(1));
	    reportSB.append("\t");
	    reportSB.append(rs.getString(2));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(srFrame, 
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

    void reportDates(StringBuffer reportSB, StringBuffer lastQuerySB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT min(startOfDataCollection), " +
			   "max(startOfDataCollection) " +
			   "from Activities ");
	    appendWhereClauseToActivityQuery(dbquery);
	    dbquery.append(";");

	    lastQuerySB.append(dbquery.toString() + "\n");

	    rs = dbcd.executeQuery(dbquery.toString());
	    rs.next(); 
	    reportSB.append(rs.getString(1));
	    reportSB.append("\t");
	    reportSB.append(rs.getString(2));

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(srFrame, 
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

    // ------------------------------
    // ----- Birdie Mask Report -----
    // ------------------------------


    public void birdieMaskReport (boolean isMain) {


	// TBD main/remote


	summaryReportTA.setText(""); // clear text
	StringBuffer reportSB = new StringBuffer();

	srpd.lastQueryTA.setText("");
	StringBuffer lastQuerySB = new StringBuffer();


        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT " +
			   "FORMAT(rfFreq - Activities.mainIfcSkyFrequency, " +
			   srpd.toleranceTF.getText() + ") " +
			   "as BirdieFreq, " +
			   "count(*) as count " +
			   "from Activities, SignalDescription ");
	    
	    appendWhereClauseToActivitySignalJoin(dbquery);

	    dbquery.append(" group by BirdieFreq having count > " +
			   srpd.birdieThresholdTF.getText());

	    dbquery.append(" order by (rfFreq - Activities.mainIfcSkyFrequency) ");

	    dbquery.append(";");

	    lastQuerySB.append(dbquery.toString());

	    rs = dbcd.executeQuery(dbquery.toString());

	    // ----- write mask -----

	    reportSB.append("# birdie mask\n");
	    reportSB.append("# host: " + hostValue.getText() + "\n");
	    reportSB.append("# database: " + dbnameValue.getText() + "\n");
	    reportSB.append("set bandcovered {0.0 40}\n");
	    reportSB.append("set masks {\n");

	    while (rs.next()) {
		reportSB.append("  " + rs.getDouble("BirdieFreq") + 
				"  " + srpd.maskwidthTF.getText() +
				"\n");
	    }

	    rs.close();

	    reportSB.append("}\n");

	    // --------------------------
	    // ----- display report -----
	    // --------------------------

	    srpd.lastQueryTA.setText(lastQuerySB.toString());
	    summaryReportTA.setText(reportSB.toString());

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(srFrame, 
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


    // --------------------------------------
    // ----- Observation Summary Report -----
    // --------------------------------------

    public void observationSummaryReport (boolean isMain) {

	// TBD main, remote, candidate

	summaryReportTA.setText(""); // clear text
	StringBuffer reportSB    = new StringBuffer();

	srpd.lastQueryTA.setText(""); // clear text
	StringBuffer lastQuerySB = new StringBuffer();

	reportSB.append("Host:\t" + dbcd.hostnameTF.getText());
	reportSB.append("\n");

	reportSB.append("Database:\t" + dbcd.dbnameTF.getText());
	reportSB.append("\n");

	reportSB.append("Activity Range:\t");
	reportActivities(reportSB, lastQuerySB);
	reportSB.append("\n");

	reportSB.append("Date Range:\t");
	reportDates(reportSB, lastQuerySB);
	reportSB.append("\n");

	// TBD use main rfc sky frequency

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT " +
			   "id, starid, " +
			   "FORMAT(mainRfcSkyFrequency, 1) as RfcFreq, " +
			   "startOfDataCollection, " +
			   "mainCwSignals + mainPulseSignals as Sig, " +
			   "allCwCandidates + allPulseCandidates as Cand, " +
			   "allCwCandidates + allPulseCandidates as MainC, " +
			   "confirmedCWCandidates + confirmedPulseCandidates as RmtC, " +
			   "type " + 
			   "from Activities ");

	    appendWhereClauseToActivityQuery(dbquery);

	    dbquery.append(" group by type ");
	    dbquery.append(" order by type ");

	    dbquery.append(";");

	    lastQuerySB.append(dbquery.toString() + "\n");

	    rs = dbcd.executeQuery(dbquery.toString());

	    reportSB.append("\n");
	    reportSB.append("Activity Id\tStar Id\tType\tFreq. (MHz)\t");
	    reportSB.append("Start\tSignals\tCandidates\t");
	    reportSB.append("Main Candidates\tRemote Candidates");
	    reportSB.append("\n");

	    while (rs.next()) {
		reportSB.append(rs.getString("id")      + "\t" + 
				rs.getString("starid")  + "\t" + 
				rs.getString("type")    + "\t" +
				rs.getString("RfcFreq") + "\t" + 
				rs.getString("startOfDataCollection") + "\t" + 
				rs.getString("Sig") + "\t" + 
				rs.getString("Cand") + "\t" + 
				rs.getString("MainC") + "\n");
	    }

	    rs.close();

	    reportSB.append("\n");

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(srFrame, 
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


	// --------------------------
	// ----- display report -----
	// --------------------------

	srpd.lastQueryTA.setText(lastQuerySB.toString());
	summaryReportTA.setText(reportSB.toString());

    }


    // --------------------------------
    // ----- Permanent RFI Report -----
    // --------------------------------


    class AAUFreqRecord {

        Double  rfFreq;
        Integer activityUnitId; // initial activity unit id
        Map     aauTree = new TreeMap(); // other activity, activity units tree

        public AAUFreqRecord (Integer actId, Integer actUnitId, 
                                   Double rf) {
            rfFreq         = rf;
            activityUnitId = actUnitId;
            aauTree.put(actId, actUnitId);
        }


        public void CountIfUnique (Integer actId, Integer actUnitId) {

            if (aauTree.containsKey(actId)) {
                Integer currentAU = (Integer) aauTree.get(actId);
                if (currentAU != actUnitId) {
                    aauTree.put(actId, actUnitId);
                }

            } else {
                aauTree.put(actId, actUnitId);
            }
        }

    }

    public void permanentRFIReport (boolean isMain) {


	// TBD main/remote


	summaryReportTA.setText(""); // clear text
	StringBuffer reportSB = new StringBuffer();

	srpd.lastQueryTA.setText("");
	StringBuffer lastQuerySB = new StringBuffer();

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT " +
			   "activityId, dbActivityUnitId, rfFreq " +
			   "from SignalDescription ");
	    
	    if (appendWhereClauseToSignalQuery(dbquery)) {
		dbquery.append(" where sigClass  = \'RFI\' ");
	    } else {	
		dbquery.append(" and sigClass  = \'RFI\' ");
	    }

	    dbquery.append(" order by rfFreq ");

	    dbquery.append(";");

	    lastQuerySB.append(dbquery.toString());

	    rs = dbcd.executeQuery(dbquery.toString());


            // TBD check empty result set
            
            // ----- tree the frequencies -----
            
            Map    maskTree  = new TreeMap();
            int    dPlaces   = Integer.parseInt(srpd.toleranceTF.getText());
            double tolFactor = Math.pow(10.0, dPlaces);

            while (rs.next()) {

                Double rfFreq    = new Double(rs.getDouble("rfFreq") * 
                                              tolFactor);
                int    roundFreq = Math.round(rfFreq.intValue());
                Double nextKey   = new Double(roundFreq / tolFactor);

                if (maskTree.containsKey(nextKey)) {
                    AAUFreqRecord currentValue = 
                        (AAUFreqRecord) maskTree.get(nextKey);
                    currentValue.CountIfUnique(
                      new Integer(rs.getInt("activityId")), 
                      new Integer(rs.getInt("dbActivityUnitId"))
                      );

                } else {
                    maskTree.put(nextKey, 
                      new AAUFreqRecord(
                        new Integer(rs.getInt("activityId")),
                        new Integer(rs.getInt("dbActivityUnitId")),
                        rfFreq));
                }

            }
            
            rs.close();

	    // ----- write mask -----

	    reportSB.append("# permanent RFI mask\n");
	    reportSB.append("# host: " + hostValue.getText() + "\n");
	    reportSB.append("# database: " + dbnameValue.getText() + "\n");
	    reportSB.append("Set perm masks {\n");


            for (Iterator it = maskTree.entrySet().iterator(); it.hasNext();) {

                Map.Entry me = (Map.Entry) it.next();

                Object ok = me.getKey();
                AAUFreqRecord ov = (AAUFreqRecord) me.getValue();

                if (ov.aauTree.size() > 
                    Integer.parseInt(srpd.minimumSeenTF.getText())) {

                    // ----- normailze to the activity units -----

                    // TBD or remote?

                    dbquery = new StringBuffer("SELECT mainPDMTuneFreq " +
					       "from ActivityUnits " +
					       "where id = " +
					       ov.activityUnitId.toString() +
					       ";");

                    rs = dbcd.executeQuery(dbquery.toString());

                    rs.next(); // there can be only one!

                    Double mainPDMTuneFreq = new Double(rs.getDouble(1));

                    dbquery = new StringBuffer("SELECT count(id) " +
					       "from ActivityUnits " +
					       "where mainPDMTuneFreq = " +
					       mainPDMTuneFreq.toString() +

					       " and " +

					       "activityId >= " + 
					       startActivityTF.getText() + 

					       " and " +

					       " activityId <= " + 
					       stopActivityTF.getText() +

					       ";");

                    rs.close();


                    rs = dbcd.executeQuery(dbquery.toString());

                    rs.next(); // there can be only one!



                    if (100.0*ov.aauTree.size()/rs.getDouble(1) > 
                        Double.parseDouble(srpd.percentSeenTF.getText())) {

                        reportSB.append("  " + ok + "\t" +
                                        srpd.maskwidthTF.getText() + "\n");

                    }


                }

                rs.close();

            }


	    rs.close();

	    reportSB.append("}\n");

	    // --------------------------
	    // ----- display report -----
	    // --------------------------

	    srpd.lastQueryTA.setText(lastQuerySB.toString());
	    summaryReportTA.setText(reportSB.toString());

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(srFrame, 
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


    // -----------------------------
    // ----- Recent RFI Report -----
    // -----------------------------


    public void recentRFIReport (boolean isMain) {


	// TBD main/remote


	summaryReportTA.setText(""); // clear text
	StringBuffer reportSB = new StringBuffer();

	srpd.lastQueryTA.setText("");
	StringBuffer lastQuerySB = new StringBuffer();

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT " +
			   "FORMAT(rfFreq - Activities.mainIfcSkyFrequency, " +
			   srpd.toleranceTF.getText() + ") " +
			   "as Freq, count(*) " +
			   "from Activities, SignalDescription ");
	    
	    if (appendWhereClauseToActivitySignalJoin(dbquery)) {
		dbquery.append(" where starid != " + srpd.starIdTF.getText());
	    } else {
		dbquery.append(" and starid != " + srpd.starIdTF.getText());
	    }

	    dbquery.append(" group by Freq ");
	    dbquery.append(" order by rfFreq ");

	    dbquery.append(";");

	    lastQuerySB.append(dbquery.toString());

	    rs = dbcd.executeQuery(dbquery.toString());

	    // ----- write mask -----

	    reportSB.append("# recent RFI mask\n");
	    reportSB.append("# host: " + hostValue.getText() + "\n");
	    reportSB.append("# database: " + dbnameValue.getText() + "\n");
	    reportSB.append("set bandcovered {0.0 40}\n");
	    reportSB.append("set masks {\n");

	    while (rs.next()) {
		reportSB.append("  " + rs.getDouble("Freq") + 
				"  " + srpd.maskwidthTF.getText() +
				"\n");
	    }

	    rs.close();

	    reportSB.append("}\n");

	    // --------------------------
	    // ----- display report -----
	    // --------------------------

	    srpd.lastQueryTA.setText(lastQuerySB.toString());
	    summaryReportTA.setText(reportSB.toString());

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(srFrame, 
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


    // ---------------------------------
    // ----- Signal Summary Report -----
    // ---------------------------------

    public void signalSummaryReport () {

	summaryReportTA.setText(""); // clear text
	StringBuffer reportSB = new StringBuffer();

	srpd.lastQueryTA.setText(""); // clear text
	StringBuffer lastQuerySB = new StringBuffer();

	reportSB.append("Host:\t" + dbcd.hostnameTF.getText());
	reportSB.append("\n");

	reportSB.append("Database:\t" + dbcd.dbnameTF.getText());
	reportSB.append("\n");

	reportSB.append("Activity Range:\t");
	reportActivities(reportSB, lastQuerySB);
	reportSB.append("\n");

	reportSB.append("Date Range:\t");
	reportDates(reportSB, lastQuerySB);
	reportSB.append("\n");

	reportSB.append("Freq. Range:\t");
	reportSignalFrequencies(reportSB, lastQuerySB);
	reportSB.append("\n");

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT " +
			   "Activities.type, sigClass as Class, " +
			   "reason as Reason, " +
			   "count(*) as count " +
			   "from SignalDescription, Activities ");

	    appendWhereClauseToActivitySignalJoin(dbquery);

	    dbquery.append(" group by Activities.type, Class, Reason ");
	    dbquery.append(" order by type ");

	    dbquery.append(";");

	    lastQuerySB.append(dbquery.toString() + "\n");

	    rs = dbcd.executeQuery(dbquery.toString());

	    reportSB.append("\n");
	    reportSB.append("Activity\tClassification\tReason\t\t\tCount");
	    reportSB.append("\n");

	    while (rs.next()) {
		reportSB.append(rs.getString("Activities.type")  + "\t" +
				rs.getString("Class") + "\t" +
				rs.getString("reason") + "\t" +
				rs.getString("count") +	"\n");
	    }

	    rs.close();

	    reportSB.append("\n");

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(srFrame, 
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


	// --------------------------
	// ----- display report -----
	// --------------------------

	srpd.lastQueryTA.setText(lastQuerySB.toString());
	summaryReportTA.setText(reportSB.toString());

    }


    void reportSignalFrequencies(StringBuffer reportSB, 
				 StringBuffer lastQuerySB) {

        try {

	    StringBuffer dbquery = new StringBuffer();
	    ResultSet    rs;

	    dbquery.append("SELECT min(rfFreq), max(rfFreq) " +
			   "from SignalDescription ");
	    appendWhereClauseToSignalQuery(dbquery);
	    dbquery.append(";");

	    lastQuerySB.append(dbquery.toString() + "\n");

            rs = dbcd.executeQuery(dbquery.toString());
	    rs.next(); 
	    reportSB.append(rs.getString(1));
	    reportSB.append(" MHz \t");
	    reportSB.append(rs.getString(2));
	    reportSB.append(" MHz");

	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(srFrame, 
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




}