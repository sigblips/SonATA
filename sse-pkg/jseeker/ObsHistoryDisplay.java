/*******************************************************************************

 File:    ObsHistoryDisplay.java
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

// Observation history display.
// Read obs history from database,
// and plot the targets in ra & dec.

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.awt.geom.*;
import java.io.*;
import java.text.*;
import javax.swing.filechooser.*;
import java.awt.print.PrinterJob;
import java.awt.print.*;
import ptolemy.plot.*;
import java.sql.*;

public class ObsHistoryDisplay extends JApplet implements ReadoutListener {

   // ObsHistoryDisplay instance data
   JFrame frame;
   String filename;
   JLabel currentFileText;
   PrinterJob printJob;
   ReadoutPlot readoutPlot;  
   JLabel rfCenterFreqValueLabel;
   JLabel bandwidthValueLabel;
   JLabel polValueLabel;
   JLabel subbandFreqReadoutValueLabel;
   JLabel activityIdValueLabel;
   JLabel cursorRaReadoutValueLabel;
   JLabel cursorDecReadoutValueLabel;
   JLabel halfFrameNumberValueLabel;
   boolean slowPlay;
   boolean plotTargetCatOnly;
   double[] accumBaselineValue;
   int baselineCount;
   static Connection dbConnection = null;
   String databaseHost;
   String databaseName;

   public ObsHistoryDisplay(JFrame frame, String filename,
			    boolean slowPlay,
			    boolean useTimeAvg,
                            boolean plotTargetCatOnly,
			    String databaseHost,
			    String databaseName)
   {
      this.frame = frame;
      this.filename = filename;
      this.slowPlay = slowPlay;
      this.plotTargetCatOnly = plotTargetCatOnly;
      this.databaseHost = databaseHost;
      this.databaseName = databaseName;
   }
   
   private void openDatabaseConnection() {

      String driver = "com.mysql.jdbc.Driver";
      String user = "nss";
      String password = "";
      String url = "jdbc:mysql://" + databaseHost + "/"
	 + databaseName;
      
      try
      {
	 Class.forName(driver);
	 dbConnection = DriverManager.getConnection(url, user, password);
      }
      catch (ClassNotFoundException cnfe)
      {
	 System.err.println(cnfe);
	 System.exit(1);
      } 
      catch (SQLException sqle)
      {
	 System.err.println(sqle);
	 System.exit(1);
      }

   }

   private void clearThePlot() {
      int datasetIndex = 0;
      readoutPlot.clear(datasetIndex);
   }
   

   private void displayActId(int actId)
   {
      activityIdValueLabel.setText(Integer.toString(actId));
   }
   
   private void plotTargets() {

      // select all targets that have been observed.
      // get them in time order, by sorting on activity id.

      clearThePlot();
      int datasetIndex = 0;
      boolean connectPoints = false;
      String sqlStmt = "";

      if (plotTargetCatOnly)
      {
	 sqlStmt = 
	    "select TargetCat.targetId, "
	    + "TargetCat.ra2000Hours, TargetCat.dec2000Deg "
	    + "from TargetCat "
            //+ " where "
            //   + " TargetCat.whichlist = 'GS'"  // gal survey only
	 // + " and TargetCat.primaryTargetId = 67000" // one grid section only
            ;
      }
      else
      {
	 sqlStmt = 
	 //"select distinct ActivityUnits.targetId, "
	 "select ActivityUnits.activityId, "
	 + "ActivityUnits.targetId, "
	 + "TargetCat.ra2000Hours, TargetCat.dec2000Deg "
	 + "from ActivityUnits, TargetCat where "
	 + "ActivityUnits.targetId = TargetCat.targetId"
	 + " and ActivityUnits.validObservation = 'Yes'"
            // + " and TargetCat.whichlist = 'GS'"  // gal survey only
	 + " order by ActivityUnits.activityId";

      }

      try 
      {
	 Statement stmt = dbConnection.createStatement();
	 ResultSet resultSet = stmt.executeQuery(sqlStmt);

	 int actUnitCount = 0;
	 while (resultSet.next())
	 {
	    actUnitCount++;

	    if (!plotTargetCatOnly)
	    {
	       int actId = resultSet.getInt("activityId");
	       displayActId(actId);

	       //if (actId > 12000) break;
	    }

	    int targetId = resultSet.getInt("targetId");
	    double ra2000Hours = resultSet.getDouble("ra2000Hours");
	    double dec2000Deg = resultSet.getDouble("dec2000Deg");
	    
	    //System.out.println(targetId + " " + ra2000Hours + " " +
	    //		       dec2000Deg);

	    readoutPlot.addPoint(datasetIndex, ra2000Hours, dec2000Deg,
				 connectPoints);

	    // pause between N actUnits
	    //int actUnitSpacing = 90;
	    int actUnitSpacing = 50;
	    if (slowPlay && (actUnitCount % actUnitSpacing == 0)) {
	       sleep2();
	    }

	 }

	 System.out.println("plotted " + actUnitCount + " activityUnit db entries");

      }
      catch (SQLException sqlExcept)
      {
	 System.err.println(sqlExcept);
	 System.err.println(sqlStmt);
      }

   }

   private void sleep2()
   {
      try {
	 //System.out.println("sleeping");
	 int latencyMs = 25; 

	 Thread.sleep(latencyMs);
      }
	 catch (InterruptedException e) {
	    // Interrupt may be thrown manually by stop()
	 }
   }
   

   // plot readout Listener interface
   public void readoutData(ReadoutPlot source, 
			   double xPlotValue,  
			   double yPlotValue)  
   {
      //System.out.println("x=" + xPlotValue + " y=" + yPlotValue);

      double raHours = xPlotValue; 
      double decDeg = yPlotValue;

      DecimalFormat raFormatter = new DecimalFormat("00.000000 Hours");
      DecimalFormat decFormatter = new DecimalFormat("00.000000 Deg  ");

      cursorRaReadoutValueLabel.setText(raFormatter.format(raHours));
      cursorDecReadoutValueLabel.setText(decFormatter.format(decDeg));

   }

   class OpenFileListener implements ActionListener {
      JFileChooser fc;

      OpenFileListener(JFileChooser fc) {
	 this.fc = fc;
      }
	
      public void actionPerformed(ActionEvent e) {
	    
	 int returnVal = fc.showOpenDialog(ObsHistoryDisplay.this);
	    
	 if (returnVal == JFileChooser.APPROVE_OPTION) {

	    File file = fc.getSelectedFile();
	    String fullpath = file.getAbsolutePath();

	    changeDisplayedFilename(file.getName());

	    //System.out.println("opened fullpath: " + fullpath);
	    //System.out.println("opened getname: " + file.getName());


	 } else {
	    //System.out.println("Open command cancelled by user.");
	 }
      }
   }

    
   class SaveAsJpegListener implements ActionListener {

      JFileChooser fc;

      SaveAsJpegListener(JFileChooser fc) {
	 this.fc = fc;
      }
	
      public void actionPerformed(ActionEvent e) {

	 int returnVal = fc.showSaveDialog(ObsHistoryDisplay.this);

	 if (returnVal == JFileChooser.APPROVE_OPTION) {

	    File file = fc.getSelectedFile();
	    String absoluteFilename = file.getAbsolutePath();

	    //try {

	       // do something here
		    
	    //}
	    //catch (IOException ioe) {
	    //   System.out.println(ioe);
	    //}


	 } else {
	    //System.out.println("Open command cancelled by user.");
	 }
      }
   }



   class PrintListener implements ActionListener {

      public void actionPerformed(ActionEvent e) {
	 //PrinterJob printJob = PrinterJob.getPrinterJob();

	 PageFormat pageFormat = printJob.defaultPage();
	 pageFormat.setOrientation(PageFormat.LANDSCAPE);
	 //printJob.setPrintable(imagePanel, pageFormat);

	 if (printJob.printDialog()) {
	    try {
	       printJob.print();  
	    } catch (Exception ex) {
	       ex.printStackTrace();
	    }
	 }
      }
   }


   // filename filter that accepts directories and files with a particular
   // extension.
   class CustomFilenameFilter extends javax.swing.filechooser.FileFilter {

      String fileExtension;    // eg, "jpeg"
      String fileDescription;  // eg, "*.jpeg (jpeg images)

      CustomFilenameFilter(String fileExtension, String fileDescription)
      {
	 this.fileExtension = fileExtension;
	 this.fileDescription = fileDescription;
      }

      public String getExtension(File f) {
	 String ext = null;
	 String s = f.getName();
	 int i = s.lastIndexOf('.');

	 if (i > 0 &&  i < s.length() - 1) {
	    ext = s.substring(i+1).toLowerCase();
	 }
	 return ext;
      }
    
      // Accept all directories and all files with "." + fileExtension.
      public boolean accept(File f) {


	 if (f.isDirectory()) {
	    return true;
	 }

	 String extension = getExtension(f);
	 if (extension != null) {
	    if (extension.equals(fileExtension)) {
	       return true;
	    } else {
	       return false;
	    }
	 }
	    
	 return false;
      }

      // The description of this filter
      public String getDescription() {
	 return fileDescription;
      }
   }


   class OffsetSliderListener implements ChangeListener {
      public void stateChanged(ChangeEvent e) {
	 JSlider source = (JSlider)e.getSource();
	 if (! source.getValueIsAdjusting()) {
	    //if ( source.getValueIsAdjusting()) {
	    float value = (float)source.getValue();
	    //System.out.println("slider value: " + value);

	    // TBD take action here
	 }    
      }
   }

   class ScaleSliderListener implements ChangeListener {
      public void stateChanged(ChangeEvent e) {
	 JSlider source = (JSlider)e.getSource();
	 if (! source.getValueIsAdjusting()) {
	    //if ( source.getValueIsAdjusting()) {
	    float value = (float)source.getValue() / 100;
	    //System.out.println("slider value: " + value);

	    // tbd take action here

	 }    
      }
   }


   private void changeDisplayedFilename(String newname)
   {
      currentFileText.setText(newname);
   }


   // Get the sse archive directory.
   // If the SSE_ARCHIVE env var is set, use that,
   // else use the default $HOME/sonata_archive.

   private String getArchiveDir()
   {
      EnvReader env = new EnvReader();
      String archiveDir = env.getEnvVar("SSE_ARCHIVE"); 
      if (archiveDir == null)
      {
	 archiveDir = env.getEnvVar("HOME") + "/sonata_archive"; 
      }
      return archiveDir;
   }

   public void plotTargetData()
   {
      openDatabaseConnection();

      plotTargets();
   }

   public void init() {

      readoutPlot = new ReadoutPlot();
      readoutPlot.addReadoutListener(this);
      readoutPlot.setTitle("Observed Targets");
      readoutPlot.setXLabel("RA (Hours)");
      readoutPlot.setYLabel("Dec (Deg)");
      readoutPlot.setXRange(0, 24);     // Show all RA hours
      readoutPlot.setYRange(-35, 90);  // Dec ranges (deg) for ATA
      //readoutPlot.setXRange(17.4, 18.1);   // gal plane RA hours
      //readoutPlot.setYRange(-34, -24);  // gal plane dec
      readoutPlot.setMarksStyle("points");
      readoutPlot.setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));

      JPanel controlPanel = new JPanel();
      //controlPanel.setLayout(new FlowLayout(FlowLayout.LEFT));

      JMenuBar menuBar = new JMenuBar();
      setJMenuBar(menuBar);

      JMenu fileMenu = new JMenu("File");
      menuBar.add(fileMenu);


      JFileChooser openFileChooser = new JFileChooser();

      // get the location of the confirmation data in the
      // archive data directory
      String confirmDataDir = getArchiveDir() + "/confirmdata";
      openFileChooser.setCurrentDirectory(new File(confirmDataDir));
      CustomFilenameFilter baselineFilenameFilter = 
	 new CustomFilenameFilter("baseline", "*.baseline (NSS binary baselines)");
      openFileChooser.setFileFilter(baselineFilenameFilter);

      JMenuItem openFileMenuItem =  new JMenuItem("Open File ...");
      fileMenu.add(openFileMenuItem);
      openFileMenuItem.addActionListener(new OpenFileListener(openFileChooser));


      class RereadListener implements ActionListener {
	 public void actionPerformed(ActionEvent e) {
	    // TBD take action here
	 }
      }
      JMenuItem rereadFileMenuItem =  new JMenuItem("Reread File");
      fileMenu.add(rereadFileMenuItem);
      rereadFileMenuItem.addActionListener(new RereadListener());


      JMenuItem printFileMenuItem =  new JMenuItem("Print ...");
      //fileMenu.add(printFileMenuItem);
      printFileMenuItem.addActionListener(new PrintListener());
      printJob = PrinterJob.getPrinterJob();


      JMenuItem saveAsJpegFileMenuItem =  new JMenuItem("Save As JPeg ...");
      JFileChooser jpegFileChooser = new JFileChooser();
      CustomFilenameFilter jpegFilenameFilter = 
	 new CustomFilenameFilter("jpeg", "*.jpeg (jpeg images)");
      jpegFileChooser.setFileFilter(jpegFilenameFilter);
      jpegFileChooser.setDialogTitle("Save Display as a JPEG image...");
      jpegFileChooser.setDialogType(JFileChooser.SAVE_DIALOG);
      saveAsJpegFileMenuItem.addActionListener(new SaveAsJpegListener(jpegFileChooser));
      //fileMenu.add(saveAsJpegFileMenuItem);


      class ExitListener implements ActionListener {
	 public void actionPerformed(ActionEvent e) {
	    System.exit(0);
	 }
      }

      JMenuItem exitMenuItem = new JMenuItem("Exit");
      exitMenuItem.addActionListener(new ExitListener());
      fileMenu.addSeparator();
      fileMenu.add(exitMenuItem);

      JMenu viewMenu = new JMenu("View");
      menuBar.add(viewMenu);

      // Clear Plot
      class ClearPlotListener implements ActionListener {
	 public void actionPerformed(ActionEvent e) {
	    // TBD take action here
	 }
      }
      JMenuItem clearPlotMenuItem =  new JMenuItem("Clear Plot");
      viewMenu.add(clearPlotMenuItem);
      clearPlotMenuItem.addActionListener(new ClearPlotListener());

      // Rescale Plot
      class RescalePlotListener implements ActionListener {
	 public void actionPerformed(ActionEvent e) {
	    readoutPlot.fillPlot();   // restore the plot to normal (unzoomed) scale
	 }
      }
      JMenuItem rescalePlotMenuItem =  new JMenuItem("Rescale Plot");
      viewMenu.add(rescalePlotMenuItem);
      rescalePlotMenuItem.addActionListener(new RescalePlotListener());



/*
  JButton resetButton =  new JButton("reset");
  controlPanel.add(resetButton);
  class ResetListener implements ActionListener {
  public void actionPerformed(ActionEvent e) {
  pixelOffsetFactor = 0;
  pixelScaleFactor = 100;
  }
  }
  resetButton.addActionListener(new ResetListener());
*/

      JSlider contrastSlider = new JSlider(JSlider.HORIZONTAL, 0, 1000, 100);
      contrastSlider.setBorder(new TitledBorder("Pixel Multiplier %"));
      contrastSlider.setMajorTickSpacing(200);
      contrastSlider.setPaintTicks(true);
      contrastSlider.setPaintLabels(true);
      contrastSlider.addChangeListener(new ScaleSliderListener());
      controlPanel.add(contrastSlider);


      JSlider offsetSlider = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
      offsetSlider.setBorder(new TitledBorder("Pixel Offset"));
      offsetSlider.setMajorTickSpacing(50);
      offsetSlider.setPaintTicks(true);
      offsetSlider.setPaintLabels(true);
      offsetSlider.addChangeListener(new OffsetSliderListener());
      controlPanel.add(offsetSlider);

      JPanel baselineControlPanel = new JPanel();
      baselineControlPanel.setLayout(new BorderLayout());

      JPanel controlLine1Panel = new JPanel();
      JPanel controlLine2Panel = new JPanel();
      baselineControlPanel.add(BorderLayout.NORTH, controlLine1Panel);
      baselineControlPanel.add(BorderLayout.CENTER, controlLine2Panel);

