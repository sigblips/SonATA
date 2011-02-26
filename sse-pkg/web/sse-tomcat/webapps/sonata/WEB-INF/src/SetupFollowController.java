/*
 * Licensed under the GPL License. You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://probe.jstripe.com/d/license.shtml
 *
 *  THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 *  WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

package org.jstripe.tomcat.probe.controllers.logs;

import org.jstripe.tomcat.probe.model.FollowedFile;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.servlet.mvc.ParameterizableViewController;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.File;
import java.util.ArrayList;
import java.sql.Timestamp;

public class SetupFollowController extends ParameterizableViewController { //LogHandlerController {

    private String fileAttributeName;

    public String getFileAttributeName() {
        return fileAttributeName;
    }

    public void setFileAttributeName(String fileAttributeName) {
        this.fileAttributeName = fileAttributeName;
    }

//    protected ModelAndView handleLogFile(HttpServletRequest request, HttpServletResponse response, File file) throws Exception {

    protected ModelAndView handleRequestInternal(HttpServletRequest request, HttpServletResponse response) throws Exception {

       System.out.println("SetupFollowController: called");

       File file = new File("/tmp/today-system-log");

        if (file.exists()) {
	   
	   System.out.println("SetupFollowController: filename: " + file.getAbsolutePath() + "file attrib name:" + fileAttributeName);

            FollowedFile ff = new FollowedFile();
            ff.setFileName(file.getAbsolutePath());
            ff.setLastKnowLength(0);
            ff.setLines(new ArrayList());
            ff.setSize(file.length());
            ff.setLastModified(new Timestamp(file.lastModified()));
            request.getSession(true).setAttribute(fileAttributeName, ff);
        }
        return new ModelAndView(getViewName());
    }
}
