<%--

 File:    dbQueryTable.jsp
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
<%@ taglib prefix="sql" uri="http://java.sun.com/jsp/jstl/sql" %>

<%--
Execute the database query in the 'query' parm
and display results as an HTML table.
Assumes database dataSource is set to 'dbSource'.
--%>

<sql:query var="queryResult" dataSource="${dbSource}"
   sql="${param.query}" />

<table border='1'>
   <tr>
      <c:forEach items="${queryResult.columnNames}" var="colName">
         <th>${fn:escapeXml(colName)}</th>
      </c:forEach>
   </tr>
   <c:forEach items="${queryResult.rowsByIndex}" var="row">
      <tr>
         <c:forEach items="${row}" var="column">
            <td>${fn:escapeXml(column)}</td>
         </c:forEach>
      </tr>
   </c:forEach>
</table>