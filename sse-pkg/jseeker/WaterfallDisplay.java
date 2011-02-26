/*******************************************************************************

 File:    WaterfallDisplay.java
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
 * @file WaterfallDisplay.java
 *
 * Waterfall display.
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

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.awt.geom.*;
import java.text.*;
import java.io.*;
import javax.swing.filechooser.*;
import javax.imageio.*;
import java.awt.print.PrinterJob;
import java.awt.print.*;
import edu.emory.mathcs.jtransforms.fft.DoubleFFT_1D;
import ptolemy.plot.*;

/**
 * Reads complex amplitude time samples from a file and plots the 
 * data as freq vs time vs power.
 */
public class WaterfallDisplay extends JPanel implements ReadoutListener 
{

    //The input filename specified on the command line or selected by the user.
    String inFilename;
    //The output filename specified on the command line or selected by the user.
    String outFilename;

    //the width and height of the waterfall image
    int width;
    int height;
    float pixelScaleFactor;
    float pixelOffsetFactor;

    // FFT waterfall image buffers
    BufferedImage origBuffImage;
    BufferedImage scaledBuffImage;

    FileScanner fileScanner;
    CoefConversion coefConversion;
    AverageComplexAmplitudes averageComplexAmplitudes;
    ComplexAmplitudes compAmps;
    ReadoutPlot readoutPlot;

    PrinterJob printJob;
    final int imageType = BufferedImage.TYPE_BYTE_GRAY;
    int MAX_SUBBAND_BINS_PER_1KHZ_HALF_FRAME = 512;
    int MAX_SUBBAND_BINS_PER_1HZ_HALF_FRAME = 1024;
    boolean readGrayscalePixels = false;
    int displayResolution = 1;
    boolean slowPlay = false;
    boolean repeatPlay = false;
    boolean usingGui = true;
    String title = "";

    // for selecting the particular subband to display among those in the file
    int subbandOffset;

    // GUI components
    JFrame mainGuiFrame;
    ImagePanel imagePanel;
    JFrame plotFrame;
    JFrame imageAdjustmentFrame;
    JLabel polValueLabel;
    JLabel currentFileText;
    JLabel subbandNumberValueLabel;
    JLabel rfCenterFreqValueLabel;
    JLabel halfFrameNumberValueLabel;
    JLabel binFreqReadoutValueLabel;
    JLabel binNumberReadoutValueLabel;
    JLabel bandwidthValueLabel;
    JLabel activityIdValueLabel;

    JLabel aveCompAmpsSubbandNumberValueLabel;
    JLabel aveCompAmpsBinFreqReadoutValueLabel;
    JLabel aveCompAmpsBinNumberReadoutValueLabel;
    JLabel aveCompAmpsPowerReadoutValueLabel;
    JLabel aveCompAmpsBandwidthValueLabel;
    JLabel aveCompAmpsCurrentFileText;

    /**
     * Manages the image panel. 
     * Also provides image printing.
     */
    class ImagePanel extends JPanel implements Printable 
    {
        BufferedImage image;

