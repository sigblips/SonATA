/*******************************************************************************

 File:    ExamineArchive.java
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

*******************************************************************************/

// Servlet for use with the Jakarata Tomcat web server

import java.io.*;
import java.text.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import java.net.URLEncoder;
import java.net.URLDecoder;
import java.util.regex.*;

/*
 Allow the user to recursively traverse the 
 archive directory tree, examining files,
 which are displayed according to their type:
 text, baseline, waterfall, etc.

 Current path is stored as 'path' parameter, with
 slashes URL encoded as hex characters.
 Path is relative to <userHome>/sonata_archive.
 'Leaf' files can be filtered by their filename 
 using UNIX-style globbing (eg, '*.baseline').
 File filter is entered on an HTML form,
 and stored in the session variable
 'examineArchiveFileFilterExpr'.

 Input Parameters: 

 path:
   - subdirectory relative to /home/<user>/sonata_archive
 fileFilterGlobex:
   - globbing wildcards used to filter files by name 

*/

public class ExamineArchive extends HttpServlet {

   public ExamineArchive()
   {
   }

   public void sendTextFileContents(String filename, PrintWriter out) {
      
      try
      {
	 BufferedReader in = new BufferedReader(new FileReader(filename));
	 String line;
	 //out.println("<pre>");
	 while ((line = in.readLine()) != null)
	 {
	    out.println(line);
            out.println("<br>");
	 }
	 in.close();
	 //out.println("</pre>");
      } 
      catch (Exception e)
      {
	 out.println("Error reading file " + filename + " "+ e);
      }
   }
   
   public void sendBaselineFileContents(String filename, PrintWriter out) {
      
      try
      {
	 out.println("<IMG SRC=\"servlet/BaselinePlotGenerator?filename="
                     + filename + "\" BORDER=1/>");
      } 
      catch (Exception e)
      {
	 out.println("Error reading file " + filename + " "+ e);
      }
   }
   
   public void sendCompampFileContents(
      String filename, int subbandOffset, PrintWriter out) {
      
      try
      {
	 // convert compamp data to jpeg image
	 // TBD: this is just a quick placeholder to get the approximate
	 // functionality -- replace with better scheme

	 String username = System.getProperty("user.name");
	 String outFile = "/tmp/" + username + "-waterfall.jpeg"; 

	 new CmdExec(out, "rm -f " + outFile);

	 new CmdExec(out, "waterfallDisplay -file " + filename
		     + " -suboff " + subbandOffset 
                     + " -batch -outfile " + outFile);
    
	 // add a unique number (current time) 
	 // to the request to force an image refresh

	 out.println("<IMG SRC=\"servlet/SendFile?file=" 
		     + outFile
		     + "&time=" + System.currentTimeMillis()
		     + "\">");

      } 
      catch (Exception e)
      {
	 out.println("Error reading file " + filename + " "+ e);
      }
   }

   public void sendFileContents(String filename, PrintWriter out) {
      
      try
      {
	 out.println("<IMG SRC=\"servlet/SendFile?file=" 
		     + filename
		     + "&time=" + System.currentTimeMillis()
		     + "\">");
      } 
      catch (Exception e)
      {
	 out.println("Error reading file " + filename + " "+ e);
      }
   }

   // convert globbing expression with wildcards to regex
   private String convertGlobToRegex(String globex)
   {
      StringBuffer buffer = new StringBuffer();
      
      char [] chars = globex.toCharArray();
      
      for (int i = 0; i < chars.length; ++i)
      {
         if (chars[i] == '*')
            buffer.append(".*");
         else if (chars[i] == '?')
            buffer.append(".");
         else if ("+()^$.{}[]|\\".indexOf(chars[i]) != -1)
            buffer.append('\\').append(chars[i]);
         else
            buffer.append(chars[i]);
      }
      
      return buffer.toString();
      
   }

