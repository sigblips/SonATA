/*******************************************************************************

 File:    PointingMap.java
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

// based on oreilly  "java servlet & jsp cookbook"

/*
   Create a map of the most recent primary beam pointing.
   Target catalog is plotted first, and then the current 
   primary pointing is shown by crosshairs.

   Input parameters:
      dbHost: database host
      dbName: database name
*/

import java.awt.*;
import java.awt.geom.*;
import java.io.*;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.*;
import java.sql.*;
import javax.servlet.*;
import javax.servlet.http.*;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.*;
import org.jfree.chart.axis.*;
import org.jfree.chart.renderer.xy.*;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.category.*;
import org.jfree.data.general.*;
import org.jfree.data.time.*;
import org.jfree.data.xy.*;
import org.jfree.date.*;

public class PointingMap extends DatabaseAccess {

   public void doGet(HttpServletRequest request, HttpServletResponse response)
      throws ServletException, java.io.IOException  {

      Connection conn = null;
      Statement stmt = null;
      ResultSet rs = null;
      ResultSetMetaData rsm = null;
        
      OutputStream out = response.getOutputStream();
      try {

	 String dbHost = request.getParameter("dbHost");
	 String dbName = request.getParameter("dbName");

         conn = connect(dbHost, dbName);

         // get & plot target catalog
         String targetCatQuery = "select "
            + "ra2000Hours, dec2000Deg "
            + "from TargetCat limit 200";

         stmt = conn.createStatement();
         rs = stmt.executeQuery(targetCatQuery);
         rsm = rs.getMetaData();
         //int colCount = rsm.getColumnCount();
         
         XYSeries series = new XYSeries("Target Catalog");
         int raColIndex = 1;
         int decColIndex = 2;
         while(rs.next()) {
            series.add(rs.getDouble(raColIndex), rs.getDouble(decColIndex));
         }

         XYDataset data = new XYSeriesCollection(series);
         stmt.close();

         // Get latest primary beam pointing position
         String latestPointingQuery = "select "
            + "actId, ts, "
            + "raHours, decDeg "
            + "from TscopePointReq "
            + "where atabeam = 'primary' "
            + "order by actId desc limit 1";
            
	 stmt = conn.createStatement();
	 rs = stmt.executeQuery(latestPointingQuery);
	 rsm = rs.getMetaData();
	 //int colCount = rsm.getColumnCount();

	 int actId = -1;
	 String timeString = "";
         double pointingRaHours = -1;
         double pointingDecDeg = -1;

         int actIdIndex = 1;
         int timeIndex = 2;
         raColIndex = 3;
         decColIndex = 4;

	 while(rs.next()) {
	    actId = rs.getInt(actIdIndex);
	    timeString = rs.getString(timeIndex);
            pointingRaHours = rs.getDouble(raColIndex);
            pointingDecDeg = rs.getDouble(decColIndex);
	 }

	 String plotTitle = "ATA Primary Pointing" 
	    + " (Act Id: " +  actId + ")"
	    + " " + timeString;

	 JFreeChart chart = ChartFactory.createScatterPlot(
	    plotTitle,
	    "RA (Hours)",  // x-axis label
	    "Dec (Deg)",  // y axis label
	    data,
	    PlotOrientation.VERTICAL,  
	    false,  // legend 
	    true,  // tooltips
	    false  // urls
	    );
	 
	 // plot RA hours with higher values to the left
	 //chart.getXYPlot().getDomainAxis().setInverted(true);
	 chart.getXYPlot().getDomainAxis().setLowerBound(-1);
	 chart.getXYPlot().getDomainAxis().setUpperBound(25);

	 // increase axis label fonts for better readability
         Font axisFont = new Font("Serif", Font.BOLD, 14);
         chart.getXYPlot().getDomainAxis().setLabelFont(axisFont);
         chart.getXYPlot().getDomainAxis().setTickLabelFont(axisFont);
         chart.getXYPlot().getRangeAxis().setLabelFont(axisFont);
         chart.getXYPlot().getRangeAxis().setTickLabelFont(axisFont);

	 // show current pointing as crosshairs
         chart.getXYPlot().setDomainCrosshairValue(pointingRaHours);
         chart.getXYPlot().setRangeCrosshairValue(pointingDecDeg);
         chart.getXYPlot().setDomainCrosshairVisible(true);
         chart.getXYPlot().setRangeCrosshairVisible(true);
         chart.getXYPlot().setDomainCrosshairPaint(Color.BLACK);
         chart.getXYPlot().setRangeCrosshairPaint(Color.BLACK);

         Stroke stroke = new BasicStroke(2);
         chart.getXYPlot().setDomainCrosshairStroke(stroke);
         chart.getXYPlot().setRangeCrosshairStroke(stroke);

	 // set hat creek dec range
	 chart.getXYPlot().getRangeAxis().setLowerBound(-40);
	 chart.getXYPlot().getRangeAxis().setUpperBound(90);

	 XYLineAndShapeRenderer renderer = (XYLineAndShapeRenderer)
	    chart.getXYPlot().getRenderer(); 
         int seriesIndex=0;
	 renderer.setSeriesPaint(seriesIndex,Color.BLUE);

	 Shape circularShape = new Ellipse2D.Double(-1.0, -1.0, 1.2, 1.2);
	 renderer.setSeriesShape(seriesIndex, circularShape);

	 // Default shape [0-9]: 0=square 1=circle 2=uptriangle 3=diamond...
         //renderer.setShape(DefaultDrawingSupplier.DEFAULT_SHAPE_SEQUENCE[1]);
	 response.setContentType("image/png");
	 int width = 800;
	 int height = 600;
	 ChartUtilities.writeChartAsPNG(out, chart, width, height);

      }
      catch (Exception e) {
	 throw new ServletException(e);
      }
      finally {

         try {
            if (stmt != null) {
               stmt.close();
            }
            
            if (conn != null) {
               conn.close();
            }
            
         }
         catch (SQLException sql) {}
      
      }

   } //doGet

}