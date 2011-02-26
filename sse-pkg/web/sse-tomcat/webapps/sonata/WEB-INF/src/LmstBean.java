/*******************************************************************************

 File:    LmstBean.java
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
Current Local Mean Sidereal Time in hours for Hat Creek.
*/

package sonata.beans;

import com.mhuss.AstroLib.*;
import java.util.GregorianCalendar;
import java.util.SimpleTimeZone;

public class LmstBean {

   //Tomcat 5.0 requires a default constructor to be defined!
   public LmstBean()
   {
   }

   public double getLmstHours() {

        GregorianCalendar utc = new GregorianCalendar(
          new SimpleTimeZone(0, "UTC"));
        double jd = DateOps.calendarToDoubleDay(utc);
        
        double gstRads = AstroOps.greenwichSiderealTime(jd);
        double HatCreekLongDeg = 121.4733;
        double lmstRads = gstRads - Math.toRadians(HatCreekLongDeg);
        lmstRads = AstroOps.normalizeRadians(lmstRads);
        
        double lmstHours = (lmstRads / (2 * Math.PI)) * 
           Astro.HOURS_PER_DAY;

        return lmstHours;
   }

}