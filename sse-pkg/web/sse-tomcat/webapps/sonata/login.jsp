<%--

 File:    login.jsp
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

--%>

<html>
<head>
<title>SonATA Operations: Login</title>
</head>
<body>

<%@ include file="header.jspf" %>

SonATA Operations Login:<br><br>

<form method="POST" action="j_security_check">
<table>
  <tr><td>Name: </td><td><input type="text" name="j_username"></td></tr>
  <tr><td>Password: </td><td><input type="password" name="j_password"></td></tr>
  <tr><td> </td><td><input type=submit value="Login"></td></tr>
</table>
</form>      

</body>
</html>