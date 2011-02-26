/*******************************************************************************

 File:    FollowController.java
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

package org.jstripe.tomcat.probe.controllers.logs;

import org.jstripe.tomcat.probe.model.FollowedFile;
import org.jstripe.tomcat.probe.tools.BackwardsFileStream;
import org.jstripe.tomcat.probe.tools.LineReader;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.mvc.ParameterizableViewController;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.File;

public class FollowController extends ParameterizableViewController  {

    private int maxLines;
    private int initialLines;
    private String fileAttributeName;

    public int getMaxLines() {
        return maxLines;
    }

    public void setMaxLines(int maxLines) {
        this.maxLines = maxLines;
    }

    public int getInitialLines() {
        return initialLines;
    }

    public void setInitialLines(int initialLines) {
        this.initialLines = initialLines;
    }

    public String getFileAttributeName() {
        return fileAttributeName;
    }

    public void setFileAttributeName(String fileAttributeName) {
        this.fileAttributeName = fileAttributeName;
    }

    protected ModelAndView handleRequestInternal(HttpServletRequest request, HttpServletResponse response) throws Exception {

       System.out.println("FollowController:");

        FollowedFile ff = (FollowedFile) request.getSession(true).getAttribute("followed_file");

	if (ff == null)
	{
	   System.out.println("FollowController: followed_file attr is null");
	}

        if (ff != null) {
	   File f = new File(ff.getFileName());

	   System.out.println("FollowController: file is: " + ff.getFileName());

            if (f.exists()) {

	   System.out.println("FollowController: file exists");

                long currentLength = f.length();
                long readSize = 0;
                int listSize = ff.getLines().size();

		System.out.println("FollowController: current len:" + currentLength + " lastknownlen: " + ff.getLastKnowLength() + " listsize: " + listSize);

                if (currentLength < ff.getLastKnowLength()) {
                    //
                    // file length got reset
                    //
                    ff.setLastKnowLength(0);
                    ff.getLines().add(listSize, " ------------- THE FILE HAS BEEN TRUNCATED --------------");
                }

                BackwardsFileStream bfs = new BackwardsFileStream(f, currentLength);
                try {
                    LineReader br = new LineReader(bfs, true);
                    String s;
                    while (readSize < currentLength - ff.getLastKnowLength() && (s = br.readLine()) != null) {
                        if (ff.getLines().size() >= maxLines) {
                            if (listSize > 0) {
                                ff.getLines().remove(0);
                                listSize--;
                            } else {
                                break;
                            }
                        }
                        ff.getLines().add(listSize, s);
                        readSize += s.length();
                        if (ff.getLastKnowLength() == 0 && ff.getLines().size() >= initialLines) break;
                    }

                    if (readSize > currentLength - ff.getLastKnowLength() && listSize > 0) {
                        ff.getLines().remove(listSize-1);
                    }

                    ff.setLastKnowLength(currentLength);
                } finally {
                    bfs.close();
                }
            } else {
                ff.getLines().clear();
            }
            request.getSession(true).setAttribute(fileAttributeName, ff);
        }

        return new ModelAndView(getViewName());
    }
}