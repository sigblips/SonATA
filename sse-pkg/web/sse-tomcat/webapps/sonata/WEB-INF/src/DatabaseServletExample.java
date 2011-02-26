/*******************************************************************************

 File:    DatabaseServletExample.java
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

import java.sql.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class DatabaseServletExample extends HttpServlet {

  public void doGet(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, java.io.IOException  {
    

     Connection conn = null;
     Statement stmt = null;
     ResultSet rs = null;
     ResultSetMetaData rsm = null;
        
     response.setContentType("text/html");
     java.io.PrintWriter out = response.getWriter();
     out.println("<html><head><title>Typical Database Access</title></head><body>");
     out.println("<h2>Database info</h2>");
     out.println("<table border='1'><tr>");
       
     try {
           
	String driver = "com.mysql.jdbc.Driver";
	String hostname = "sol";
	String databaseName = "tom_iftest";
	String user = "nss";
	String password = "";

	out.println("Database Host: " + hostname 
		    + "  Database: " + databaseName + "<br>");
	   
	String url = "jdbc:mysql://" + hostname + "/"
	   + databaseName;
 
	//load the database driver
	Class.forName(driver);

	//Create the java.sql.Connection to the database
	conn = DriverManager.getConnection(url, user, password);
	
	String tableName = "Antenna";
	out.println("Table: " + tableName);
		   
	//Create a statement for executing some SQL
	String sql = "select * from " + tableName;
	stmt = conn.createStatement();
	rs = stmt.executeQuery(sql);
	rsm = rs.getMetaData();
	int colCount =  rsm.getColumnCount();
            
	//print column names
	for (int i = 1; i <=colCount; ++i) {
	   out.println("<th>" + rsm.getColumnName(i) + "</th>");
	}
            
	out.println("</tr>");
          
	while( rs.next()) {
	   out.println("<tr>");
                
	   for (int i = 1;  i <=colCount; ++i) {
	      out.println("<td>" + rs.getString(i) + "</td>");
	   }
	   out.println("</tr>");
	}
				
	stmt.close();
	conn.close();


     }
     catch (Exception e) {
	throw new ServletException(e);
     }

     out.println("</table><br><br>");

     out.println("</body>");
     out.println("</html>");
    
     out.close();
      
  } //doGet

}