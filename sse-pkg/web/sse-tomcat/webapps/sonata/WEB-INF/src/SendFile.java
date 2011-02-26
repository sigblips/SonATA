// example from Falkner, Servlets and JSP the J2EE Web Tier
// www.jspbook.com

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*; 

/*
  Send back requested file as raw bytes.

  Params:
    file: filename to send
 */

public class SendFile extends HttpServlet {

   public void doGet(HttpServletRequest request, 
		     HttpServletResponse response)
      throws ServletException, IOException {

      String fileParamName="file";
      String filename = request.getParameter(fileParamName);
      if (filename == null) {
         throw new ServletException("SendFile error: no file param given");
      }
      File inputFile = new File(filename);
      FileInputStream in = new FileInputStream(inputFile);

      // Send back file
      ServletOutputStream sos = response.getOutputStream(); 
      int c;
      while ((c = in.read()) != -1) {
	 sos.write(c);
      }
      in.close();

   }
}

      
      
      
