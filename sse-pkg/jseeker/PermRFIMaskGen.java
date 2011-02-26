/*******************************************************************************

 File:    PermRFIMaskGen.java
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

public class PermRFIMaskGen implements DBApplicationInterface {

    JFrame             rfiFrame;
    DBConnectionDialog dbcd;

    JLabel             hostValue;
    JLabel             dbnameValue;

    // ----- parameters -----

    JTextField precisionTF;
    JTextField percentSeenTF;
    JTextField minimumSeenTF;
    JTextField startActivityTF;
    JTextField stopActivityTF;
    JTextField maskwidthTF;

    JRadioButton mainRB;
    static String mainST = new String("main");

    JRadioButton remoteRB;
    static String remoteST = new String("remote");

    // ----- mask area -----

    JTextArea   permRFIMaskTA;

    String      saveAsFilename;
  
  
    // -------------------
    // ----- methods -----
    // -------------------

    public PermRFIMaskGen (DBConnectionDialog dbConnectionDialog) {

        dbcd = dbConnectionDialog;
        dbcd.setDBAI(this);

        rfiFrame = new JFrame("Permanent RFI");

        Container cp = rfiFrame.getContentPane();

        // ----- grid bag -----

	GridBagLayout      rfiPanelGB  = new GridBagLayout();
	cp.setLayout(rfiPanelGB);

	GridBagConstraints rfiPanelGBC = new GridBagConstraints();
	rfiPanelGBC.fill      = GridBagConstraints.HORIZONTAL;
	rfiPanelGBC.insets    = new Insets(5, 5, 5, 5);


	// ----------------------------------
	// ----- query parameters panel -----
	// ---------------------------------

	int tfWidth = 8;

        hostValue = new JLabel("TBD");
        dbnameValue = new JLabel("TBD");


	// ----- precision & percentSeen-----

	JLabel precisionLabel = new JLabel("Precision");
	rfiPanelGBC.gridx = 0;
	rfiPanelGBC.gridy = 1;
	rfiPanelGB.setConstraints(precisionLabel, rfiPanelGBC);
	cp.add(precisionLabel);

	precisionTF = new JTextField(tfWidth);
	rfiPanelGBC.gridx = 1;
	rfiPanelGBC.gridy = 1;
	rfiPanelGB.setConstraints(precisionTF, rfiPanelGBC);
	cp.add(precisionTF);

	JLabel percentSeenLabel = new JLabel("% of Seen/(Total Obs.)");
	rfiPanelGBC.gridx = 2;
	rfiPanelGBC.gridy = 1;
	rfiPanelGB.setConstraints(percentSeenLabel, rfiPanelGBC);
	cp.add(percentSeenLabel);

	percentSeenTF = new JTextField(tfWidth);
	rfiPanelGBC.gridx = 3;
	rfiPanelGBC.gridy = 1;
	rfiPanelGB.setConstraints(percentSeenTF, rfiPanelGBC);
	cp.add(percentSeenTF);

	// ----- maskwidth -----

	JLabel minimumSeenLabel = new JLabel("Minimum Hits");
	rfiPanelGBC.gridx = 0;
	rfiPanelGBC.gridy = 2;
	rfiPanelGB.setConstraints(minimumSeenLabel, rfiPanelGBC);
	cp.add(minimumSeenLabel);

	minimumSeenTF = new JTextField(tfWidth);
	rfiPanelGBC.gridx = 1;
	rfiPanelGBC.gridy = 2;
	rfiPanelGB.setConstraints(minimumSeenTF, rfiPanelGBC);
	cp.add(minimumSeenTF);

	JLabel maskwidthLabel = new JLabel("Mask Width (MHz)");
	rfiPanelGBC.gridx = 2;
	rfiPanelGBC.gridy = 2;
	rfiPanelGB.setConstraints(maskwidthLabel, rfiPanelGBC);
	cp.add(maskwidthLabel);

	maskwidthTF     = new JTextField(tfWidth);
	rfiPanelGBC.gridx = 3;
	rfiPanelGBC.gridy = 2;
	rfiPanelGB.setConstraints(maskwidthTF, rfiPanelGBC);
	cp.add(maskwidthTF);


	// ----- start/stop activity -----

	JLabel startActivityLabel = new JLabel("Start Activity");
	rfiPanelGBC.gridx = 0;
	rfiPanelGBC.gridy = 3;
	rfiPanelGB.setConstraints(startActivityLabel, rfiPanelGBC);
	cp.add(startActivityLabel);

	startActivityTF = new JTextField(tfWidth);
	rfiPanelGBC.gridx = 1;
	rfiPanelGBC.gridy = 3;
	rfiPanelGB.setConstraints(startActivityTF, rfiPanelGBC);
	cp.add(startActivityTF);

	JLabel stopActivityLabel = new JLabel("Stop Activity");
	rfiPanelGBC.gridx = 2;
	rfiPanelGBC.gridy = 3;
	rfiPanelGB.setConstraints(stopActivityLabel, rfiPanelGBC);
	cp.add(stopActivityLabel);

	stopActivityTF = new JTextField(tfWidth);
	rfiPanelGBC.gridx = 3;
	rfiPanelGBC.gridy = 3;
	rfiPanelGB.setConstraints(stopActivityTF, rfiPanelGBC);
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

	rfiPanelGBC.gridwidth = 4;
	rfiPanelGBC.gridx = 0;
	rfiPanelGBC.gridy = 4;
	rfiPanelGB.setConstraints(mrPanel, rfiPanelGBC);
	cp.add(mrPanel);
	rfiPanelGBC.gridwidth = 1; // restore default
	


	// ----------------------
	// ----- mask panel -----
	// ----------------------
	
	permRFIMaskTA               = new JTextArea();
	JScrollPane permRFIMaskSP   = new JScrollPane(permRFIMaskTA);

	permRFIMaskSP.setPreferredSize(new Dimension(250, 300));
	permRFIMaskSP.setBorder(BorderFactory.createTitledBorder(
				 BorderFactory.createLoweredBevelBorder(), 
				 "Permanent RFI Mask"));

	rfiPanelGBC.gridwidth = 4;
	
	rfiPanelGBC.gridx = 0;
	rfiPanelGBC.gridy = 5;
	rfiPanelGB.setConstraints(permRFIMaskSP, rfiPanelGBC);
	cp.add(permRFIMaskSP);

	rfiPanelGBC.gridwidth = 1; // restore default

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
			saveAsFileChooser.showDialog(rfiFrame, "Save");
          
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

	rfiPanelGBC.gridwidth = 4;
	rfiPanelGBC.gridx = 0;
	rfiPanelGBC.gridy = 6;
	rfiPanelGB.setConstraints(controlPanel, rfiPanelGBC);
	cp.add(controlPanel);
	rfiPanelGBC.gridwidth = 1; // restore default



        // ----- add to frame -----

        rfiFrame.pack();

        rfiFrame.setVisible(true);

        rfiFrame.addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    System.exit(0);
                }
            });

    }

    public void saveAs (String filename) {

	try {

	    BufferedOutputStream os = 
		new BufferedOutputStream(new FileOutputStream(saveAsFilename));
	    
	    String sOut = permRFIMaskTA.getText();

	    os.write(sOut.getBytes());
	    os.flush();

	} catch (IOException e) {
	    JOptionPane.showMessageDialog(rfiFrame, "Unexpected IO Error: " 
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
	    
	    // ----------------------------
	    // ----- find perm RFIs -------
	    // ----------------------------

	    if (mainRB.isSelected()) {
	    
		dbquery = 
		    new String("SELECT  " + 
			       "activityId, dbActivityUnitId, rfFreq " +

			       "from SignalDescription " +
			       "where " + 

			       "activityId >= " + 
			       startActivityTF.getText() + 

			       " and activityId <= " + 
			       stopActivityTF.getText() +

			       " and ( pdmMode = \'main\' or " +
			       "pdmMode = \'main_only\') " +

			       " and sigClass = \'RFI\' " +

			       " order by rfFreq;");

	    } else {

		dbquery = 
		    new String("SELECT  " + 
			       "activityId, dbActivityUnitId, rfFreq " +

			       "from SignalDescription " +
			       "where " + 

			       "activityId >= " + 
			       startActivityTF.getText() + 

			       " and activityId <= " + 
			       stopActivityTF.getText() +

			       " and pdmMode = \'remote\' " +

			       " and sigClass = \'RFI\' " +

			       " order by rfFreq;");

	    }


	    rs = dbcd.executeQuery(dbquery.toString());

	    // TBD check empty result set
	    
	    // ----- tree the frequencies -----
	    
	    Map    maskTree       = new TreeMap();
	    int    dPlaces        = Integer.parseInt(precisionTF.getText());
	    double tolFactor      = Math.pow(10.0, dPlaces);

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



	    // ----------------------
	    // ----- write mask -----
	    // ----------------------
	    
	    
	    permRFIMaskTA.setText(""); // clear text
	    StringBuffer masktext = new StringBuffer();

            masktext.append("Set perm masks {\n");
	    
	    for (Iterator it = maskTree.entrySet().iterator(); it.hasNext();) {
		
		Map.Entry me = (Map.Entry) it.next();
		
		Object ok = me.getKey();
		AAUFreqRecord ov = (AAUFreqRecord) me.getValue();


		if (ov.aauTree.size() > 
		    Integer.parseInt(minimumSeenTF.getText())) {

		    // ----- normailze to the activity units -----

		    // TBD or remote?

		    dbquery = new String("SELECT mainPDMTuneFreq " +
					 "from ActivityUnits " +
					 "where id = " +
					 ov.activityUnitId.toString() +
					 ";");

		    rs = dbcd.executeQuery(dbquery.toString());

		    rs.next(); // there can be only one!

		    Double mainPDMTuneFreq = new Double(rs.getDouble(1));

		    dbquery = new String("SELECT count(id) " +
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
			Double.parseDouble(percentSeenTF.getText())) {


			// DEBUG masktext.append("  " + ok + "\t" + ov.aauTree.size() + 
			// DEUBG "\t" + rs.getString(1) + "\n");



			masktext.append("  " + ok + "\t" +
					maskwidthTF.getText() + "\n");

		    }


		}

		rs.close();

            }

            masktext.append("}\n#RFI Mask");

	    permRFIMaskTA.setText(masktext.toString());



	}

	catch (SQLException e) {
	    JOptionPane.showMessageDialog(rfiFrame, 
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
                JOptionPane.showMessageDialog(rfiFrame, 
                                              "Empty result for min(id)? ");
		rs.close();
		return;

            } else {

		startActivityTF.setText(rs.getString(1));

            }
            
            // There can be only one.
            if (rs.next()) {
                JOptionPane.showMessageDialog(rfiFrame, 
                                              "Multiple results found for " +
                                              "min(id)?");
		rs.close();
		return;
            }

            rs.close();

            
        }

        catch (SQLException e) {
            JOptionPane.showMessageDialog(rfiFrame, 
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
            JOptionPane.showMessageDialog(rfiFrame, "Unexpected IO Error: " 
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
                JOptionPane.showMessageDialog(rfiFrame, 
                                              "Empty result for max(id)?");
		rs.close();
		return;

            } else {

		stopActivityTF.setText(rs.getString(1));

            }
            
            // There can be only one.
            if (rs.next()) {
                JOptionPane.showMessageDialog(rfiFrame, 
                                              "Multiple results found for " +
                                              "max(id)?");
		rs.close();
		return;
            }

            rs.close();

            
        }

        catch (SQLException e) {
            JOptionPane.showMessageDialog(rfiFrame, 
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
            JOptionPane.showMessageDialog(rfiFrame, "Unexpected IO Error: " 
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

	PermRFIMaskGen permRFIMaskGen =  new PermRFIMaskGen(dbcd);

	// set default vaules

	permRFIMaskGen.precisionTF.setText("5");
	permRFIMaskGen.percentSeenTF.setText("75");
	permRFIMaskGen.minimumSeenTF.setText("5");
	permRFIMaskGen.maskwidthTF.setText("0.000643");


    }

}
