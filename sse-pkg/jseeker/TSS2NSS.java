/*******************************************************************************

 File:    TSS2NSS.java
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
// Filename:    TSS2NSS.java
// Description: converts TSS data to NSS data
//
// Authors:     L.R. McFarland
// Language:    java
//
// Created:     2002-11-1
// ==========================================================

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.JTable;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableColumn;

import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.ListSelectionEvent;

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;

import java.text.*;

import java.io.*;
import java.util.*;

import java.util.regex.*;

import java.sql.*;

import java.lang.ClassLoader;
import java.net.URL;

public class TSS2NSS implements DBApplicationInterface {

    JFrame             tss2nssFrame;

    DBConnectionDialog tssdbcd;
    DBConnectionDialog nssdbcd;

    static JTextField         tssStartObsTF;
    static JTextField         tssStopObsTF;
    static JTextField         tssXrefTF;

    public TSS2NSS (DBConnectionDialog TSSdbcd, DBConnectionDialog NSSdbcd) {

        tssdbcd = TSSdbcd;
        nssdbcd = NSSdbcd;


        tss2nssFrame = new JFrame("TSS to NSS");

        Container cp = tss2nssFrame.getContentPane();

        int tfWidth = 8;

        // ----- grid bag -----

        GridBagLayout      tss2nssPanelGB  = new GridBagLayout();
        cp.setLayout(tss2nssPanelGB);

        GridBagConstraints tss2nssPanelGBC = new GridBagConstraints();
        tss2nssPanelGBC.fill      = GridBagConstraints.HORIZONTAL;
        tss2nssPanelGBC.insets    = new Insets(2, 4, 2, 4);


	// ----- start stop observations -----


	JLabel tssStartObsLabel = new JLabel("TSS start observation:");

        tss2nssPanelGBC.gridx = 0;
        tss2nssPanelGBC.gridy = 0;
        tss2nssPanelGB.setConstraints(tssStartObsLabel, tss2nssPanelGBC);
        cp.add(tssStartObsLabel);

	tssStartObsTF = new JTextField(tfWidth);
        tss2nssPanelGBC.gridx = 1;
        tss2nssPanelGBC.gridy = 0;
        tss2nssPanelGB.setConstraints(tssStartObsTF, tss2nssPanelGBC);
        cp.add(tssStartObsTF);



	JLabel tssStopObsLabel = new JLabel("TSS stop observation:");

        tss2nssPanelGBC.gridx = 0;
        tss2nssPanelGBC.gridy = 1;
        tss2nssPanelGB.setConstraints(tssStopObsLabel, tss2nssPanelGBC);
        cp.add(tssStopObsLabel);

	tssStopObsTF = new JTextField(tfWidth);
        tss2nssPanelGBC.gridx = 1;
        tss2nssPanelGBC.gridy = 1;
        tss2nssPanelGB.setConstraints(tssStopObsTF, tss2nssPanelGBC);
        cp.add(tssStopObsTF);


	// ----- xref name -----

	JLabel tssXrefLabel = new JLabel("Cross refernce name: ");

        tss2nssPanelGBC.gridx = 0;
        tss2nssPanelGBC.gridy = 2;
        tss2nssPanelGB.setConstraints(tssXrefLabel, tss2nssPanelGBC);
        cp.add(tssXrefLabel);

	tssXrefTF = new JTextField(tfWidth);
        tss2nssPanelGBC.gridx = 1;
        tss2nssPanelGBC.gridy = 2;
        tss2nssPanelGB.setConstraints(tssXrefTF, tss2nssPanelGBC);
        cp.add(tssXrefTF);



        // -------------------------
        // ----- control panel -----
        // -------------------------

        JPanel controlPanel = new JPanel();

        // ----- TSS dbConnection button -----

        JButton TSSdbConnectionBT = new JButton("TSS Connection");
        TSSdbConnectionBT.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    tssdbcd.setVisible(true);
                }
            });

        controlPanel.add(TSSdbConnectionBT);

        // ----- NSS dbConnection button -----

        JButton NSSdbConnectionBT = new JButton("NSS Connection");
        NSSdbConnectionBT.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    nssdbcd.setVisible(true);
                }
            });

        controlPanel.add(NSSdbConnectionBT);

        // ----- xfer button -----

        JButton xferBT = new JButton("Transfer Data");
        xferBT.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    xfer ();
                }
            });

        controlPanel.add(xferBT);

        // ----- exit button -----

        JButton exitBT = new JButton("Exit");
        exitBT.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    System.exit(0);
                }
            });

        controlPanel.add(exitBT);

        tss2nssPanelGBC.gridwidth = 4;
        tss2nssPanelGBC.gridx = 0;
        tss2nssPanelGBC.gridy = 5;
        tss2nssPanelGB.setConstraints(controlPanel, tss2nssPanelGBC);
        cp.add(controlPanel);
        tss2nssPanelGBC.gridwidth = 1; // restore default


        // ----- add to frame -----

        tss2nssFrame.pack();

        tss2nssFrame.setVisible(true);

        tss2nssFrame.addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    System.exit(0);
                }
            });


    }


    class ActivityRecord {

	Integer activityId;
	Double  center_freqA;
	Double  center_freqB;
	String  startOfDC;

	public ActivityRecord (Integer activityId, Double center_freqA,
			       Double center_freqB, String startOfDC)  {
	    this.activityId   = activityId;
	    this.center_freqA = center_freqA;
	    this.center_freqB = center_freqB;
	    this.startOfDC    = startOfDC;
	}


    }


    public void xfer () {

	String    tssdbquery;
	ResultSet tssrs;

	String    nssdbquery;
	ResultSet nssrs;

	Pattern   aoPat = Pattern.compile("ARECIBO");
	Pattern   jbPat = Pattern.compile("JODRELL BAN");


        try {
            

	    tssdbquery = new String("SELECT * " + 
				    "from OBS_REC " +
				    "where obs_id > " + 
				    tssStartObsTF.getText() +
				    " and obs_id < " +
				    tssStopObsTF.getText() +
				    ";");


            tssrs = tssdbcd.executeQuery(tssdbquery.toString());

	    Map idTree = new TreeMap();

	    while(tssrs.next()) {

		// ----- telescopes -----

		Matcher main_site_is_AO = 
		    aoPat.matcher(tssrs.getString("main_observatory"));

		Matcher main_site_is_JB = 
		    jbPat.matcher(tssrs.getString("main_observatory"));


		int mainTscopeId   = 0;

		// TBD look up ids in database
		if (main_site_is_AO.find()) {
		    mainTscopeId = 1;	// TBD look up ids in database
		}

		if (main_site_is_JB.find()) {
		    mainTscopeId = 8;	// TBD look up ids in database
		}


		Matcher remote_site_is_AO = 
		    aoPat.matcher(tssrs.getString("remote_observatory"));

		Matcher remote_site_is_JB = 
		    jbPat.matcher(tssrs.getString("remote_observatory"));

		int remoteTscopeId = 0;

		if (remote_site_is_AO.find()) {
		    remoteTscopeId = 1;	// TBD look up ids in database

		}

		if (remote_site_is_JB.find()) {
		    remoteTscopeId = 8;	// TBD look up ids in database
		}


		// ----- activity type -----

		StringBuffer typeSB = new StringBuffer();
		typeSB.append("TSS: ");
		typeSB.append(tssrs.getString("act_name"));


		// ----- start of data collection -----

		StringBuffer docSB = new StringBuffer();

		docSB.append(tssrs.getString("obs_date"));
		docSB.append(" ");
		docSB.append(tssrs.getString("obs_time"));

		// ----- mcsa center frequencies -----

		Double center_freqA = 
		    new Double(tssrs.getString("center_freqA"));

		Double center_freqB = 
		    new Double(tssrs.getString("center_freqB"));


		// ----- insert into activities table -----

		nssdbquery = new String("INSERT into Activities  " + 
					"(startOfDataCollection," + 
					"type, starid, " + 
					"mainTscopeId, remoteTscopeId, " +
					"validObservation" +
					") " +
					"VALUES (\'" +
					docSB.toString() + "\', \'" +
					typeSB.toString() + "\', " +
					tssrs.getInt("seti_starnum") + ", " +
					mainTscopeId + ", " +
					remoteTscopeId + ", " +
					"\'Yes\'" +
					")" +
					";");
		
		nssrs = nssdbcd.executeQuery(nssdbquery.toString());

		nssrs.close();


		nssdbquery = new String("SELECT LAST_INSERT_ID () ");
		nssrs = nssdbcd.executeQuery(nssdbquery.toString());

		nssrs.next(); // only one row


		idTree.put(new Integer(tssrs.getInt("obs_id")),
			   new ActivityRecord(new Integer(nssrs.getInt(1)), 
					      center_freqA,
					      center_freqB,
					      docSB.toString()));


		// insert into activity xref table

		nssdbquery = new String("INSERT into ActivityXRef " +
					"(newId, oldId, oldName) " +
					"VALUES (" +
					nssrs.getInt(1) + ", " +
					tssrs.getInt("obs_id") + ", \'" +
					tssXrefTF.getText() + 
					"\')" +
					";");

		// TBD delete last insert if this fails

		nssrs.close(); // previouse query

		nssrs = nssdbcd.executeQuery(nssdbquery.toString());

		nssrs.close();



	    }

	    tssrs.close();

	    // -----------------------------------------
	    // ----- load signal description table -----
	    // -----------------------------------------


	    Pattern   cwPat = Pattern.compile("CW[RL]");
	    Pattern   pdPat = Pattern.compile("PD");

	    Pattern   leftPat  = Pattern.compile("LEFT");
	    Pattern   rightPat = Pattern.compile("RIGHT");
	    Pattern   bothPat  = Pattern.compile("BOTH");
	    Pattern   mixedPat = Pattern.compile("MIXED");

	    Pattern   rfiScanPat = Pattern.compile("RFIScan");
	    Pattern   zeroDriftPat = Pattern.compile("ZeroDrift");



            for (Iterator it = idTree.entrySet().iterator(); it.hasNext();) {

		Map.Entry me = (Map.Entry) it.next();

                Object ok = me.getKey();
                ActivityRecord ov = (ActivityRecord) me.getValue();


		// ----- load activity unit for A -----

		nssdbquery = new String("INSERT into ActivityUnits " + 
					"(startOfDataCollection, " +
					"activityId, " +
					"mainPDMTuneFreq) " +
					"VALUES (\'" + 
					ov.startOfDC + "\', " +
					ov.activityId + ", " +
					ov.center_freqA +
					");");

		nssrs = nssdbcd.executeQuery(nssdbquery.toString());

		nssrs.close();



		nssdbquery = new String("SELECT LAST_INSERT_ID () ");
		nssrs = nssdbcd.executeQuery(nssdbquery.toString());

		nssrs.next(); // only one row

		Integer activityUnitIdA = new Integer(nssrs.getInt(1));

		nssrs.close();



		// ----- load activity unit for B -----

		nssdbquery = new String("INSERT into ActivityUnits " + 
					"(startOfDataCollection, " +
					"activityId, " +
					"mainPDMTuneFreq) " +
					"VALUES (\'" + 
					ov.startOfDC + "\', " +
					ov.activityId + ", " +
					ov.center_freqB +
					");");

		nssrs = nssdbcd.executeQuery(nssdbquery.toString());

		nssrs.close();



		nssdbquery = new String("SELECT LAST_INSERT_ID () ");
		nssrs = nssdbcd.executeQuery(nssdbquery.toString());

		nssrs.next(); // only one row

		Integer activityUnitIdB = new Integer(nssrs.getInt(1));

		nssrs.close();



		// ----- load signal description -----



		tssdbquery = new String("SELECT * " + 
					"from SUP_CLUS " +
					"where obs_id = " + 
					ok.toString() +
					";");


		tssrs = tssdbcd.executeQuery(tssdbquery.toString());

		while(tssrs.next()) {

		    // ----- signal type -----

		    Matcher signal_is_cw = 
			cwPat.matcher(tssrs.getString("signal_type"));

		    Matcher signal_is_pulse = 
			pdPat.matcher(tssrs.getString("signal_type"));

		    String signal_type = new String();;

		    if (signal_is_cw.find()) {
			signal_type = new String("CwP");
		    }

		    if (signal_is_pulse.find()) {
			signal_type = new String("Pul");
		    }

		    // ----- polarization -----

		    Matcher signal_is_left = 
			leftPat.matcher(tssrs.getString("polarization"));

		    Matcher signal_is_right = 
			rightPat.matcher(tssrs.getString("polarization"));

		    Matcher signal_is_both = 
			bothPat.matcher(tssrs.getString("polarization"));

		    Matcher signal_is_mixed = 
			mixedPat.matcher(tssrs.getString("polarization"));


		    String polarization = new String();

		    if (signal_is_left.find()) {
			polarization = new String("left");
		    }

		    if (signal_is_right.find()) {
			polarization = new String("right");
		    }

		    if (signal_is_both.find()) {
			polarization = new String("both");
		    }

		    if (signal_is_mixed.find()) {
			polarization = new String("mixed");
		    }


		    // ----- signal type -----

		    // TBD other types

		    Matcher signal_is_rfi_scan = 
			rfiScanPat.matcher(tssrs.getString("rfi_reason"));

		    Matcher signal_is_zero_drift = 
			zeroDriftPat.matcher(tssrs.getString("rfi_reason"));

		    String reason = new String();;

		    if (signal_is_rfi_scan.find()) {
			reason = new String("RFI_SCAN");
		    }

		    if (signal_is_zero_drift.find()) {
			reason = new String("ZERO_DRIFT");
		    }


		    // ----- activity start time -----
		    
		    StringBuffer astSB = new StringBuffer();

		    astSB.append(tssrs.getString("obs_date"));
		    astSB.append(" ");
		    astSB.append(tssrs.getString("obs_time"));

		    
		    Integer activityUnitId;

		    if (Math.abs(ov.center_freqA.doubleValue() - 
				 tssrs.getDouble("rf_freq"))   <
			Math.abs(ov.center_freqB.doubleValue() - 
				 tssrs.getDouble("rf_freq"))){

			activityUnitId = activityUnitIdA;

		    } else {
			activityUnitId = activityUnitIdB;
		    }




		    nssdbquery = new String("INSERT into SignalDescription " + 
					    "(activityId, " +
					    "dbActivityUnitId, " +
					    "type, " + 
					    "rfFreq, " + 
					    "drift, " +
					    "width, " +
					    "power, " +
					    "pol, " +
					    "sigClass, " +
					    "reason, " +
					    "subbandNumber, " +
					    "pdmNumber, " +
					    "activityStartTime, " +
					    "pdmMode " +
					    ") " +
					    "VALUES (" +
					    ov.activityId.toString() + ", " +
					    activityUnitId + ", \'" +
					    signal_type + "\', " +
					    tssrs.getString("rf_freq") + ", " +
					    tssrs.getString("drift") + ", " +
					    tssrs.getString("signal_width") + ", " +
					    tssrs.getString("power") + ", \'" +
					    polarization + "\', " +
					    "\'RFI\'" + ", \'" + // TBD other classifications
					    reason + "\', " +
					    tssrs.getString("subband") + ", " +
					    "777, "  + // TBD fake pdm number
					    "\'" + astSB.toString() + "\', " +
					    "\'Main\'" + // TBD pdm mode
					    ");");

		    nssrs = nssdbcd.executeQuery(nssdbquery.toString());
		    
		    nssrs.close();


		}

		tssrs.close();

	    }



        }

        catch (SQLException e) {
            JOptionPane.showMessageDialog(tss2nssFrame, 
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


    public void initRecord() {
	// TBD
    }

    public JLabel getHost() {
        return(new JLabel("TBD"));  // TBD not implemented in this application
    }

    public JLabel getDBName() {
        return(new JLabel("TBD")); // TBD not implemented in this application
    }


    public static void main(String[] args) {

        try {
            Class.forName("com.mysql.jdbc.Driver").newInstance();
        }
        catch (Exception e) {
            e.printStackTrace();
        }


        DBConnectionDialog tssdbcd = new DBConnectionDialog();
        DBConnectionDialog nssdbcd = new DBConnectionDialog();

        TSS2NSS tss2nss = new TSS2NSS(tssdbcd, nssdbcd);

	// ----- set default values -----

	tssdbcd.dbnameTF.setText("tss");

	tssStartObsTF.setText("32984");
	tssStopObsTF.setText("33113");
	tssXrefTF.setText("Arecibo, Spring 2002");



    }

}
