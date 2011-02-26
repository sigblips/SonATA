<%--

 File:    showStatusSummary.jsp
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
Purpose: Show the system status summary page.
Description: 
   Check the system status file in the sonata_archive templogs dir.
      If not there, then report no status available.
      If there but too old, then report system offline.
      Otherwise put out the status file contents.

   Display a red or green status bar showing system up or down.

   Display recent observation history, using database connection
   information from the config file in the sonata_archive templogs dir.

   Page is automatically refreshed every N seconds.
--%>

<%@ include file="taglibsHeader.jspf" %>

<%-- autorefresh page every N seconds --%>
<%@ include file="refreshHeader.jspf" %>

<html>
<head>
  <title> SonATA Operations: System Status Summary</title>
  <%@ include file="scroll-body-css.jspf" %>
</head>

<body onload='document.location="#contentTop"'>

<div id="header">
  <%@ include file="header.jspf" %>
</div>

<div id="contents">

<%----- show system status ------%>

  <div id="contentTop"></div>
  System Status: &nbsp;&nbsp;&nbsp;[autorefresh]
  <div id="statusRegion" style="border-style:groove">

  <%
      // get current time
      long currentTime = new java.util.Date().getTime();
      pageContext.setAttribute("currentTimeMs", Long.toString(currentTime));

      // define path to status file
      String filePath = System.getProperty("user.home");
      filePath += "/sonata_archive/templogs/system-status-summary.txt";
      pageContext.setAttribute("statusFilename", filePath);

      // get timestamp on status file (will be zero if file not found)
      java.io.File jspFile = new java.io.File(filePath);
      long fileTime = new java.util.Date(jspFile.lastModified()).getTime();
      pageContext.setAttribute("fileTimeMs", Long.toString(fileTime));
  %>

<%--
  Current Timestamp mS: ${currentTimeMs} <br>
  File timestamp mS: ${fileTimeMs} <br>
  Time diff ms: ${currentTimeMs - fileTimeMs}
--%>

  <%-- set the maximum time we'll wait before declaring the system as off --%>
  <c:set var="maxTimeDiffMs" value="45000" />

  <%-- get current time as a string --%>
  <jsp:useBean id="statusTime" class="java.util.Date" />
  <fmt:formatDate pattern="yyyy-MM-dd HH:mm:ss z" timeZone="UTC" value="${statusTime}" var="currentDateTimeStr" />

  <c:set var="currentState" value="offline"/>

  <%-- check if status file is available, and if it's too old --%>

  <c:choose>
     <c:when test="${fileTimeMs == 0}" >
	<c:out escapeXml="false"
           value="${currentDateTimeStr}<br> 
	   System is offline (status file not available).<br><br>"
        />
     </c:when>
     <c:when test="${currentTimeMs - fileTimeMs > maxTimeDiffMs}" >
	<c:out escapeXml="false" 
           value="${currentDateTimeStr}<br> 
           System is offline (last status is too old). <br><br>"
        />
     </c:when>
     <c:otherwise>
	<%-- display status file contents, replacing newlines with break tags --%>
  	<c:catch var="importException">
           <% String newline = "\n"; pageContext.setAttribute("newline",
                newline); %>
           <c:import var="statusText" url="file://${statusFilename}"/>
           ${fn:replace(statusText, newline, "<br>")}
           <c:set var="currentState" value="up"/>
  	</c:catch>
        <c:if test="${importException != null}">
           <c:out escapeXml="false" 
           value="${currentDateTimeStr}<br> 
           System is offline (cannot read status summary file). <br><br>"/>
        </c:if>
     </c:otherwise>
   </c:choose>
  
  </div> <%-- statusRegion --%>

   <%-- display a color bar showing up or down status --%>
   <c:choose>
     <c:when test="${currentState == \"up\"}">
        <img src="icons/greenHorizBar.gif">
     </c:when>
     <c:otherwise>
        <img src="icons/redHorizBar.gif">
     </c:otherwise>
  </c:choose>
  <br>

<%-- Hat Creek Mean Sidereal Time --%>
<jsp:useBean id="hatCreekLmst" class="sonata.beans.LmstBean" />
<fmt:formatNumber var="lmstHours" value="${hatCreekLmst.lmstHours}" 
    minFractionDigits="3" maxFractionDigits="3"/>

<%-- get database configuration information from the config file --%>
<ptags:dbConfig dbHost="host" dbName="name"/>

Recent Targets:&nbsp;&nbsp; [${name} on ${host}]
&nbsp;&nbsp;&nbsp;&nbsp;Hat Creek LMST: ${lmstHours} hours.
<br>

<c:set var="nActsToDisplay" value="2"/>
<c:import url="servlet/DatabaseQueryTable">
   <c:param name="dbHost" value="${host}"/>
   <c:param name="dbName" value="${name}"/>
   <c:param name="query"
   value="select
   Activities.id as ActId, 
   Activities.ts as 'StartTime (UTC)',
   ActParameters.targetprimary as 'PrimaryId',
   TargetCat.ra2000Hours as 'RA (hours)',
   TargetCat.dec2000Deg as 'Dec (deg)'
   from Activities, ActParameters, TargetCat
   where
   Activities.actParametersId = ActParameters.id
   and ActParameters.targetprimary = TargetCat.targetId
   and Activities.id > 
   greatest(0,((select max(Activities.id) from Activities) - ${nActsToDisplay}))
   " />
</c:import>
<br>

Recent Observation History: &nbsp;&nbsp; [${name} on ${host}]<br>

<%-- get info on the last N activities --%>
<c:set var="nActsToDisplay" value="7"/>
<c:import url="servlet/DatabaseQueryTable">
   <c:param name="dbHost" value="${host}"/>
   <c:param name="dbName" value="${name}"/>
   <c:param name="query"
   value="select
   Activities.id as ActId, 
   Activities.ts as InitTime,
   Activities.type as Type,
   ActParameters.targetbeam1 as Beam1Id,
   ActParameters.targetbeam2 as Beam2Id,
   ActParameters.targetbeam3 as Beam3Id,
   ActParameters.targetbeam4 as Beam4Id,
   minDxSkyFreqMhz as MinMhz, maxDxSkyFreqMhz as MaxMhz,
   cwSignals + pulseSignals as '#Sig',
   zeroDriftSignals as ZeroDft,
   recentRfiDatabaseMatches as RctRfi,  
   allCwCandidates + allPulseCandidates as '#Cand',
   confirmedCwCandidates + confirmedPulseCandidates as '#Conf' 
   from Activities, ActParameters where
   Activities.actParametersId = ActParameters.id
   and Activities.id > 
   greatest(0,((select max(Activities.id) from Activities) - ${nActsToDisplay}))
   order by Activities.id asc
   " />
</c:import>

<c:set var="logBase" value="\${HOME}/sonata_archive/permlogs"/>
<hr>
<b>System Log:</b><br>
<c:import url="/servlet/CmdExecServlet">
   <c:param name="command"
       value="tail -5 ${logBase}/systemlogs/systemlog-`date -u +%Y-%m-%d`.txt"/>
   <c:param name="noEcho" value="true"/>
</c:import>
<hr>

<%--
<hr>
<b>Error Log:</b><br>
<c:import url="/servlet/CmdExecServlet">
   <c:param name="command" 
       value="tail -5 ${logBase}/errorlogs/errorlog-`date -u +%Y-%m-%d`.txt"/>
   <c:param name="noEcho" value="true"/>
</c:import>
--%>

<div id="contentBottom"></div>
</div> <%-- end contents div --%>

</body>
</html>