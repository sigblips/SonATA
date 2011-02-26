/*******************************************************************************

 File:    DBConnectionDialog.java
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


interface DBApplicationInterface {

    void initRecord();

    JLabel getHost();
    JLabel getDBName();


}


class DBConnectionDialog {

    JFrame     dbcFrame;

    JTextField hostnameTF;
    JTextField dbnameTF;
    JTextField userTF;
    JTextField passwdTF;

    boolean    open = false;

    Connection conn = null;
    Statement  stmt = null;

    DBApplicationInterface dbai = null;

    int major_version_;
    int minor_version_;

    // -------------------
    // ----- methods -----
    // -------------------

    public int majorVersion() {
	return(major_version_);
    }

    public int minorVersion() {
	return(minor_version_);
    }

    public void setDBAI (DBApplicationInterface dbai) {
	this.dbai = dbai;
    }

    public void setVisible (boolean visible) {
	dbcFrame.setVisible(visible);
    }

    public DBConnectionDialog () {

	dbcFrame = new JFrame("Database Connection");

	Container cp = dbcFrame.getContentPane();
    
        GridBagLayout      mainPanelGB  = new GridBagLayout();
        cp.setLayout(mainPanelGB);

        GridBagConstraints mainPanelGBC = new GridBagConstraints();
        mainPanelGBC.fill      = GridBagConstraints.HORIZONTAL;
        mainPanelGBC.insets    = new Insets(2,2,2,2);

        // -------------------------------
        // ----- DB connection panel -----
        // -------------------------------

	final int tfWidth = 16;
				
	// ----- database connection parameters -----
	

	JLabel hostnameLabel = new JLabel("Hostname");
	mainPanelGBC.gridx = 0;
	mainPanelGBC.gridy = 1;
	mainPanelGB.setConstraints(hostnameLabel, mainPanelGBC);
	cp.add(hostnameLabel);
	
	hostnameTF = new JTextField(tfWidth);
	mainPanelGBC.gridx = 1;
	mainPanelGBC.gridy = 1;
	mainPanelGB.setConstraints(hostnameTF, mainPanelGBC);
	cp.add(hostnameTF);
	hostnameTF.setText("localhost");
	
	hostnameTF.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    JTextField saidTF = (JTextField) e.getSource();

		    try {
			open();
		    }

		    catch (NumberFormatException excp) {
			JOptionPane.showMessageDialog(dbcFrame, excp);
		    }


		}
	    });


	JLabel dbnameLabel   = new JLabel("Database");
	mainPanelGBC.gridx = 2;
	mainPanelGBC.gridy = 1;
	mainPanelGB.setConstraints(dbnameLabel, mainPanelGBC);
	cp.add(dbnameLabel);

	dbnameTF = new JTextField(tfWidth);
	mainPanelGBC.gridx = 3;
	mainPanelGBC.gridy = 1;
	mainPanelGB.setConstraints(dbnameTF, mainPanelGBC);
	cp.add(dbnameTF);
	dbnameTF.setText("seeker");


	dbnameTF.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    JTextField saidTF = (JTextField) e.getSource();

		    try {
			open();
		    }

		    catch (NumberFormatException excp) {
			JOptionPane.showMessageDialog(dbcFrame, excp);
		    }


		}
	    });


	
	JLabel userLabel     = new JLabel("User");
	mainPanelGBC.gridx = 0;
	mainPanelGBC.gridy = 2;
	mainPanelGB.setConstraints(userLabel, mainPanelGBC);
	cp.add(userLabel);

	userTF = new JTextField(tfWidth);
	mainPanelGBC.gridx = 1;
	mainPanelGBC.gridy = 2;
	mainPanelGB.setConstraints(userTF, mainPanelGBC);
	cp.add(userTF);
	userTF.setText("nss");
	

	JLabel passwdLabel   = new JLabel("Password");
	mainPanelGBC.gridx = 2;
	mainPanelGBC.gridy = 2;
	mainPanelGB.setConstraints(passwdLabel, mainPanelGBC);
	cp.add(passwdLabel);

	passwdTF = new JTextField(tfWidth);
	mainPanelGBC.gridx = 3;
	mainPanelGBC.gridy = 2;
	mainPanelGB.setConstraints(passwdTF, mainPanelGBC);
	cp.add(passwdTF);
	passwdTF.setText("");


        // -------------------------
        // ----- control panel -----
        // -------------------------
	
        // ----- open button -----
	
        JButton openBT = new JButton("Open");
        openBT.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
		    open();
                }
            });

	
        mainPanelGBC.gridwidth = 2;
        mainPanelGBC.gridx = 0;
        mainPanelGBC.gridy = 3;
        mainPanelGB.setConstraints(openBT, mainPanelGBC);
        cp.add(openBT);
	mainPanelGBC.gridwidth = 1; // restore default

        // ----- close button -----
	
        JButton closeBT = new JButton("Close");
        closeBT.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
		    close();
		    dbcFrame.setVisible(false);
                }
            });

	
        mainPanelGBC.gridwidth = 2;
        mainPanelGBC.gridx = 2;
        mainPanelGBC.gridy = 3;
        mainPanelGB.setConstraints(closeBT, mainPanelGBC);
        cp.add(closeBT);
	mainPanelGBC.gridwidth = 1; // restore default

	dbcFrame.pack();

        dbcFrame.addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    System.exit(0);
                }
            });
	
    }


    public void open() {

        try {

	    if (open) close();

            String DBurl = new String("jdbc:mysql://" + 
				      hostnameTF.getText() + "/" +
				      dbnameTF.getText()   + 
				      "?user=" + userTF.getText() + 
				      "&password=" + passwdTF.getText()
				      );

            conn = DriverManager.getConnection(DBurl);
            stmt = conn.createStatement();
	    open = true;
	    dbcFrame.setVisible(false);

	    if (dbai == null) {

		JOptionPane.showMessageDialog(dbcFrame.getContentPane(),
					      "DBConnectionDialog::open()" +
					      "null dbai.",
					      "DB Error",
					      JOptionPane.ERROR_MESSAGE);
		return;

	    }


	    // ASSUMES: ordered by timestamp puts latest revision on top

	    String dbquery = new String("SELECT * " +
					"from seeker_db_version " +
					"order by ts" +
					";");
            
            
	    ResultSet rs = executeQuery(dbquery.toString());
            
	    if (!rs.next()) {
		JOptionPane.showMessageDialog(dbcFrame.getContentPane(),
					      "DBConnectionDialog::open()" +
					      "No version records found.");
		return;

	    } else {

		String version_str = rs.getString("revision");

		StringTokenizer st1 = new StringTokenizer(version_str);
		st1.nextToken();

		StringTokenizer st2 = 
		    new StringTokenizer(st1.nextToken(), ".");

		major_version_ = Integer.parseInt(new String(st2.nextToken()));
		minor_version_ = Integer.parseInt(new String(st2.nextToken()));

	    }


	    rs.close();

	    dbai.initRecord();
	    dbai.getHost().setText(hostnameTF.getText());
	    dbai.getDBName().setText(dbnameTF.getText());

        }

        catch (SQLException e) {
            JOptionPane.showMessageDialog(dbcFrame.getContentPane(),
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
            JOptionPane.showMessageDialog(dbcFrame.getContentPane(), e);
        }


    }


    public void close() {

	try {
	    if (stmt != null) stmt.close();
            if (conn != null) conn.close();
	    open = false;
	}

        catch (SQLException e) {
            JOptionPane.showMessageDialog(dbcFrame.getContentPane(),
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
            JOptionPane.showMessageDialog(dbcFrame.getContentPane(), e);
        }


    }

    public void destroy() {
	close();
    }

    public ResultSet executeQuery(String dbquery) throws SQLException {
	if (!open) open();
	return(stmt.executeQuery(dbquery.toString()));
    }
    
}
