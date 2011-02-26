<%--

 File:    obsHist.jsp
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
Purpose: Show recent observation history.

Description: 

   Display oneline summaries of recent observations, using database connection
   information from the config file in the sonata_archive templogs dir.
--%>

<%@ include file="taglibsHeader.jspf" %>

<html>
<head>
<title> SonATA Operations: Obs History</title>

<%@ include file="scroll-body-css.jspf" %>

</head>
<body onload='document.location="#contentTop"'>

<div id="header">
<%@ include file="header.jspf" %>
<br>
</div>

<div id="contents">

<%-- set focus to top of page so page up/down keys will work --%>
<div id="contentTop"></div>

<%-- get database configuration information from the config file --%>
<ptags:dbConfig dbHost="host" dbName="name"/>

<c:set var="nActsToDisplay" value="50"/>
Recent Observation History (last ${nActsToDisplay} Acts):
 &nbsp;&nbsp; [${name} on ${host}]<br>

<%-- get info on the last N activities --%>

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

<div id="contentBottom"></div>
</div>

</body>
</html>