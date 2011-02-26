/*******************************************************************************

 File:    BaselineDisplay.java
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
 * @file BaselineDisplay.java
 *
 * The Baseline display.
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
import java.io.*;
import java.text.*;
import javax.swing.filechooser.*;
import javax.imageio.*;
import java.awt.print.PrinterJob;
import java.awt.print.*;
import ptolemy.plot.*;

/**
 * Read binary baselines (stored as C structs) on the input file and plot 
 * them.
 * Structs & enums are defined in ssePdmInterface.h and sseInterface.h.
 * Assumes the baseline structs are in network byte order.
 */
public class BaselineDisplay extends JApplet implements ReadoutListener 
{

    // BaselineDisplay instance data
    JFrame frame;
    String filename;
    int width;
    int height;
    float pixelScaleFactor;
    float pixelOffsetFactor;
    BufferedImage origBuffImage;
    BufferedImage scaledBuffImage;
    ImagePanel imagePanel;
    BaselineDataFileScanner fileScanner;
    //JLabel currentFileText;
    PrinterJob printJob;
    ReadoutPlot readoutPlot;  
    JLabel rfCenterFreqValueLabel;
    JLabel bandwidthValueLabel;
    JLabel polValueLabel;
    JLabel subbandFreqReadoutValueLabel;
    JLabel subbandNumberReadoutValueLabel;
    JLabel subbandPowerReadoutValueLabel;
    JLabel halfFrameNumberValueLabel;
    JLabel activityIdValueLabel;
    Baseline baseline;
    boolean slowPlay;
    double[] accumBaselineValue;
    int baselineCount;
    boolean useTimeAvg;
    static final int MAX_SUBBANDS = 8192;

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

