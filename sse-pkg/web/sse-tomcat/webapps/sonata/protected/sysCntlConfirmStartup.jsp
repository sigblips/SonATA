<%--

 File:    sysCntlConfirmStartup.jsp
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
Purpose: Confirm runsse startup, selecting startup options.
Description:
--%>

<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>

<html>
<head>
<title> SonATA Operations: System Control Confirm Runsse Startup</title>
</head>
<body>
<%@ include file="/header.jspf" %>

Start Sonata:
<p>

<div id="runSseOptions" style="border-style:groove">
<form action="sysCntlConfirmStartup.jsp" method="post">
<input type="submit" value="Start" name="action">
<input type="submit" value="Cancel" name="action">
</form>
</div>

<%-- 
Prepare a list of startup flags for the runsse script based on the
form settings.
- Simulator mode overrides the other options.
--%>

<c:if test="${param.action == 'Start'}">
   <c:set var="options" value=""/>

   Options: ${options}

   <jsp:forward page="sysCntlDoStartup.jsp">
      <jsp:param name="options" value="${options}" />
   </jsp:forward>

</c:if>

<c:if test="${param.action == 'Cancel'}">
   <c:out value="cancel action"/> 
   <jsp:forward page="/displayMsg.jsp">
      <jsp:param name="msg" value="System control (runsse) startup request cancelled." />
   </jsp:forward>
</c:if>


</body>
</html>