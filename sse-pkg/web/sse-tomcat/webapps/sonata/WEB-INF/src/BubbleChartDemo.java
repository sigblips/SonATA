/*******************************************************************************

 File:    BubbleChartDemo.java
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
JFreeChart Bubble Chart Demo
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

public class BubbleChartDemo extends HttpServlet {

   public void doGet(HttpServletRequest request, HttpServletResponse response)
      throws ServletException, java.io.IOException  {

      Connection conn = null; 
      Statement stmt = null;
      ResultSet rs = null;
      ResultSetMetaData rsm = null;
        
      OutputStream out = response.getOutputStream();
      try {

         DefaultXYZDataset dataset = new DefaultXYZDataset();
         double[] x = {2.1, 2.3, 2.3, 2.2, 2.2, 1.8, 1.8, 1.9, 2.3, 3.8};
         double[] y = {14.1, 11.1, 10.0, 8.8, 8.7, 8.4, 5.4, 4.1, 4.1, 25};
         double[] z = {2.4, 2.7, 2.7, 2.2, 2.2, 2.2, 2.1, 2.2, 1.6, 4};
         double[][] series = new double[][] { x, y, z };
         dataset.addSeries("Series 1", series);
	 
         JFreeChart chart = ChartFactory.createBubbleChart(
            "Bubble Chart Demo 1", "X", "Y", dataset,
            PlotOrientation.HORIZONTAL,
            true, true, true);
         XYPlot plot = (XYPlot) chart.getPlot();
         plot.setForegroundAlpha(0.65f);
          
         XYItemRenderer renderer = plot.getRenderer();
         renderer.setSeriesPaint(0, Color.blue);
         
         // increase the margins to account for the fact that the auto-range
         // doesn't take into account the bubble size...
         NumberAxis domainAxis = (NumberAxis) plot.getDomainAxis();
         domainAxis.setLowerMargin(0.15);
         domainAxis.setUpperMargin(0.15);
         NumberAxis rangeAxis = (NumberAxis) plot.getRangeAxis();
         rangeAxis.setLowerMargin(0.15);
         rangeAxis.setUpperMargin(0.15);
         
         response.setContentType("image/png");
         int width = 800;
         int height = 600;
         ChartUtilities.writeChartAsPNG(out, chart, width, height);

      }
      catch (Exception e) {
	 throw new ServletException(e);
      }

      
   } //doGet


}