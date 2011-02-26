/*******************************************************************************

 File:    BackwardsFileStream.java
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

import java.io.*;

public class BackwardsFileStream extends InputStream {
    private RandomAccessFile raf;
    private long seekPos;

    public BackwardsFileStream(File file) throws IOException {
        raf = new RandomAccessFile(file, "r");
        seekPos = raf.length();
    }

    public BackwardsFileStream(File file, long pos) throws IOException {
        raf = new RandomAccessFile(file, "r");
        seekPos = pos;
    }

    public int read() throws IOException {
        if (seekPos > 0) {
            raf.seek(--seekPos);
            return raf.read();
        } else {
            //
            // return EOF (so to speak)
            //
            return -1;
        }
    }

    public void close() throws IOException {
        if (raf != null) raf.close();
    }

    public static void main(String[] args) throws IOException {
        BackwardsFileStream bfs = new BackwardsFileStream(new File("D:/Work/probe/probe/1.txt"));
        try {
            LineReader br = new LineReader(bfs, true);
            String s;
            while ((s = br.readLine()) != null) {
                System.out.print(s);
                System.out.println(s.length());
            }
        } finally {
            bfs.close();
        }

        FileInputStream fis = new FileInputStream(new File("D:/Work/probe/probe/1.txt"));
        try {
            LineReader br = new LineReader(fis, false);
            String s;
            while ((s = br.readLine()) != null) {
                System.out.print(s);
            }
        } finally {
            bfs.close();
        }
    }
}