   /*
     Display the file in the form indicated by its suffix.
    */
   private void displayFile(
      HttpServletResponse response,
      String filename, 
      String path,
      PrintWriter out) {

      if (filename.endsWith(".txt")) {
         sendTextFileContents(path, out);
      }
      else if (filename.endsWith(".baseline")) {
         sendBaselineFileContents(path, out);
      }
      else if (filename.endsWith(".compamp")) {
         response.setContentType("image/jpeg");
         int subbandOffset=0;
         sendCompampFileContents(path, subbandOffset, out);
      }
      else if (filename.endsWith(".archive-compamp")) {
         response.setContentType("image/jpeg");
         int subbandOffset=8;  // subband offset signal normally appears in
         sendCompampFileContents(path, subbandOffset, out);
      }
      else if (filename.endsWith(".gif")) {
         response.setContentType("image/gif");
         sendFileContents(path, out);
      }
      else if (filename.endsWith(".jpg")) {
         response.setContentType("image/jpeg");
         sendFileContents(path, out);
      } else {
         out.println("TBD: show contents of " + path + "<br>");
         out.println("Don't know how to display a file of this type.<br>");
      }
   }

   /*
     Make links to the child files.  Use the fileFilterExpr
     to eliminate unwanted nondirectory files.
   */
   void processChildrenFiles(File[] children, 
                          String fileFilterGlobex,
                          String baseUrlWithPathParam,
                          String urlDecodedParentPath, 
                          char subdirSeparator,
                          String urlEncoding, 
                          PrintWriter out)
      throws IOException {

      // Filter filenames by user specified globbing expression.
      // First convert to regex before applying.
      String fileRegexFilter = convertGlobToRegex(fileFilterGlobex);
      Pattern pattern = Pattern.compile(fileRegexFilter);

      /*
	Sort files by name.  Use CollationKey for faster performance.
      */
      TreeMap fileMap = new TreeMap();
      Collator collator = Collator.getInstance();
      for (int i=0; i < children.length; ++i) {
         fileMap.put(collator.getCollationKey(children[i].getName()), new Integer(i));
      }

      // For each child, print the file size, and make a link
      // from its name to the full file path.

      DecimalFormat sizeFormat = new DecimalFormat("00000000");
      SimpleDateFormat dateFormat =
         new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

      for (Iterator it = fileMap.keySet().iterator(); it.hasNext(); )
      {
	 Integer childIndex = (Integer) fileMap.get(it.next());
	 File childFile = children[childIndex.intValue()];
         
         // Apply restriction filter to all non-directories
         if (! childFile.isDirectory()) {
            if (! pattern.matcher(childFile.getName()).matches()) {
               continue;
            }
         }
               
         out.print(sizeFormat.format(childFile.length()));
         out.print("&nbsp; &nbsp;");
            
         Date date = new Date(childFile.lastModified());
         out.print(dateFormat.format(date));
         out.print("&nbsp; &nbsp;");

         String childUrl =
            baseUrlWithPathParam + URLEncoder.encode(
               urlDecodedParentPath + subdirSeparator
               + childFile.getName(), urlEncoding);

         out.println("<a href=\"" + childUrl + "\">"
                     + childFile.getName() + "</a> <br>");

      }
   }

