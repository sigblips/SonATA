/*******************************************************************************

 File:    ReadoutListener.java
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


/**
 * @file ReadoutListener.java
 *
 * An interface used with ReadoutPlot that is called when
 * a new x,y position is to be read out.
 *
 * Project: OpenSonATA
 * <BR>
 * Version: 1.0
 * <BR>
 * Authors:
 * - Jon Richards (current maintainer)
 * - The OpenSonATA code is the result of many programmers over many
 * years.
 */

package opensonata.dataDisplays;

/**
 * An interface used with ReadoutPlot that is called when
 * a new x,y position is to be read out.
 */
public interface ReadoutListener 
{
    /** 
     * Notify that a given xValue, yValue in the specified plot has
     * been selected for readout.
     *
     * @param source The plot containing the readout data.
     * @param xPlotValue  The xvalue of the plot position.
     * @param yPlotValue  The yvalue of the plot position.
     */
    public void readoutData(ReadoutPlot source, double xPlotValue,
			    double yPlotValue);
}