<%--

 File:    sysCntlConfirmShutdown.jsp
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

<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>

<html>
<head>
<title> SonATA Operations: System Control Confirm Runsse Shutdown</title>
</head>
<body>
<%@ include file="/header.jspf" %>

Please confirm runsse *shutdown*:

<form action="sysCntlConfirmShutdown.jsp" method="post">
<input type="submit" value="OK" name="action">
<input type="submit" value="Cancel" name="action">
</form>

<c:if test="${param.action == 'OK'}">
 <jsp:forward page="sysCntlDoShutdown.jsp"/>
</c:if>

<c:if test="${param.action == 'Cancel'}">
<c:out value="cancel action"/> 
<jsp:forward page="/displayMsg.jsp">
 <jsp:param name="msg" value="System control (runsse) shutdown request cancelled" />
</jsp:forward>
</c:if>


</body>
</html>