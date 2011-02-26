/*******************************************************************************

 File:    DatabaseAccess.java
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

// base class for accessing database

public class DatabaseAccess extends HttpServlet {

   protected Connection connect(String dbHost, 
                                String dbName) throws ServletException {

      String user = System.getProperty("user.name");
      String password = "";
      String url = "jdbc:mysql://" + dbHost + "/" + dbName + 
	      "?autoReconnect=true&characterEncoding=UTF-8";

      try {
	 
	 //load the database driver
	 String driver = "com.mysql.jdbc.Driver";
	 Class.forName(driver);
	 
	 // open the database connection
	 Connection conn = DriverManager.getConnection(url, user, password);
	 return conn;
      }
      catch (ClassNotFoundException e) {
	 throw new UnavailableException("Could not load database driver");
      }
      catch (SQLException e) {
	 throw new UnavailableException(e.getMessage() + "\nUser=" + user + "\n" +  "Could not connect to database: "
            + "dbHost: '" + dbHost + "' dbName: '" + dbName + "'");
      }
   } 

}