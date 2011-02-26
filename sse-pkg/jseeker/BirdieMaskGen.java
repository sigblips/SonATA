/*******************************************************************************

 File:    BirdieMaskGen.java
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
// Filename:    BirdieMaskGen.java
// Description: generates birdie mask
//
// Authors:     L.R. McFarland
// Language:    java
//
// Created:     2002-08-26
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

public class BirdieMaskGen  implements DBApplicationInterface {

    JFrame             bmgFrame;

    DBConnectionDialog dbcd;

    JLabel             hostValue;
    JLabel             dbnameValue;


    // ----- parameters -----

    JTextField precisionTF;
    JTextField percentSeenTF;
    JTextField minimumSeenTF;
    JTextField startActivityTF;
    JTextField stopActivityTF;
    JTextField bandwidthTF;
    JTextField maskwidthTF;

    JRadioButton mainRB;
    static String mainST = new String("main");

    JRadioButton remoteRB;
    static String remoteST = new String("remote");

    // ----- mask area -----

    JTextArea   birdieMaskTA;

    String      saveAsFilename;
  
  
    // -------------------
    // ----- methods -----
    // -------------------

    public BirdieMaskGen (DBConnectionDialog dbConnectionDialog) {

	dbcd = dbConnectionDialog;
        dbcd.setDBAI(this);

	bmgFrame = new JFrame("Birdie Mask Generator");

        Container cp = bmgFrame.getContentPane();

	int tfWidth = 8;

        // ----- grid bag -----

	GridBagLayout      bmgPanelGB  = new GridBagLayout();
	cp.setLayout(bmgPanelGB);

	GridBagConstraints bmgPanelGBC = new GridBagConstraints();
	bmgPanelGBC.fill      = GridBagConstraints.HORIZONTAL;
	bmgPanelGBC.insets    = new Insets(2, 4, 2, 4);

	// ----------------------------------
	// ----- query parameters panel -----
	// ----------------------------------


	hostValue = new JLabel("TBD");
	dbnameValue = new JLabel("TBD");




	// ----- precision & percentSeen-----

	JLabel precisionLabel = new JLabel("Precision");
	bmgPanelGBC.gridx = 0;
	bmgPanelGBC.gridy = 1;
	bmgPanelGB.setConstraints(precisionLabel, bmgPanelGBC);
	cp.add(precisionLabel);

	precisionTF = new JTextField(tfWidth);
	bmgPanelGBC.gridx = 1;
	bmgPanelGBC.gridy = 1;
	bmgPanelGB.setConstraints(precisionTF, bmgPanelGBC);
	cp.add(precisionTF);

	JLabel percentSeenLabel = new JLabel("% of Seen/(Total Obs.)");
	bmgPanelGBC.gridx = 2;
	bmgPanelGBC.gridy = 1;
	bmgPanelGB.setConstraints(percentSeenLabel, bmgPanelGBC);
	cp.add(percentSeenLabel);

	percentSeenTF = new JTextField(tfWidth);
	bmgPanelGBC.gridx = 3;
	bmgPanelGBC.gridy = 1;
	bmgPanelGB.setConstraints(percentSeenTF, bmgPanelGBC);
	cp.add(percentSeenTF);

        JLabel minimumSeenLabel = new JLabel("Minimum Hits");
        bmgPanelGBC.gridx = 0;
        bmgPanelGBC.gridy = 2;
        bmgPanelGB.setConstraints(minimumSeenLabel, bmgPanelGBC);
        cp.add(minimumSeenLabel);

        minimumSeenTF = new JTextField(tfWidth);
        bmgPanelGBC.gridx = 1;
        bmgPanelGBC.gridy = 2;
        bmgPanelGB.setConstraints(minimumSeenTF, bmgPanelGBC);
        cp.add(minimumSeenTF);

	JLabel maskwidthLabel = new JLabel("Mask Width (MHz)");
	bmgPanelGBC.gridx = 2;
	bmgPanelGBC.gridy = 2;
	bmgPanelGB.setConstraints(maskwidthLabel, bmgPanelGBC);
	cp.add(maskwidthLabel);

	maskwidthTF     = new JTextField(tfWidth);
	bmgPanelGBC.gridx = 3;
	bmgPanelGBC.gridy = 2;
	bmgPanelGB.setConstraints(maskwidthTF, bmgPanelGBC);
	cp.add(maskwidthTF);


	// ----- bandwidth -----

	JLabel bandwidthLabel = new JLabel("Bandwidth (MHz)");
	bmgPanelGBC.gridx = 0;
	bmgPanelGBC.gridy = 3;
	bmgPanelGB.setConstraints(bandwidthLabel, bmgPanelGBC);
	cp.add(bandwidthLabel);

	bandwidthTF = new JTextField(tfWidth);
	bmgPanelGBC.gridx = 1;
	bmgPanelGBC.gridy = 3;
	bmgPanelGB.setConstraints(bandwidthTF, bmgPanelGBC);
	cp.add(bandwidthTF);

	// ----- start/stop activity -----

	JLabel startActivityLabel = new JLabel("Start Activity");
	bmgPanelGBC.gridx = 0;
	bmgPanelGBC.gridy = 4;
	bmgPanelGB.setConstraints(startActivityLabel, bmgPanelGBC);
	cp.add(startActivityLabel);

	startActivityTF = new JTextField(tfWidth);
	bmgPanelGBC.gridx = 1;
	bmgPanelGBC.gridy = 4;
	bmgPanelGB.setConstraints(startActivityTF, bmgPanelGBC);
	cp.add(startActivityTF);

	JLabel stopActivityLabel = new JLabel("Stop Activity");
	bmgPanelGBC.gridx = 2;
	bmgPanelGBC.gridy = 4;
	bmgPanelGB.setConstraints(stopActivityLabel, bmgPanelGBC);
	cp.add(stopActivityLabel);

	stopActivityTF = new JTextField(tfWidth);
	bmgPanelGBC.gridx = 3;
	bmgPanelGBC.gridy = 4;
	bmgPanelGB.setConstraints(stopActivityTF, bmgPanelGBC);
	cp.add(stopActivityTF);

        // ----- main remote radio buttons -----

        mainRB = new JRadioButton(mainST);
        mainRB.setActionCommand(mainST);
        mainRB.setSelected(true);

        remoteRB = new JRadioButton(remoteST);
        remoteRB.setActionCommand(remoteST);

        ButtonGroup mrBG = new ButtonGroup();
        mrBG.add(mainRB);
        mrBG.add(remoteRB);

        JPanel mrPanel = new JPanel();

        mrPanel.add(mainRB);
        mrPanel.add(remoteRB);

        bmgPanelGBC.gridwidth = 4;
        bmgPanelGBC.gridx = 0;
        bmgPanelGBC.gridy = 5;
        bmgPanelGB.setConstraints(mrPanel, bmgPanelGBC);
        cp.add(mrPanel);
        bmgPanelGBC.gridwidth = 1; // restore default


	// ----------------------
	// ----- mask panel -----
	// ----------------------
	
	birdieMaskTA               = new JTextArea();
	JScrollPane birdieMaskSP   = new JScrollPane(birdieMaskTA);

	birdieMaskSP.setPreferredSize(new Dimension(250, 300));
	birdieMaskSP.setBorder(BorderFactory.createTitledBorder(
				 BorderFactory.createLoweredBevelBorder(), 
				 "Birdie Mask"));

	bmgPanelGBC.gridwidth = 4;
	
	bmgPanelGBC.gridx = 0;
	bmgPanelGBC.gridy = 6;
	bmgPanelGB.setConstraints(birdieMaskSP, bmgPanelGBC);
	cp.add(birdieMaskSP);

	bmgPanelGBC.gridwidth = 1; // restore default

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
		    updateMask();
		}
	    });

	controlPanel.add(updateBT);

	// ----- saveAs button -----

	final JFileChooser saveAsFileChooser = new JFileChooser();
	JButton            saveAsBT          = new JButton("Save As");



	saveAsBT.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {

		    int returnVal = 
			saveAsFileChooser.showDialog(bmgFrame, "Save");
          
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

	bmgPanelGBC.gridwidth = 4;
	bmgPanelGBC.gridx = 0;
	bmgPanelGBC.gridy = 7;
	bmgPanelGB.setConstraints(controlPanel, bmgPanelGBC);
	cp.add(controlPanel);
	bmgPanelGBC.gridwidth = 1; // restore default


	// ----- add to frame -----

        bmgFrame.pack();

        bmgFrame.setVisible(true);

        bmgFrame.addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    System.exit(0);
                }
            });


    }

    public void saveAs (String filename) {

	try {

	    BufferedOutputStream os = 
		new BufferedOutputStream(new FileOutputStream(saveAsFilename));
	    
	    String sOut = birdieMaskTA.getText();

	    os.write(sOut.getBytes());
	    os.flush();

	} catch (IOException e) {
	    JOptionPane.showMessageDialog(bmgFrame, "Unexpected IO Error: " 
					  + e);
	}

    }  

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

    public void updateMask () {

        try {
	    
	    String    dbquery;
	    ResultSet rs;

	    if (startActivityTF.getText().length() == 0) {
		setFirst();
	    }

	    if (stopActivityTF.getText().length() == 0) {
		setLast();
	    }


	    // TBD check for non birdie mask scan types
	    

	    // ------------------------
	    // ----- find birdies -----
	    // ------------------------


            if (mainRB.isSelected()) {
	    
		dbquery = new String("SELECT activityId, dbActivityUnitId, " +
				     "rfFreq, drift, width, power " +
				     "from SignalDescription " +
				     "where " +

				     " activityId >= " +
				     startActivityTF.getText() +

				     " and activityId <= " + 
				     stopActivityTF.getText() +

				     " and ( pdmMode = \'main\' or " +
				     "pdmMode = \'main_only\') " +

				     " and drift = 0 " +
				     " order by rfFreq;");


	    } else {

		dbquery = new String("SELECT activityId, dbActivityUnitId, " +
				     "rfFreq, drift, width, power " +
				     "from SignalDescription " +
				     "where " +

				     " activityId >= " +
				     startActivityTF.getText() +

				     " and activityId <= " + 
				     stopActivityTF.getText() +

				     " and pdmMode = \'remote\' " +

				     " and drift = 0 " +
				     " order by rfFreq;");


	    }


	    rs = dbcd.executeQuery(dbquery.toString());
	    
	    // ----- tree the frequencies -----
	    
	    Map    maskTree       = new TreeMap();
	    int    dPlaces        = Integer.parseInt(precisionTF.getText());
	    double tolFactor      = Math.pow(10.0, dPlaces);

	    int lastActivityId = 0;
	    
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
		
		lastActivityId = rs.getInt("activityId");
	    }
	    
	    rs.close();


	    // ---------------------------------
	    // ----- find center frequency -----
	    // ---------------------------------
	    
	    if (lastActivityId == 0) {
                JOptionPane.showMessageDialog(bmgFrame, 
                                              "Empty result for this "   +
                                              "Activity ID: " +
					      lastActivityId);
	    }


	    dbquery = new String("SELECT mainIfcSkyFrequency, " + 
				 "remoteIfcSkyFrequency " +
				 "from Activities " +
				 "where id = " +
				 lastActivityId +
				 ";");
	    
	    rs = dbcd.executeQuery(dbquery.toString());


	    if (rs == null) {
                JOptionPane.showMessageDialog(bmgFrame, 
                                              "Empty result for this "   +
                                              "Activity ID: " +
					      lastActivityId);
		return;
	    }
	    
	    rs.next(); // there can be only one

	    Double ifcSkyFrequency;

            if (mainRB.isSelected()) {
		ifcSkyFrequency = 
		    new Double(rs.getDouble("mainIfcSkyFrequency"));
	    } else {
		ifcSkyFrequency = 
		    new Double(rs.getDouble("remoteIfcSkyFrequency"));
	    }

	    // check that the sky frequency was the same over the range

	    if (mainRB.isSelected()) {

		dbquery = new String("SELECT " + 
		     "count(distinct(mainIfcSkyFrequency)) " +
		     "from Activities " +
		     "where id >= " +
		     startActivityTF.getText() + " and " +
		     " id <= " + 
		     stopActivityTF.getText() + " and " +
		     "validObservation = \'Yes\'" +
		     ";");

	    } else {

		dbquery = new String("SELECT " + 
		     "count(distinct(remoteIfcSkyFrequency)) " +
		     "from Activities " +
		     "where id >= " +
		     startActivityTF.getText() + " and " +
		     " id <= " + 
		     stopActivityTF.getText() + " and " +
		     "validObservation = \'Yes\'" +
		     ";");
	    }

	    rs.close(); // the last one
	    rs = dbcd.executeQuery(dbquery.toString());
	    rs.next(); // only one ...
	    
	    if (rs.getInt(1) > 1) {
                JOptionPane.showMessageDialog(bmgFrame, 
		      "Warning: multiple IFC sky frequencies in " +
		      "this activity id range: " +
		      startActivityTF.getText() + 
		      " and " +
		      stopActivityTF.getText() +
					      ";");
		rs.close();
		// TBD return;
	    }

	    // ----------------------
	    // ----- write mask -----
	    // ----------------------
	    
	    ifcSkyFrequency 
		= new Double(ifcSkyFrequency.doubleValue()); //  in MHz
	    
	    birdieMaskTA.setText(""); // clear text
	    StringBuffer masktext = new StringBuffer();
	    
	    masktext.append("set bandcovered {0.0 ");
	    masktext.append(bandwidthTF.getText());
	    masktext.append("}\n");
	    masktext.append("set masks {\n");
	    
	    for (Iterator it = maskTree.entrySet().iterator(); it.hasNext();) {
		
		Map.Entry me = (Map.Entry) it.next();
		
		Object ok = me.getKey();
		AAUFreqRecord ov = (AAUFreqRecord) me.getValue();

		if (ov.aauTree.size() > 
		    Integer.parseInt(minimumSeenTF.getText())) {

		    // ----- normailze to the activity units -----

		    dbquery = new String("SELECT mainPDMTuneFreq " +
					 "from ActivityUnits " +
					 "where id = " +
					 ov.activityUnitId.toString() +
					 ";");

		    rs = dbcd.executeQuery(dbquery.toString());

		    rs.next(); // there can be only one!

		    Double pdmTuneFreq = new Double(rs.getDouble(1));

		    if (mainRB.isSelected()) {

			dbquery = new String("SELECT count(id) " +
					     "from ActivityUnits " +
					     "where mainPDMTuneFreq = " +
					     pdmTuneFreq.toString() +

					     " and " +

					     "activityId >= " + 
					     startActivityTF.getText() + 

					     " and " +

					     " activityId <= " + 
					     stopActivityTF.getText() +

					     ";");

		    } else {

			dbquery = new String("SELECT count(id) " +
					     "from ActivityUnits " +
					     "where remotePDMTuneFreq = " +
					     pdmTuneFreq.toString() +

					     " and " +

					     "activityId >= " + 
					     startActivityTF.getText() + 

					     " and " +

					     " activityId <= " + 
					     stopActivityTF.getText() +

					     ";");

		    }

		    rs.close(); // the last one

		    rs = dbcd.executeQuery(dbquery.toString());

		    rs.next(); // there can be only one!


		    if (100.0*ov.aauTree.size()/rs.getDouble(1) > 
			Double.parseDouble(percentSeenTF.getText())) {


			// TBD masktext.append("  " + ok + "\t" + ov + "\n");

			double keyFreq = Double.parseDouble(ok.toString());

			double deltaFreq = keyFreq - 
			    ifcSkyFrequency.doubleValue();

			Double offsetFreq = new Double(deltaFreq * tolFactor);
			int roundOffsetFreq = Math.round(offsetFreq.intValue());
			Double nextOffset  = new Double(roundOffsetFreq/tolFactor);


		    
			masktext.append("  " + nextOffset + "\t" + 
					maskwidthTF.getText() +
					// TBD "\t count = " + ov.aauTree.size() + // TBD rm
					"\n");

		    }

		}

            }

	    masktext.append("}\n#birdieMask");

	    birdieMaskTA.setText(masktext.toString());

	    rs.close();


	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(bmgFrame, 
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


    public void setFirst () {

        try {

            String dbquery = new String("SELECT min(id) " +
                                        "from Activities " +
                                        ";");
            
            ResultSet rs = dbcd.executeQuery(dbquery.toString());
            
            if (!rs.next()) {
                JOptionPane.showMessageDialog(bmgFrame, 
                                              "Empty result for min(id)?");
		rs.close();
		return;
            } else {

		startActivityTF.setText(rs.getString(1));

            }
            
            // There can be only one.
            if (rs.next()) {
                JOptionPane.showMessageDialog(bmgFrame, 
                                              "Multiple results found for " +
                                              "min(id)?");
		rs.close();
		return;
            }

            rs.close();


	}

        catch (SQLException e) {
            JOptionPane.showMessageDialog(bmgFrame, 
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
            JOptionPane.showMessageDialog(bmgFrame, "Unexpected IO Error: " 
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
                JOptionPane.showMessageDialog(bmgFrame, 
                                              "Empty result for max(id)?");
		rs.close();
		return;
            } else {

		stopActivityTF.setText(rs.getString(1));

            }
            
            // There can be only one.
            if (rs.next()) {
                JOptionPane.showMessageDialog(bmgFrame, 
                                              "Multiple results found for " +
                                              "max(id)?");
		rs.close();
		return;
            }

            rs.close();
            
        }

        catch (SQLException e) {
            JOptionPane.showMessageDialog(bmgFrame, 
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
            JOptionPane.showMessageDialog(bmgFrame, "Unexpected IO Error: " 
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

	BirdieMaskGen birdieMaskGen = new BirdieMaskGen(dbcd);

	// set default vaules

	dbcd.dbnameTF.setText("birdiemask");
	// TBD dbcd.dbnameTF.setText("sse_birdie_beta");

	birdieMaskGen.precisionTF.setText("5");
	birdieMaskGen.percentSeenTF.setText("75");
	birdieMaskGen.minimumSeenTF.setText("5");

	birdieMaskGen.bandwidthTF.setText("40");
	birdieMaskGen.maskwidthTF.setText("0.000643");

    }

}
