<%--

 File:    seekerCmd.jsp
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
Let the user enter a seeker command in a form.
Echo command results, and show system and error log tails.
--%>

<html>
<head>
<title> SonATA Operations: Seeker Command</title>
 <%@ include file="../scroll-body-css.jspf" %>
</head>

<%-- javascript: focus on command input field --%>
<body onload="document.commandForm.commandText.focus()">

<div id="header">
<%@ include file="../header.jspf" %>
</div>
<br>


<div id="contents">

<%-- execute the command --%>
<c:if test="${not empty param.commandText}"> 
   <c:import url="/servlet/CmdExecServlet" var="cmdResponse" scope="page">
      <c:param name="command" 
         value="send-seeker-command-via-telnet.expect ${param.commandText}" />
      <c:param name="noEcho" value="true"/>
   </c:import>

   <%-- save the last N chars of command & response in the buffer --%>
   <c:set var="maxCharsToSave" value="10000"/>
   <c:set var="buffLen"
       value="${fn:length(seekerCmdHist)}"/>

   <c:set var="seekerCmdHist" scope="session" 
       value="${fn:substring(sessionScope.seekerCmdHist,buffLen-maxCharsToSave,buffLen)}% ${cmdResponse}"/>

</c:if>


<%-- display command & reponse history --%>
<div id="cmdBufferDiv" style="height: 9em; overflow:auto; border-style:groove;"> 
   ${sessionScope.seekerCmdHist}
</div>

<%-- keep scroll bar at bottom of the cmdbuffer window --%>
<script>
var cmdDiv = document.getElementById("cmdBufferDiv");
cmdDiv.scrollTop = cmdDiv.scrollHeight; 
</script>

<%-- command entry --%>
<form name="commandForm" method=POST action=seekerCmd.jsp>
cmd:
<input type=text size=80 maxlength=160 name=commandText>
</form>

<%-- show the tail ends of the system and error logs --%>
<hr>
<c:set var="logBase" value="\${HOME}/sonata_archive/permlogs"/>

<b>System Log:</b><br>
<c:import url="/servlet/CmdExecServlet">
   <c:param name="command"
       value="tail -8 ${logBase}/systemlogs/systemlog-`date -u +%Y-%m-%d`.txt"/>
   <c:param name="noEcho" value="true"/>
</c:import>

<hr>
<b>Error Log:</b><br>
<c:import url="/servlet/CmdExecServlet">
   <c:param name="command" 
       value="tail -8 ${logBase}/errorlogs/errorlog-`date -u +%Y-%m-%d`.txt"/>
   <c:param name="noEcho" value="true"/>
</c:import>

</div> <%-- end contents div --%>

</body>
</html>
