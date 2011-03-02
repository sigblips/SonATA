<%-- 

Tag file to read database configuration info from an XML file.
Values returned in arguments: 
   dbHost: database hostname
   dbName: database name

Config file location & name:
   ${HOME}/sonata_archive/templogs/system-config.txt

Config file format:
-----------------------
<systemConfig>
   <dbHost>host</dbHost>
   <dbName>name</dbName>
</systemConfig>
------------------------

Example call:

<ptags:dbConfig dbHost="host" dbName="name"/>
Database config: ${host} ${name}<br>

--%>

<%@ tag body-content="empty" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<%@ taglib prefix="x" uri="http://java.sun.com/jsp/jstl/xml" %>

<%@ attribute name="dbHost" rtexprvalue="false" required="true" %>
<%@ attribute name="dbName" rtexprvalue="false" required="true" %>

<%@ variable name-from-attribute="dbHost" alias="dbHostLocal" 
variable-class="java.lang.Object" scope="AT_END"%>

<%@ variable name-from-attribute="dbName" alias="dbNameLocal" 
variable-class="java.lang.Object" scope="AT_END"%>

<%
   // define path to config file
   String filePath = System.getProperty("user.home");
   filePath += "/sonata_archive/templogs/system-config.txt";

   PageContext pageContext = (PageContext)jspContext;
   pageContext.setAttribute("configFilename", filePath);
%>

<c:import url="file://${configFilename}"
varReader="xmlSource">
   <x:parse var="doc" doc="${xmlSource}" scope="page" />
</c:import>

<%-- 
Return the requested values.
Use c:set rather than x:set in order to get string conversion.
--%>    
<c:set var="dbHostLocal">
   <x:out select="$doc/systemConfig/dbHost"/>
</c:set>

<c:set var="dbNameLocal">
   <x:out select="$doc/systemConfig/dbName"/>
</c:set>


