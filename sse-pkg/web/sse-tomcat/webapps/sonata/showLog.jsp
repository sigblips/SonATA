<%--

 File:    showLog.jsp
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
Purpose: Shows the tail end of a log, with a link to the full log 
in the sonata_archive.  This is meant to be included in other jsp pages
(not used standalone).

  Params: 'logName'  Log file to be displayed.  Should be a 
relative path under 'sonata_archive' directory.

--%>

<%@ include file="taglibsHeader.jspf" %>

<%-- URL encode slashes in path so that
examineArchive.jsp can traverse the path properly
--%>
<c:set var="pathWithUrlEncodedSlashes" 
   value="${fn:replace(param.logName, '/', '%2F')}" />
<c:set var="logFileUrl" value="/examineArchive.jsp?path=${pathWithUrlEncodedSlashes}" />

<%-- link to full log file in the archive --%>
<a href="<c:url value='${logFileUrl}'/>">Full Logfile</a><br>

<%-- 
Tail the specified log.  Append a short sleep to allow the command
enough time to execute.
--%>
<c:import url="servlet/CmdExecServlet">
   <c:param name="command" 
   value="tail -200 \${HOME}/sonata_archive/${param.logName}; sleep 1" />
</c:import>

<%-- link to full log file in the archive --%>
<a href="<c:url value='${logFileUrl}'/>">Full Logfile</a><br>

