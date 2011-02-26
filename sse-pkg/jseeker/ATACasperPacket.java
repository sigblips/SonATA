/*******************************************************************************

 File:    ATACasperPacket.java
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

import java.io.DataInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;

/**
 * A simple class to hold ATA-style data packets from beamformer to backend.
 */
public class ATACasperPacket
{
    public static final int DataLength = 1024;  // Number of real/complex pairs
    public static final int PacketLength;
    static
    {
        // calculate packet length by simulating serialization
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dos = new DataOutputStream(baos);
        ATACasperPacket p = new ATACasperPacket();
        try {
            p.writeObject(dos);
        }
        catch (Exception ex) {
            System.out.println("Can't serialize ATACasperPacket. Aborting.");
            ex.printStackTrace();
            System.exit(1);
        }
        PacketLength = baos.toByteArray().length;
    }

    // From ATA/Casper/SonATA packets document 8/25/06
    static int correctEndian = 0xaabbccdd;
    static int xPol = 2;
    static int yPol = 3;

    public int group = 0;  // ATA, Casper, etc., 8 bit
    public int version = 0; // 8 bit
    public int bitsPerSample = 16;  // 8 bit
    public int binaryPointPos = 0; // 8 bit
    public int byteOrder = correctEndian;  // endian magic number, 32 bit
    public int packetType = 0;  // 8 bit, mask
    public int numberOfStreams = 1; // 8 bit
    public int polCode = yPol;  // 8 bit
    public int alignPad1 = 0;  // 8 bit
    public int dataSrc = 0;  // 32 bit
    public int chanNumber = 0; // 32 bit
    public int seqNumber = 0; // 32 bit
    double freq = 0; // sampling rate/freq step
    long absTime = 0L; // "NTP" time, 64 bit
    public int goodDataFlags = 0; // 32 bit
    public int payloadLen = 0;  // 32 bit

    public final short[] cdata;

    /**
     * Default constructor for serializing. Creates data array of correct length;
     */
    public ATACasperPacket()
    {
        cdata = new short[2 * DataLength];
    }


    /**
     * Use this method for serializing to the network.
     * @param dos output stream to write to
     * @throws IOException
     */
    public void writeObject(DataOutputStream dos) throws IOException
    {
        dos.writeByte(group);
        dos.writeByte(version);
        dos.writeByte(bitsPerSample);
        dos.writeByte(binaryPointPos);
        dos.writeInt(byteOrder);
        dos.writeByte(packetType);
        dos.writeByte(numberOfStreams);
        dos.writeByte(polCode);
        dos.writeByte(alignPad1);
        dos.writeInt(dataSrc);
        dos.writeInt(chanNumber);
        dos.writeInt(seqNumber);
        dos.writeDouble(freq);
        dos.writeLong(absTime);
        dos.writeInt(goodDataFlags);

        // Payload

        // write data
        //payloadLen = cdata.length;

        payloadLen = DataLength;
        dos.writeInt(payloadLen);
        for (int i = 0; i < payloadLen; ++i)
        {
            // write complex number
            dos.writeShort(cdata[2*i]);
            dos.writeShort(cdata[2*i+1]);
        }

    }

    /**
     * Use this method to deserialize packet from network.
     * @param dis input stream to read from
     * @throws IOException
     */
    public void readObject(DataInputStream dis) throws IOException
    {
        group = dis.readByte();
        version = dis.readByte();
        bitsPerSample = dis.readByte();
        binaryPointPos = dis.readByte();
        byteOrder = dis.readInt();
        packetType = dis.readByte();
        numberOfStreams = dis.readByte();
        polCode = dis.readByte();
        alignPad1 = dis.readByte();
        dataSrc= dis.readInt();
        chanNumber= dis.readInt();
        seqNumber= dis.readInt();
        freq = dis.readDouble();
        absTime = dis.readLong();
        goodDataFlags = dis.readInt();

        // read data
        payloadLen = dis.readInt();

        // debug
        //printHeader();

/*
        if (payloadLen != DataLength) throw new IOException(
            "ATACasperPacket: Incorrect data length while deserializing: "
            + payloadLen);
*/

        if (payloadLen != DataLength)
        {
            System.out.println("ATACasperPacket: Unexpected data length while deserializing: "
                               + "expected: " + DataLength + " got: " + payloadLen);
        }

        payloadLen = DataLength;

        for (int i = 0; i < payloadLen; ++i)
        {
            // read complex numbers
            cdata[2*i] = dis.readShort();
            cdata[2*i+1] = dis.readShort();
        }

    }

    public void printHeader()
    {
        System.out.println("ATACasperPacket Header:");
        System.out.println("group: " + group);
        System.out.println("version: " + version);
        System.out.println("bitsPerSample: " + bitsPerSample);
        System.out.println("binaryPointPos: " + binaryPointPos);
        System.out.println("byteOrder: " + Integer.toHexString(byteOrder));
        System.out.println("packetType: " + packetType);
        System.out.println("numberOfStreams: " + numberOfStreams);
        System.out.println("polCode: " + polCode);
        System.out.println("dataSrc: " + dataSrc);
        System.out.println("seqNumber: " + seqNumber);
        System.out.println("chanNumber: " + chanNumber);
        System.out.println("freq: " + freq);
        System.out.println("absTime: " + absTime);
        System.out.println("goodDataFlags: " + goodDataFlags);
        System.out.println("payloadLen: " + payloadLen);
        System.out.println("");
           
    }

}