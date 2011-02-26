/*******************************************************************************

 File:    BaselinePlotGenerator.java
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


// Make an XY chart of the NSS baseline data in the 
// given file and return it as an image

import java.io.*;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.*;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.*;
import org.jfree.chart.axis.*;
import org.jfree.chart.title.*;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.category.*;
import org.jfree.data.general.*;
import org.jfree.data.time.*;
import org.jfree.data.xy.*;
import org.jfree.date.*;

public class BaselinePlotGenerator extends HttpServlet {

   public BaselinePlotGenerator() {
   }

   public void doGet(HttpServletRequest request, HttpServletResponse response)
      throws ServletException, IOException {

      OutputStream out = response.getOutputStream();
      NssBaseline nssBaseline = new NssBaseline();
      String filename = "";
      try {

	 filename = request.getParameter("filename");
	 nssBaseline.openFile(filename);
	 nssBaseline.readAllData();

	 JFreeChart chart = null;
	 chart = createChart(filename, nssBaseline);
	 if (chart != null) {
	    response.setContentType("image/png");
	    ChartUtilities.writeChartAsPNG(out, chart, 660, 400);
	 }
      }
      catch (Exception e) {
	 System.err.println("BaselinePlotGenerator: " + e +
	    " for file: " + filename);
      }
      finally {
	 out.close();
	 nssBaseline.closeFile();
      }
   }


   private JFreeChart createChart(String filename, NssBaseline nssBaseline) {

      XYSeries series = new XYSeries("");
      float[] baselineValues = nssBaseline.getBaselineValues();

      // plot subband values
      for (int i = 0; i < baselineValues.length; i++) {
	 series.add(i, baselineValues[i]);
      }

      // add a final point at the end with a zero Y value,
      // to force the y axis to display full scale
      series.add(baselineValues.length, 0.0);

      XYDataset data = new XYSeriesCollection(series);

      String title = "SonATA Baseline";
      String xAxisLabel = "Subband";
      String yAxisLabel = "Power";

      JFreeChart chart = ChartFactory.createXYLineChart(
	 title, xAxisLabel, yAxisLabel,
	 data,
	 PlotOrientation.VERTICAL,  
	 false,  // legend 
	 true,  // tooltips
	 false  // urls
	 );

      String filenameBase = new File(filename).getName();

      String subTitle1 = filenameBase;
      chart.addSubtitle(new TextTitle(subTitle1));

      DecimalFormat freqFormatter = new DecimalFormat("0000.000 MHz  ");
      String freqString = freqFormatter.format(nssBaseline.getCenterFreqMhz());

      DecimalFormat bandwidthFormatter = new DecimalFormat("0.00 MHz  ");
      String bandwidthString = bandwidthFormatter.format(nssBaseline.getBandwidthMhz());

      String subTitle2 = "Center Freq: " + freqString
         + "     Bandwidth: " + bandwidthString;
      chart.addSubtitle(new TextTitle(subTitle2));


      // move the data off of the axes 
      // by extending the minimum axis value

      XYPlot plot = (XYPlot) ((JFreeChart) chart).getPlot(); 
      double axisMarginPercent = 0.02;
      NumberAxis valueAxis = (NumberAxis) plot.getRangeAxis();
      valueAxis.setLowerBound(-1.0 * valueAxis.getUpperBound() * axisMarginPercent);   
      valueAxis = (NumberAxis) plot.getDomainAxis();
      valueAxis.setLowerBound(-1.0 * valueAxis.getUpperBound() * axisMarginPercent);   
      return chart;

   }

   
   // see ssePdmInterface.h for C++ version of this struct
   // (BaselineHeader and BaselineValue structs)
   class NssBaseline {

      int maxSubbands = 10000;  // sanity check

      // header
      double rfCenterFrequency;
      double bandwidth;
      int halfFrameNumber;
      int numberOfSubbands;
      int polarization;  // defined as Polarization enum.  Assumed to be 32bit.
      int alignPad;

      // variable length body
      float[] baselineValue;

      DataInputStream inStream;

      float[] getBaselineValues() {
	 return baselineValue;
      }

      double getCenterFreqMhz() {
	 return rfCenterFrequency;
      }

      double getBandwidthMhz() {
	 return bandwidth;
      }
       
      public void openFile(String filename) throws java.io.IOException
      {
	 inStream = new DataInputStream(
	    new BufferedInputStream(
	       new FileInputStream(filename)));
      }
      
      public void closeFile() throws java.io.IOException
      {
	 inStream.close();
      }

      public void readAllData() throws java.io.IOException {

	 // TBD take time average instead of last baseline in file
	 try {
	    while (inStream != null && (inStream.available() > 0)) {
	       read(inStream);
	    }
	 } catch (EOFException e) {
	    // do nothing
	 }

      }

      public void read(DataInputStream in) throws java.io.IOException {

	 //System.out.println("BaselinePlotGenerator: reading data");

	 // read the header
	 rfCenterFrequency = in.readDouble();
	 bandwidth = in.readDouble();
	 halfFrameNumber = in.readInt();
	 numberOfSubbands = in.readInt();
	 polarization = in.readInt();
	 alignPad = in.readInt();

	 // read the variable length baseline value array
	 if (numberOfSubbands < 1 || numberOfSubbands > maxSubbands)
	 {
	    System.out.println(
	       "ERROR: subbands value " + numberOfSubbands +
	       " out of range.  Must be 1 - " + maxSubbands);
	    //System.exit(1);
		    
	    throw new java.io.IOException();
	 }

	 baselineValue = new float[numberOfSubbands];
	 for (int i=0; i<numberOfSubbands; ++i)
	 {
	    baselineValue[i] = in.readFloat();
	 }

      }

      private String polIntToString(int pol) {
	 String value;
	 switch (pol) {
	 case 0: value ="R";  // right circular
	    break;
	 case 1: value = "L"; // left circular
	    break;
	 case 2: value = "B"; // both
	    break;
	 case 3: value = "M"; // mixed
	    break;
	 default: value = "?";
	    break;
	 }

	 return value;
      }

      public String getPolAsString() {
	 return polIntToString(polarization);
      }


      public void printHeader() {
	 System.out.println(
	    "rfcenterFreq " + rfCenterFrequency + " MHz," +
	    " bandwidth " + bandwidth + " MHz," +
	    " halfFrameNumber " + halfFrameNumber + "," +
	    " #subbands " +  numberOfSubbands + "," +
	    " pol " + polIntToString(polarization)
	    );
	 // alignPad
      }

      public void printBody() {
	 int maxToPrint = 5;
	 System.out.print("baselineValues: ");
	 for (int i=0; i<maxToPrint && i<numberOfSubbands; ++i)
	 {
	    System.out.print(baselineValue[i] + " ");
	 }
	 System.out.println("");

	 //for (int index = 3168; index <= 3175; index++)
	 //{
	 //	System.out.println("bin " + index + "=" + baselineValue[index]);
	 //}

      }

   }

}