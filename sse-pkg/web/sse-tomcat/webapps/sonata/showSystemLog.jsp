<%--

 File:    showSystemLog.jsp
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

<%--
Purpose:  show the current system log.
--%>

<%@ include file="taglibsHeader.jspf" %>

<html>
<head>
   <title> SonATA Operations: System Log</title>
   <%@ include file="scroll-body-css.jspf" %>
</head>

<body onload=document.location="#contentBottom">

<div id="header">
<%@ include file="logHeader.jspf" %>
<br>
</div>

<div id="contentsWithSubmenu">

System Log:

<jsp:useBean id="fileDate" class="java.util.Date" />
<fmt:formatDate pattern="yyyy-MM-dd" timeZone="UTC" type="date" 
   value="${fileDate}" var="fileDateString" />

<%-- define log filename relative to sonata_archive --%>
<c:set var="filename" value="permlogs/systemlogs/systemlog-${fileDateString}.txt"/>

<c:import url="showLog.jsp">
   <c:param name="logName" value="${filename}"/>
</c:import>

<a name="contentBottom"></a>
</div>

</body>
</html>