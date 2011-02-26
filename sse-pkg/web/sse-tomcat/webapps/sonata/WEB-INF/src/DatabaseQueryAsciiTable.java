/*******************************************************************************

 File:    DatabaseQueryAsciiTable.java
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

import java.io.*;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.*;
import java.sql.*;
import javax.servlet.*;
import javax.servlet.http.*;

/*
   Execute a database query, putting the output in an ascii table.
   Input parameters:
      dbHost: database host
      dbName: database name
      query: query text
      separator: string used as column separator (comma, etc)
      showQuery: echo the query (optional, use any value to enable)
*/

public class DatabaseQueryAsciiTable extends DatabaseAccess {

   public void doGet(HttpServletRequest request, HttpServletResponse response)
      throws ServletException, java.io.IOException  {
      Connection conn = null;
      Statement stmt = null;
      ResultSet rs = null;
      ResultSetMetaData rsm = null;

      response.setContentType("text/html");
      java.io.PrintWriter out = response.getWriter();

      try {
	 String dbHost = request.getParameter("dbHost");
	 String dbName = request.getParameter("dbName");
	 String showQuery = request.getParameter("showQuery");
         String separator = request.getParameter("separator");

         conn = connect(dbHost, dbName);

	 String query = request.getParameter("query");
	 stmt = conn.createStatement();
	 rs = stmt.executeQuery(query);
	 rsm = rs.getMetaData();
	 int colCount = rsm.getColumnCount();

         if (showQuery != null) {
            out.println(query);
         }
         out.println("<br>");
         out.println("<pre>");

	 // print column names
         out.println("");
	 for (int i = 1; i <=colCount; ++i) {
	    out.print(rsm.getColumnLabel(i));

            // print separator between fields, not at the end
            if (i < colCount) {
               out.print(separator);
            }
	 }
         out.println("");

	 // print values 
         int rowCount = 0;
	 while(rs.next()) {
            rowCount++;
	    
	    for (int i = 1;  i <=colCount; ++i) {
	       out.print(rs.getString(i));

               // print separator between fields, not at the end
               if (i < colCount) {
                  out.print(separator);
               }
	    }
            out.println("");
	 }
         //out.println("# " + rowCount + " rows.");
         out.println("</pre>");

      }
      catch (Exception e) {

	 throw new ServletException(e);

      } finally {

         try {

            // closing the statement also closes the result set
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