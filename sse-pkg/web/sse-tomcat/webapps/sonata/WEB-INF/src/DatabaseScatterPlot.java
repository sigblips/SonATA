// based on oreilly  "java servlet & jsp cookbook"

/*
   Create a scatter plot PNG image based on the supplied database query.
   First & second query columns are used as X & Y.

   Input parameters:
      dbHost: database host
      dbName: database name
      query: query text
      title: plot title
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

public class DatabaseScatterPlot extends DatabaseAccess {

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
	 String sqlQuery = request.getParameter("query");
         String title = request.getParameter("title");

         if (title == null)
         {
            title = "";
         }

         conn = connect(dbHost, dbName);
	 stmt = conn.createStatement();
	 rs = stmt.executeQuery(sqlQuery);
	 rsm = rs.getMetaData();
	 int colCount =  rsm.getColumnCount();

         String seriesTitle = "";
	 XYSeries series = new XYSeries(seriesTitle);

         int xCol=1;
         int yCol=2;
	 while(rs.next()) {
            series.add(rs.getDouble(xCol), rs.getDouble(yCol));
	 }
	 XYDataset data = new XYSeriesCollection(series);

	 JFreeChart chart = ChartFactory.createScatterPlot(
	    title,  // Title
            rsm.getColumnLabel(xCol), // x-axis label
            rsm.getColumnLabel(yCol), // y-axis label
	    data,
	    PlotOrientation.VERTICAL,  
	    false,  // legend 
	    true,  // tooltips
	    false  // urls
	    );
	 
	 // plot RA hours with higher values to the left
	 //chart.getXYPlot().getDomainAxis().setInverted(true);
	 //chart.getXYPlot().getDomainAxis().setLowerBound(-1);
	 //chart.getXYPlot().getDomainAxis().setUpperBound(25);

	 // increase axis label fonts for better readability
         Font axisFont = new Font("Serif", Font.BOLD, 14);
         chart.getXYPlot().getDomainAxis().setLabelFont(axisFont);
         chart.getXYPlot().getDomainAxis().setTickLabelFont(axisFont);
         chart.getXYPlot().getRangeAxis().setLabelFont(axisFont);
         chart.getXYPlot().getRangeAxis().setTickLabelFont(axisFont);

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