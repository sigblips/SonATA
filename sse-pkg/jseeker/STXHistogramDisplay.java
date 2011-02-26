/*******************************************************************************

 File:    STXHistogramDisplay.java
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
// Filename:    STXHistogramDisplay.java
// Description: plots STX histogram data for two pols
//
// Authors:     L.R. McFarland
// Created:     2002-08-07
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

import ptolemy.plot.*;

public class STXHistogramDisplay extends JPanel {

  JFrame mainFrame;
  
  // ----- controls -----
  
  JFileChooser openFileChooser    = new JFileChooser();
  JFileChooser saveAsFileChooser  = new JFileChooser();
  
  String       currentFilename;
  String       saveAsFilename;
  
  JPanel       controlPanel  = new JPanel();
  JComboBox    fileCB;

  JButton      clearBT;
  JButton      updateBT;


  // ----- plot -----
  
  Plot   plot          = new Plot();
  
   int    lcpDataSet    = 0;  // red plot
   int    rcpDataSet    = 1;  // blue plot
  
  Vector lcpData = new Vector();
  Vector rcpData = new Vector();

  
  // ----- stataistics -----
  
  JLabel        statLabel      = new JLabel("Statistics");
  JLabel        sumWLabel      = new JLabel("total counts");
  JLabel        meanLabel      = new JLabel("mean");
  JLabel        varLabel       = new JLabel("variance");
  
  JLabel        lcpLabel       = new JLabel("LCP");
  JLabel        rcpLabel       = new JLabel("RCP");
  
  int           tfWidth        = 8;
  
  DecimalFormat decimalFormat  = new DecimalFormat();
  
  JTextField    lcpSumWTF      = new JTextField(tfWidth);
  JTextField    lcpMeanTF      = new JTextField(tfWidth);
  JTextField    lcpVarTF       = new JTextField(tfWidth);
  
  JTextField    rcpSumWTF      = new JTextField(tfWidth);
  JTextField    rcpMeanTF      = new JTextField(tfWidth);
  JTextField    rcpVarTF       = new JTextField(tfWidth);
  
  
  
  public STXHistogramDisplay(JFrame f, String filename) {
    
    mainFrame = f;
    
    Insets             mainInsets   = new Insets(5, 5, 5, 5);
    GridBagLayout      mainPanelGB  = new GridBagLayout();
    GridBagConstraints mainPanelGBC = new GridBagConstraints();
    
    setLayout(mainPanelGB);
    mainPanelGBC.fill      = GridBagConstraints.HORIZONTAL;
    mainPanelGBC.insets    = mainInsets;

    // Use the current directory as the file selection starting point
    String currentDir = System.getProperty("user.dir");
    if (currentDir != null) {
       openFileChooser.setCurrentDirectory(new File(currentDir));
    }    



    // ----- control menu -----
    
    String[] controls = {"File", "Open", "Save As", "Exit"};
    
    fileCB = new JComboBox(controls);
    
    fileCB.addActionListener(new ActionListener() {
      
      public void actionPerformed(ActionEvent e) {
	
	JComboBox cb = (JComboBox) e.getSource();
	
	if (cb.getSelectedItem() == "Exit") {
	  System.exit(0);
	}
	
	if (cb.getSelectedItem() == "Open") {
	  
	  int returnVal = openFileChooser.showOpenDialog(mainFrame);
	  
	  if (returnVal == JFileChooser.APPROVE_OPTION) {
	    
	    currentFilename =
	      openFileChooser.getSelectedFile().getPath();
	    
	    plot.setTitle(currentFilename);
	    updateDisplay(currentFilename);
	    
	    
	  }
	  
	}
	
	if (cb.getSelectedItem() == "Save As") {
	  
	  int returnVal = saveAsFileChooser.showOpenDialog(mainFrame);
	  
	  if (returnVal == JFileChooser.APPROVE_OPTION) {
	    
	    saveAsFilename =
	      saveAsFileChooser.getSelectedFile().getPath();
	    
	    saveAs(saveAsFilename);
	    
	  }
	  
	}
	
	cb.setSelectedIndex(0); // restore index
	
      }
    });
    
    
    controlPanel.add(fileCB);
    
    mainPanelGBC.gridwidth = 4;
    mainPanelGBC.gridx = 0;
    mainPanelGBC.gridy = 0;
    mainPanelGB.setConstraints(controlPanel, mainPanelGBC);
    add(controlPanel);
    mainPanelGBC.gridwidth = 1; // restore default

    // ----- clear button -----

    clearBT = new JButton("Clear");
    clearBT.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	plot.clear(lcpDataSet);
	plot.clear(rcpDataSet);
	plot.setTitle("");
	lcpSumWTF.setText("");
	rcpSumWTF.setText("");
	lcpMeanTF.setText("");
	rcpMeanTF.setText("");
	lcpVarTF.setText("");
	rcpVarTF.setText("");


      }
    });

    controlPanel.add(clearBT);

    // ----- update button -----

    updateBT = new JButton("Update");
    updateBT.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	plot.setTitle(currentFilename);
	updateDisplay(currentFilename);
      }
    });

    controlPanel.add(updateBT);

    
    // ----- plot -----
    
    plot.setMarksStyle("none");
    plot.setXLabel("Value:   Red=Left Pol   Blue=Right Pol");
    plot.setYLabel("Count");
    
    mainPanelGBC.gridwidth = 5;
    mainPanelGBC.gridx = 0;
    mainPanelGBC.gridy = 1;
    mainPanelGB.setConstraints(plot, mainPanelGBC);
    add(plot);
    mainPanelGBC.gridwidth = 1; // restore default
    
    // ----- statistics panel -----
    
    Insets statInsets   = new Insets(5, 15, 5, 15);
    mainPanelGBC.insets = statInsets;
    
    mainPanelGBC.gridx = 0;
    mainPanelGBC.gridy = 2;
    mainPanelGB.setConstraints(statLabel, mainPanelGBC);
    add(statLabel);
    
    mainPanelGBC.gridx = 1;
    mainPanelGBC.gridy = 2;
    mainPanelGB.setConstraints(sumWLabel, mainPanelGBC);
    add(sumWLabel);
    
    mainPanelGBC.gridx = 2;
    mainPanelGBC.gridy = 2;
    mainPanelGB.setConstraints(meanLabel, mainPanelGBC);
    add(meanLabel);
    
    mainPanelGBC.gridx = 3;
    mainPanelGBC.gridy = 2;
    mainPanelGB.setConstraints(varLabel, mainPanelGBC);
    add(varLabel);
    
    mainPanelGBC.gridx = 0;
    mainPanelGBC.gridy = 3;
    mainPanelGB.setConstraints(lcpLabel, mainPanelGBC);
    add(lcpLabel);
    
    lcpSumWTF.setDocument(new FormattedDocument(decimalFormat));
    mainPanelGBC.gridx = 1;
    mainPanelGBC.gridy = 3;
    mainPanelGB.setConstraints(lcpSumWTF, mainPanelGBC);
    add(lcpSumWTF);
    
    lcpMeanTF.setDocument(new FormattedDocument(decimalFormat));
    mainPanelGBC.gridx = 2;
    mainPanelGBC.gridy = 3;
    mainPanelGB.setConstraints(lcpMeanTF, mainPanelGBC);
    add(lcpMeanTF);
    
    lcpVarTF.setDocument(new FormattedDocument(decimalFormat));
    mainPanelGBC.gridx = 3;
    mainPanelGBC.gridy = 3;
    mainPanelGB.setConstraints(lcpVarTF, mainPanelGBC);
    add(lcpVarTF);
    
    mainPanelGBC.gridx = 0;
    mainPanelGBC.gridy = 4;
    mainPanelGB.setConstraints(rcpLabel, mainPanelGBC);
    add(rcpLabel);
    
    rcpSumWTF.setDocument(new FormattedDocument(decimalFormat));
    mainPanelGBC.gridx = 1;
    mainPanelGBC.gridy = 4;
    mainPanelGB.setConstraints(rcpSumWTF, mainPanelGBC);
    add(rcpSumWTF);
    
    rcpMeanTF.setDocument(new FormattedDocument(decimalFormat));
    mainPanelGBC.gridx = 2;
    mainPanelGBC.gridy = 4;
    mainPanelGB.setConstraints(rcpMeanTF, mainPanelGBC);
    add(rcpMeanTF);
    
    rcpVarTF.setDocument(new FormattedDocument(decimalFormat));
    mainPanelGBC.gridx = 3;
    mainPanelGBC.gridy = 4;
    mainPanelGB.setConstraints(rcpVarTF, mainPanelGBC);
    add(rcpVarTF);
    
    mainPanelGBC.insets    = mainInsets; // restore default
    

    if (filename != "") {
       currentFilename = filename;
       updateDisplay(currentFilename);
       plot.setTitle(currentFilename);
    }

    
  }
  
  
  public void updateDisplay (String filename) {
    
    plot.clear(lcpDataSet);
    plot.clear(rcpDataSet);
    
    lcpData.clear();
    rcpData.clear();
    
    try {
      
      BufferedReader is = new BufferedReader(new FileReader(filename));
      String         line;
      
      int index, lcp, rcp;
      Pattern p = Pattern.compile("\\d+");

      while ((line = is.readLine()) != null) {

	Matcher m = p.matcher(line);

	while (m.find()) {

	  index = Integer.parseInt(m.group());

	  if (m.find())
	    lcpData.add(index, new Integer(m.group()));
	  
	  if (m.find())
	    rcpData.add(index, new Integer(m.group()));

	}
	
      }
      
      is.close();
      

    } catch (Exception e) {
	JOptionPane.showMessageDialog(mainFrame, 
				      "Unexpected Error: " + e,
				      "Update Display Error",
				      JOptionPane.ERROR_MESSAGE
				      );
    }
    

    Vector lcpDataWrap128 = new Vector();
    Vector rcpDataWrap128 = new Vector();
    
    // ----- order 2s complement data in "normal" form -----
    
    for (int i = lcpData.size()/2, j = 0; i < lcpData.size() && 
	   i < rcpData.size(); i++, j++) {
      lcpDataWrap128.add(j, lcpData.get(i));
      rcpDataWrap128.add(j, rcpData.get(i));
    }
    
    for (int i = 0, j = lcpData.size()/2; i < lcpData.size()/2 && 
	   i < rcpData.size()/2; i++, j++) {
      lcpDataWrap128.add(j, lcpData.get(i));
      rcpDataWrap128.add(j, rcpData.get(i));
    }
    
    
    // ----- plot normalized data -----
    
    for (int i = 0; i < lcpDataWrap128.size() && 
	   i < rcpDataWrap128.size(); i++) {
      
      int index = i - lcpData.size()/2;
      
      Integer lcp = (Integer) lcpDataWrap128.get(i);
      Integer rcp = (Integer) rcpDataWrap128.get(i);
      
      plot.addPoint(lcpDataSet, index, lcp.intValue(), true);
      plot.addPoint(rcpDataSet, index, rcp.intValue(), true);
      
    }
    
    
    // ----- sum w -----
    
    long lcpSumW = 0;
    long rcpSumW = 0;
    
    long lcpSumWX = 0;
    long rcpSumWX = 0;
    
    long lcpSumWX2 = 0;
    long rcpSumWX2 = 0;
    
    for (int i = 0; i < lcpDataWrap128.size() && 
	   i < rcpDataWrap128.size(); i++) {
      
      Integer lcp = (Integer) lcpDataWrap128.get(i);
      Integer rcp = (Integer) rcpDataWrap128.get(i);
      
      lcpSumW += lcp.intValue();
      rcpSumW += rcp.intValue();
      
      lcpSumWX += lcp.intValue() * (i - lcpData.size()/2);
      rcpSumWX += rcp.intValue() * (i - rcpData.size()/2);
      
      long x2 = (i - lcpData.size()/2) * (i - lcpData.size()/2);
      
      lcpSumWX2 += lcp.intValue() * x2;
      rcpSumWX2 += rcp.intValue() * x2;
      
    }
    
    lcpSumWTF.setText(decimalFormat.format(lcpSumW));
    rcpSumWTF.setText(decimalFormat.format(rcpSumW));

    double lcpAve = 0;
    double rcpAve = 0;
    
    double lcpVar = 0;
    double rcpVar = 0;


    // TBD try block, catch divide by 0 
    
    if (lcpSumW != 0) {
	lcpAve = (double) lcpSumWX/ (double) lcpSumW;
    }

    if (rcpSumW != 0) {
      rcpAve = (double) rcpSumWX/ (double) rcpSumW;
    }

    double lcpDiffMeanSum = 0;
    double rcpDiffMeanSum = 0;
    
    
    for (int i = 0; i < lcpDataWrap128.size() && 
	     i < rcpDataWrap128.size(); i++) {
	
	Integer lcp = (Integer) lcpDataWrap128.get(i);
	Integer rcp = (Integer) rcpDataWrap128.get(i);
	
	double lcpMeanX = (i - lcpData.size()/2) - lcpAve;
	double rcpMeanX = (i - rcpData.size()/2) - rcpAve;
	
	lcpDiffMeanSum += (double) lcp.intValue() * lcpMeanX * lcpMeanX;
	rcpDiffMeanSum += (double) rcp.intValue() * rcpMeanX * rcpMeanX;
	
    }
    
    if (lcpSumW != 0) {
	lcpVar = lcpDiffMeanSum / (double) lcpSumW;
    }

    if (rcpSumW != 0) {
	rcpVar = rcpDiffMeanSum / (double) rcpSumW;
    }
    
    lcpMeanTF.setText(decimalFormat.format(lcpAve));
    rcpMeanTF.setText(decimalFormat.format(rcpAve));
    
    lcpVarTF.setText(decimalFormat.format(lcpVar));
    rcpVarTF.setText(decimalFormat.format(rcpVar));
    
  }

  public void saveAs (String filename) {

    try {

      BufferedOutputStream os = 
	new BufferedOutputStream(new FileOutputStream(saveAsFilename));

      String sOut;
      
      for (int i = 0; i < lcpData.size() && i < rcpData.size(); i++) {

	sOut = i + " " + lcpData.get(i) + " " + 
	  rcpData.get(i) + "\n";

	os.write(sOut.getBytes());

      }

      os.flush();

    } catch (Exception e) {
	JOptionPane.showMessageDialog(mainFrame, 
				      "Unexpected IO Error: " + e,
				      "Save As Error",
				      JOptionPane.ERROR_MESSAGE
				      );
    }

  }  
  
  public static void main(String args[]) {
    
    JFrame  mainFrame = new JFrame("STX Histogram");

    // optional data filename argument
    String filename = "";
    if (args.length > 0) {
       filename = args[0];
    }
    
    mainFrame.getContentPane().add(new STXHistogramDisplay(mainFrame,
							   filename));
    
    mainFrame.pack();
    mainFrame.setVisible(true);
    
    mainFrame.addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
	System.exit(0);
      }
    });
    
    
  }
  
}