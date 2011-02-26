/*******************************************************************************

 File:    LineReader.java
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

package org.jstripe.tomcat.probe.tools;

import org.jstripe.tokenizer.Token;
import org.jstripe.tokenizer.Tokenizer;
import org.jstripe.tokenizer.TokenizerSymbol;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;

public class LineReader {

    public static final String LINE_SEPARATOR = "line.separator";

    private Tokenizer tokenizer;
    private boolean readBackwards;
    private String lastLineSeparator = null;
    private Reader streamReader;

    public LineReader(InputStream stream, boolean backwards) {
        this.readBackwards = backwards;

        this.streamReader = new InputStreamReader(stream);
        tokenizer = new Tokenizer(streamReader);
        tokenizer.addSymbol(new TokenizerSymbol(LINE_SEPARATOR, "\n", null, false, false, true, false));
        tokenizer.addSymbol(new TokenizerSymbol(LINE_SEPARATOR, "\r", null, false, false, true, false));

        if (backwards) {
            tokenizer.addSymbol(new TokenizerSymbol(LINE_SEPARATOR, "\n\r", null, false, false, true, false));
        } else {
            tokenizer.addSymbol(new TokenizerSymbol(LINE_SEPARATOR, "\r\n", null, false, false, true, false));
        }
    }

    public String readLine() throws IOException {
        String result = "";

        String thisLineSeparator = lastLineSeparator;
        lastLineSeparator = null;

        if (tokenizer.hasMore()) {
            while (tokenizer.hasMore()) {
                Token tk = tokenizer.nextToken();
                if (LINE_SEPARATOR.equals(tk.getName())) {
                    lastLineSeparator = tk.getText();
                    break;
                } else {
                    result = tk.getText();
                }
            }
        } else if (thisLineSeparator == null) {
            result = null;
        }

        if (result != null) {
            if (readBackwards) {
                if (thisLineSeparator != null) {
                    result = new StringBuffer(thisLineSeparator).append(result).reverse().toString();
                } else {
                    result = new StringBuffer(result).reverse().toString();
                }
            } else if (lastLineSeparator != null) {
                result = result + lastLineSeparator;
            }
        }
        return result;
    }

    public void close() throws IOException {
        if (streamReader != null) {
            streamReader.close();
        }
    }

}