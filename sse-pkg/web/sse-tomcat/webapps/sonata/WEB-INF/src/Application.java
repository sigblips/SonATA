/**
 * Licensed under the GPL License. You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://probe.jstripe.com/d/license.shtml
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
package org.jstripe.tomcat.probe.model;

import java.io.Serializable;
import java.util.List;
import java.util.ArrayList;

/**
 * POJO representing Tomcat's web application.
 *
 * Author: Vlad Ilyushchenko
 */
public class Application implements Serializable {
    private String name;
    private String displayName;
    private String docBase;
    private boolean available;
    private long sessionCount;
    private long sessionAttributeCount;
    private int contextAttributeCount;
    private int dataSourceUsageScore;
    private boolean distributable;
    private int sessionTimeout;
    private String servletVersion;
    private boolean serializable;
    private long size;
    private List attributes = new ArrayList();

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDisplayName() {
        return displayName;
    }

    public void setDisplayName(String displayName) {
        this.displayName = displayName;
    }

    public String getDocBase() {
        return docBase;
    }

    public void setDocBase(String docBase) {
        this.docBase = docBase;
    }

    public boolean isAvailable() {
        return available;
    }

    public void setAvailable(boolean available) {
        this.available = available;
    }

    public long getSessionCount() {
        return sessionCount;
    }

    public void setSessionCount(long sessionCount) {
        this.sessionCount = sessionCount;
    }

    public long getSessionAttributeCount() {
        return sessionAttributeCount;
    }

    public void setSessionAttributeCount(long sessionAttributeCount) {
        this.sessionAttributeCount = sessionAttributeCount;
    }

    public int getContextAttributeCount() {
        return contextAttributeCount;
    }

    public void setContextAttributeCount(int contextAttributeCount) {
        this.contextAttributeCount = contextAttributeCount;
    }

    public int getDataSourceUsageScore() {
        return dataSourceUsageScore;
    }

    public void setDataSourceUsageScore(int dataSourceUsageScore) {
        this.dataSourceUsageScore = dataSourceUsageScore;
    }

    public boolean isDistributable() {
        return distributable;
    }

    public void setDistributable(boolean distributable) {
        this.distributable = distributable;
    }

    public int getSessionTimeout() {
        return sessionTimeout;
    }

    public void setSessionTimeout(int sessionTimeout) {
        this.sessionTimeout = sessionTimeout;
    }

    public String getServletVersion() {
        return servletVersion;
    }

    public void setServletVersion(String servletVersion) {
        this.servletVersion = servletVersion;
    }

    public long getSize() {
        return size;
    }

    public void setSize(long size) {
        this.size = size;
    }

    public void addSize(long size) {
        this.size += size;
    }

    public boolean isSerializable() {
        return serializable;
    }

    public void setSerializable(boolean serializable) {
        this.serializable = serializable;
    }

    public List getAttributes() {
        return attributes;
    }

    public void setAttributes(List attributes) {
        this.attributes = attributes;
    }
}