   public void doGet(HttpServletRequest request,
		     HttpServletResponse response)
      throws IOException, ServletException  {

      response.setContentType("text/html");
      PrintWriter out = response.getWriter();

      String urlEncoding = "UTF-8";
      String pathParamName = "path";

      // Get current relative archive path from param variable.
      String encodedPathFromParam = request.getParameter(pathParamName);
      String decodedPathFromParam = "";
      if (encodedPathFromParam != null)
      {
         decodedPathFromParam = URLDecoder.decode(encodedPathFromParam,
                                                  urlEncoding);
      }
      else 
      {
         encodedPathFromParam = "";
      }

      /*
        Use the filename filter values in this order:
        request params (ie, from form); session param; default.
      */
      String submittedParamName = "submitted";
      boolean isSubmitted = false;
      if (request.getParameter(submittedParamName) != null) {
         isSubmitted = true;
      }
      
      String fileFilterExprFieldName = "fileFilterExprField";
      String fileFilterSessionParamName = "examineArchiveFileFilterExpr";
      String fileFilterExprValue = "";
      String defaultFilterParamName = "useFilterDefault";
      HttpSession session = request.getSession(true);
      String fileFilterDefault="*"; // allow all files
      if (isSubmitted) {

         // Get filter expression from input form, via request param.
         // Was default button pressed?
         String useDefaultFilterParamValue = request.getParameter(defaultFilterParamName);
         if (useDefaultFilterParamValue != null
             && useDefaultFilterParamValue.equals("true")) {

            fileFilterExprValue = fileFilterDefault;

         } else {

            fileFilterExprValue = request.getParameter(fileFilterExprFieldName);
         }
      }
      else {

         // Look for file filter expression in the session params.
         // If not there, use the default
         fileFilterExprValue =
            (String) session.getAttribute(fileFilterSessionParamName);
         if (fileFilterExprValue == null) {
            fileFilterExprValue = fileFilterDefault;
         }
      }

      // save file filter in session param
      if (isSubmitted) {
         session.setAttribute(fileFilterSessionParamName, fileFilterExprValue);
      }
      
      // debug
      //out.println("File filter: " + fileFilterExprValue + "<br>");

      // form for specifying filename globbing filter
      // Has 
      // [Filter text entry] [apply button] [default filter button]
      // ---------------------------------------------
      out.println("<form>");
      out.println("Filename Filter: <input type=text name='"
                  + fileFilterExprFieldName + "'");
      out.println("value='" + fileFilterExprValue + "'>");

      // Apply Button
      out.println("<input type=button value='Apply'");
      out.println(" onclick='this.form.submit()'>");

      // Filter default button
      out.println("<input type=button value='Default'");
      out.println(" onclick='this.form." + defaultFilterParamName +
                  ".value=true; this.form.submit()'>");
      out.println("<input type=hidden name='" + defaultFilterParamName + 
                  "' value='false'>");

      out.println("Examples: * (all files), *.baseline, 2007-11-14*, *dx19*.compamp"); 

      /*
        Add the 'path' param variable to the response so that the
        current page gets redisplayed with the new filter
      */

      out.println("<input type=hidden name='" + pathParamName + "'");
      out.println(" value='" + encodedPathFromParam + "'>");

      out.println("<input type=hidden name='" + submittedParamName +
                  "' value='true'>");
      out.println("</form>");
      // ---------------------------------------------

      boolean atArchiveRoot = false;
      if (decodedPathFromParam.equals(""))
      {
         atArchiveRoot = true;
      }

      // debug
      //out.println("decodedPathFromParam: '" + decodedPathFromParam + "'<br>");

      // Assume the desired sonata_archive directory is stored in the home
      // directory of the username the web server is running under.
      String archiveTop = System.getProperty("user.home") +"/sonata_archive";

      //Store current path in URL with given param name.
      //Subdirs in the path are separated by the subdirSeparator.
      String urlWithPathParam = request.getRequestURL() + "?"
         + pathParamName + "=";
      char subdirSeparator = '/';

      // add link to top of archive tree
      out.println("<a href=\"" + urlWithPathParam
                  + "\">archive top</a> <br>");

      // link to parent dir, if not already at the archive root
      if (! atArchiveRoot) {

	 // drop the last element (ie, last string preceded by subdirSeparator)
	 String parentPath = "";
	 int lastseparatorIndex = decodedPathFromParam.lastIndexOf(subdirSeparator);
	 if (lastseparatorIndex > 0) {
	    parentPath = 
	       decodedPathFromParam.substring(0, lastseparatorIndex);
	 }
	 String parentUrl = urlWithPathParam + URLEncoder.encode(parentPath,
                                                                 urlEncoding);
	 out.println("<a href=\"" + parentUrl
		     + "\">.. (parent directory)</a> <br>");
      }

      // Define file system path for current directory
      String filesystemPath = archiveTop;
      if (! atArchiveRoot) {
	 String subdirPath = decodedPathFromParam;  
         filesystemPath = filesystemPath + subdirSeparator
            + subdirPath;
      }

      out.println(filesystemPath + "<br>");
      File file = new File(filesystemPath);

      if (! file.isDirectory()) {
	 if (file.length() == 0) {
	    out.println("File is empty.");
	    return;
	 }

         displayFile(response, file.getName(), 
                     file.getPath(), out);
      }
      else {
         // Process all children in the directory
	 File[] children = file.listFiles();

	 if (children != null) {

            processChildrenFiles(children, fileFilterExprValue,
                                 urlWithPathParam, decodedPathFromParam,
                                 subdirSeparator, urlEncoding, out);
	 }
      }
   }
}