        /**
         * Constructor.
         */
        public ImagePanel() 
        {
            setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));
        }

        /**
         * Assign the image buffer for this instance.
         *
         * @param image the BufferedImage instance.
         */
        public void setImage(BufferedImage image)
        {
            this.image = image;
        }

        /**
         * Paint the image.
         *
         * @param g the Graphics instance for drawing.
         */
        public void paintComponent(Graphics g)
        {
            super.paintComponent(g); // paint background

            // Draw image at its natural size.
            g.drawImage(image, 0, 0, this); 

        }

        /**
         * Prints the image.
         *
         * @param g the Graphics instance for drawing.
         * @param pageFormat the format of the page.
         * @param pageIndex the index of the page. Only a value of 1 is 
         * acceptable.
         * @return Printable.PAGE_EXISTS if this is a valid page and it was 
         * printed. otherwise Printable.NO_SUCH_PAGE is returned.
         * @throws PrinterException 
         **/
        public int print(Graphics g, PageFormat pageFormat, int pageIndex)
            throws PrinterException
            {
                if (pageIndex >= 1)
                {
                    return Printable.NO_SUCH_PAGE;
                }

                // create a copy of the image that has the grayscale inverted 
                // (black for white)
                // to save toner on printouts
                int tableLen = 256;
                short[] grayscaleInvertTable = new short[tableLen];
                for (int i = 0; i < tableLen; i++)
                {
                    grayscaleInvertTable[i] = (short) ((tableLen-1) - i);
                }
                BufferedImageOp grayInvertOp =
                    new LookupOp(new ShortLookupTable(0, grayscaleInvertTable),
                            null);

                BufferedImage grayInvertedImage = 
                    new BufferedImage(width, height, imageType);
                grayInvertOp.filter(image, grayInvertedImage);

                Graphics2D g2 = (Graphics2D) g;

                //--- Translate the origin to 0,0 for the top left corner
                g2.translate (pageFormat.getImageableX (),
                        pageFormat.getImageableY ());

                // Scale the image to fit properly on the output printed page.
                double POINTS_PER_INCH = 72;
                double widthScaleFactor = 
                    pageFormat.getImageableWidth()  / POINTS_PER_INCH;
                double heightScaleFactor = 
                    pageFormat.getImageableHeight() / POINTS_PER_INCH; 

                // finally, draw the image.
                g2.drawImage (grayInvertedImage,
                        (int) (0.25 * POINTS_PER_INCH),
                        (int) (0.25 * POINTS_PER_INCH),
                        (int) (widthScaleFactor * POINTS_PER_INCH),
                        (int) (heightScaleFactor * POINTS_PER_INCH),  
                        this);

                return Printable.PAGE_EXISTS;
            }

    } //End of ImagePanel class.


    /**
     * Converts complex amplitude time samples into 1,2 or 4 Hz data and
     * converts the freq domain values to pixels.
     * Note that the cofficients arrive in half frames. So 512 samples
     * at a time, 1024 values which are real,imaginary,real,imaginary...
     * One full frame is 1024 samples, which is 2048 values 
     * (real,imaginary,real,imaginary...)
     */
    class CoefConversion
    {

        /**
         * Storage of the previous conversions coefficients. 
         * coefficients arrive in half frames.
         */
        private double[] prevCoef = null;  

        /**
         * Initialze the previous conversions coefficients.
         */
        synchronized public void startNewDataStream() 
        {
            // need to flush out the pipelined data
            prevCoef = null;
        }

        /**
         * Convert the 1kHz data to 2kHz values.
         * 
         * @param coef the array of coefficients. This is a half
         * frame of coefficients.
         */
        public double[] convertCoefsTo2HzFreqValues(double coef[]) 
        {
            // Time sample order is real, imag in adjacent array elements.
            // Note that transform occurs in-place, so a copy
            // of the input array is used.
            double[] results = new double[coef.length];
            for (int i=0; i<coef.length; i++)
                results[i] = coef[i];

            convertCoefsToFreqValues(results);

            return results;
        }

        /**
         * Convert the 1kHz data to 4kHz values.
         *
         * @todo Is this the algorithm correct?
         *
         * @param coef the array of coefficients. This is a half
         * frame of coefficients.
         */
        public double[] convertCoefsTo4HzFreqValues(double coef[]) 
        {
            // Time sample order is real, imag in adjacent array elements.
            // Note that transform occurs in-place, so a copy
            // of the input array is used.
            double[] results = new double[coef.length/2];
            for (int i=0; i<coef.length; i+=4)
            {
                results[i/2]   = (coef[i] + coef[i+2])/2.0;
                results[i/2+1] = (coef[i+1] + coef[i+3])/2.0;;
            }

            convertCoefsToFreqValues(results);

            return results;
        }


        /**
         * Convert the 1Hz complex amplitude values to frequency values.
         * This pieces together the last half frame and the current half
         * frame to operate on one full frame. Pipelined.
         *
         * @param newCoef the latest half frame.
         * @return an array of frequency values. Sonce this is pipelined, the
         * first time there is no half frame of coeffiecients stored so a
         * zeroed array is returned.
         */
        synchronized public double[] convertCoefsTo1HzFreqValues(double newCoef[]) 
        {
            // arrays are twice as big as the MAX_SUBBAND_BINS consts
            // because the real/imag values have been unpacked into
            // adjacent array elements.


            // must be able to hold two 1KHz arrays
            double[] combinedCoef = 
                new double[2 * 2 * MAX_SUBBAND_BINS_PER_1KHZ_HALF_FRAME];

            // If first time or a reset has occurred
            if (prevCoef == null) // first time or reset
            {
                prevCoef = new double[2 * MAX_SUBBAND_BINS_PER_1KHZ_HALF_FRAME];

                // Copy the new coefficients into the last half frame array
                for (int i=0; i<newCoef.length; ++i)
                {
                    prevCoef[i] = newCoef[i];
                }

                //Zero out the array to return.
                for (int i=0; i<combinedCoef.length; ++i) 
                {
                    combinedCoef[i] = 0;
                }

                return combinedCoef;  // return empty array the first time
            }

            // copy the previous coefs
            for (int i=0; i<prevCoef.length; ++i)
            {
                combinedCoef[i] = prevCoef[i];
            }	

            // add the new coefs on to the end of the last ones
            for (int i=0; i<newCoef.length; ++i)
            {
                combinedCoef[i+2*MAX_SUBBAND_BINS_PER_1KHZ_HALF_FRAME] = newCoef[i];
            }

            // convert them
            convertCoefsToFreqValues(combinedCoef);

            // save the most recent data for next time
            for (int i=0; i<newCoef.length; ++i)
            {
                prevCoef[i] = newCoef[i];
            }

            return combinedCoef;

        }

        /**
         * Convert the complex amplitude time samples to the frequency domain.
         *
         * Time sample order is real, imag in adjacent array elements
         *
         * @param coef the array of complex amplitude time samples.
         * The return values are placed back into the coef[] array.
         */
        private void convertCoefsToFreqValues(double coef[])
        {

            //The number of coefficients for the FFT 
            int transformLength = coef.length / 2;  // N real, imag pairs

            // Perform the FFT. Values are placed back in coef[].
            DoubleFFT_1D fft = new DoubleFFT_1D(transformLength);
            fft.complexForward(coef);

            // Swap the left/right halves of the output array
            // to get the negative freqs into the proper place.
            for (int i = 0; i < transformLength; ++i)
            {
                double temp = coef[i + transformLength];
                coef[i+transformLength] = coef[i];
                coef[i] = temp;
            }


            // Normalize by the mean square value
            double sumSquare = 0;
            for (int i=0; i<coef.length; i+=2)
            {
                sumSquare += Math.abs(coef[i]) * Math.abs(coef[i]) +
                    Math.abs(coef[i+1]) * Math.abs(coef[i+1]); 
            }
            double meanSquare = sumSquare / transformLength;
            double norm = Math.sqrt(meanSquare);

            for (int i=0; i<coef.length; ++i) 
            {
                coef[i] /= norm;

                // trim to min/max values
                int maxValue = 7;
                int minValue = -maxValue;
                if (coef[i] > maxValue) coef[i] = maxValue;
                if (coef[i] < minValue) coef[i] = minValue;
            }

        }


        /**
         * Convert the FFT values to pixel values.
         * 
         * @todo kes: we need to eliminate the edge bins created by the 
         * oversampling process; they are discarded because they lie outside 
         * the actual bandwidth of the subchannel.
         *
         * @param coef the array of FFTp coeffieients.
         * @param pixbuff the output pixels.
         */
        public void convertFFTValuesToPixels(double coef[], int[] pixbuff)
        {

            int maxPixelValue = 255;
            double maxPairValue = 7;
            double maxPowerValue =  maxPairValue * maxPairValue + 
                maxPairValue * maxPairValue;
            double scaleFactor = 4;  

            double scale = (double) maxPixelValue / maxPowerValue * scaleFactor;

            // zero output buffer in case it's only partially filled
            for (int i=0; i<pixbuff.length; ++i)
            {
                pixbuff[i] = 0;
            }

            // See the todo
            int pixIndex = 0;
            int discard = (int) (coef.length * compAmps.overSampling);
            int ofs = discard / 2;
            int length = coef.length - discard;
            //System.out.println("pixbuff length = " + pixbuff.length + ", Coef.length=" + coef.length);
            //System.out.println("ofs = " + ofs + " length = " + length);


            int pixelShift = (int)(width/2) - (int)(length/4);

            //System.out.println("Shift=" + pixelShift);

            // convert to power (sum of the squares)
            for (int i=0; i<length; i+=2)
            {

                double realValue = coef[ofs+i];
                double imagValue = coef[ofs+i+1];

                int power = (int) (realValue * realValue + 
                        imagValue * imagValue);

                int pixel = (int) (power * scale);
                if (pixel > maxPixelValue)
                    pixel = maxPixelValue;

                int index = pixelShift +  pixIndex++;
                if(index < pixbuff.length)
                {
                    pixbuff[index] = pixel;
                }

            }

        }

    }
    // end class CoefConversion




    /**
     * Manages complex amplitudes. 
     * <p>
     * See <a href="../../include/html/ssePdmInterface_8h_source.html">
     * ssePdmInterface.h</a> for the C++ version of this struct
     * (ComplexAmplitudeHeader and SubbandCoef1KHz structs).
     */
    class ComplexAmplitudes 
    {

        //Usually the data is 1 subband, but can be more than one.
        //As a sanity check limit the max number of subbands to 16.
        int maxSubbands = 16;  // sanity check

        // data header
        double rfCenterFrequency;
        int halfFrameNumber;
        int activityId;
        double hzPerSubband;
        int startSubbandId;
        int numberOfSubbands;
        float overSampling;
        int polarization;  // defined as Polarization enum.  Assumed to be 32bit.
        int compAmpsSubbandOffset = 0;

        // fixed length body, but is repeated numberOfSubbands times
        int rawCoef[];

        /**
         * Stores the complex values when converted to doubles.
         * RRRRIIII real & image complex pair
         * <p>
         * 4 bit signed (2's complement), high bits on the left
         * extracted coefs as doubles
         */
        double coef[];

        /**
         * Pull out two 4-bit coef values & store in double array.
         * Real, imaginary values are stored in adjacent array elements
         * The result is stored in the coek[] array.
         */
        private void unpackCoefsIntoDoubles()
        {
            coef = new double[rawCoef.length * 2];

            int outIndex = 0;
            for (int i=0; i<rawCoef.length; ++i)
            {
                // coef's are:
                // RRRRIIII (4 bits real, 4 bits imaginary in 2's complement)

                int realValue = (rawCoef[i] & 0x000000f0) >> 4; 
                if ((realValue & 0x00000008) != 0)
                    realValue |= 0xfffffff0;  // sign extend

                int imagValue = rawCoef[i] & 0x0000000f;
                if ((imagValue & 0x00000008) != 0)
                    imagValue |= 0xfffffff0;  // sign extend

                coef[outIndex++] = realValue;
                coef[outIndex++] = imagValue;
            }

        }

        /**
         * Read the data header.
         *
         * @param in the input data stream to read from.
         * @throws java.io.IOException.
         */
        private void readHeader(DataInputStream in) throws java.io.IOException 	
        {

            // read the header
            rfCenterFrequency = in.readDouble();
            halfFrameNumber = in.readInt();
            activityId = in.readInt();
            hzPerSubband = in.readDouble();
            startSubbandId = in.readInt();
            numberOfSubbands = in.readInt();
            overSampling = in.readFloat();
            polarization = in.readInt();

            // validate length of compamp value array
            if (numberOfSubbands < 1 || numberOfSubbands > maxSubbands)
            {
                System.out.println(
                        "ERROR: subbands value " + numberOfSubbands +
                        " out of range.  Must be 1 - " + maxSubbands);
                //System.exit(1);

                throw new java.io.IOException();
            }

        }

        /**
         * Set the subband offset for reading the correct subband from the data.
         *
         * @param subbandOffset the offset from the subband.
         */
        public void setSubbandOffset(int subbandOffset)
        {
            compAmpsSubbandOffset = subbandOffset;
        }

        /**
         * Read one half frame of data.
         *
         * @param in the input data stream to read from.
         * @throws java.io.IOException.
         */
        public void readSubbandData(DataInputStream in) throws java.io.IOException 	
        {

            // TBD read numberOfSubbands times

            // read the variable length baseline value array	
            rawCoef = new int[MAX_SUBBAND_BINS_PER_1KHZ_HALF_FRAME];
            for (int i=0; i<MAX_SUBBAND_BINS_PER_1KHZ_HALF_FRAME; ++i)
            {
                rawCoef[i] = in.readUnsignedByte();
            }

            unpackCoefsIntoDoubles();


        }

        /**
         * Read one half frame of data and throw the data away.
         *
         * @param in the input data stream to read from.
         * @throws java.io.IOException.
         */
        public void skipSubbandData(DataInputStream in) throws java.io.IOException 	
        {

            // read the variable length baseline value array	
            for (int i=0; i<MAX_SUBBAND_BINS_PER_1KHZ_HALF_FRAME; ++i)
            {
                int discard = in.readUnsignedByte();
            }

        }


        /**
         * Manage reading in one full record of header and data.
         *
         * @param in the input data stream to read from.
         * @throws java.io.IOException.
         */
        public void read(DataInputStream in) throws java.io.IOException 	
        {

            // assume requested subbandOffset runs from 0 to numberOfSubbands-1

            readHeader(in);

            // make sure the request doesn't exceed the actual
            // number of subbands

            int subbandOffset = compAmpsSubbandOffset;
            if (subbandOffset > numberOfSubbands-1)
            {
                subbandOffset = 0;

                System.out.println("subbandOffset exceeds number of available subbands");
                // TBD warning
            }

            // pick out the requested subband (based on the subbandOffset) to display

            // skip the data before the desired subband
            for (int i=0; i<subbandOffset && i < numberOfSubbands-1; ++i)
            {
                startSubbandId++;

                //System.out.println("skipping pre subband " + i);
                skipSubbandData(in);
            }

            // adjust the center frequency to match the selected
            // subband.  the one in the header is defined to be the
            // center freq of the first subband.

            double hzPerMHz = 1e6;
            double offsetMHz = (subbandOffset) * hzPerSubband / hzPerMHz;
            rfCenterFrequency += offsetMHz;

            // read the desired subband
            readSubbandData(in);

            // skip the data after the desired subband
            for (int i=subbandOffset+1; i<=numberOfSubbands-1; ++i)
            {
                //System.out.println("skipping post subband " + i);
                skipSubbandData(in);
            }


        }

        /**
         * Convert the polarization value in the header to a string.
         *
         * @param pol the polarization number from the header.
         * @return  R, L, B, M, or ?
         */
        private String polIntToString(int pol)
        {
            String value;
            switch (pol)
            {
                case 0: value ="R";  // right circular
                        break;
                case 1: value = "L"; // left circular
                        break;
                case 2: value = "B"; // both
                        break;
                case 3: value = "M"; // mixed
                        break;
                default: value = "?";
                         break;
            }

            return value;
        }

        /**
         * get the latest read in polarization from the header as a string.
         *
         * @return  R, L, B, M, or ?
         */
        public String getPolAsString()
        {
            return polIntToString(polarization);
        }


        /**
         * Print to standard out the lastest read in header information.
         */
        public void printHeader()
        {

            System.out.println(
                    "rfcenterFreq " + rfCenterFrequency + " MHz," +
                    " halfFrameNumber " + halfFrameNumber + "," +
                    " activityId " + activityId + "," +
                    " hzPerSubband " + hzPerSubband + "," +
                    " startSubbandId " + startSubbandId + "," +
                    " #subbands " +  numberOfSubbands + "," +
                    " overSampling " + overSampling + "," +
                    " pol " + polIntToString(polarization)
                    );

        }

        /**
         * Print to standard out the latest read in complex amplitude values.
         */
        public void printBody()
        {
            int maxToPrint = 5;
            System.out.print("Compamp Values: ");
            for (int i=0; i<maxToPrint && i<MAX_SUBBAND_BINS_PER_1KHZ_HALF_FRAME; ++i)
            {
                System.out.print(coef[i] + " ");
            }
            System.out.println("");

        }

    } //End of ComplexAmplitudes class


    /**
     * Manage the average complex amplitudes.
     * Keeps track of averageComplexAmplitudes stored as power values.
     */ 
    class AverageComplexAmplitudes 
    {
        private double[] powerSum = null;
        private double[] powerAve = null;
        int numberOfFrames = 0;
        int arraySize = 0;

        /**
         * Constructor.
         *
         * @param arraySize the size of the complex amplitide array.
         */
        AverageComplexAmplitudes(int arraySize)
        {
            this.arraySize = arraySize;

            reset();
        }

        /**
         * Reset the data managed by this class.
         */
        void reset()
        {
            numberOfFrames = 0;
            powerSum = new double[arraySize];
            powerAve = new double[arraySize];
        }

        /**
         * Calculate the power of each fft value and add it to the
         * powerSum and powerAve arrays.
         *
         * @param fftValues an array of FFTp values, real,imaginary, real...
         */
        void add(double[] fftValues)
        {
            numberOfFrames++;

            // find the sum & the average for each fftValue
            int j=0;
            for (int i=0; i<fftValues.length; i+=2)
            {

                double realValue = fftValues[i];
                double imagValue = fftValues[i+1];

                double power = realValue * realValue + 
                    imagValue * imagValue;

                powerSum[j] += power;
                powerAve[j] = powerSum[j] / numberOfFrames;
                j++;
            }
        }

        /**
         * Get the array of average power values.
         *
         * @return the array of power average power values.
         */
        double[] getAverageValues()
        {
            return powerAve;
        }

    } //End of AverageComplexAmplitudes class.


    /**
     * Constructor.
     *
     * @param usingGui true if using GUI, false if not.
     * @param mainGuiFrame the JFrame containing the GUI.
     * @param inFilename the input data file name.
     * @param outFilename the output data file name.
     * @param subbandOffset the subband offset.
     * @param width the width if the waterfall image window.
     * @param height the hieght if the waterfall image window.
     * @param pixelScaleFactor the waterfall image scale.
     * @param pixelOffsetFactor the waterfall image offset.
     * @param resolutionHz the resolution in Hz (1, 2 or 4)
     * @param slowPlay if true display the data slowly to approximate
     * the speed of the real time system.
     * @param repeatPlay if true repeat over and over.
     * @param title the title to display at the top of the frame.
     */
    public WaterfallDisplay(boolean usingGui,
            JFrame mainGuiFrame, 
            String inFilename,
            String outFilename,
            int subbandOffset,
            int width, int height, 
            float pixelScaleFactor,  float pixelOffsetFactor,
            int resolutionHz,
            boolean slowPlay,
            boolean repeatPlay,
            String title)
    {
        this.usingGui = usingGui;
        this.mainGuiFrame = mainGuiFrame;
        this.inFilename = inFilename;
        this.outFilename = outFilename;
        this.subbandOffset = subbandOffset;
        this.width = width;
        this.height = height;
        this.pixelScaleFactor = pixelScaleFactor;
        this.pixelOffsetFactor = pixelOffsetFactor;
        this.title = title;

        if (usingGui)
        {
            this.currentFileText = new JLabel("");
            this.aveCompAmpsCurrentFileText = new JLabel("");
        }

        this.displayResolution = resolutionHz;

        this.slowPlay = slowPlay;
        this.repeatPlay = repeatPlay;


        origBuffImage = new BufferedImage(width, height, imageType);
        scaledBuffImage = new BufferedImage(width, height, imageType);
        compAmps = new ComplexAmplitudes();
        compAmps.setSubbandOffset(this.subbandOffset);
        coefConversion = new CoefConversion();
        averageComplexAmplitudes = 
            new AverageComplexAmplitudes(MAX_SUBBAND_BINS_PER_1HZ_HALF_FRAME);
    }

    /**
     * Read a file containing gray pixel data or complex amplitude data and
     * turn it into an image.
     *
     * If an EOFException is received, it means the file has been truncated
     * and it needs to be reopened.
     * <p>
     * Effectively does a 'tail -f' on the file, sleeping periodically
     * until new data appears.
     */
    private class FileScanner extends Thread 
    {

        int width;
        int height;
        String filename;
        DataInputStream in;
        // transform: shift image by 1 Y pixel
        AffineTransformOp aftOpShift1LineDown;
        // transform: copy image unchanged
        AffineTransformOp aftOpCopy;
        BufferedImage shiftedBuffImage;
        int imageRowCount;

        /**
         * Constructor.
         *
         * @param filename the name of the file to read.
         * @param width the width of the image.
         * @param height the height of the image.
         */
        FileScanner(String filename, int width, int height)
        {

            this.width = width;
            this.height = height;
            this.filename = filename;

            // create transform to shift orig image down by 1 line (1 Y pixel)
            AffineTransform aftShift = new AffineTransform();
            aftShift.setToTranslation(0.0, 1.0);  // shift y 
            aftOpShift1LineDown = new AffineTransformOp(aftShift, null);

            // create transform to copy orig image, unchanged
            AffineTransform aftCopy = new AffineTransform();
            aftCopy.setToTranslation(0.0, 0.0);  // no shift
            aftOpCopy = new AffineTransformOp(aftCopy, null);

            // make empty temp image
            shiftedBuffImage = new BufferedImage(width, height, 
                    imageType);

            start();
        }

        /**
         * Assign the filename to this class instance and open the file.
         *
         * @param filename the name of the file to open.
         */
        public void openNewFile(String filename)
        {
            this.filename = filename;
            openFile();
        }

        /**
         * Open the file.
         */
        private void openFile()
        {
            if (filename != "")
            {

                //System.out.println("openFile called");


                try 
                {
                    // close previously open stream
                    // TBD synchronize this?
                    if (in != null)
                    {
                        in.close();
                        in = null;
                    }

                    // remove any previously stored complex amplitude data
                    // (used for pipelining fft conversion to 1Hz data)

                    imageRowCount = 0;

                    averageComplexAmplitudes.reset();
                    clearPlot();

                    coefConversion.startNewDataStream();

                    separatePreviouslyDisplayedData();

                    in = new DataInputStream(
                            new BufferedInputStream(
                                new FileInputStream(filename)));

                }
                catch (IOException e)
                {
                    System.out.println(e);
                }

            }
        }

        /**
         * Sleep 200 milliseconds.
         */
        private void sleep()
        {
            try 
            {
                int latency = 200;  //mS
                Thread.sleep(latency);
            }
            catch (InterruptedException e)
            {
                // Interrupt may be thrown manually by stop()
            }
        }

        /**
         * Add a separator between the old data & the new.
         * make the line just bright enough to see without being too bright.
         */
        private void separatePreviouslyDisplayedData() 
        {

            int lineBrightnessPixelValue = 40;
            int nRows = 1;

            addRowsOfConstantBrightness(lineBrightnessPixelValue,
                    nRows);
        }

        /**
         * Add one or more rows to the image of a constant brightness.
         *
         * @param brightnessPixelValue the brighness of the pixel, 0 to 255.
         * @param nRows the number of rows to add.
         */
        private void addRowsOfConstantBrightness(int brightnessPixelValue,
                int nRows) 
        {
            // make a solid line
            int rowSize = width;
            int [] pixrow = new int[rowSize];
            for (int i=0; i<pixrow.length; ++i) 
            {
                pixrow[i] = brightnessPixelValue;
            }

            for (int i=0; i < nRows; ++i) 
            {
                addPixelRowToImage(pixrow);
            }

            redrawImage();
        }

        /**
         * Add the row of pixels to the top of the image.
         * 
         * @param pixrow an array of data to be added as one row of the image.
         */
        private void addPixelRowToImage(int[] pixrow)
        {
            // shift orig buff down by one pixel
            aftOpShift1LineDown.filter(origBuffImage, shiftedBuffImage);

            // add in new line at the top
            int nrows = 1;

            //System.out.println("" + pixrow[0] + "," + pixrow[1] + "," + pixrow[2] + "," + pixrow[3]);
            shiftedBuffImage.getRaster().setPixels(0, 0, width, nrows, pixrow);

            // copy shifted image back to orig
            aftOpCopy.filter(shiftedBuffImage, origBuffImage);

            imageRowCount++;

            redrawImage();
        }

        /**
         * Update the GUI text information.
         *  
         * @param compAmps an instance of a ComplexAmplitudes class containing
         * the information to display.
         */
        private void updateDisplayHeaders(ComplexAmplitudes compAmps)
        {


            DecimalFormat rfCenterFreqFormatter =
                new DecimalFormat("0000.000000 MHz ");
            DecimalFormat halfFrameFormatter =
                new DecimalFormat("0000 ");
            DecimalFormat subbandNumberFormatter =
                new DecimalFormat("0000 ");
            DecimalFormat bandwidthFormatter = 
                new DecimalFormat("000.0 Hz ");
            DecimalFormat activityIdFormatter = 
                new DecimalFormat("0000 ");

            if (usingGui) 
            {
                // update widgets

                polValueLabel.setText("" + compAmps.getPolAsString() + " ");

                subbandNumberValueLabel.setText(
                        subbandNumberFormatter.format(compAmps.startSubbandId));

                rfCenterFreqValueLabel.setText(
                        rfCenterFreqFormatter.format(compAmps.rfCenterFrequency));

                halfFrameNumberValueLabel.setText(
                        halfFrameFormatter.format(compAmps.halfFrameNumber));

                bandwidthValueLabel.setText(
                        bandwidthFormatter.format(compAmps.hzPerSubband));

                aveCompAmpsSubbandNumberValueLabel.setText(
                        subbandNumberFormatter.format(compAmps.startSubbandId));

                aveCompAmpsBandwidthValueLabel.setText(
                        bandwidthFormatter.format(compAmps.hzPerSubband));

                activityIdValueLabel.setText(
                        activityIdFormatter.format(compAmps.activityId));
            }
            else 
            {

                // Not in GUI mode, draw directly on image

                String header1 = "Waterfall: ";
                if (title.equals(""))
                {
                    // use last part of filename as title
                    header1 += " File: " + new File(inFilename).getName();  
                }
                else
                {
                    header1 += title;
                }

                String header2 = "Center Freq: "
                    + rfCenterFreqFormatter.format(compAmps.rfCenterFrequency)
                    + "  Subband: "
                    + subbandNumberFormatter.format(compAmps.startSubbandId)
                    + "  BW: "
                    + bandwidthFormatter.format(compAmps.hzPerSubband)
                    + "  #Half Frames: "
                    + halfFrameFormatter.format(compAmps.halfFrameNumber)
                    + "  ActId: "
                    + activityIdFormatter.format(compAmps.activityId);

                // make room in top of buffer for header labels
                // line separator
                int pixelBrightness=100;
                int nRows=1;
                addRowsOfConstantBrightness(pixelBrightness, nRows);

                Graphics2D g2d = scaledBuffImage.createGraphics();
                int fontHeight = g2d.getFontMetrics().getHeight();

                //System.out.println("font height is: " + fontHeight);

                // space for header labels
                pixelBrightness=0; 
                int nHeaderLines=2;
                int nWaterfallRows=nHeaderLines * fontHeight;
                addRowsOfConstantBrightness(pixelBrightness, nWaterfallRows);

                int headerX = 5;
                int headerY = g2d.getFontMetrics().getAscent();
                g2d.drawString(header1, headerX, headerY);
                headerY += fontHeight;
                g2d.drawString(header2, headerX, headerY);

            }

        }


        /**
         * Add the average complex amplitites and add to the image.
         *
         * @param fftValues th earray of computed FFT values.
         */ 
        private void plotAverageComplexAmplitudes(double[] fftValues)
        {

            if (usingGui)
            {

                averageComplexAmplitudes.add(fftValues);

                plotComplexAmplitudes(averageComplexAmplitudes.getAverageValues());
            }
        }

        /**
         * Add a line of complex amplitude averates to the image.
         *
         * @param values the array of average values.
         */
        private void plotComplexAmplitudes(double[] values)
        {
            //System.out.println("plotComplexAmplitudes");

            int datasetIndex = 0;
            boolean connected = true;
            double xvalue;
            double yvalue;

            clearPlot();

            compAmps.printHeader();

            int discard = (int) (values.length * compAmps.overSampling);
            int start = discard / 2;
            int end = values.length - discard / 2;
            // go through data array & plot all the points
            for (int i = 0; i < values.length; ++i)
            {
                xvalue = i;  // use index for now
                if (i < start || i >= end)
                    yvalue = 0;
                else
                    yvalue = values[i];
                readoutPlot.addPoint(datasetIndex, xvalue, yvalue, connected);
            }

            // add a final point at the end with a zero Y value,
            // to force the y axis to display full scale
            xvalue = values.length;
            yvalue = 0;
            readoutPlot.addPoint(datasetIndex, xvalue, yvalue, connected);

            // properly scale the plot to fit this data
            readoutPlot.fillPlot();  

        }

        /**
         * Clear the plot. 
         */
        private void clearPlot()
        {
            int datasetIndex = 0;

            if (usingGui)
            {
                readoutPlot.clear(datasetIndex);
            }
        }

        /**
         * Sleep for 750 milliseconds.
         */
        private void slowPlaySleep()
        {
            try 
            {
                int latency = 750;  //mS, simulate data delivery rate

                Thread.sleep(latency);
            }
            catch (InterruptedException e)
            {
                // Interrupt may be thrown manually by stop()
            }
        }


        /**
         * Save the waterfall image to a jpeg file.
         */
        private void saveWaterfallAsJpegFile() throws java.io.IOException
        {

            // Crop the image buffer so that any blank
            // areas at the bottom are eliminated.
            BufferedImage croppedWaterfall = 
                scaledBuffImage.getSubimage(
                        0,0,scaledBuffImage.getWidth(),
                        imageRowCount);

            File jpegFile = new File(outFilename);
            ImageIO.write(croppedWaterfall, "jpg", jpegFile);
        }

        /**
         * Run continually.
         */
        public void run()
        {

            openFile();

            int rowSize = width;
            int count = 0;
            int[] pixbuff = new int[rowSize];
            double[] fftValues;

            boolean continueRunning = true;

            while (continueRunning)
            {

                try 
                {

                    // Keep reading pixel data.  When enough
                    // for a row shows up, then add it to the display.

                    int x = 0;
                    int minBytesToRead = 1;
                    while (in != null && (in.available() >= minBytesToRead))   
                    {

                        if (readGrayscalePixels) 
                        {		    
                            int rawByte = in.readUnsignedByte();
                            pixbuff[x++] = rawByte;

                            count++;
                            if (x >= rowSize)
                            {
                                addPixelRowToImage(pixbuff);
                                x = 0;
                            }
                        } 
                        else 
                        {

                            if (slowPlay) 
                            {
                                slowPlaySleep();
                            }

                            //read in the data
                            compAmps.read(in);

                            if (usingGui) 
                            {
                                updateDisplayHeaders(compAmps);
                            }			       

                            double[] timeSamples = compAmps.coef;


                            if (displayResolution == 1) 
                            {

                                fftValues = 
                                    coefConversion.convertCoefsTo1HzFreqValues(
                                            timeSamples);
                            }
                            else if(displayResolution == 2)
                            {

                                fftValues = 
                                    coefConversion.convertCoefsTo2HzFreqValues(
                                            timeSamples);
                            }
                            else //4
                            {

                                fftValues = 
                                    coefConversion.convertCoefsTo4HzFreqValues(
                                            timeSamples);
                            }

                            plotAverageComplexAmplitudes(fftValues);

                            coefConversion.convertFFTValuesToPixels(
                                    fftValues, pixbuff);

                            addPixelRowToImage(pixbuff);


                        }
                    }

                    if (! usingGui) 
                    {

                        updateDisplayHeaders(compAmps);

                        saveWaterfallAsJpegFile();

                        continueRunning = false;

                    }

                    // Detecting file truncation in this
                    // way is not documented but seems to work.
                    // TBD: come up with a better method
                    if (in != null && in.available() < 0)
                    {
                        throw new EOFException();
                    }

                }
                catch (EOFException e)
                {
                    // Input file was truncated (or error)
                    // Reopen file to start over.

                    System.out.println(e);

                    if (usingGui)
                    {
                        openFile();
                    }
                    else
                    {
                        // batch mode, hit EOF unexpectedly, so quit
                        continueRunning = false;
                    }
                }
                catch (IOException e)
                {
                    System.out.println(e);
                }

                if (repeatPlay)
                {
                    sleep();
                    openFile();
                }

                //System.out.println("read " + count + " bytes");

                // wait for more data to appear
                sleep();
            }
        }
    }

    /**
     * Scale the image. 
     */
    public void scaleImage()
    {
        // apply scale & offset to pixels
        RescaleOp rescaleOp = new RescaleOp(pixelScaleFactor, pixelOffsetFactor, null);
        rescaleOp.filter(origBuffImage, scaledBuffImage);
    }

    /**
     * Clear the image.
     */
    public void eraseImage()
    {

        // set all the pixels in original image to zero
        float scaleFactor = (float)0.0;
        float offsetFactor = (float)0.0;
        RescaleOp zeroRescaleOp = new RescaleOp(scaleFactor, offsetFactor, null);
        zeroRescaleOp.filter(scaledBuffImage, origBuffImage);

        redrawImage();

    }

    /**
     * Redraw the image.
     */
    public void redrawImage()
    {
        scaleImage();

        if (usingGui)
        {
            imagePanel.repaint();
        }
    }


    /**
     * Listener that opens a file.
     */
    class OpenFileListener implements ActionListener
    {
        JFileChooser fc;

        /**
         * Constructor.
         *
         * @param fc the JFileChooser to use.
         */
        OpenFileListener(JFileChooser fc)
        {
            this.fc = fc;
        }

        /**
         * Perform the file open.
         *
         * param e the ActionEvent instance.
         */
        public void actionPerformed(ActionEvent e)
        {

            int returnVal = fc.showOpenDialog(WaterfallDisplay.this);

            if (returnVal == JFileChooser.APPROVE_OPTION)
            {

                File file = fc.getSelectedFile();
                String fullpath = file.getAbsolutePath();
                fileScanner.openNewFile(fullpath);
                changeDisplayedFilename(file.getName());

            }
            else
            {
                //System.out.println("Open command cancelled by user.");
            }
        }
    }


    /**
     * Save the waterfall data (header text and graphics) as a JPEG file.
     */
    class SaveAsJpegListener implements ActionListener
    {

        JFileChooser fc;

        /**
         * Constructor.
         *
         * @param fc the JFileChooser to use.
         */
        SaveAsJpegListener(JFileChooser fc)
        {
            this.fc = fc;
        }

        /**
         * Perform the file saving.
         *
         * param e the ActionEvent instance.
         */
        public void actionPerformed(ActionEvent e)
        {

            int returnVal = fc.showSaveDialog(WaterfallDisplay.this);

            if (returnVal == JFileChooser.APPROVE_OPTION)
            {

                File file = fc.getSelectedFile();
                String absoluteFilename = file.getAbsolutePath();

                try 
                {
                    Container content = mainGuiFrame.getContentPane();
                    Dimension size = content.getSize();
                    BufferedImage image = 
                        new BufferedImage(size.width, size.height,
                                BufferedImage.TYPE_INT_RGB);
                    Graphics2D g2d = image.createGraphics();
                    content.paint(g2d);

                    File jpegFile = new File(absoluteFilename);
                    ImageIO.write(image, "jpg", jpegFile);
                }
                catch (IOException ioe)
                {
                    System.out.println(ioe);
                }

            }
            else
            {
                //System.out.println("Open command cancelled by user.");
            }
        }
    }



    /**
     * Perform the printing initiated by a user action.
     */
    class PrintListener implements ActionListener
    {

        /**
         * Perform the print action.
         *
         * param e the ActionEcent containing information about this event.
         */
        public void actionPerformed(ActionEvent e)
        {
            PageFormat pageFormat = printJob.defaultPage();
            pageFormat.setOrientation(PageFormat.LANDSCAPE);
            printJob.setPrintable(imagePanel, pageFormat);

            if (printJob.printDialog())
            {
                try 
                {
                    printJob.print();  
                } catch (Exception ex)
                {
                    ex.printStackTrace();
                }
            }
        }
    }


    /**
     * Filename filter that accepts directories and files with a particular
     * extension.
     */
    class CustomFilenameFilter extends javax.swing.filechooser.FileFilter
    {

        /** The file extension eg, "jpeg" */
        String fileExtension;
        /** The dile description eg, "*.jpeg (jpeg images) */
        String fileDescription;

        /**
         * Constructor.
         *
         * @param fileExtension the file extension to filter on.
         * @param fileDescription to list in the selection dialog.
         */
        CustomFilenameFilter(String fileExtension, String fileDescription)
        {
            this.fileExtension = fileExtension;
            this.fileDescription = fileDescription;
        }

        /**
         * Get the extension of a file name.
         *
         * @param f the File instance.
         * @return the extension part of the file name.
         **/
        public String getExtension(File f)
        {
            String ext = null;
            String s = f.getName();
            int i = s.lastIndexOf('.');

            if (i > 0 &&  i < s.length() - 1)
            {
                ext = s.substring(i+1).toLowerCase();
            }
            return ext;
        }

        /**
         * Accept all directories and all files with "." + fileExtension.
         *
         * @param f a File instance
         */
        public boolean accept(File f)
        {


            if (f.isDirectory())
            {
                return true;
            }

            String extension = getExtension(f);
            if (extension != null)
            {
                if (extension.equals(fileExtension))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

            return false;
        }

        /** 
         * Get the description of this filter.
         *
         * @return the file destription string.
         */
        public String getDescription()
        {
            return fileDescription;
        }
    }

    /**
     * Handle the pixel offset slider.
     */
    class OffsetSliderListener implements ChangeListener
    {
        /**
         * Perform the slide state change action.
         *
         * @param e the ChangeEvent information.
         */
        public void stateChanged(ChangeEvent e)
        {
            JSlider source = (JSlider)e.getSource();
            if (! source.getValueIsAdjusting())
            {
                float value = (float)source.getValue();
                pixelOffsetFactor = value;
                redrawImage();
            }
        }
    }

    /**
     * Handle the scale slider.
     */
    class ScaleSliderListener implements ChangeListener
    {
        /**
         * Perform the scale state changed action.
         *
         * @param e the ChangeEvent information.
         */
        public void stateChanged(ChangeEvent e)
        {
            JSlider source = (JSlider)e.getSource();
            if (! source.getValueIsAdjusting())
            {
                float value = (float)source.getValue() / 100;
                pixelScaleFactor = value;
                redrawImage();
            }    
        }
    }

    /**
     * Handle the resolution combo box selection.
     */
    class ResolutionListener implements ActionListener
    {

        /**
         * Perform the resolution combo box selection action.
         *
         * param e the ActionEvent information.
         */
        public void actionPerformed(ActionEvent e) 
        {
            JComboBox cb = (JComboBox)e.getSource();
            String resolutionValue = (String)cb.getSelectedItem();

            if (resolutionValue.equals("1 Hz")) 
            {
                displayResolution = 1;
            }
            else if (resolutionValue.equals("2 Hz")) 
            {
                displayResolution = 2;
            }
            else if (resolutionValue.equals("4 Hz")) 
            {
                displayResolution = 4;
            }
            else 
            {
                System.out.println("Error selecting resolution");
            }

            // reopen data file to display in the new resolution
            fileScanner.openFile();

        }
    }


    /**
     * Handle the subband offset combo box selection.
     **/
    class SubbandOffsetListener implements ActionListener
    {

        /**
         * Perform the subband combo box selection action.
         *
         * param e the ActionEvent information.
         */

        public void actionPerformed(ActionEvent e)
        {
            JComboBox cb = (JComboBox)e.getSource();
            String subbandOffsetString = (String)cb.getSelectedItem();

            //System.out.println("selected subbandOffset " + subbandOffsetString);

            subbandOffset = Integer.parseInt(subbandOffsetString); 

            compAmps.setSubbandOffset(subbandOffset);

            // re-read the file at the new subbandOffset
            fileScanner.openFile();

        }
    }


    /**
     * Change the display name text field.
     *
     * @param newname the new file name to display.
     */
    private void changeDisplayedFilename(String newname)
    {
        currentFileText.setText("" + newname + "");
        aveCompAmpsCurrentFileText.setText(newname);
    }


    /**
     * Get the sse archive directory.
     * If the SSE_ARCHIVE env var is set, use that,
     * else use the default $HOME/sonata_archive.
     * @return the archive directory string.
     */
    private String getArchiveDir()
    {
        EnvReader env = new EnvReader();
        String archiveDir = env.getEnvVar("SSE_ARCHIVE"); 
        if (archiveDir == null)
        {
            archiveDir = env.getEnvVar("HOME") + "/sonata_archive"; 
        }
        return archiveDir;
    }

    /**
     * Convert the a bin number to its frequency.
     *
     * @param binNumber the number of the bin.
     * @param rfCenterFrequency the center frequency of the data.
     * @param hzPerSubband hertz/subband for this data.
     * @return the frequency of the bin.
     */
    private double convertBinNumberToFreq(int binNumber,
            double rfCenterFrequency,
            double hzPerSubband)
    {
        // define the nominal 1Hz values
        int numberOfBinsPerSubband = MAX_SUBBAND_BINS_PER_1HZ_HALF_FRAME;
        if (compAmps.activityId > 0)
        {
            numberOfBinsPerSubband = (int) (numberOfBinsPerSubband
                    * (1.0 - compAmps.overSampling));
        }

        // adjust number of bins if the display mode is currently 2 Hz
        numberOfBinsPerSubband /= this.displayResolution;

        double hzPerBin = hzPerSubband / numberOfBinsPerSubband;
        double hzPerMHz = 1e6;

        // figure out how far the desired bin is from the 
        // center.

        int centerBinNumber = numberOfBinsPerSubband / 2;
        double offsetInHz = (binNumber - centerBinNumber) * hzPerBin;
        double freqInMHz = offsetInHz / hzPerMHz + rfCenterFrequency;

        return freqInMHz;
    }

    /**
     * Display the bin number the cursor is over.
     */
    class ReadoutBinNumberMouseListener implements MouseMotionListener 
    {
        // offset to subtract from the x position due to the left border
        // of the container the image is in

        int leftBorderOffset;  

        /**
         * Constructor.
         *
         * @param leftBorderOffset the offest of the image from the left 
         * border in pixls.
         */
        ReadoutBinNumberMouseListener(int leftBorderOffset) 
        {
            this.leftBorderOffset = leftBorderOffset;
        }

        /**
         * Handle the mouse dragged event.
         * This event is not used.
         *
         * @param event the MouseEvent information for this event.
         */
        public void mouseDragged(MouseEvent event)
        {
        }

        /**
         * Handle the mouse moved event. Calculate the bin number and print the
         * value to the binNumberReadoutValueLabel.
         *
         * @param event the MouseEvent information for this event.
         */
        public void mouseMoved(MouseEvent event) 
        {

            int x = event.getX() - leftBorderOffset;
            int bin = (width/2) - ((width/2) - x)*displayResolution ; 

            // don't let bin go negative or positive
            if (bin < 0 || bin > width)
            {
                binNumberReadoutValueLabel.setText("?");
                binFreqReadoutValueLabel.setText("?");
            }
            else
            {

                DecimalFormat binFormatter = new DecimalFormat("0000");
                binNumberReadoutValueLabel.setText(binFormatter.format(bin));

                double freq = convertBinNumberToFreq(bin, 
                        compAmps.rfCenterFrequency,
                        compAmps.hzPerSubband);

                DecimalFormat freqFormatter = new DecimalFormat("0000.000000 MHz");
                binFreqReadoutValueLabel.setText(freqFormatter.format(freq));
            }
        }
    }


    /**
     * Calculate and display the aveCompAmpsBinFreqReadoutValueLabel,
     * aveCompAmpsBinNumberReadoutValueLabel, and 
     * aveCompAmpsPowerReadoutValueLabel.
     *
     * @param source the source ReadoutPlot instance.
     * @param xPlotValue the subband number.
     * @param yPlotValue the subband power.
     */
    public void readoutData(ReadoutPlot source, 
            double xPlotValue,
            double yPlotValue)
    {

        // don't let bin number or power go negative
        int binNumber = (int) (xPlotValue + 0.5);
        if (binNumber < 0) binNumber = 0;

        float power = (float) yPlotValue;
        if (power < 0) power = 0;

        double freq = convertBinNumberToFreq(binNumber, compAmps.rfCenterFrequency,
                compAmps.hzPerSubband);

        DecimalFormat freqFormatter = new DecimalFormat("0000.000000 MHz");
        DecimalFormat binFormatter = new DecimalFormat("0000");
        DecimalFormat powerFormatter = new DecimalFormat("00000.000  ");

        aveCompAmpsBinFreqReadoutValueLabel.setText(freqFormatter.format(freq));
        aveCompAmpsBinNumberReadoutValueLabel.setText(binFormatter.format(binNumber));
        aveCompAmpsPowerReadoutValueLabel.setText(powerFormatter.format(power));

    }

    /**
     * Create the image adjustment control panel.
     */
    private void createImageAdjustmentControlPanel()
    {
        int frameWidth = 425;
        int frameHeight = 125;

        imageAdjustmentFrame = new JFrame("SonATA Complex Amplitudes - Image Adjustment");
        imageAdjustmentFrame.setSize(frameWidth, frameHeight);

        JPanel controlPanel = new JPanel();
        controlPanel.setBorder(BorderFactory.createLineBorder(Color.blue));
        controlPanel.setLayout(new BorderLayout());

        JPanel controlPanelPart1 = new JPanel();
        controlPanelPart1.setLayout(new BorderLayout());
        controlPanel.add(BorderLayout.NORTH, controlPanelPart1);

        // constrast control (pixel multiplier)
        JSlider multiplySlider = new JSlider(JSlider.HORIZONTAL, 0, 1000, 400);
        multiplySlider.setBorder(new TitledBorder("Contrast"));
        multiplySlider.setMajorTickSpacing(200);
        multiplySlider.setPaintTicks(true);
        multiplySlider.setPaintLabels(true);
        multiplySlider.addChangeListener(new ScaleSliderListener());
        controlPanelPart1.add(BorderLayout.WEST, multiplySlider);

        // offset control
        JSlider offsetSlider = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        offsetSlider.setBorder(new TitledBorder("Brightness"));
        offsetSlider.setMajorTickSpacing(50);
        offsetSlider.setPaintTicks(true);
        offsetSlider.setPaintLabels(true);
        offsetSlider.addChangeListener(new OffsetSliderListener());
        controlPanelPart1.add(BorderLayout.EAST, offsetSlider);

        Container cp = imageAdjustmentFrame.getContentPane();
        cp.setLayout(new BorderLayout());
        cp.add(BorderLayout.NORTH, controlPanel);


    }

    /**
     * Create a complex amplitude time average plot in a JFrame.
     */
    private void createCompAmpTimeAvgPlot()
    {
        int plotFrameWidth = 660;
        int plotFrameHeight = 400;

        plotFrame = new JFrame("SonATA Complex Amplitudes - Time Average");
        plotFrame.setSize(plotFrameWidth, plotFrameHeight);

        readoutPlot = new ReadoutPlot();
        readoutPlot.addReadoutListener(this);
        readoutPlot.setTitle("SonATA Complex Amplitudes - Time Average");
        readoutPlot.setXLabel("Bin Number");
        readoutPlot.setYLabel("Power");
        readoutPlot.setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));

        //plotFrame.setVisible(true);

        JPanel controlPanel = new JPanel();
        controlPanel.setBorder(BorderFactory.createLineBorder(Color.blue));
        controlPanel.setLayout(new BorderLayout());

        JPanel controlPanelPart1 = new JPanel();
        controlPanelPart1.setLayout(new BorderLayout());
        controlPanel.add(BorderLayout.NORTH, controlPanelPart1);

        JPanel controlLine1Panel = new JPanel();
        controlPanelPart1.add(BorderLayout.NORTH, controlLine1Panel);

        JPanel controlLine2Panel = new JPanel();
        controlPanelPart1.add(BorderLayout.SOUTH, controlLine2Panel);

        // subband number
        JLabel aveCompAmpsSubbandNumberTitleLabel = new JLabel("Subband: ");
        aveCompAmpsSubbandNumberValueLabel = new JLabel("?");
        controlLine1Panel.add(aveCompAmpsSubbandNumberTitleLabel);
        controlLine1Panel.add(aveCompAmpsSubbandNumberValueLabel);

        // cursor readout values
        // bin freq
        JLabel aveCompAmpsBinFreqReadoutTitleLabel = new JLabel("Cursor Freq: ");
        aveCompAmpsBinFreqReadoutValueLabel = new JLabel("?");
        controlLine1Panel.add(aveCompAmpsBinFreqReadoutTitleLabel);
        controlLine1Panel.add(aveCompAmpsBinFreqReadoutValueLabel);

        // cursor readout - bin number
        JLabel aveCompAmpsBinNumberReadoutTitleLabel = new JLabel("Bin: ");
        aveCompAmpsBinNumberReadoutValueLabel = new JLabel("?");
        controlLine1Panel.add(aveCompAmpsBinNumberReadoutTitleLabel);
        controlLine1Panel.add(aveCompAmpsBinNumberReadoutValueLabel);

        // cursor readout - power
        JLabel aveCompAmpsPowerReadoutTitleLabel = new JLabel("Power: ");
        aveCompAmpsPowerReadoutValueLabel = new JLabel("?");
        controlLine1Panel.add(aveCompAmpsPowerReadoutTitleLabel);
        controlLine1Panel.add(aveCompAmpsPowerReadoutValueLabel);

        // bandwidth
        JLabel aveCompAmpsBandwidthTitleLabel = new JLabel("Bandwidth: ");
        aveCompAmpsBandwidthValueLabel = new JLabel("?");
        controlLine1Panel.add(aveCompAmpsBandwidthTitleLabel);
        controlLine1Panel.add(aveCompAmpsBandwidthValueLabel);


        // current filename
        JLabel aveCompAmpsCurrentFileLabel = new JLabel("File: ");
        aveCompAmpsCurrentFileText = new JLabel("None");
        controlLine2Panel.add(aveCompAmpsCurrentFileLabel);
        controlLine2Panel.add(aveCompAmpsCurrentFileText);
        changeDisplayedFilename(new File(inFilename).getName());  // display last part only

        Container cp = plotFrame.getContentPane();
        cp.setLayout(new BorderLayout());
        cp.add(BorderLayout.NORTH, controlPanel);
        cp.add(BorderLayout.CENTER, readoutPlot);

    }

    /**
     * Set up the GUI.
     */
    private void setUpGui()
    {

        // ------- Menu ---------------------

        JMenuBar menuBar = new JMenuBar();
        mainGuiFrame.setJMenuBar(menuBar);

        JMenu fileMenu = new JMenu("File");
        menuBar.add(fileMenu);


        JFileChooser openFileChooser = new JFileChooser();

        // get the location of the confirmation data in the
        // archive data directory
        String confirmDataDir = getArchiveDir() + "/confirmdata";
        openFileChooser.setCurrentDirectory(new File(confirmDataDir));

        if (readGrayscalePixels)
        {
            CustomFilenameFilter grayFilenameFilter = 
                new CustomFilenameFilter("gray", "*.gray (gray scale pixels)");
            openFileChooser.setFileFilter(grayFilenameFilter);
        }
        else
        {

            CustomFilenameFilter archiveCompampFilenameFilter = 
                new CustomFilenameFilter("archive-compamp", 
                        "*.archive-compamp (archive complex amplitudes)");
            openFileChooser.setFileFilter(archiveCompampFilenameFilter);

            CustomFilenameFilter compampFilenameFilter = 
                new CustomFilenameFilter("compamp", "*.compamp (complex amplitudes)");
            openFileChooser.setFileFilter(compampFilenameFilter);


        }

        JMenuItem openFileMenuItem =  new JMenuItem("Open File ...");
        fileMenu.add(openFileMenuItem);
        openFileMenuItem.addActionListener(new OpenFileListener(openFileChooser));

        /**
         * Handle the file scanner open file action.
         */
        class RereadListener implements ActionListener
        {
            /**
             * Handle the file scanner open action.
             *
             * param e the ActionEvent information for this action.
             */
            public void actionPerformed(ActionEvent e)
            {
                fileScanner.openFile();
            }
        }
        JMenuItem rereadFileMenuItem =  new JMenuItem("Reread File");
        fileMenu.add(rereadFileMenuItem);
        rereadFileMenuItem.addActionListener(new RereadListener());


        JMenuItem printFileMenuItem =  new JMenuItem("Print ...");
        fileMenu.add(printFileMenuItem);
        printFileMenuItem.addActionListener(new PrintListener());
        printJob = PrinterJob.getPrinterJob();


        JMenuItem saveAsJpegFileMenuItem =  new JMenuItem("Save As JPeg ...");
        JFileChooser jpegFileChooser = new JFileChooser();
        CustomFilenameFilter jpegFilenameFilter = 
            new CustomFilenameFilter("jpeg", "*.jpeg (jpeg images)");
        jpegFileChooser.setFileFilter(jpegFilenameFilter);
        jpegFileChooser.setDialogTitle("Save Display as a JPEG image...");
        jpegFileChooser.setDialogType(JFileChooser.SAVE_DIALOG);
        saveAsJpegFileMenuItem.addActionListener(new SaveAsJpegListener(jpegFileChooser));
        fileMenu.add(saveAsJpegFileMenuItem);


        /**
         * Handle the exit event.
         */
        class ExitListener implements ActionListener
        {
            /**
             * Handle the exit.
             *
             * param e the ActionEvent intance with information about this 
             * event.
             */
            public void actionPerformed(ActionEvent e)
            {
                System.exit(0);
            }
        }

        JMenuItem exitMenuItem = new JMenuItem("Exit");
        exitMenuItem.addActionListener(new ExitListener());
        fileMenu.addSeparator();
        fileMenu.add(exitMenuItem);


        // ------- Control Panel ---------------------

        JPanel controlPanel = new JPanel();
        controlPanel.setBorder(BorderFactory.createLineBorder(Color.blue));
        controlPanel.setLayout(new BorderLayout());

        JPanel controlPanelPart1 = new JPanel();
        JPanel controlPanelPart2 = new JPanel();
        controlPanelPart1.setLayout(new BorderLayout());
        controlPanelPart2.setLayout(new BorderLayout());

        controlPanel.add(BorderLayout.NORTH, controlPanelPart1);
        controlPanel.add(BorderLayout.CENTER, controlPanelPart2);

        JPanel controlLine1Panel = new JPanel();
        JPanel controlLine2Panel = new JPanel();
        controlPanelPart1.add(BorderLayout.NORTH, controlLine1Panel);
        controlPanelPart1.add(BorderLayout.CENTER, controlLine2Panel);

        JPanel controlLine3Panel = new JPanel();
        controlPanelPart2.add(BorderLayout.NORTH, controlLine3Panel);


        // -- control line 1 ------
        JLabel resLabel = new JLabel("Res:");
        controlLine1Panel.add(resLabel);

        // use a combo box to select the display resolution
        String[] resolutionStrings = { "1 Hz", "2 Hz", "4 Hz"};
        JComboBox resolutionComboBox = new JComboBox(resolutionStrings);
        resolutionComboBox.setSelectedIndex((int)(this.displayResolution/2));
        resolutionComboBox.addActionListener(new ResolutionListener());
        controlLine1Panel.add(resolutionComboBox);


        // use a combo box to select the subbandOffset (subband selection)
        JLabel subbandOffsetLabel = new JLabel("Subband Offset:");
        controlLine1Panel.add(subbandOffsetLabel);

        String[] subbandOffsetStrings = { "0", "1", "2", "3", "4", "5", "6",
            "7", "8", "9", "10", "11", "12", 
            "13", "14", "15"};
        JComboBox subbandOffsetComboBox = new JComboBox(subbandOffsetStrings);
        subbandOffsetComboBox.setSelectedIndex(subbandOffset);
        subbandOffsetComboBox.addActionListener(new SubbandOffsetListener());
        controlLine1Panel.add(subbandOffsetComboBox);


        // current filename
        JLabel currentFileLabel = new JLabel("File:");
        currentFileText = new JLabel("None");
        controlLine1Panel.add(currentFileLabel);
        controlLine1Panel.add(currentFileText);
        changeDisplayedFilename(new File(inFilename).getName());  // display last part only


        // -- control line 2 ------

        // Pol
        JLabel polTitleLabel = new JLabel("Pol:");
        polValueLabel = new JLabel("?");
        controlLine2Panel.add(polTitleLabel);
        controlLine2Panel.add(polValueLabel);


        // RF center freq
        JLabel rfCenterFreqTitleLabel = new JLabel("Center Freq:");
        rfCenterFreqValueLabel = new JLabel("?");
        controlLine2Panel.add(rfCenterFreqTitleLabel);
        controlLine2Panel.add(rfCenterFreqValueLabel);

        // subband number
        JLabel subbandNumberTitleLabel = new JLabel("Subband:");
        subbandNumberValueLabel = new JLabel("?");
        controlLine2Panel.add(subbandNumberTitleLabel);
        controlLine2Panel.add(subbandNumberValueLabel);

        // half frame number
        JLabel halfFrameNumberTitleLabel = new JLabel("Half Frame:");
        halfFrameNumberValueLabel = new JLabel("?");
        controlLine2Panel.add(halfFrameNumberTitleLabel);
        controlLine2Panel.add(halfFrameNumberValueLabel);

        // cursor readout values
        // bin freq
        JLabel binFreqReadoutTitleLabel = new JLabel("Cursor Freq:");
        binFreqReadoutValueLabel = new JLabel("?");
        controlLine2Panel.add(binFreqReadoutTitleLabel);
        controlLine2Panel.add(binFreqReadoutValueLabel);

        // bandwidth
        JLabel bandwidthTitleLabel = new JLabel("BW:");
        bandwidthValueLabel = new JLabel("?");
        controlLine1Panel.add(bandwidthTitleLabel);
        controlLine1Panel.add(bandwidthValueLabel);

        // Activity Id
        JLabel activityIdTitleLabel = new JLabel("Act:");
        activityIdValueLabel = new JLabel("?");
        controlLine1Panel.add(activityIdTitleLabel);
        controlLine1Panel.add(activityIdValueLabel);

        // cursor readout - bin number
        JLabel binNumberReadoutTitleLabel = new JLabel("Bin:");
        binNumberReadoutValueLabel = new JLabel("?");
        controlLine1Panel.add(binNumberReadoutTitleLabel);
        controlLine1Panel.add(binNumberReadoutValueLabel);

        // Image panel to display the waterfall plot
        imagePanel = new ImagePanel();
        imagePanel.setImage(scaledBuffImage);
        imagePanel.setPreferredSize(new Dimension(width, height));

        int leftBorderWidth = 0;
        imagePanel.addMouseMotionListener(
                new ReadoutBinNumberMouseListener(leftBorderWidth));

        // Scroll panel that contains the image panel
        JScrollPane scrollPane = new JScrollPane(imagePanel);


        JMenu viewMenu = new JMenu("View");
        menuBar.add(viewMenu);

        /**
         * Clear the display.
         */
        class ClearDisplayListener implements ActionListener
        {
            /**
             * Handle the erase image action.
             *
             * param e the ActionEvent instance with information about the 
             * event.
             */
            public void actionPerformed(ActionEvent e)
            {
                eraseImage();  
            }
        }
        JMenuItem clearDisplayMenuItem =  new JMenuItem("Clear Display");
        viewMenu.add(clearDisplayMenuItem);
        clearDisplayMenuItem.addActionListener(new ClearDisplayListener());


        createImageAdjustmentControlPanel();

        /**
         *  Display contrast/brightness panel
         */
        class ImageAdjustmentControlPanelListener implements ActionListener
        {
            /**
             * Handle the image adjustment action.
             *
             * param e the ActionEvent information for this action.
             */
            public void actionPerformed(ActionEvent e)
            {
                imageAdjustmentFrame.setVisible(true);
            }
        }

        JMenuItem imageAdjustmentControlPanelMenuItem =  
            new JMenuItem("Adjust Brightness/Contrast...");
        viewMenu.add(imageAdjustmentControlPanelMenuItem);
        imageAdjustmentControlPanelMenuItem.addActionListener(new 
                ImageAdjustmentControlPanelListener());


        createCompAmpTimeAvgPlot();

        /**
         * Display Ave. Comp Amp Plot
         */
        class AveCompAmpDisplayListener implements ActionListener
        {
            /**
             * Handle the average complex amplitude display action.
             *
             * param e the ActionEvent information for this action.
             */
            public void actionPerformed(ActionEvent e)
            {
                plotFrame.setVisible(true);
            }
        }

        JMenuItem aveCompAmpDisplayMenuItem =  new JMenuItem("Display Time Average Plot...");
        viewMenu.add(aveCompAmpDisplayMenuItem);
        aveCompAmpDisplayMenuItem.addActionListener(new AveCompAmpDisplayListener());


        JMenu helpMenu = new JMenu("Help");
        menuBar.add(helpMenu);

        /**
         * Handle the Help Version action.
         */
        class HelpVersionListener implements ActionListener
        {
            /**
             * Handle the action that displays the help verion dialog.
             *
             * param e the ActionEvent information for this action.
             */
            public void actionPerformed(ActionEvent e)
            {
                JOptionPane.showMessageDialog(
                        mainGuiFrame,
                        "Version: $Revision: 1.76 $ $Date: 2009/06/12 00:14:06 $",
                        "Waterfall Display",  // dialog title
                        JOptionPane.INFORMATION_MESSAGE ); 
            }
        }
        JMenuItem showVersionMenuItem =  new JMenuItem("Show Version...");
        showVersionMenuItem.addActionListener(new HelpVersionListener());
        helpMenu.add(showVersionMenuItem);


        Container cp = mainGuiFrame.getContentPane();
        cp.setLayout(new BorderLayout());
        cp.add(BorderLayout.NORTH, controlPanel);
        cp.add(BorderLayout.CENTER, scrollPane);
    }


    /**
     * Initialize the program.
     */
    public void init()
    {

        if (usingGui)
        {
            setUpGui();
        }

        // start file reading thread
        fileScanner = new FileScanner(inFilename, width, height);

        redrawImage();

    }

    /**
     * Process the command line options.
     */
    private static class WaterfallCmdLineOptions 
    {

        String inFilename;
        String outFilename;
        int resolutionHz;
        int xpos;
        int ypos;
        int subbandOffset;
        boolean slowPlay;
        boolean repeatPlay;
        boolean batch;
        String title;

        /**
         * Constructor.
         */
        public WaterfallCmdLineOptions()
        {
            inFilename = "";
            outFilename = "waterfall.jpeg";
            resolutionHz = 1;
            xpos = 0;
            ypos = 0;
            subbandOffset = 0;
            slowPlay = false;
            repeatPlay = false;
            batch = false;
            title = "";
        }

        /**
         * Get the input file name.
         *
         * @return the input file name.
         */
        public String getInFilename()
        {
            return inFilename;
        }

        /**
         * Get the output file name.
         *
         * @return the output file name.
         */
        public String getOutFilename()
        {
            return outFilename;
        }

        /**
         * Get the resolution in Hertz.
         *
         * @return the resolution in Hertz.
         */
        public int getResolutionHz()
        {
            return resolutionHz;
        }

        /**
         * Get the X position.
         *
         * @return the X position.
         */
        public int getXpos()
        {
            return xpos;
        }

        /**
         * Get the Y position.
         *
         * @return the Y position.
         */
        public int getYpos()
        {
            return ypos;
        }

        /**
         * Get the subband offset.
         *
         * @return the subband offset.
         */
        public int getSubbandOffset()
        {
            return subbandOffset;
        }

        /**
         * Get the slow play flag.
         *
         * @return true if slow play should be performed, false if not.
         */
        public boolean getSlowPlay()
        {
            return slowPlay;
        }

        /**
         * Get the repeat play flag.
         *
         * @return true if repeat play should be performed, false if not.
         */
        public boolean getRepeatPlay()
        {
            return repeatPlay;
        }

        /**
         * Get the batch flag.
         *
         * @return true if batch should be performed, false if not.
         */
        public boolean getBatch()
        {
            return batch;
        }

        /**
         * Get the title of the frame.
         *
         * @return the title of the frame.
         */
        public String getTitle()
        {
            return title;
        }

        /**
         * Parse the command line paramters.
         *
         * @param args the array of command line argument strings.
         * @return true if arguments are read in with no errors. false if
         * there are errors.
         */
        public boolean parseCommandLineArgs(String[] args)
        {

            int i = 0;
            while (i < args.length)
            {

                if (args[i].equals("-file"))
                {
                    i++;
                    if (i < args.length)
                    {
                        inFilename = args[i++];
                    }
                    else
                    {
                        System.out.println("Missing input filename argument");
                        return false;
                    }

                }
                else if (args[i].equals("-outfile"))
                {
                    i++;
                    if (i < args.length)
                    {
                        outFilename = args[i++];
                    }
                    else
                    {
                        System.out.println("Missing output filename argument");
                        return false;
                    }

                }
                else if (args[i].equals("-res"))
                {

                    i++;
                    if (i < args.length)
                    {
                        String resString = args[i++];
                        if (resString.equals("1")) 
                        {
                            resolutionHz = 1;
                        }
                        else if (resString.equals("2")) 
                        {
                            resolutionHz = 2;
                        } 
                        else if (resString.equals("4")) 
                        {
                            resolutionHz = 4;
                        } 
                        else 
                        {
                            System.out.println("invalid resolution Hz: must be 1, 2 or 4");
                            return false;
                        }

                    }  
                    else 
                    {
                        System.out.println("Missing resolution argument");
                        return false;
                    }

                } 
                else if (args[i].equals("-xpos")) 
                {

                    i++;
                    if (i < args.length) 
                    {

                        try 
                        {
                            xpos = Integer.parseInt(args[i++]);
                        } 
                        catch (Exception e) 
                        {
                            System.out.println("invalid x position");
                            return false;
                        }

                    }
                    else
                    {
                        System.out.println("Missing xpos argument");
                        return false;
                    }

                }
                else if (args[i].equals("-ypos"))
                {

                    i++;
                    if (i < args.length)
                    {

                        try 
                        {
                            ypos = Integer.parseInt(args[i++]);
                        } catch (Exception e)
                        {
                            System.out.println("invalid y position");
                            return false;
                        }

                    }
                    else
                    {
                        System.out.println("Missing ypos argument");
                        return false;
                    }

                }
                else if (args[i].equals("-suboff"))
                {

                    i++;
                    if (i < args.length)
                    {

                        try 
                        {
                            subbandOffset = Integer.parseInt(args[i++]);
                        } 
                        catch (Exception e)
                        {
                            System.out.println("invalid subband offset");
                            return false;
                        }

                        int MaxSubbandOffset = 15;
                        if (subbandOffset < 0 || subbandOffset > MaxSubbandOffset)
                        {
                            System.out.println("subband offset out of range");                  
                            System.out.println("must be >= 0 and <= " + MaxSubbandOffset);
                            return false;
                        }

                    }
                    else
                    {
                        System.out.println("Missing subband offset argument");
                        return false;
                    }
                } 
                else if (args[i].equals("-slow"))
                {
                    i++;
                    slowPlay = true;
                } 
                else if (args[i].equals("-repeat"))
                {
                    i++;
                    repeatPlay = true;

                } 
                else if (args[i].equals("-batch"))
                {
                    i++;
                    batch = true;
                }
                else if (args[i].equals("-title"))
                {
                    i++;
                    if (i < args.length)
                    {
                        // grab first word of title
                        title += args[i++];
                    }
                    else
                    {
                        System.out.println("Missing title argument");
                        return false;
                    }

                    // Grab rest of title words
                    // until arguments run out or 
                    // another keyword shows up
                    // (prefixed with hypen)

                    while (i < args.length)
                    {
                        if (args[i].charAt(0) == '-')
                        {
                            break;
                        }
                        title += " ";
                        title += args[i++];
                    }

                }
                else
                {
                    System.out.println("invalid option: " + args[i]);
                    System.out.println("valid options: [-file <inFilename>] [-res <1 | 2>] [-xpos <x position>] [-ypos <y position>] [-slow] [-repeat] [-suboff <subband offset>] [-batch] [-outfile <outFilename>] [-title <text>]");
                    System.out.println("-file: input filename");
                    System.out.println("-res: data display resolution in Hz (1 or 2)");
                    System.out.println("-xpos: window x position");
                    System.out.println("-ypos: window y position");
                    System.out.println("-slow: play slowly");
                    System.out.println("-repeat: repeatedly read data file");
                    System.out.println("-suboff: subband offset to display (0-15)");
                    System.out.println("-batch: no GUI, send waterfall image to jpeg file");
                    System.out.println("-outfile: output filename for batch mode (default: 'waterfall.jpeg')");
                    System.out.println("-title: text for title (batch mode only, default: input filename)");
                    return false;
                } 
            }

            return true;
        }

    }

    /**
     * The main entry pont for the program.
     *
     * @param args the command line arguments string array.
     */
    public static void main(String[] args)
    {

        // Note: -batch mode must be run with "java -Djava.awt.headless=true"

        int imageWidth=768;
        int imageHeight=800;
        int extraWidth=40;   // add enough to show full image 
        int frameWidth=imageWidth+extraWidth; 
        int frameHeight=500;

        float pixelScaleFactor=4;
        float pixelOffsetFactor=0;

        WaterfallCmdLineOptions options = new WaterfallCmdLineOptions();
        if ( ! options.parseCommandLineArgs(args) )
        {
            System.exit(1);
        }

        boolean usingGui = ! options.getBatch();

        if (options.getResolutionHz() == 2)
        {
            if (usingGui)
            {
                // Narrow the frame width for 2 hz resolution.
                // Don't change the image width in case the
                // frame gets expanded back to 1 Hz mode later.

                frameWidth=imageWidth / 2 + extraWidth;
            }
            else
            {
                // Adjust image size so that created jpeg image is
                // only as wide as it needs to be.

                imageWidth = 539;
            }
        }

        //System.out.println("filename is " + options.getFilename());
        //System.out.println("res is " + options.getResolutionHz());

        JFrame frame = null;
        if (usingGui)
        {
            frame = new JFrame("Waterfall Display - SonATA Complex Amplitudes");
            frame.setLocation(new Point(options.getXpos(), options.getYpos()));
        }
        WaterfallDisplay waterfall =
            new WaterfallDisplay(usingGui,
                    frame, 
                    options.getInFilename(),
                    options.getOutFilename(),
                    options.getSubbandOffset(),
                    imageWidth, imageHeight, 
                    pixelScaleFactor, pixelOffsetFactor,
                    options.getResolutionHz(),
                    options.getSlowPlay(), 
                    options.getRepeatPlay(),
                    options.getTitle());

        if (usingGui)
        {
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.getContentPane().add(waterfall);
            frame.setSize(frameWidth, frameHeight);
        }

        waterfall.init();

        if (usingGui) 
        {
            frame.setVisible(true);
        }

    }
}