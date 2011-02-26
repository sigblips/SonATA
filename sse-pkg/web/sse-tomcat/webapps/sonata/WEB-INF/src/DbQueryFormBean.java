/*******************************************************************************

 File:    DbQueryFormBean.java
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

/*
Bean to verify user input for the Database Query Form in exploreDatabase.jsp.
Based on JSP Bean Examples, Ch. 20, JavaServer Pages.
*/

package sonata.beans;

import java.io.*;
import java.util.*;
import java.text.*;

public class DbQueryFormBean implements Serializable {

   static private String minDateName = "Date Min";
   static private String maxDateName = "Date Max";
   static private String minActIdName = "ActId Min";
   static private String maxActIdName = "ActId Max";
   static private String minTargetIdName = "TargetId Min";
   static private String maxTargetIdName = "TargetId Max";
   static private String minRfFreqMhzName = "RfFreq Min";
   static private String maxRfFreqMhzName = "RfFreq Max";
   static private String maskHitsName = "Mask Hits";

   // properties
   private String minDateField;
   private String maxDateField;
   private String minActIdField;
   private String maxActIdField;
   private String minRfFreqMhzField;
   private String maxRfFreqMhzField;
   private String minTargetIdField;
   private String maxTargetIdField;
   private String maskHitsField;

   private StringBuffer errorMsgs;

   public DbQueryFormBean() {
      minDateField = "";
      maxDateField = "";
      minActIdField = "";
      maxActIdField = "";
      minRfFreqMhzField = "";
      maxRfFreqMhzField = "";
      minTargetIdField = "";
      maxTargetIdField = "";
      maskHitsField = "";
      errorMsgs = new StringBuffer("");
   }

   // get fields
   public String getMinDateField() {
      return minDateField;
   }

   public String getMaxDateField() {
      return maxDateField;
   }

   public String getMinActIdField() {
      return minActIdField;
   }

   public String getMaxActIdField() {
      return minActIdField;
   }

   public String getMinRfFreqMhzField() {
      return minRfFreqMhzField;
   }

   public String getMaxRfFreqMhzField() {
      return maxRfFreqMhzField;
   }

   public String getMinTargetIdField() {
      return minTargetIdField;
   }

   public String getMaxTargetIdField() {
      return maxTargetIdField;
   }

   public String getMaskHitsField() {
      return maskHitsField;
   }

   public String getErrors() {
      return errorMsgs.toString();
   }

   // set fields
   public void setMinDateField(String date) {
      this.minDateField = date;
   }

   public void setMaxDateField(String date) {
      this.maxDateField = date;
   }

   public void setMinActIdField(String id) {
      this.minActIdField = id;
   }

   public void setMaxActIdField(String id) {
      this.maxActIdField = id;
   }

   public void setMinRfFreqMhzField(String rfFreqMhz) {
      this.minRfFreqMhzField = rfFreqMhz;
   }

   public void setMaxRfFreqMhzField(String rfFreqMhz) {
      this.maxRfFreqMhzField = rfFreqMhz;
   }

   public void setMinTargetIdField(String id) {
      this.minTargetIdField = id;
   }

   public void setMaxTargetIdField(String id) {
      this.maxTargetIdField = id;
   }

   public void setMaskHitsField(String hits) {
      this.maskHitsField = hits;
   }


   private void addError(String fieldName, String error) {
      errorMsgs.append("*** " + fieldName + ": " + error + ".<br>");
   }

   private boolean isEmptyField(String fieldName, String fieldValue) {
      if (fieldValue.equals("")) {
         addError(fieldName,"no value given");
         return true;
      }
      return false;
   }

   private boolean isLtZero(String fieldName, double value) {
      if (value < 0) {
         addError(fieldName,"value is < zero");
         return true;
      }
      return false;
   }

   private boolean validLongField(String fieldName, String fieldValue) {

      if (isEmptyField(fieldName, fieldValue)) {
         return false;
      }

      try {
         long value = Long.parseLong(fieldValue);
         return true;
      } catch (NumberFormatException e) {
         addError(fieldName, "'" + fieldValue + "' is not a valid number");
         return false;
      }

   }

   private boolean validDoubleField(String fieldName, String fieldValue) {

      if (isEmptyField(fieldName, fieldValue)) {
         return false;
      }

      try {
         double value = Double.parseDouble(fieldValue);
         return true;

      } catch (NumberFormatException e) {
         addError(fieldName, "'" + fieldValue + "' is not a valid number");
         return false;
      }

   }

   private boolean validDateField(String fieldName, String fieldValue) {

      if (isEmptyField(fieldName, fieldValue)) {
         return false;
      }

      // dates must be in a format compatible with sql queries
      String sqlDateFormat = "yyyy-MM-dd HH:mm:ss";

      SimpleDateFormat formatter =
         new SimpleDateFormat(sqlDateFormat);
      formatter.setLenient(false);

      try {
         formatter.parse(fieldValue);
         return true;
      } catch (ParseException pex) {
         addError(fieldName, "'" + fieldValue +
                  "' is invalid, format is: " +
                  sqlDateFormat.toLowerCase());
         return false;
      }
   }

   private boolean isMinLtOrEqualToMaxDouble(
      String minFieldName, String minFieldValue,
      String maxFieldName, String maxFieldValue)
   {
      try {
         double minValue = Double.parseDouble(minFieldValue);
         double maxValue = Double.parseDouble(maxFieldValue);

         if (minValue <= maxValue)
         {
            return true;
         } else {
            addError(minFieldName + "," + maxFieldName,"min > max");
            return false;
         }
      }
      catch (NumberFormatException nfe) {
         addError(minFieldName + " " + maxFieldName,
                  "invalid min or max value");
         return false;
      }

   }


   public boolean isFormValid() {
      boolean allValid = true;

      if (!validDateField("Date Min", minDateField)) {
         allValid = false;
      }

      if (!validDateField("Date Max", maxDateField)) {
         allValid = false;
      }

      if (!validLongField(minActIdName, minActIdField)) {
         allValid = false;
      }

      if (!validLongField(maxActIdName, maxActIdField)) {
         allValid = false;
      }

      if (!validDoubleField(minRfFreqMhzName, minRfFreqMhzField)) {
         allValid = false;
      }

      if (!validDoubleField(maxRfFreqMhzName, maxRfFreqMhzField)) {
         allValid = false;
      }

      if (!validLongField(minTargetIdName, minTargetIdField)) {
         allValid = false;
      }

      if (!validLongField(maxTargetIdName, maxTargetIdField)) {
         allValid = false;
      }

      if (!validLongField(maskHitsName, maskHitsField)) {
         allValid = false;
      }

      // verify min is <= max
      if (allValid) {

         if (!isMinLtOrEqualToMaxDouble(
            minActIdName, minActIdField, 
            maxActIdName, maxActIdField)) {

            allValid = false;
         }

         if (!isMinLtOrEqualToMaxDouble(
            minTargetIdName, minTargetIdField, 
            maxTargetIdName, maxTargetIdField)) {

            allValid = false;
         }
         
         if (!isMinLtOrEqualToMaxDouble(
            minRfFreqMhzName, minRfFreqMhzField, 
            maxRfFreqMhzName, maxRfFreqMhzField)) {

            allValid = false;
         }

      }

      return allValid;
   }

}