            //System.out.println("Painting");

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
        public int print(Graphics g, PageFormat pageFormat, int pageIndex) throws PrinterException 
        {
            if (pageIndex >= 1)
            {
                return Printable.NO_SUCH_PAGE;
            }

            // create a copy of the image that has the grayscale 
            // inverted (black for white) to save toner on printouts
            int tableLen = 256;
            short[] grayscaleInvertTable = new short[tableLen];
            for (int i = 0; i < tableLen; i++)
            {
                grayscaleInvertTable[i] = (short) ((tableLen-1) - i);
            }
            BufferedImageOp grayInvertOp =
                new LookupOp(new ShortLookupTable(0, grayscaleInvertTable), null);

            BufferedImage grayInvertedImage = 
                new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
            grayInvertOp.filter(image, grayInvertedImage);

            Graphics2D g2 = (Graphics2D) g;

            //--- Translate the origin to 0,0 for the top left corner
            g2.translate (pageFormat.getImageableX (), pageFormat.getImageableY ());

            // Scale the image to fit properly on the output printed page.
            double POINTS_PER_INCH = 72;
            double widthScaleFactor = pageFormat.getImageableWidth()  
                / POINTS_PER_INCH;
            double heightScaleFactor = pageFormat.getImageableHeight() 
                / POINTS_PER_INCH; 

            g2.drawImage (grayInvertedImage,
                    (int) (0.25 * POINTS_PER_INCH),
                    (int) (0.25 * POINTS_PER_INCH),
                    (int) (widthScaleFactor * POINTS_PER_INCH),
                    (int) (heightScaleFactor * POINTS_PER_INCH),  
                    this);

            return Printable.PAGE_EXISTS;
        }

    }


    /**
     * Manages the baseline data.
     * See ssePdmInterface.h for C++ version of this struct 
     * (BaselineHeader and BaselineValue structs)
     */
    class Baseline
    {
        int maxSubbands = 10000;  // sanity check
        int headerSizeBytes = 32;
        int bytesPerBaselineValue = 4;

        // header
        double rfCenterFrequency;
        double bandwidth;
        int halfFrameNumber;
        int numberOfSubbands;
        int polarization;  // defined as Polarization enum.  Assumed to be 32bit.
        int activityId;
        //int alignPad;

        // variable length body
        float[] baselineValue;

        /** 
         * Get the size of the data header.
         *
         * @return the header size in bytes.
         */
        public int getHeaderSizeBytes()
        {

            return headerSizeBytes;
        }

        /**
         * Get the size of one line of data.
         *
         * @return the data size in bytes.
         */
        public int getBodySizeBytes()
        {
            int bodySizeBytes = numberOfSubbands *  bytesPerBaselineValue;          

            return bodySizeBytes;
        }

        /**
         * Read in one line of data.
         * First the header, then the data body.
         *
         * @param in the input data stream to read from.
         * @throws java.io.IOException.
         */
        public void read(DataInputStream in) throws java.io.IOException 
        {
            readHeader(in);
            readBody(in);
        }

        /**
         * Read the header.
         *
         * @param in the input data stream to read from.
         * @throws java.io.IOException.
         */
        public void readHeader(DataInputStream in) throws java.io.IOException {

            // read the header
            rfCenterFrequency = in.readDouble();
            bandwidth = in.readDouble();
            halfFrameNumber = in.readInt();
            numberOfSubbands = in.readInt();
            polarization = in.readInt();
            activityId = in.readInt();
            //alignPad = in.readInt();

        }

        /**
         * Read the body of the data.
         *
         * @param in the input data stream to read from.
         * @throws java.io.IOException.
         */
        public void readBody(DataInputStream in) throws java.io.IOException {

            // read the variable length baseline value array
            if (numberOfSubbands < 1 || numberOfSubbands > maxSubbands)
            {
                System.out.println(
                        "ERROR: subbands value " + numberOfSubbands +
                        " out of range.  Must be 1 - " + maxSubbands);
                //System.exit(1);

                throw new java.io.IOException();
            }

            baselineValue = new float[numberOfSubbands];
            for (int i=0; i<numberOfSubbands; ++i)
            {
                baselineValue[i] = in.readFloat();
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
                    " bandwidth " + bandwidth + " MHz," +
                    " halfFrameNumber " + halfFrameNumber + "," +
                    " #subbands " +  numberOfSubbands + "," +
                    " pol " + polIntToString(polarization) + "," + 
                    " activityId " + activityId
                    );
            // alignPad
        }

        /**
         * Print to standard out the latest read in complex amplitude values.
         */
        public void printBody()
        {
            int maxToPrint = 5;
            System.out.print("baselineValues: ");
            for (int i=0; i<maxToPrint && i<numberOfSubbands; ++i)
            {
                System.out.print(baselineValue[i] + " ");
            }
            System.out.println("");

        }

    }



    /**
     * Constructor.
     *
     * @param frame the JFrame containing the GUI.
     * @param filename the input data file name.
     * @param width the width if the waterfall image window.
     * @param height the hieght if the waterfall image window.
     * @param pixelScaleFactor the scale factor.
     * @param pixelOffsetFactor the pixel offset of the image.
     * @param slowPlay if true display the data slowly to approximate
     * the speed of the real time system.
     * @param useTimeAvg if true display the average over time.
     */
    public BaselineDisplay(JFrame frame, String filename,
            int width, int height, 
            float pixelScaleFactor,  float pixelOffsetFactor,
            boolean slowPlay,
            boolean useTimeAvg)
    {
        this.frame = frame;
        this.filename = filename;
        this.width = width;
        this.height = height;
        this.pixelScaleFactor = pixelScaleFactor;
        this.pixelOffsetFactor = pixelOffsetFactor;
        this.slowPlay = slowPlay;
        this.useTimeAvg = useTimeAvg;

        origBuffImage = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        scaledBuffImage = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
        baseline = new Baseline();

        this.accumBaselineValue = new double[MAX_SUBBANDS];

    }

    /**
     * Convert a subband number to the frequency it represents.
     *
     * @param subband the subband to convert.
     * @return the frequency in MHz of the subband.
     */
    private double convertSubbandNumberToFreq(int subband)
    {
        int numberOfSubbands = baseline.numberOfSubbands;
        int centerSubband = numberOfSubbands / 2;

        double mhzPerSubband = baseline.bandwidth / 
            numberOfSubbands;

        // figure out how far the desired subband is from the 
        // center.

        double freq = (subband - centerSubband) * mhzPerSubband +
            baseline.rfCenterFrequency;

        return freq;
    }


    /**
     * Calculate and display the subbandFreqReadoutValueLabel,
     * subbandNumberReadoutValueLabel, and
     * subbandPowerReadoutValueLabel.
     *
     * @param source the source ReadoutPlot instance.
     * @param xPlotValue the subband number.
     * @param yPlotValue the subband power.
     */
    public void readoutData(ReadoutPlot source, 
            double xPlotValue, 
            double yPlotValue)
    {
        //System.out.println("x=" + xPlotValue + " y=" + yPlotValue);

        // don't let subband number or power go negative
        int subbandNumber = (int) (xPlotValue + 0.5);
        if (subbandNumber < 0) subbandNumber = 0;

        float subbandPower = (float) yPlotValue;
        if (subbandPower < 0) subbandPower = 0;

        double freq = convertSubbandNumberToFreq(subbandNumber);


        DecimalFormat freqFormatter = new DecimalFormat("0000.000000 MHz  ");
        DecimalFormat subbandFormatter = new DecimalFormat("0000  ");
        DecimalFormat powerFormatter = new DecimalFormat("00000.000  ");

        subbandFreqReadoutValueLabel.setText(freqFormatter.format(freq));
        subbandNumberReadoutValueLabel.setText(subbandFormatter.format(subbandNumber));
        subbandPowerReadoutValueLabel.setText(powerFormatter.format(subbandPower));

    }

    /**
     * Read a file containing gray pixel data and turn it into an image.
     *
     * If an EOFException is received, it means the file has been truncated
     * and it needs to be reopened.
     * <p>
     * Effectively does a 'tail -f' on the file, sleeping periodically
     * until new data appears.
     */
    private class BaselineDataFileScanner extends Thread
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

        /**
         * Constructor.
         *
         * @param filename the name of the file to read.
         * @param width the width of the image.
         * @param height the height of the image.
         */
        BaselineDataFileScanner(String filename, int width, int height)
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
                    BufferedImage.TYPE_BYTE_GRAY);

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

                clearPlot();

                resetTimeAvg();

                try
                {
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
                //System.out.println("sleeping");
                int latency = 200;  //mS

                // for pauses between plots
                if (slowPlay)
                {
                    latency = 3000;  //mS
                }
                Thread.sleep(latency);
            }
            catch (InterruptedException e)
            {
                // Interrupt may be thrown manually by stop()
            }
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
            shiftedBuffImage.getRaster().setPixels(0, 0, width, nrows, pixrow);

            // copy shifted image back to orig
            aftOpCopy.filter(shiftedBuffImage, origBuffImage);

            redrawImage();
        }

        /**
         * Clear the plot.
         */
        private void clearPlot()
        {
            int datasetIndex = 0;
            readoutPlot.clear(datasetIndex);
        }

        /**
         * Update the GUI text information based on the header information.
         *
         * @param baseline the instance of the Baseline class containing
         * the information to display.
         */
        private void updateDisplayHeaders(Baseline baseline)
        {

            DecimalFormat rfCenterFreqFormatter = new DecimalFormat("0000.000000 MHz  ");
            DecimalFormat bandwidthFormatter = new DecimalFormat("0.00 MHz  ");
            DecimalFormat halfFrameFormatter = new DecimalFormat("0000  ");
            DecimalFormat activityIdFormatter =
                new DecimalFormat("0000 ");

            rfCenterFreqValueLabel.setText(
                    rfCenterFreqFormatter.format(baseline.rfCenterFrequency));
            bandwidthValueLabel.setText(
                    bandwidthFormatter.format(baseline.bandwidth));
            halfFrameNumberValueLabel.setText(
                    halfFrameFormatter.format(baseline.halfFrameNumber));
            polValueLabel.setText(baseline.getPolAsString() + "  ");
            activityIdValueLabel.setText(
                        activityIdFormatter.format(baseline.activityId));

        }

        /**
         * Reset the time average to zeros.
         */
        private void resetTimeAvg()
        {
            baselineCount = 0;

            for (int i = 0; i < accumBaselineValue.length; ++i)
            {
                accumBaselineValue[i] = 0;
            }

        }

        /**
         * Calculate and plot the baseline.
         *
         * @param baseline the Baseline instance to plot.
         */
        private void plotBaseline(Baseline baseline)
        {
            int datasetIndex = 0;
            double xvalue;
            double yvalue;

            clearPlot();

            // accumulate values for time average
            baselineCount++;
            for (int i = 0; i < baseline.baselineValue.length; ++i)
            {
                accumBaselineValue[i] += baseline.baselineValue[i];
            }

            // go through baseline data array & plot all the points
            for (int i = 0; i < baseline.baselineValue.length; ++i)
            {
                xvalue = i;  // use index for now

                if (useTimeAvg)
                {
                    yvalue = accumBaselineValue[i] / baselineCount;
                }
                else 
                {
                    yvalue = baseline.baselineValue[i];
                }
                //Add the point. Last arg "true" means connect
                readoutPlot.addPoint(datasetIndex, xvalue, yvalue, true);
            }

            // add a final point at the end with a zero Y value,
            // to force the y axis to display full scale
            xvalue = baseline.baselineValue.length;
            yvalue = 0;
            //JR - April 14, 2010, changed conected (last argulement) to
            //false. This makes the last point not draw a line that drops
            //off to zero.
            readoutPlot.addPoint(datasetIndex, xvalue, yvalue, false);

            // properly scale the plot to fit this data
            readoutPlot.fillPlot();  

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

            boolean continueRunning = true;
            boolean waitingForHeader = true;

            while (continueRunning)
            {

                try
                {

                    // Try to read data, but wait until at least the
                    // minimum acceptable record size is available to
                    // avoid hitting EOF prematurely.  Ie, try
                    // to get the EOF exception only when the file is
                    // truncated, and not just because a partial record 
                    // has been appended to the file. 

                    // Because the body of the baseline record is of variable 
                    // length, first wait for a header, then wait for the
                    // length of array defined in the header.

                    while (in != null && (in.available() > 0))
                    {

                        if (waitingForHeader)
                        {
                            if (in.available() >= baseline.getHeaderSizeBytes())
                            {
                                baseline.readHeader(in);
                                baseline.printHeader();
                                updateDisplayHeaders(baseline);

                                waitingForHeader = false;
                            }
                        }

                        if (!waitingForHeader)
                        {
                            if (in.available() >= baseline.getBodySizeBytes())
                            {
                                baseline.readBody(in);
                                baseline.printBody();
                                plotBaseline(baseline);

                                waitingForHeader = true;
                            }
                        }

                        // pause between reads
                        if (slowPlay)
                        {
                            sleep();
                        }

                    }

                    // Detecting file truncation in this
                    // way is not documented but seems to work.
                    /** @todo Devise a better way to handle exceptions raised
                     *        while reading and displaying the data.<BR>
                     *        Maybe reset file and start over?
                     */
                    if (in != null && in.available() < 0)
                    {
                        throw new EOFException();
                    }

                }
                catch (EOFException e)
                {
                    // Input file was truncated.  
                    // Reopen file to start over.

                    openFile();

                }
                catch (IOException e)
                {
                    System.out.println(e);

                    openFile();
                }

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
     * Redraw the image.
     */
    public void redrawImage()
    {
        scaleImage();

        imagePanel.repaint();
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

            int returnVal = fc.showOpenDialog(BaselineDisplay.this);

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
     * Save the Baseline data as a JPEG file.
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

            int returnVal = fc.showSaveDialog(BaselineDisplay.this);

            if (returnVal == JFileChooser.APPROVE_OPTION)
            {

                File file = fc.getSelectedFile();
                String absoluteFilename = file.getAbsolutePath();

                try
                {
                    File jpegFile = new File(absoluteFilename);
                    ImageIO.write(scaledBuffImage, "jpg", jpegFile);
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
                }
                catch (Exception ex)
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

        String fileExtension;    // eg, "jpeg"
        String fileDescription;  // eg, "*.jpeg (jpeg images)

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
         * param e the ChangeEvent information.
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
         * param e the ChangeEvent information.
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
     * Change the display name text field.
     *
     * @param newname the new file name to display.
     */
    private void changeDisplayedFilename(String newname)
    {
        //currentFileText.setText(newname);

        readoutPlot.setTitle(newname);
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
     * Initialize the program.
     */
    public void init()
    {

        readoutPlot = new ReadoutPlot();
        readoutPlot.addReadoutListener(this);
        readoutPlot.setXLabel("Subband Number");
        readoutPlot.setYLabel("Power");
        readoutPlot.setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));

        JPanel controlPanel = new JPanel();

        JMenuBar menuBar = new JMenuBar();
        setJMenuBar(menuBar);

        JMenu fileMenu = new JMenu("File");
        menuBar.add(fileMenu);


        JFileChooser openFileChooser = new JFileChooser();

        // get the location of the confirmation data in the
        // archive data directory
        String confirmDataDir = getArchiveDir() + "/confirmdata";
        openFileChooser.setCurrentDirectory(new File(confirmDataDir));
        CustomFilenameFilter baselineFilenameFilter = 
            new CustomFilenameFilter("baseline", "*.baseline (SonATA binary baselines)");
        openFileChooser.setFileFilter(baselineFilenameFilter);

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
        // TBD: this is not yet fully implemented
        //fileMenu.add(saveAsJpegFileMenuItem);

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

        JMenu viewMenu = new JMenu("View");
        menuBar.add(viewMenu);

        /**
         * Handle the clear plot event.
         */
        class ClearPlotListener implements ActionListener
        {
            /**
             * Handle the clear plot.
             *
             * param e the ActionEvent intance with information about this
             * event.
             */
            public void actionPerformed(ActionEvent e)
            {
                fileScanner.clearPlot();  
            }
        }

        JMenuItem clearPlotMenuItem =  new JMenuItem("Clear Plot");
        viewMenu.add(clearPlotMenuItem);
        clearPlotMenuItem.addActionListener(new ClearPlotListener());

        /**
         * Handle the rescale plot event.
         */
        class RescalePlotListener implements ActionListener
        {
            /**
             * Handle the rescale plot.
             *
             * param e the ActionEvent intance with information about this
             * event.
             */
            public void actionPerformed(ActionEvent e)
            {
                // restore the plot to normal (unzoomed) scale
                readoutPlot.fillPlot();
            }
        }

        JMenuItem rescalePlotMenuItem =  new JMenuItem("Rescale Plot");
        viewMenu.add(rescalePlotMenuItem);
        rescalePlotMenuItem.addActionListener(new RescalePlotListener());

        JSlider contrastSlider = new JSlider(JSlider.HORIZONTAL, 0, 1000, 100);
        contrastSlider.setBorder(new TitledBorder("Pixel Multiplier %"));
        contrastSlider.setMajorTickSpacing(200);
        contrastSlider.setPaintTicks(true);
        contrastSlider.setPaintLabels(true);
        contrastSlider.addChangeListener(new ScaleSliderListener());
        controlPanel.add(contrastSlider);


        JSlider offsetSlider = new JSlider(JSlider.HORIZONTAL, -100, 100, 0);
        offsetSlider.setBorder(new TitledBorder("Pixel Offset"));
        offsetSlider.setMajorTickSpacing(50);
        offsetSlider.setPaintTicks(true);
        offsetSlider.setPaintLabels(true);
        offsetSlider.addChangeListener(new OffsetSliderListener());
        controlPanel.add(offsetSlider);

        imagePanel = new ImagePanel();
        imagePanel.setImage(scaledBuffImage);
        imagePanel.setPreferredSize(new Dimension(width, height));
        imagePanel.setBorder(BorderFactory.createLineBorder(Color.blue));

        JScrollPane scrollPane = new JScrollPane(imagePanel);


        JPanel baselineControlPanel = new JPanel();
        baselineControlPanel.setLayout(new BorderLayout());

        JPanel controlLine1Panel = new JPanel();
        JPanel controlLine2Panel = new JPanel();
        baselineControlPanel.add(BorderLayout.NORTH, controlLine1Panel);
        baselineControlPanel.add(BorderLayout.CENTER, controlLine2Panel);



        /**
         * Handle the time average on/off event.
         */
        class TimeAvgListener implements ItemListener
        {
            /**
             * Handle the time average on/off.
             *
             * param e the ItemEvent intance with information about this
             * event.
             */
            public void itemStateChanged(ItemEvent e)
            {
                if (e.getStateChange() == ItemEvent.DESELECTED)
                {
                    useTimeAvg = false;
                } 
                else
                {
                    useTimeAvg = true;
                }
            }
        }
        JCheckBox timeAvgCheckBox = new JCheckBox("Time Avg", useTimeAvg);
        timeAvgCheckBox.addItemListener(new TimeAvgListener());
        controlLine1Panel.add(timeAvgCheckBox);


        // Pol
        JLabel polTitleLabel = new JLabel("Pol: ");
        polValueLabel = new JLabel("?  ");
        controlLine1Panel.add(polTitleLabel);
        controlLine1Panel.add(polValueLabel);


        // RF center freq
        JLabel rfCenterFreqTitleLabel = new JLabel("Center Freq: ");
        rfCenterFreqValueLabel = new JLabel("?  ");
        controlLine1Panel.add(rfCenterFreqTitleLabel);
        controlLine1Panel.add(rfCenterFreqValueLabel);

        // Bandwidth
        JLabel bandwidthTitleLabel = new JLabel("BW:");
        bandwidthValueLabel = new JLabel("?  ");
        controlLine1Panel.add(bandwidthTitleLabel);
        controlLine1Panel.add(bandwidthValueLabel);


        // current filename
/*
        JLabel currentFileLabel = new JLabel("File: ");
        currentFileText = new JLabel("None            ");
        controlLine2Panel.add(currentFileLabel);
        controlLine2Panel.add(currentFileText);
        // display filename, last part only
*/
        changeDisplayedFilename(new File(filename).getName()); 


        // half frame number
        JLabel halfFrameNumberTitleLabel = new JLabel("Half Frame #: ");
        halfFrameNumberValueLabel = new JLabel("?  ");
        controlLine2Panel.add(halfFrameNumberTitleLabel);
        controlLine2Panel.add(halfFrameNumberValueLabel);


        // cursor readout values
        // subband freq
        JLabel subbandFreqReadoutTitleLabel = new JLabel("Cursor Freq: ");
        subbandFreqReadoutValueLabel = new JLabel("?  ");
        controlLine2Panel.add(subbandFreqReadoutTitleLabel);
        controlLine2Panel.add(subbandFreqReadoutValueLabel);

        // subband number
        JLabel subbandNumberReadoutTitleLabel = new JLabel("Subband: ");
        subbandNumberReadoutValueLabel = new JLabel("?  ");
        controlLine2Panel.add(subbandNumberReadoutTitleLabel);
        controlLine2Panel.add(subbandNumberReadoutValueLabel);

        // subband power
        JLabel subbandPowerReadoutTitleLabel = new JLabel("Power: ");
        subbandPowerReadoutValueLabel = new JLabel("?  ");
        controlLine2Panel.add(subbandPowerReadoutTitleLabel);
        controlLine2Panel.add(subbandPowerReadoutValueLabel);

        // Activity Id
        JLabel activityIdTitleLabel = new JLabel("Act:");
        activityIdValueLabel = new JLabel("?");
        controlLine1Panel.add(activityIdTitleLabel);
        controlLine1Panel.add(activityIdValueLabel);



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
             * param event the ActionEvent information for this action.
             */
            public void actionPerformed(ActionEvent e)
            {
                JOptionPane.showMessageDialog(
                        frame,
                        "Version: $Revision: 1.30 $ $Date: 2008/10/24 23:39:57 $",
                        "Baseline Display",  // dialog title
                        JOptionPane.INFORMATION_MESSAGE ); 
            }
        }

        JMenuItem showVersionMenuItem =  new JMenuItem("Show Version...");
        showVersionMenuItem.addActionListener(new HelpVersionListener());
        helpMenu.add(showVersionMenuItem);

        Container cp = getContentPane();
        cp.setLayout(new BorderLayout());

        cp.add(BorderLayout.NORTH, baselineControlPanel);
        cp.add(BorderLayout.CENTER, readoutPlot);

        // start file reading thread
        fileScanner = new BaselineDataFileScanner(filename, width, height);

        redrawImage();

    }

    /**
     * Process the command line options.
     */
    private static class BaselineCmdLineOptions
    {

        String filename;
        int xpos;
        int ypos;
        int width;
        int height;
        boolean slowPlay;
        boolean timeAvg;

        /**
         * Constructor.
         */
        public BaselineCmdLineOptions()
        {
            filename = "";
            xpos = 0;
            ypos = 0;
            width = 660;
            height = 400;
            slowPlay = false;
            timeAvg = false;
        }

        /**
         * Get the input file name.
         *
         * @return the input file name.
         */
        public String getFilename()
        {
            return filename;
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
         * Get the width.
         *
         * @return the width.
         */
        public int getWidth()
        {
            return width;
        }

        /**
         * Get the height.
         *
         * @return the width.
         */
        public int getHeight()
        {
            return height;
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
         * Get the time average flag.
         *
         * @return true if time averaging should be performed, false if not.
         */
        public boolean getTimeAvg()
        {
            return timeAvg;
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
                        filename = args[i++];
                    }
                    else
                    {
                        System.out.println("Missing filename argument");
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
                        }
                        catch (Exception e)
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
                else if (args[i].equals("-width"))
                { 

                    i++;
                    if (i < args.length)
                    {

                        try
                        {
                            width = Integer.parseInt(args[i++]);
                        }
                        catch (Exception e)
                        {
                            System.out.println("invalid width");
                            return false;
                        }

                    }
                    else
                    {
                        System.out.println("Missing width argument");
                        return false;
                    }
                }
                else if (args[i].equals("-height"))
                { 

                    i++;
                    if (i < args.length)
                    {

                        try
                        {
                            height = Integer.parseInt(args[i++]);
                        }
                        catch (Exception e)
                        {
                            System.out.println("invalid height");
                            return false;
                        }

                    }
                    else
                    {
                        System.out.println("Missing height argument");
                        return false;
                    }
                }
                else if (args[i].equals("-slow"))
                {

                    i++;
                    slowPlay = true;

                }
                else if (args[i].equals("-timeavg"))
                {

                    i++;
                    timeAvg = true;

                }
                else
                {
                    System.out.println("invalid option: " + args[i]);
                    System.out.println("valid options: -file <filename> -xpos <x position> -ypos <y position> -width <width> -height <height> -slow -timeavg");
                    System.out.println("-file: SonATA baselines filename");
                    System.out.println("-xpos: window x position");
                    System.out.println("-ypos: window y position");
                    System.out.println("-width: window width (default: " + width + ")");
                    System.out.println("-height: window height (default: " + height + ")");
                    System.out.println("-slow: display data slowly");
                    System.out.println("-timeavg: display data as a time average");
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

        int imageWidth=1024;
        int imageHeight=800;

        float pixelScaleFactor=1;
        float pixelOffsetFactor=0;

        BaselineCmdLineOptions options = new BaselineCmdLineOptions();
        if ( ! options.parseCommandLineArgs(args) )
        {
            System.exit(1);
        }

        JFrame frame = new JFrame("Baseline Display");
        frame.setLocation(new Point(options.getXpos(), options.getYpos()));

        JApplet applet =
            new BaselineDisplay(frame, options.getFilename(), 
                    imageWidth, imageHeight, 
                    pixelScaleFactor, pixelOffsetFactor,
                    options.getSlowPlay(),
                    options.getTimeAvg());

        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.getContentPane().add(applet);
        frame.setSize(options.getWidth(), options.getHeight());
        applet.init();
        applet.start();
        frame.setVisible(true);
    }
}