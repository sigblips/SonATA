<%--

 File:    pointingmap.jsp
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
Purpose: Show a map of the most recent ATA beam pointings.
Description:
--%>

<html>
<head>
<title> SonATA Operations: Pointing Map</title>
</head>
<body>
<%@ include file="header.jspf" %>

<%-- Hat Creek Mean Sidereal Time --%>
<jsp:useBean id="hatCreekLmst" class="sonata.beans.LmstBean" />
<fmt:formatNumber var="lmstHours" value="${hatCreekLmst.lmstHours}" 
    minFractionDigits="3" maxFractionDigits="3"/>

<%-- get database configuration information from the config file --%>
<ptags:dbConfig dbHost="host" dbName="name"/>

[${name} on ${host}]
&nbsp;&nbsp;&nbsp;&nbsp;Hat Creek LMST: ${lmstHours} hours.
<br>

<img src="
<c:url value="servlet/PointingMap">
   <c:param name="dbHost" value="${host}"/>
   <c:param name="dbName" value="${name}"/>
</c:url>
">



</body>
</html>