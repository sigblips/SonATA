// Example servlet for use with the Jakarata Tomcat web server

import java.io.*;
import java.text.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;

public class HelloUniverseExample extends HttpServlet {

   public void doGet(HttpServletRequest request,
		     HttpServletResponse response)
      throws IOException, ServletException
   {
      //ResourceBundle rb =
      //ResourceBundle.getBundle("LocalStrings",request.getLocale());
      response.setContentType("text/html");
      PrintWriter out = response.getWriter();

      out.println("<html>");
      out.println("<head>");

      String title = "hello, universe!";
      //    String title = rb.getString("hellouniverse.title");

      out.println("<title>" + title + "</title>");
      out.println("</head>");

      out.println("<h1>" + title + "</h1>");

      out.println("<form method=GET action=\"HelloUniverseExample\">");
      out.println("seeker cmd: ");
      out.println("<input type=text size=80 maxlength=160 name=seekercmd>");
      out.println("</form>");

      String cmdText = request.getParameter("seekercmd");
      if (cmdText != null) {
	 out.println("cmd is: " + cmdText);
	 out.println("<br>");

	 new CmdExec(out, "send-seeker-command-via-telnet " + cmdText);
      }

      new CmdExec(out, "send-seeker-command-via-telnet act stat");
      
      new CmdExec(out, "tail -10 ${HOME}/sse_archive/permlogs/systemlogs/systemlog-`date -u +%Y-%m-%d`.txt");
      
      new CmdExec(out, "tail -10 ${HOME}/sse_archive/permlogs/errorlogs/errorlog-`date -u +%Y-%m-%d`.txt");

      new CmdExec(out, "whoami");
      new CmdExec(out, "ps -ef | grep jakarta| grep -v grep");
      new CmdExec(out, "date -u");
      new CmdExec(out, "pwd");
      new CmdExec(out, "cd /etc/cron.d; ls -l");
      new CmdExec(out, "tail -20 /bad/filename");

      new CmdExec(out, "send-seeker-command-via-telnet status");

      out.println("</body>");
      out.println("</html>");
   }
}



