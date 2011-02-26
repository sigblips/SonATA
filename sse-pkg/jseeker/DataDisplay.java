/*******************************************************************************

 File:    DataDisplay.java
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

package opensonata.dataDisplays;

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;

import javax.swing.*;
import javax.swing.text.*;

import java.text.*;

import java.io.*;
import java.util.*;


class DataDisplay {

    JLabel itemValue;

    DataDisplay (String name, String units, JPanel panel,
		  GridBagLayout gb, GridBagConstraints gbc) {

	JLabel itemLabel = new JLabel(name);
	gb.setConstraints(itemLabel, gbc);
        panel.add(itemLabel);

	itemValue = new JLabel("TBD", JLabel.RIGHT);
	gbc.gridx += 1;
	gb.setConstraints(itemValue, gbc);
	panel.add(itemValue);
	itemValue.setForeground(Color.blue);

	JLabel itemUnits = new JLabel(units);
	gbc.gridx += 1;
	gb.setConstraints(itemUnits, gbc);
	panel.add(itemUnits);
	itemUnits.setForeground(Color.blue);

    }

    public JLabel getValue() {
	return(itemValue);
    }

}