<%--

 File:    showVars.jsp
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


<%@ include file="taglibsHeader.jspf" %>
<html>
<head>
</head>
<body>

<%-- Loop over the session scope, which is a map --%>

SessionScope:<br>

<table border>
<c:forEach items='${sessionScope}' var='scopeItem'>
   <%-- Display the key of the current item; that item
           is a Map.Entry --%>
   <tr>
   <td><c:out value='${scopeItem.key}'/></td>
   <td><c:out value='${scopeItem.value}'/></td>
   </td>
  </tr>
</c:forEach>
</table>

</body>
</html>
