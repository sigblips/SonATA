<%--

 File:    examineArchive.jsp
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
Allow access to all the files in the archive.
--%>

<html>
<head>
<title> SonATA Operations: Archive Files</title>
  <%@ include file="scroll-body-css.jspf" %>
</head>

<body onload=document.location="#contents">

<div id="header">
<%@ include file="header.jspf" %>
</div>

<div id="contents">

<jsp:include page="servlet/ExamineArchive"/>

<div id="contentBottom"></div>
</div> <%-- end contents div --%>

</body>
</html>