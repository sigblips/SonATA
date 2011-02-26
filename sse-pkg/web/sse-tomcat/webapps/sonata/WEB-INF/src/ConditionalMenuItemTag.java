/*******************************************************************************

 File:    ConditionalMenuItemTag.java
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

package sonata;

import java.io.*;
import javax.servlet.http.*;
import javax.servlet.jsp.*;
import javax.servlet.jsp.tagext.*;

/**
 * Insert an HTML link only if the link is not referencing the current page.
 */
public class ConditionalMenuItemTag extends SimpleTagSupport 
{

    private String toPage;

    /**
     * Writes either the body content as-is or enclosed in an HTML link
     */
    public void doTag() throws JspException, IOException 
    {
        JspFragment body = getJspBody();

        PageContext pageContext = (PageContext) getJspContext();

        HttpServletRequest request = 
            (HttpServletRequest) pageContext.getRequest();

        String requestURI = request.getServletPath();

        //If the page is the same, only print out the body.
        if (requestURI.equals(toPage)) 
        {
            body.invoke(null);
        }
        else 
        {
            // Else, add the body as a link....

            String newURI = request.getContextPath() + toPage;
            HttpServletResponse response = 
                (HttpServletResponse) pageContext.getResponse();

            //Get a buffer and write to it.
            StringWriter evalResult = new StringWriter();
            StringBuffer resultBuf = evalResult.getBuffer();
            resultBuf.append("<a href=\"").append(response.encodeURL(newURI)).
                append("\">");
            body.invoke(evalResult);
            resultBuf.append("</a>");

            //Add the link to the page.
            getJspContext().getOut().print(resultBuf);
        }
    }

    /**
     * Sets the toPage attribute.
     * 
     * @param toPage the page attribute.
     */
    public void setToPage(String toPage) 
    {
        this.toPage = toPage;
    }
    

}