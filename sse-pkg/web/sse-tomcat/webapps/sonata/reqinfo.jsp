<%--

 File:    reqinfo.jsp
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

<%@ page contentType="text/html" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>

<html>
  <head>
    <title>Request Info</title>
  </head>
  <body bgcolor="white">

    The following information was received:
    <ul>
      <li>Request Method: 
        <c:out value="${pageContext.request.method}" />
      <li>Request Protocol: 
        <c:out value="${pageContext.request.protocol}" />
      <li>Context Path: 
        <c:out value="${pageContext.request.contextPath}" />
      <li>Servlet Path: 
        <c:out value="${pageContext.request.servletPath}" />
      <li>Request URI: 
        <c:out value="${pageContext.request.requestURI}" />
      <li>Request URL: 
        <c:out value="${pageContext.request.requestURL}" />
      <li>Server Name: 
        <c:out value="${pageContext.request.serverName}" />
      <li>Server Port: 
        <c:out value="${pageContext.request.serverPort}" />
      <li>Remote Address: 
        <c:out value="${pageContext.request.remoteAddr}" />
      <li>Remote Host: 
        <c:out value="${pageContext.request.remoteHost}" />
      <li>Secure: 
        <c:out value="${pageContext.request.secure}" />
      <li>Cookies:<br>
        <c:forEach items="${pageContext.request.cookies}" var="c"> 
          &nbsp;&nbsp;<b><c:out value="${c.name}" /></b>:
          <c:out value="${c.value}" /><br>
        </c:forEach>
      <li>Headers:<br>
        <c:forEach items="${headerValues}" var="h"> 
          &nbsp;&nbsp;<b><c:out value="${h.key}" /></b>:
          <c:forEach items="${h.value}" var="value">
            <br>
            &nbsp;&nbsp;&nbsp;&nbsp;<c:out value="${value}" />
          </c:forEach>
          <br>
        </c:forEach>
    </ul>
  </body>
</html>