/*
      // Pol
      JLabel polTitleLabel = new JLabel("Pol: ");
      polValueLabel = new JLabel("   ");
      controlLine1Panel.add(polTitleLabel);
      controlLine1Panel.add(polValueLabel);


      // RF center freq
      JLabel rfCenterFreqTitleLabel = new JLabel("Center Freq: ");
      rfCenterFreqValueLabel = new JLabel("   ");
      controlLine1Panel.add(rfCenterFreqTitleLabel);
      controlLine1Panel.add(rfCenterFreqValueLabel);
	
      // Bandwidth
      JLabel bandwidthTitleLabel = new JLabel("BW: ");
      bandwidthValueLabel = new JLabel("   ");
      controlLine1Panel.add(bandwidthTitleLabel);
      controlLine1Panel.add(bandwidthValueLabel);
	

      // current filename
      JLabel currentFileLabel = new JLabel("File: ");
      currentFileText = new JLabel("None            ");
      controlLine1Panel.add(currentFileLabel);
      controlLine1Panel.add(currentFileText);
      // display filename, last part only
      changeDisplayedFilename(new File(filename).getName()); 


      // half frame number
      JLabel halfFrameNumberTitleLabel = new JLabel("Half Frame #: ");
      halfFrameNumberValueLabel = new JLabel("   ");
      controlLine2Panel.add(halfFrameNumberTitleLabel);
      controlLine2Panel.add(halfFrameNumberValueLabel);
	
	
      // cursor readout values
      // subband freq
      JLabel subbandFreqReadoutTitleLabel = new JLabel("Cursor Freq: ");
      subbandFreqReadoutValueLabel = new JLabel("   ");
      controlLine2Panel.add(subbandFreqReadoutTitleLabel);
      controlLine2Panel.add(subbandFreqReadoutValueLabel);

*/

      // activity id
      JLabel activityIdTitleLabel = new JLabel("Act Id: ");
      activityIdValueLabel = new JLabel("   ");
      controlLine2Panel.add(activityIdTitleLabel);
      controlLine2Panel.add(activityIdValueLabel);

      // cursor ra
      JLabel cursorRaReadoutTitleLabel = new JLabel("Cursor RA: ");
      cursorRaReadoutValueLabel = new JLabel("   ");
      controlLine2Panel.add(cursorRaReadoutTitleLabel);
      controlLine2Panel.add(cursorRaReadoutValueLabel);

      // cursor dec
      JLabel cursorDecReadoutTitleLabel = new JLabel("Dec: ");
      cursorDecReadoutValueLabel = new JLabel("   ");
      controlLine2Panel.add(cursorDecReadoutTitleLabel);
      controlLine2Panel.add(cursorDecReadoutValueLabel);



      JMenu helpMenu = new JMenu("Help");
      menuBar.add(helpMenu);


      class HelpVersionListener implements ActionListener {
	 public void actionPerformed(ActionEvent e) {
	    JOptionPane.showMessageDialog(
	       frame,
	       "Version: $Revision: 1.11 $ $Date: 2006/11/30 00:12:58 $",
	       "Obs History Display",  // dialog title
	       JOptionPane.INFORMATION_MESSAGE ); 
	 }
      }
      JMenuItem showVersionMenuItem =  new JMenuItem("Show Version...");
      showVersionMenuItem.addActionListener(new HelpVersionListener());
      helpMenu.add(showVersionMenuItem);

      /*
	scrollPane.setVerticalScrollBarPolicy(
	ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
	scrollPane.setHorizontalScrollBarPolicy(
	ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);
      */

      Container cp = getContentPane();
      cp.setLayout(new BorderLayout());

      cp.add(BorderLayout.NORTH, baselineControlPanel);
      cp.add(BorderLayout.CENTER, readoutPlot);

      //cp.add(BorderLayout.NORTH, controlPanel);
      //cp.add(BorderLayout.CENTER, scrollPane);

   }



   private static class CmdLineOptions {

      String filename;
      int xpos;
      int ypos;
      int width;
      int height;
      boolean slowPlay;
      boolean timeAvg;
      boolean showTargetCat;
      String databaseHost;
      String databaseName;

      public CmdLineOptions() {
	 filename = "";
	 xpos = 0;
	 ypos = 0;
	 width = 660;
	 height = 500;
	 slowPlay = false;
	 timeAvg = false;
         showTargetCat = false;
	 databaseHost = "localhost";
	 databaseName = "";
      }

      public String getFilename() {
	 return filename;
      }

      public int getXpos() {
	 return xpos;
      }

      public int getYpos() {
	 return ypos;
      }


      public int getWidth() {
	 return width;
      }

      public int getHeight() {
	 return height;
      }

      public boolean getSlowPlay() {
	 return slowPlay;
      }

      public boolean getShowTargetCat() {
	 return showTargetCat;
      }

      public boolean getTimeAvg() {
	 return timeAvg;
      }

      public String getDatabaseName() {
	 return databaseName;
      }

      public String getDatabaseHost() {
	 return databaseHost;
      }

      public boolean parseCommandLineArgs(String[] args) {

	 int i = 0;
	 while (i < args.length) {

	    if (args[i].equals("-file")) {
	       i++;
	       if (i < args.length) {
		  filename = args[i++];
	       } else {
		  System.out.println("Missing filename argument");
		  return false;
	       }

	    }  else  if (args[i].equals("-dbhost")) {
	       i++;
	       if (i < args.length) {
		  databaseHost = args[i++];
	       } else {
		  System.out.println("Missing database host argument");
		  return false;
	       }
		    
	    }  else  if (args[i].equals("-dbname")) {
	       i++;
	       if (i < args.length) {
		  databaseName = args[i++];
	       } else {
		  System.out.println("Missing database name argument");
		  return false;
	       }
		    
	    } else if (args[i].equals("-xpos")) {
			
	       i++;
	       if (i < args.length) {

		  try {
		     xpos = Integer.parseInt(args[i++]);
		  } catch (Exception e) {
		     System.out.println("invalid x position");
		     return false;
		  }

	       } else {
		  System.out.println("Missing xpos argument");
		  return false;
	       }
		    
	    } else if (args[i].equals("-ypos")) {
			
	       i++;
	       if (i < args.length) {

		  try {
		     ypos = Integer.parseInt(args[i++]);
		  } catch (Exception e) {
		     System.out.println("invalid y position");
		     return false;
		  }

	       } else {
		  System.out.println("Missing ypos argument");
		  return false;
	       }
	    } else if (args[i].equals("-width")) { 
			
	       i++;
	       if (i < args.length) {

		  try {
		     width = Integer.parseInt(args[i++]);
		  } catch (Exception e) {
		     System.out.println("invalid width");
		     return false;
		  }

	       } else {
		  System.out.println("Missing width argument");
		  return false;
	       }
	    } else if (args[i].equals("-height")) { 
			
	       i++;
	       if (i < args.length) {

		  try {
		     height = Integer.parseInt(args[i++]);
		  } catch (Exception e) {
		     System.out.println("invalid height");
		     return false;
		  }

	       } else {
		  System.out.println("Missing height argument");
		  return false;
	       }
	    } else if (args[i].equals("-slow")) {
			
	       i++;
	       slowPlay = true;

	    } else if (args[i].equals("-targetcat")) {
			
	       i++;
	       showTargetCat = true;

	    } else if (args[i].equals("-timeavg")) {
			
	       i++;
	       timeAvg = true;

	    } else {
	       System.out.println("invalid option: " + args[i]);
//	       System.out.println("valid options: -file <filename> -xpos <x position> -ypos <y position> -width <width> -height <height> -slow -timeavg -dbhost -dbname");
//	       System.out.println("-file: NSS baselines filename");
	       System.out.println("-xpos: window x position");
	       System.out.println("-ypos: window y position");
	       System.out.println("-width: window width (default: " + width + ")");
	       System.out.println("-height: window height (default: " + height + ")");
	       System.out.println("-slow: display data slowly");
//	       System.out.println("-timeavg: display data as a time average");
	       System.out.println("-targetcat: plot target catalog");
	       System.out.println("-dbhost: database host");
	       System.out.println("-dbname: database name");
	       return false;
	    } 
	 }

	 return true;
      }

   }




   public static void main(String[] args) {

      CmdLineOptions options = new CmdLineOptions();
      if ( ! options.parseCommandLineArgs(args) ) {
	 System.exit(1);
      }

      JFrame frame = new JFrame("Obs History Display");
      frame.setLocation(new Point(options.getXpos(), options.getYpos()));
	 
      ObsHistoryDisplay obsHistDisp =
	 new ObsHistoryDisplay(frame,
			       options.getFilename(), 
			       options.getSlowPlay(),
			       options.getTimeAvg(),
                               options.getShowTargetCat(),
			       options.getDatabaseHost(),
			       options.getDatabaseName());

      frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
      frame.getContentPane().add(obsHistDisp);
      frame.setSize(options.getWidth(), options.getHeight());
      obsHistDisp.init();
      obsHistDisp.start();
      frame.setVisible(true);

      obsHistDisp.plotTargetData();
      
   }
}