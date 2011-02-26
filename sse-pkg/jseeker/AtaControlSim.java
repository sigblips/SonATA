/*******************************************************************************

 File:    AtaControlSim.java
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

import java.net.*;
import java.io.*;
import java.text.*;
import java.util.*;

/**
 * Simulator for an Arecibo-like interface for the Prelude system
 * for control of the ATA.

Description:
 The simulator emulates the ASCII ATA to backend control interface defined in
 nss/doc/sonata/ata-backend-control-interface.html.

 There are 32 synthesized beams, and for each beam, there is a dedicated
 "subarray" model that maintains pointing status for that subarray.

 */
public class AtaControlSim implements Runnable
{
   static int defaultCommandPort = 1081;
   static int defaultStatusPort = 1082;

   int mCommandPort;
   int mStatusPort;

   Thread mCommandThread;
   Thread mStatusThread;
   long mStatusInterval = 10000;
   Thread mModelThread;
   static long mModelInterval = 1000;

   private int mObsLenSecs = 600;

   private Object mAttnDbMutex = new Object();
   private double mAttnDb = 0;

   // command strings
   private static final String CMD_ALLOCATE = "ALLOCATE"; // Enable computer control
   private static final String CMD_DEALLOCATE = "DEALLOCATE"; // Disable computer control
   private static final String CMD_STOW = "STOW"; // Stow
   private static final String CMD_STOP = "STOP"; // Stop (at current position)
   private static final String CMD_POINT = "POINT";  // 
   private static final String CMD_TUNE = "TUNE"; // Set RF frequency for Tuning
   private static final String CMD_ATTN = "ATTN"; // Set ATTN for tuning
   private static final String CMD_ZFOCUS = "ZFOCUS"; // Set zfocus RF frequency
   private static final String CMD_MONITOR = "MONITOR"; // Periodic monitor
   private static final String CMD_WRAP = "WRAP"; // set wrap numbers
   private static final String CMD_LNA_ON = "LNAON"; // turn on lnas
   private static final String CMD_PAM_SET = "PAMSET"; // set pams
   private static final String CMD_ANTGROUP = "ANTGROUP"; 
   private static final String CMD_AUTOSELECT = "AUTOSELECT"; 
   private static final String CMD_SET = "SET";
   private static final String CMD_ADD = "ADD";
   private static final String CMD_LIST = "LIST";
   private static final String CMD_CLEAR = "CLEAR";

   private static final String CMD_ALL = "ALL";

   // beamformer commands 
   private static final String CMD_BEAMFORMER = "BF"; // beamformer cmd base
   private static final String CMD_BF_CAL = "CAL"; // calibration
   private static final String CMD_BF_RESET = "RESET"; // reset beamformer
   private static final String CMD_BF_STOP = "STOP"; // stop beamformer
   private static final String CMD_BF_OBSLEN = "OBSLEN"; // beamformer observation length
   private static final String CMD_BF_NULLTYPE = "NULLTYPE";
   private static final String CMD_BF_NULLS = "NULLS";
   private static final String CMD_BF_NULL = "NULL";

   private static final String CMD_BF_INIT = "INIT"; 
   private static final String CMD_BF_COORDS = "COORDS"; 
   private static final String CMD_BF_ANTS = "ANTS"; 
   private static final String CMD_BF_POINT = "POINT"; 
   private static final String CMD_BF_AUTOATTEN = "AUTOATTEN"; 
   private static final String CMD_BF_CONFIG = "CONFIG"; 


   // reponses
   private static final String CMD_OK = "OK"; 
   private static final String READY_PREFIX = "READY: ";
   private static final String ERROR_PREFIX = "ERROR: ";
   public static final String WARNING_PREFIX = "WARNING: "; 
   public static final String INFO_PREFIX = "INFO: ";

   private static final String UNRECOGNIZED_COMMAND = ERROR_PREFIX + "unrecognized command";
   private static final String UNALLOCATED_TSCOPE = ERROR_PREFIX + "unallocated"; 
   private static final String INVALID_NUMBER_OF_ARGS = ERROR_PREFIX + "invalid number of arguments"; 
   private static final String OUT_OF_RANGE = ERROR_PREFIX + "arg out of range"; 
   private static final String TIMEOUT = ERROR_PREFIX + "timeout"; 
   private static final String UNIMPLEMENTED_COMMAND = ERROR_PREFIX + "unimplemented command"; 
   private static final String INVALID_ARGUMENT = ERROR_PREFIX + "invalid argument";
   private static final String INVALID_BEAM = ERROR_PREFIX + "invalid beam name";
   private static final String INVALID_TUNING_NAME = ERROR_PREFIX + "invalid tuning name";
   private static final String INVALID_COORD_SYS = ERROR_PREFIX + "invalid coord system";
   private static final String INVALID_ANTLIST = ERROR_PREFIX + "invalid ant list";
   private static final String INVALID_ANTGROUP = ERROR_PREFIX + "invalid antgroup";
   private static final String ANTGROUP_UNASSIGNED = ERROR_PREFIX + "antgroup unassigned";
   private static final String SUBARRAY_NOT_ASSIGNED_TO_ANY_BEAMS =
   WARNING_PREFIX + "subarray not assigned to any beams";
   private static final String SYSTEM_ERROR = ERROR_PREFIX + "general system error"; // Other system errors
   private static final String NO_COORDS_ASSIGNED_TO_BEAMS = ERROR_PREFIX + "can't point beams, no coordinates assigned";


   // Strings for generating monitor reports
   private static final String DRIVE_SLEW = "SLEW";
   private static final String DRIVE_TRACK = "TRACK";
   private static final String DRIVE_STOP = "STOP";
   private static final String DRIVE_ERROR = "DRIVE_ERROR";

   private static final String COORD_SYS_AZEL = "AZEL"; // Az, El
   private static final String COORD_SYS_J2000 = "J2000"; // RA, Dec (J2000)
   private static final String COORD_SYS_GAL = "GAL"; // Galactic Long,Lat
   private static final String COORD_SYS_UNINIT = "UNINIT"; 


   private static final String CAL_TYPE_DELAY = "DELAY";
   private static final String CAL_TYPE_PHASE = "PHASE";
   private static final String CAL_TYPE_FREQ = "FREQ";

   private static final double MAX_TUNE_FREQ_MHZ = 11200.0;
   private static final double MAX_ATTN_DB = 128.0;

   private static final String AUTOSELECT_ANTS_TEST_LIST = "1a,1b,1c";

   // for writing coordinates
   DecimalFormat mAngleFormatter = null;
   DecimalFormat mFreqFormatter = null;
   Calendar mCalendar = Calendar.getInstance(new SimpleTimeZone(0, "UTC"));
   DecimalFormat mTimeFormatter = null;

   Map mSubarrayModelMap = null;

   private static final String PRIMARY_BEAM_NAME = "PRIMARY";

   // synthesized beam name format is BEAM{X|Y}{A-D}{1-4}
   private static final String[] mBeamNames = {
      "BEAMXA1", "BEAMYA1", "BEAMXA2", "BEAMYA2",
      "BEAMXA3", "BEAMYA3", "BEAMXA4", "BEAMYA4",

      "BEAMXB1", "BEAMYB1", "BEAMXB2", "BEAMYB2",
      "BEAMXB3", "BEAMYB3", "BEAMXB4", "BEAMYB4",

      "BEAMXC1", "BEAMYC1", "BEAMXC2", "BEAMYC2",
      "BEAMXC3", "BEAMYC3", "BEAMXC4", "BEAMYC4",

      "BEAMXD1", "BEAMYD1", "BEAMXD2", "BEAMYD2",
      "BEAMXD3", "BEAMYD3", "BEAMXD4", "BEAMYD4"
   }; 


   private static final String[] mTuningNames = {
      "A", "B", "C", "D"
   };

   private class Pointing {
      public String commandedCoordSystem = COORD_SYS_AZEL;
      public double raHours = 0.0;
      public double decDeg = 0.0;
      public double azDeg = 0.0;
      public double elDeg = 0.0;
      public double glongDeg = 0.0;
      public double glatDeg = 0.0;

      Pointing() {
      }

      Pointing(String commandedCoordSystem,
               double raHours,
               double decDeg,
               double azDeg,
               double elDeg,
               double glongDeg,
               double glatDeg) {
	 this.commandedCoordSystem = commandedCoordSystem;
	 this.raHours = raHours;
	 this.decDeg = decDeg;
	 this.azDeg = azDeg;
	 this.elDeg = elDeg;
	 this.glongDeg = glongDeg;
	 this.glatDeg = glatDeg;
      }

      Pointing(Pointing pointing) {
	 this(pointing.commandedCoordSystem,
	      pointing.raHours,
	      pointing.decDeg,
	      pointing.azDeg,
	      pointing.elDeg,
	      pointing.glongDeg,
	      pointing.glatDeg);
      }

      void copy(Pointing from) {
	 this.commandedCoordSystem = from.commandedCoordSystem;
	 this.raHours = from.raHours;
	 this.decDeg = from.decDeg;
	 this.azDeg = from.azDeg;
	 this.elDeg = from.elDeg;
	 this.glongDeg = from.glongDeg;
	 this.glatDeg = from.glatDeg;
      }
   }

   private class BeamPointingStorage {

      private Map statusMap = new HashMap();

      // store a copy of the status
      public synchronized void add(String beamName, Pointing status) {

	 statusMap.put(beamName, new Pointing(status));
      }

      // update the currently stored status
      public synchronized void update(String beamName, Pointing newStatus) {

	 if (statusMap.containsKey(beamName)) {
	    Pointing oldStatus = (Pointing)statusMap.get(beamName);
	    
	    oldStatus.copy(newStatus); 

	 } else {
	    System.out.println("BeamPointingStorage update error: could not find beamname " + beamName);
	 }

      }

      // return a copy of the beam status
      public synchronized Pointing get(String beamName) {

	 if (statusMap.containsKey(beamName)) {

	    Pointing status = (Pointing) statusMap.get(beamName);
	    return new Pointing(status);

	 }

	 // not found
	 return null;

      }

   }

   private class TuningStatus {
      public double skyFreqMhz = 0.0;
      public int attnXdb = 0;
      public int attnYdb = 0;

      TuningStatus() {
      }

      TuningStatus(double skyFreqMhz,
		   int attnXdb,
		   int attnYdb)
      {
	 this.skyFreqMhz = skyFreqMhz;
	 this.attnXdb = attnXdb;
	 this.attnYdb = attnYdb;
      }

      TuningStatus(TuningStatus status) {
	 this(status.skyFreqMhz,
	      status.attnXdb,
	      status.attnYdb);
      }

      void copy(TuningStatus from) {
	 this.skyFreqMhz = from.skyFreqMhz;
	 this.attnXdb = from.attnXdb;
	 this.attnYdb = from.attnYdb;
      }
   }

   private class TuningStatusStorage {
      private Map statusMap = new HashMap();

      // store a copy of the status
      public synchronized void add(String tuningName, TuningStatus status) {

	 statusMap.put(tuningName, new TuningStatus(status));
      }

      // update the currently stored status
      public synchronized void update(String tuningName, TuningStatus newStatus) {

	 if (statusMap.containsKey(tuningName)) {
	    TuningStatus oldStatus = (TuningStatus)statusMap.get(tuningName);
	    
	    oldStatus.copy(newStatus); 

	 } else {
	    System.out.println("TuningStatusStorage update error: could not find tuning name "
			       + tuningName);
	 }

      }

      // return a copy of the tuning status
      public synchronized TuningStatus get(String tuningName) {

	 if (statusMap.containsKey(tuningName)) {

	    TuningStatus status = (TuningStatus) statusMap.get(tuningName);
	    return new TuningStatus(status);

	 }

	 // not found
	 return null;

      }
   }

   BeamPointingStorage mBeamStatusStorage;
   BeamPointingStorage mBeamPointCmdStorage;
   TuningStatusStorage mTuningStatusStorage;

   private static class CmdLineOptions {

      int commandPort;
      int statusPort;
      boolean runTestSuite = false;

      public CmdLineOptions() {
	 commandPort = defaultCommandPort;
	 statusPort = defaultStatusPort;
      }

      int getCommandPort() {
	 return commandPort;
      }

      int getStatusPort() {
	 return statusPort;
      }

      boolean getRunTestSuite() {
	 return runTestSuite;
      }

      public boolean parseCommandLineArgs(String[] args) {

	 int i = 0;
	 while (i < args.length) {

	    if (args[i].equals("-commandport")) {
			
	       i++;
	       if (i < args.length) {

		  try {
		     commandPort = Integer.parseInt(args[i++]);
		  } catch (Exception e) {
		     System.out.println("invalid command port");
		     return false;
		  }

	       } else {
		  System.out.println("Missing command port argument");
		  return false;
	       }
		    
	    } else if (args[i].equals("-statusport")) {
			
	       i++;
	       if (i < args.length) {

		  try {
		     statusPort = Integer.parseInt(args[i++]);
		  } catch (Exception e) {
		     System.out.println("invalid status port");
		     return false;
		  }

	       } else {
		  System.out.println("Missing status port argument");
		  return false;
	       }

	    } else if (args[i].equals("-test")) {
	       i++;
	       runTestSuite = true;
	    } else {
	       System.out.println("invalid option: " + args[i]);
	       System.out.println("valid options: -commandport <port> -statusport <port> -test");
	       return false;
	    } 
	 }

	 return true;
      }

   }

   private static class AntGroups
   {
      public static final String ANTGROUP_XPOL = "XPOL";
      public static final String ANTGROUP_YPOL = "YPOL";
      public static final String ANTGROUP_PRIMARY = "PRIMARY";
      public static final String ANTGROUP_ALL = "ALL";

      private Map groupsMap = new HashMap();

      AntGroups()
      {
         groupsMap.put(ANTGROUP_XPOL, "");
         groupsMap.put(ANTGROUP_YPOL, "");
         groupsMap.put(ANTGROUP_PRIMARY, "");
      }

      boolean validGroup(String groupName)
      {
         return
            groupName.matches(ANTGROUP_XPOL)
            || groupName.matches(ANTGROUP_YPOL)
            || groupName.matches(ANTGROUP_PRIMARY)
            || groupName.matches(ANTGROUP_ALL);
      }

      void setAntGroup(String groupName, String antList)
      {
         if (! validGroup(groupName))
         {
            // TBD error handling
            return;
         }

         String lcAntList = antList.toLowerCase();

         if (groupName.matches(ANTGROUP_ALL))
         {
            groupsMap.put(ANTGROUP_XPOL, lcAntList);
            groupsMap.put(ANTGROUP_YPOL, lcAntList);
            groupsMap.put(ANTGROUP_PRIMARY, lcAntList);  
         }
         else
         {
            groupsMap.put(groupName, lcAntList);
         }
      }

      String getRawAntList(String groupName)
      {
         return (String)groupsMap.get(groupName);
      }

      private String getFormattedAntList(String groupName)
      {
         StringBuffer buff = new StringBuffer();
         buff.append(groupName);
         buff.append(": ");
         buff.append((String)groupsMap.get(groupName));

         return buff.toString();
      }

      String getAnts(String groupName)
      {
         if (! validGroup(groupName))
         {
            // TBD error handling
            return "error getting group";
         }

         if (groupName.matches(ANTGROUP_ALL))
         {
            StringBuffer buff = new StringBuffer();
            buff.append(getFormattedAntList(ANTGROUP_XPOL));
            buff.append("\n");
            buff.append(getFormattedAntList(ANTGROUP_YPOL));
            buff.append("\n");
            buff.append(getFormattedAntList(ANTGROUP_PRIMARY));

            return buff.toString();
         }
         else
         {
            return getFormattedAntList(groupName);
         }
      }
   }

   AntGroups mAntGroups;

   public AtaControlSim(int commandPort, int statusPort)
   {
      mCommandPort = commandPort;
      mStatusPort = statusPort;

      mAngleFormatter = (DecimalFormat)DecimalFormat.getInstance();
      mAngleFormatter.applyPattern("###.000000");
      mFreqFormatter = (DecimalFormat)DecimalFormat.getInstance();
      mFreqFormatter.applyPattern("###.000000");
      mTimeFormatter = (DecimalFormat)DecimalFormat.getInstance();
      mTimeFormatter.applyPattern("00");

      mBeamStatusStorage = new BeamPointingStorage();
      for (int i=0; i<mBeamNames.length; ++i)
      {
	 mBeamStatusStorage.add(mBeamNames[i], new Pointing());
      }

      mBeamPointCmdStorage = new BeamPointingStorage();
      for (int i=0; i<mBeamNames.length; ++i)
      {
         Pointing point = new Pointing();
         point.commandedCoordSystem = COORD_SYS_UNINIT;
	 mBeamPointCmdStorage.add(mBeamNames[i], point);
      }

      mTuningStatusStorage = new TuningStatusStorage();
      for (int i=0; i<mTuningNames.length; ++i) 
      {
	 mTuningStatusStorage.add(mTuningNames[i], new TuningStatus());
      }
      
      // Subarray pointing model, one associated with each synth beam
      mSubarrayModelMap = new HashMap();
      for (int i=0; i<mBeamNames.length; ++i)
      {
         mSubarrayModelMap.put(mBeamNames[i], new SubarrayModel());
      }

      mAntGroups = new AntGroups();

      // start the command and monitor threads
      mCommandThread = new Thread(this);
      mCommandThread.start();

      mStatusThread = new Thread(this);
      mStatusThread.setDaemon(true);
      mStatusThread.start();

      mModelThread = new Thread(this);
      mModelThread.setDaemon(true);
      mModelThread.start();

   }

   /**
    * Implementing runnable interface.
    */
   public void run()
   { 
      // Decide which thread this is and enter appropriate loop method.
      try {
	 Thread this_thread = Thread.currentThread();

	 if (this_thread == mCommandThread) {
	    commandLoop();
	 }
	 else if (this_thread == mStatusThread) {
	    monitorLoop();
	 }
	 else if (this_thread == mModelThread) {
	    modelLoop();
	 }
	 else {
	    System.out.println("Attempted to start unknown thread, ignoring.");
	 }
      } catch (IOException ioe) {
	 System.out.println(ioe);
      }
 

   }

   /**
    * Receives commands on command socket and dispatches them. Returns
    * messages to NSS such as "OK" or error messages if something goes wrong.
    */
   private void commandLoop() throws IOException {

      ServerSocket server = new ServerSocket(mCommandPort, 1);

      // loop forever -- if the connection goes away, just start over
      // listening for a new one
      for(;;) {
	      
	 Socket socket = null;
	 try
	 {
	    System.out.println("Listening for command connection on port "
			       + server.getLocalPort());
			
	    socket = server.accept();
	    System.out.println("Command socket established with " + socket);

	    // get input from this
	    BufferedReader br = new BufferedReader(
	       new InputStreamReader(socket.getInputStream()));

	    // send output to this
	    PrintStream ps = new PrintStream(socket.getOutputStream());

	    for (;;)
	    {
               // read a new command
	       String line = br.readLine();
	       if (line == null)
	       {
		  // socket broken
		  break;
	       }
	       System.out.println("Received: " + line);

	       String response = dispatchCommand(line);

	       System.out.println("Returning: " + response);
	       ps.print(response + "\n");
	    }
	 }
	 catch (SocketException ex)
	 {
	    System.out.println(ex.toString());
	 }
	 catch (Exception ex)
	 {
	    ex.printStackTrace();
	 }
	 finally
	 {
	    try
	    {
	       if (socket != null) socket.close();
	    }
	    catch (IOException e)
	    {
	       e.printStackTrace();
	    }
	 }
      }
   }

   /**
    * Generate monitor messages and send them to NSS over monitor socket.
    */
   private void monitorLoop() throws IOException {
	   
      ServerSocket server = new ServerSocket(mStatusPort, 1);

      // loop forever -- if the connection goes away, just start over
      // listening for a new one
      for(;;) {

	 Socket socket = null;
	 try
	 {
	    System.out.println("Listening for monitor connection on port "
			       + server.getLocalPort());

	    socket = server.accept();
	    System.out.println("Status socket established with " + socket);

	    // send output to this
	    PrintStream ps = new PrintStream(socket.getOutputStream());

	    // send monitor every so often
	    for (;;)
	    {
	       ps.print(statusString());

	       if (ps.checkError()) {
		  //the socket was closed underneath
		  break;
	       }

	       sleepLocal(mStatusInterval);
	    }
	 }
	 catch (Exception ex)
	 {
	    ex.printStackTrace();
	 }
	 finally
	 {
	    try
	    {
	       if (socket != null) socket.close();
	    }
	    catch (IOException e)
	    {
	       e.printStackTrace();
	    }
	 }

      }
   }

   /*
     Find and return pointers to all the ant models
     whose antenna list is a subset of the one given.
   */
   private Vector getSubarrayModelsThatMatchAntList(
      String antNamesList) {

      Vector modelVector = new Vector();

      Iterator it = mSubarrayModelMap.entrySet().iterator();
      while (it.hasNext())
      {
         Map.Entry entry = (Map.Entry) it.next();
         SubarrayModel model = (SubarrayModel)entry.getValue();
         if (model.antNamesAreSubsetOfNameList(antNamesList))
         {
            modelVector.add(model);
         }

      }
      return modelVector;
   }


   private static class SubarrayModel {

      // comma separated list of ants associated with this model
      //eg, "ant3d,ant3e"
      private volatile String mAntennaNamesString = "";

      private String[] mAntNames = new String[0];

      // telescope variables
      // TBD: rework allocation scheme, default to true for now
      public volatile boolean mIsAllocated = true;
      public volatile String mDriveState = DRIVE_STOP;
      public volatile int mWrap = 1;
      public volatile String mCoordSys = COORD_SYS_J2000;
      public volatile double mAz = 135.0;  // deg
      public volatile double mZen = 30.0;  // deg
      public volatile double mRA = 0.0;
      public volatile double mDec = 0.0;
      public volatile double mError = 0.0;
      public volatile double mGlat = 0.0;
      public volatile double mGlong = 0.0;
      public volatile boolean mCalIsOn = false;
      public volatile double mZfocusFreqMhz = 3000.0;

      // telescope model parameters
      public volatile double mSetRA;
      public volatile double mSetDec;
      public volatile double mSetAz;
      public volatile double mSetZen;
      public volatile double mSetGlat;
      public volatile double mSetGlong;
      public volatile boolean mIsStopped = true;

      // Dish counts
      public volatile int mNumDishesTotal;
      public volatile int mNumDishesSharedPointing;
      public volatile int mNumDishesTrack;
      public volatile int mNumDishesSlew;
      public volatile int mNumDishesStop;
      public volatile int mNumDishesOffline;
      public volatile int mNumDishesDriveError;
      
      private Random rand = null;

      // TBD - what is estimated slew rate of new ata drive motors?
      double slewRateDegPerSec = 4.0;
      double slew_step =  slewRateDegPerSec / mModelInterval * 1000; // deg per interval

      SubarrayModel() {
         rand = new Random();
      }

      public String getAntNamesList() {
         return mAntennaNamesString;
      }
      
      /*
        Split out comma separated names, and prefix
        with 'ant' if missing.  All names are converted
        to lower case.
      */
      private String[] parseAntNames(String subarray)
      {
         String[] names = subarray.trim().split(",");
         for (int i = 0; i < names.length; ++i)
         {
            names[i] = names[i].toLowerCase();
            if (!names[i].startsWith("ant"))
            {
               names[i] = "ant" + names[i];
            }
         }
         return names;
      }


      public void setAntNamesList(String antNamesList)
      {
         mAntennaNamesString = antNamesList;
         mAntNames = parseAntNames(antNamesList);
         mNumDishesTotal = mAntNames.length;
      }
      
      public boolean antNamesAreSubsetOfNameList(String antNamesList)
      {
         if (mAntNames.length == 0)
         {
            return false;
         }

         String[] incomingNames = parseAntNames(antNamesList);
         List incomingNamesList = Arrays.asList(incomingNames);

         for (int i=0; i<mAntNames.length; ++i)
         {
            if (! incomingNamesList.contains(mAntNames[i]))
            {
               return false;
            }
         }

         return true;
      }

      private void clearDishCounts()
      {
         mNumDishesSharedPointing = 0;
         mNumDishesTrack = 0;
         mNumDishesSlew = 0;
         mNumDishesStop = 0;
         mNumDishesOffline = 0;
         mNumDishesDriveError = 0;
      }

      public void update() {

	 if (mIsStopped)
	 {
            // do nothing (just stay put)
	    mSetAz = mAz;
	    mSetZen = mZen;
	    mSetRA = mRA;
	    mSetDec = mDec;
	    mSetGlat = mGlat;
	    mSetGlong = mGlong;
	    mError = 0.0;
	    mDriveState = DRIVE_STOP;

            clearDishCounts();
            mNumDishesStop = mNumDishesTotal;

	 }
	 else // !mIsStopped
	 {
            // check for slew state by calculating error
	    double azerr = mSetAz - mAz;
	    double zenerr = mSetZen - mZen;
	    double raerr = mSetRA - mRA;
	    double decerr = mSetDec - mDec;
	    double glaterr = mSetGlat - mGlat;
	    double glongerr = mSetGlong - mGlong;
	    double err1 = azerr * azerr + zenerr * zenerr;
	    double err2 = raerr * raerr + decerr * decerr;
	    double err3 = glaterr * glaterr + glongerr * glongerr;
	    double maxErr = Math.max(err1, err2);
	    maxErr = Math.max(maxErr, err3);
	    mError = Math.sqrt(maxErr)
	       + rand.nextDouble() * 0.0005 /* deg */;

            // update model appropriately for slewing or tracking
	    if (mError > 0.1 /* deg */) // we must be slewing
	    {
	       // we are slewing
	       mDriveState = DRIVE_SLEW;

               clearDishCounts();
               mNumDishesSlew = mNumDishesTotal;

	       // wind down the difference between set and actual

	       // AZEL
	       if (Math.abs(azerr) > slew_step)
	       {
		  if (azerr > 0.0) mAz += slew_step;
		  else             mAz -= slew_step;
	       }
	       else mAz = mSetAz;

	       if (Math.abs(zenerr) > slew_step)
	       {
		  if (zenerr > 0.0) mZen += slew_step;
		  else              mZen -= slew_step;
	       }
	       else mZen = mSetZen;

	       // RADEC
	       if (Math.abs(raerr) > slew_step)
	       {
		  if (raerr > 0.0) mRA += slew_step;
		  else             mRA -= slew_step;
	       }
	       else mRA = mSetRA;

	       if (Math.abs(decerr) > slew_step)
	       {
		  if (decerr > 0.0) mDec += slew_step;
		  else             mDec -= slew_step;
	       }
	       else mDec = mSetDec;

	       // GAL
	       if (Math.abs(glaterr) > slew_step)
	       {
		  if (glaterr > 0.0) mGlat += slew_step;
		  else             mGlat -= slew_step;
	       }
	       else mGlat = mSetGlat;

	       if (Math.abs(glongerr) > slew_step)
	       {
		  if (glongerr > 0.0) mGlong += slew_step;
		  else             mGlong -= slew_step;
	       }
	       else mGlong = mSetGlong;
	    }
	    else  // we must be tracking
	    {
	       // we are tracking
	       mSetAz = mAz;
	       mSetZen = mZen;
	       mSetRA = mRA;
	       mSetDec = mDec;
	       mSetGlat = mGlat;
	       mSetGlong = mGlong;
	       mDriveState = DRIVE_TRACK;

               clearDishCounts();
               mNumDishesTrack = mNumDishesTotal;
               mNumDishesSharedPointing = mNumDishesTotal;
	    }
	 }
      }

      /**
       * Set the telescope azimuth and elevation, both as degrees
       * @param string versions of az,el
       * @return a string indicating success or error
       */
      String setPrimaryAzEl(String azDegString, String elDegString)
      {
         mIsStopped = false;

         try
         {
            // parse az
            double az = Double.parseDouble(azDegString);
            if (az < 0.0 || az >= 360.0) return OUT_OF_RANGE;
            mSetAz = az;

            // parse el
            double el = Double.parseDouble(elDegString);
            if (el < 0.0 || el > 90.00000) return OUT_OF_RANGE;
            mSetZen = 90.0 - el;

            mCoordSys = COORD_SYS_AZEL;

            System.out.println("(Az, El) set to (" + az + ", " + el + "). ");
            return CMD_OK;
         }
         catch (NumberFormatException nfe)
         {
            return INVALID_ARGUMENT;
         }
      }


      /**
       * Set the telescope (RA, Dec) in J2000 coordinates.
       * ra comes in as hours, dec as degs
       * ra is converted to degrees for internal use
       * @param string versions of ra dec to set J2000
       * @return a string indicating success or error
       */
      String setPrimaryJ2000(String raHoursString, String decDegString)
      {
         mIsStopped = false;

         try
         {
            // parse ra
            double ra = Double.parseDouble(raHoursString);
            if (ra < 0.0 || ra >= 24.0) return OUT_OF_RANGE;
            double degPerHour = 15;
            mSetRA = ra * degPerHour;

            // parse dec
            double dec = Double.parseDouble(decDegString);
            if (dec < -90.0 || dec > 90.0) return OUT_OF_RANGE;
            mSetDec = dec;

            mCoordSys = COORD_SYS_J2000;

            System.out.println("(RA, Dec) set to (" + ra + ", " + dec + "). ");
            return CMD_OK;
         }
         catch (NumberFormatException nfe)
         {
            return INVALID_ARGUMENT;
         }
      }

      /**
       * Set the telescope position in Galactic longitude and latitude.
       * @param coord strings to set Gal
       * @return a string indicating success or error
       */
      String setPrimaryGal(String glongDegString, String glatDegString)
      {
         mIsStopped = false;

         try
         {
            // parse longitude
            double lon = Double.parseDouble(glongDegString);
            if (lon < 0.0 || lon > 360.0) return OUT_OF_RANGE;
            mSetGlong = lon;

            // parse latitude
            double lat = Double.parseDouble(glatDegString);
            if (lat < -90.0 || lat > 90.0) return OUT_OF_RANGE;
            mSetGlat = lat;

            mCoordSys = COORD_SYS_GAL;

            System.out.println("(glong, glat) set to (" + lon + ", " + lat + "). ");
            return CMD_OK;
         }
         catch (NumberFormatException nfe)
         {
            return INVALID_ARGUMENT;
         }
      }

      /**
       * Set up the zfocus with the sky frequency indicated by val.
       * @param val command string containing desired frequency
       * @return string indicated success or error
       */
      String setZfocus(String freqVal)
      {
         try
         {
            // parse the frequency parameter
            double freq = Double.parseDouble(freqVal);
            if (freq < 1.0 || freq > MAX_TUNE_FREQ_MHZ) return OUT_OF_RANGE;
            mZfocusFreqMhz = freq;

            System.out.println("Zfocus Frequency set to " + mZfocusFreqMhz + ". ");
            return CMD_OK;
         }
         catch (NumberFormatException nfe)
         {
            return INVALID_ARGUMENT;
         }
         catch (IndexOutOfBoundsException iobe)
         {
            return INVALID_NUMBER_OF_ARGS + " for zfocus";
         }

      }


      /**
       * Set up the wrap as indicated by val.
       * @param val command string containing desired wrap
       * @return string indicated success or error
       */
      String setWrap(String val)
      {
         String cmdName = ": wrap";

         try
         {
            // parse the wrap parameter
            int wrap = Integer.parseInt(val);
            
            // TBD: range check
            //if (wrap < 1 || wrap > MAX_WRAP) return OUT_OF_RANGE;
            
            mWrap = wrap;
            
            System.out.println("Wrap set to " + wrap + ". ");
            return CMD_OK;
         }
         catch (NumberFormatException nfe)
         {
            return INVALID_ARGUMENT + cmdName;
         }
         catch (IndexOutOfBoundsException iobe)
         {
            return INVALID_NUMBER_OF_ARGS + cmdName;
         }
         
      }


      /**
       * Issue a stop.
       */
      void stop()
      {
         mIsStopped = true;
      }

      void setCal(boolean calIsOn)
      {
         mCalIsOn = calIsOn;
      }


   } // end SubarrayModel

   private void modelLoop()
   {
      // loop forever, updating the telescope position at intervals
      for (;;)
      {
         // update all the subarray models (indexed by synth beam name)
         Set modelMapEntries = mSubarrayModelMap.entrySet();
         Iterator it = modelMapEntries.iterator();
         while (it.hasNext()) {
            Map.Entry entry = (Map.Entry) it.next();
            //System.out.println(entry.getKey());
            SubarrayModel model = (SubarrayModel)entry.getValue();
            model.update();
         }

	 // sleep for interval
	 sleepLocal(mModelInterval);
      }
   }


   /*
     Get commanded coordinates assigned to this beam
   */
   private String getCoordsForBeam(String beam)
   {
      Pointing point = (Pointing)mBeamPointCmdStorage.get(beam);
      if (point == null)
      {
         return INVALID_BEAM + ": " + beam;
      }
        
      return beam + " " + beamStatusString(point);
   }

   public String listCoordsAssignedToBeam(String beam) 
   {
      if (beam.equals(CMD_ALL))
      {
         StringBuffer buff = new StringBuffer();
         for (int i = 0; i < mBeamNames.length; ++i)
         {
            buff.append(getCoordsForBeam(mBeamNames[i]));
            buff.append("\n");
         }
         return buff.toString();
      }

      return getCoordsForBeam(beam);
   }

   private String handleBeamListCoordsCmd(String[] parms)
   {
      String cmdName = ": bf list coords";

      // BF LIST COORDS <beamname|'ALL'>
      int nExpectedWords = 4;
      int beamNameIndex = 3;
      if (parms.length != nExpectedWords)
      {
         return INVALID_NUMBER_OF_ARGS + cmdName;
      }
      return listCoordsAssignedToBeam(parms[beamNameIndex]);
   }

   private String clearCoordsCmdForBeam(String beam)
   {
      if (mBeamPointCmdStorage.get(beam) == null)
      {
         return INVALID_BEAM + ": " + beam;
      }

      Pointing pointing = new Pointing();
      pointing.commandedCoordSystem = COORD_SYS_UNINIT;
      mBeamPointCmdStorage.update(beam, pointing);
        
      return CMD_OK;
   }


   private String handleBeamClearCoordsCmd(String[] parms) 
   {
      // BF Clear Coords <beamname|ALL>
        
      int nExpectedWords = 4;
      if (parms.length != nExpectedWords)
      {
         return INVALID_NUMBER_OF_ARGS;
      }

      int beamNameIndex = 3;
      if (parms[beamNameIndex].equals(CMD_ALL))
      {
         for (int i = 0; i < mBeamNames.length; ++i)
         {
            clearCoordsCmdForBeam(mBeamNames[i]);
         }
            
         return CMD_OK;
      }
      else
      {
         return clearCoordsCmdForBeam(parms[beamNameIndex]);
      }
   }

   public String getAntsForBeam(String beam)
   {
      SubarrayModel model = (SubarrayModel) mSubarrayModelMap.get(beam);
      if (model == null)
      {
         return INVALID_BEAM;
      }
      
      return beam + ": " + model.getAntNamesList();    
   }

   public String listAntsAssignedToBeam(String beam)
   {
      if (beam.equals(CMD_ALL))
      {
         StringBuffer buff = new StringBuffer();
         for (int i = 0; i < mBeamNames.length; ++i)
         {
            buff.append(getAntsForBeam(mBeamNames[i]));
            buff.append("\n");
         }
         return buff.toString();
      }
      
      return getAntsForBeam(beam);
   }

   private String clearAntAssignmentsForBeam(String beam)
   {
      SubarrayModel model = (SubarrayModel) mSubarrayModelMap.get(beam);
      if (model == null)
      {
         return INVALID_BEAM + ": " + beam;
      }
      
      model.setAntNamesList("");

      return CMD_OK;
   }


   private String handleBeamClearAntsCmd(String[] parms)
   {
      // BF Clear ANTS <beamname|ALL>
        
      int nExpectedWords = 4;
      if (parms.length != nExpectedWords)
      {
         return INVALID_NUMBER_OF_ARGS;
      }

      int beamNameIndex = 3;
      if (parms[beamNameIndex].equals(CMD_ALL))
      {
         for (int i = 0; i < mBeamNames.length; ++i)
         {
            clearAntAssignmentsForBeam(mBeamNames[i]);
         }
            
         return CMD_OK;
      }
      else
      {
         return clearAntAssignmentsForBeam(parms[beamNameIndex]);
      }
   }

   private String copyBeamPointCmdsToStatus()
   {
      boolean cmdsFound = false;
      for (int i = 0; i < mBeamNames.length; ++i)
      {
         String beam = mBeamNames[i];
         Pointing beamPointCmd = (Pointing)mBeamPointCmdStorage.get(beam);
         if (! beamPointCmd.commandedCoordSystem.equals(COORD_SYS_UNINIT))
         {
            mBeamStatusStorage.update(beam, beamPointCmd);
            cmdsFound = true;
         }
      }
       
      if (!cmdsFound)
      {
         return NO_COORDS_ASSIGNED_TO_BEAMS;
      }
       
      return CMD_OK;
   }

   private double getAttnDb()
   {
      synchronized(mAttnDbMutex)
      {
         return mAttnDb;
      }

   }

   private void setAttnDb(double attnDb)
   {
      synchronized(mAttnDbMutex)
      {
         mAttnDb = attnDb;
      }
   }


   private String handleSetAttnCmd(String[] parms) 
   {
      String cmdName = ": bf set attn";

      // bf set attn <beam> <attn-db> 
      int nExpectedWords = 5;
      if (parms.length != nExpectedWords)
      {
         return INVALID_NUMBER_OF_ARGS + cmdName;
      }
      int attnIndex = 4;
      double attnDb = -1.0;
      try
      {
         attnDb = Double.parseDouble(parms[attnIndex]);
      }
      catch (Exception ex)
      {
         return INVALID_ARGUMENT + cmdName + " " + parms[attnIndex];
      }
      int beamNameIndex = 3;

      // TBD change to use attn by beam

      setAttnDb(attnDb);

      return CMD_OK;
   }

   private String handleListAttn()
   {
      // TBD change to handle multiple beam names

      return "" + getAttnDb();
   }

   private String handleBfConfig()
   {
      return "BEAMXC1 bf#1 x1";
   }

   // base class for running commands on subarrays
   private abstract class IssueSubarrayCmd
   {
      private String mCmdName;

      IssueSubarrayCmd(String cmdName)
      {
         mCmdName = cmdName;
      }

      String getCmdName()
      {
         return mCmdName;
      }

      abstract String goThroughSubarray(Iterator it);

      /*
        Prepare ant list, then run through subarray.
        Return value is cmd response (status).
      */
      String run(String antNameList)
      {
         if (antNameList.matches(CMD_ANTGROUP))
         {
            antNameList = mAntGroups.getRawAntList(AntGroups.ANTGROUP_PRIMARY);
            if (antNameList.equals(""))
            {
               return ANTGROUP_UNASSIGNED + mCmdName;
            }
         }
         else if (! isValidAntList(antNameList))
         {
            return INVALID_ANTLIST + mCmdName;
         }

         Vector modelVector = getSubarrayModelsThatMatchAntList(
            antNameList);
         if (modelVector.size() == 0)
         {
            return SUBARRAY_NOT_ASSIGNED_TO_ANY_BEAMS + mCmdName;
         }

         Iterator it = modelVector.iterator();
         String status = goThroughSubarray(it);
         return status;
      }
   }

   private class CmdPoint extends IssueSubarrayCmd
   {
      private String mCoordSys;
      private String mCoord1;
      private String mCoord2;

      CmdPoint(String coordSys, String coord1, String coord2)
      {
         super(": point");
         mCoordSys = coordSys;
         mCoord1 = coord1;
         mCoord2 = coord2;
      }

      /*
        Point subarray:
        Find all models that have the associated subarray list,
        and point them.
        Return an error if the list is not associated with any beams.
      */  

      String goThroughSubarray(Iterator it)
      {
         while (it.hasNext())
         {
            SubarrayModel model = (SubarrayModel) it.next();

            String result;          
            if (mCoordSys.matches(COORD_SYS_AZEL))
            {
               result = model.setPrimaryAzEl(mCoord1, mCoord2);
            }
            else if (mCoordSys.matches(COORD_SYS_J2000))
            {
               result = model.setPrimaryJ2000(mCoord1, mCoord2);
            }
            else if (mCoordSys.matches(COORD_SYS_GAL))
            {
               result = model.setPrimaryGal(mCoord1, mCoord2);
            }
            else {
               return INVALID_COORD_SYS + getCmdName();
            }
               
            if (! result.matches(CMD_OK))
            {
               return result;
            }
         }
            
         return CMD_OK;
      }

   }

   private class CmdStop extends IssueSubarrayCmd
   {
      CmdStop()
      {
         super(": stop");
      }

      String goThroughSubarray(Iterator it)
      {
         while (it.hasNext())
         {
            SubarrayModel model = (SubarrayModel) it.next();
            model.stop();
         }

         String status = CMD_OK;
	 return status;
      }
   }

   private class CmdWrap extends IssueSubarrayCmd
   {
      String mWrap;

      CmdWrap(String wrap)
      {
         super(": wrap");
         mWrap = wrap;
      }

      String goThroughSubarray(Iterator it)
      {
         while (it.hasNext())
         {
            SubarrayModel model = (SubarrayModel) it.next();
            String result = model.setWrap(mWrap);
            if (! result.matches(CMD_OK))
            {
               return result;
            }
         }

	 return CMD_OK;
      }
   }

   private class CmdZfocus extends IssueSubarrayCmd
   {
      String mFreqMhz;

      CmdZfocus(String freqMhz)
      {
         super(": zfocus");
         mFreqMhz = freqMhz;
      }

      String goThroughSubarray(Iterator it)
      {
         while (it.hasNext())
         {
            SubarrayModel model = (SubarrayModel) it.next();
            String result = model.setZfocus(mFreqMhz);
            if (! result.matches(CMD_OK))
            {
               return result;
            }
         }

	 return CMD_OK;
      }
   }


   /**
    * Parse the command string and call appropriate method (or return error msg).
    * @param command string that arrived 
    * @return "OK" (if command worked) or an error message
    */
   private String dispatchCommand(String command)
   {
      String uppercaseCmd = command.toUpperCase();
      String multispaceRegexp = " +";
      String cmdWords[] = uppercaseCmd.split(multispaceRegexp); 

      String primaryCmd = cmdWords[0];
      if (primaryCmd.equals(CMD_BEAMFORMER))
      {
         return dispatchBeamformerCmd(cmdWords);
      }
      else if (primaryCmd.equals(CMD_ALLOCATE))
      {
         // TBD: rework allocation scheme
	 //mScopeModel.mIsAllocated = true;
	 return CMD_OK;
      }
      else if (primaryCmd.equals(CMD_DEALLOCATE))
      {
	 // TBD: rework allocation scheme
         //mScopeModel.mIsAllocated = false;
	 return CMD_OK;
      }
      else if (primaryCmd.equals(CMD_STOP) || primaryCmd.equals(CMD_STOW))
      {
         // stop <'antgroup'| antxx[,antxx...]>

         int nExpectedWords = 2;
	 if (cmdWords.length != nExpectedWords)
	 {
	    return INVALID_NUMBER_OF_ARGS;
	 }
         int subarrayIndex = 1;

         String antNameList = cmdWords[subarrayIndex];
         CmdStop cmdStop = new CmdStop();

         return cmdStop.run(antNameList);
      }
      else if (primaryCmd.equals(CMD_POINT))
      {
         String cmdName = ": point";

	 // POINT <'antgroup' | antxx[,antxx...]> <AZEL|J2000|GAL> <coord1> <coord2>
	 
	 int nExpectedWords = 5;
	 if (cmdWords.length != nExpectedWords)
	 {
	    return INVALID_NUMBER_OF_ARGS + cmdName;
	 }

	 int subarrayIndex = 1;
	 int coordSysIndex = 2;
	 int coord1Index = 3;
	 int coord2Index = 4;
	 String coordSys = cmdWords[coordSysIndex];
	 String coord1 = cmdWords[coord1Index];
	 String coord2 = cmdWords[coord2Index];

	 //System.out.println("coord sys is: " + coordSys);

         CmdPoint cmdPoint = new CmdPoint(
            coordSys, coord1, coord2);
         
         String antNameList = cmdWords[subarrayIndex];

         return cmdPoint.run(antNameList);

      }
      else if (primaryCmd.equals(CMD_TUNE))
      {
         String cmdName = ": tune";

	 // TUNE <{A-D}> <freq mhz>
	 int nExpectedWords = 3;
	 if (cmdWords.length != nExpectedWords)
	 {
	    return INVALID_NUMBER_OF_ARGS + cmdName;
	 }
	 
	 // get its stored status & update
	 int tuningNameIndex = 1;
	 TuningStatus tuningStatus = mTuningStatusStorage.get(
	    cmdWords[tuningNameIndex]);

	 if (tuningStatus == null) {
	    return INVALID_TUNING_NAME + cmdName;
	 }

	 int freqIndex = 2;
	 String result = tune(tuningStatus, cmdWords[freqIndex]);
	 mTuningStatusStorage.update(cmdWords[tuningNameIndex],
				     tuningStatus);
	 return result;
      }
      else if (primaryCmd.equals(CMD_ATTN))
      {
         String cmdName = ": attn";

	 // ATTN <tuning=a-d> <x-attn-db> <y-attn-db>
	 int nExpectedWords = 4;
	 if (cmdWords.length != nExpectedWords)
	 {
	    return INVALID_NUMBER_OF_ARGS + cmdName;
	 }
	 
	 // get its stored status & update
	 int tuningNameIndex = 1;
	 TuningStatus tuningStatus = mTuningStatusStorage.get(
	    cmdWords[tuningNameIndex]);

	 if (tuningStatus == null) {
	    return INVALID_TUNING_NAME + cmdName;
	 }

	 int attnXIndex = 2;
	 int attnYIndex = 3;
	 String result = attn(tuningStatus, cmdWords[attnXIndex],
			      cmdWords[attnYIndex]);
	 mTuningStatusStorage.update(cmdWords[tuningNameIndex],
				     tuningStatus);
	 return result;
      }
      else if (primaryCmd.equals(CMD_ZFOCUS))
      {
         String cmdName = ": zfocus";

         // zfocus antxx[,antxx...] <freqMhz>
         int nExpectedWords = 3;
	 if (cmdWords.length != nExpectedWords)
	 {
	    return INVALID_NUMBER_OF_ARGS + cmdName;
	 }
         int subarrayIndex = 1;
         int freqIndex = 2;

         String antNameList = cmdWords[subarrayIndex];
         CmdZfocus cmdZfocus = new CmdZfocus(cmdWords[freqIndex]);

         return cmdZfocus.run(antNameList);
      }
      else if (primaryCmd.equals(CMD_WRAP))
      {
         String cmdName = ": wrap";

         // wrap antxx[,antxx...] <wrap number>
         int nExpectedWords = 3;
	 if (cmdWords.length != nExpectedWords)
	 {
	    return INVALID_NUMBER_OF_ARGS + cmdName;
	 }
         int subarrayIndex = 1;
         int wrapIndex = 2;
         
         String antNameList = cmdWords[subarrayIndex];
         CmdWrap cmdWrap = new CmdWrap(cmdWords[wrapIndex]);

         return cmdWrap.run(antNameList);
      }
      else if (primaryCmd.equals(CMD_LNA_ON))
      {
         // TBD verify args

         return CMD_OK;
      }
      else if (primaryCmd.equals(CMD_PAM_SET))
      {
         // TBD verify args

         return CMD_OK;
      }
      else if (primaryCmd.equals(CMD_ANTGROUP))
      {
         String cmdName = ": " + CMD_ANTGROUP;
         int nMinWords = 2;
         if (cmdWords.length < nMinWords)
         {
            return INVALID_NUMBER_OF_ARGS + cmdName;
         }
         
         int subCmdIndex = 1;
         if (cmdWords[subCmdIndex].equals(CMD_AUTOSELECT))
         {
            //ANTGROUP AUTOSELECT SEFD <maxJy> FREQ <maxFreqMhz)
            int nExpectedWords = 6;
            if (cmdWords.length != nExpectedWords)
            {
               return INVALID_NUMBER_OF_ARGS + cmdName;
            }
            mAntGroups.setAntGroup(mAntGroups.ANTGROUP_ALL, 
                                   AUTOSELECT_ANTS_TEST_LIST);

            return READY_PREFIX + CMD_AUTOSELECT;
         }
         else if (cmdWords[subCmdIndex].equals(CMD_SET))
         {
            //ANTGROUP SET <PRIMARY | XPOL | YPOL | ALL> antNN[,antNN,...]

            nMinWords = 4;
            if (cmdWords.length < nMinWords)
            {
               return INVALID_NUMBER_OF_ARGS + cmdName;
            }
            String group = cmdWords[2];
            String ants = cmdWords[3];

            if (! mAntGroups.validGroup(group))
            {
               return INVALID_ANTGROUP + ": " + group;
            }

            if (! isValidAntList(ants))
            {
               return INVALID_ANTLIST + cmdName;
            }

            mAntGroups.setAntGroup(group, ants);

            return CMD_OK;
         }
         else if (cmdWords[subCmdIndex].equals(CMD_CLEAR))
         {
            //ANTGROUP CLEAR <PRIMARY | XPOL | YPOL | ALL>

            nMinWords = 3;
            if (cmdWords.length < nMinWords)
            {
               return INVALID_NUMBER_OF_ARGS + cmdName;
            }
            String group = cmdWords[2];

            if (! mAntGroups.validGroup(group))
            {
               return INVALID_ANTGROUP + ": " + group;
            }

            mAntGroups.setAntGroup(group, "");
            return CMD_OK;
         }
         else if (cmdWords[subCmdIndex].equals(CMD_LIST))
         {
            //ANTGROUP LIST <primary | xpol | ypol | all>

            nMinWords = 3;
            if (cmdWords.length < nMinWords)
            {
               return INVALID_NUMBER_OF_ARGS + cmdName;
            }
            String group = cmdWords[2];

            if (! mAntGroups.validGroup(group))
            {
               return INVALID_ANTGROUP + ": " + group;
            }

            return mAntGroups.getAnts(group);
         }
         else
         {
            return ERROR_PREFIX + "don't know how to " + CMD_ANTGROUP + " "
               + cmdWords[subCmdIndex];     
         }
      }
      else if (primaryCmd.equals(CMD_MONITOR))
      {
         String cmdName = ": monitor";

         // monitor <period>
         int nExpectedWords = 2;
	 if (cmdWords.length != nExpectedWords)
	 {
	    return INVALID_NUMBER_OF_ARGS + cmdName;
	 }
         int periodIndex = 1;
	 return monitorPeriod(cmdWords[periodIndex]);
      }
      else
      {
	 return UNRECOGNIZED_COMMAND+ ": " + command;
      }
   }

   private String dispatchBeamformerCmd(String cmdWords[])
   {
      String cmdPrefix = ": bf";

      // BF ...

      int nMinWords = 2;
      if (cmdWords.length < nMinWords)
      {
         return INVALID_NUMBER_OF_ARGS + cmdPrefix;
      }
         
      int subCmdIndex = 1;
      if (cmdWords[subCmdIndex].equals(CMD_SET))
      {
         // BF SET ...

         nMinWords = 3;
         if (cmdWords.length < nMinWords)
         {
            return INVALID_NUMBER_OF_ARGS + cmdPrefix;
         }

         int setTypeIndex = 2;
         if (cmdWords[setTypeIndex].equals(CMD_BF_COORDS))
         {
            int nExpectedWords = 7;
            if (cmdWords.length != nExpectedWords)
            {
               return INVALID_NUMBER_OF_ARGS + cmdPrefix;
            }

            String cmdName = ": bf set coords";
            // bf set coords beamxxx coordsys coord1 coord2

            int beamNameIndex = 3;
            int coordSysIndex = 4;
            int coord1Index = 5;
            int coord2Index = 6;

            // get its stored status & update
            String beamName = cmdWords[beamNameIndex];
            if (mBeamPointCmdStorage.get(beamName) == null)
            {
               return INVALID_BEAM + cmdName;
            }
            // start with cleared settings
            Pointing pointing = new Pointing();

            String coordSys = cmdWords[coordSysIndex];
            String coord1 = cmdWords[coord1Index];
            String coord2 = cmdWords[coord2Index];

            String result;
            if (coordSys.matches(COORD_SYS_AZEL))
            {
               result = setAzEl(pointing, coord1, coord2);
            }
            else if (coordSys.matches(COORD_SYS_J2000))
            {
               result = setJ2000(pointing, coord1, coord2);
            }
            else if (coordSys.matches(COORD_SYS_GAL))
            {
               result = setGal(pointing, coord1, coord2);
            }
            else {
               return INVALID_COORD_SYS + cmdName;
            }

            if (result.equals(CMD_OK)) 
            {
               mBeamPointCmdStorage.update(beamName, pointing);
            }
	 
            return result;
         }
         else if (cmdWords[setTypeIndex].equals(CMD_BF_ANTS))
         {
            String cmdName = ": bf set ants";
            // bf set ants beamxxx <'antgroup' | antxx,antxx...>

            int nExpectedWords = 5;
            if (cmdWords.length != nExpectedWords)
            {
               return INVALID_NUMBER_OF_ARGS + cmdName;
            }
            int beamNameIndex = 3;
            int antNameListIndex = 4;

            String beamName = cmdWords[beamNameIndex];
            String antNameList = cmdWords[antNameListIndex];

            SubarrayModel model = (SubarrayModel) mSubarrayModelMap.get(beamName);
            if (model == null)
            {
               return INVALID_BEAM + cmdName;
            }
               
            if (antNameList.matches(CMD_ANTGROUP))
            {
               // use ant group of correct polarity for this beam
               if (getPolFromBeamName(beamName).equals("X"))
               {
                  antNameList = mAntGroups.getRawAntList(AntGroups.ANTGROUP_XPOL);
               }
               else if (getPolFromBeamName(beamName).equals("Y"))
               {
                  antNameList = mAntGroups.getRawAntList(AntGroups.ANTGROUP_YPOL);
               }
               else
               {
                  // This shouldn't happen, but just in case:
                  return INVALID_BEAM;
               }

               if (antNameList.equals(""))
               {
                  return ANTGROUP_UNASSIGNED + cmdName;
               }

            }
            else if (! isValidAntList(antNameList))
            {
               return INVALID_ANTLIST + cmdName;
            }
                   
            model.setAntNamesList(antNameList);
               
            return CMD_OK;
         }
         else if (cmdWords[setTypeIndex].equals(CMD_BF_OBSLEN))
         {
            // bf set obslen <secs>
            return handleSetObslen(cmdWords);
         }
         else if (cmdWords[setTypeIndex].equals(CMD_BF_NULLTYPE))
         {
            // BF SET NULLTYPE <AXIAL | PROJECTION | NONE> 
            return handleSetNullType(cmdWords);
         }
         else if (cmdWords[setTypeIndex].equals(CMD_ATTN))
         {
            // bf set attn beamxxx <attndb>
            return handleSetAttnCmd(cmdWords);
         }
         else
         {
            return ERROR_PREFIX + "don't know how to " + CMD_SET + " "
               + cmdWords[setTypeIndex]; 
         }
      }
      else if (cmdWords[subCmdIndex].equals(CMD_LIST))
      {
         // BF LIST ...

         cmdPrefix = ": bf list";

         int minListWords = 3;
         if (cmdWords.length < minListWords)
         {
            return INVALID_NUMBER_OF_ARGS + cmdPrefix;
         }
                
         int listTypeIndex = 2;
         if (cmdWords[listTypeIndex].equals(CMD_BF_ANTS))
         {
            String cmdName = ": bf list ants";

            // bf list ants <beamname>
            int nExpectedWords = 4;
            if (cmdWords.length != nExpectedWords)
            {
               return INVALID_NUMBER_OF_ARGS + cmdName;
            }
            int beamNameIndex = 3;
            String beamName = cmdWords[beamNameIndex];

            return listAntsAssignedToBeam(beamName);
         }
         else if (cmdWords[listTypeIndex].equals(CMD_BF_COORDS))
         {
            return handleBeamListCoordsCmd(cmdWords);
         }
         else if (cmdWords[listTypeIndex].equals(CMD_BF_OBSLEN))
         {
            return handleListObsLen();
         }
         else if (cmdWords[listTypeIndex].equals(CMD_ATTN))
         {
            return handleListAttn();
         }
         else if (cmdWords[listTypeIndex].equals(CMD_BF_CONFIG))
         {
            return handleBfConfig();
         }
         else if (cmdWords[listTypeIndex].equals(CMD_BF_NULLS))
         {
            // bf list nulls
            return UNIMPLEMENTED_COMMAND;
         }
         else
         {
            return ERROR_PREFIX + "don't know how to " + CMD_LIST + " "
               + cmdWords[listTypeIndex]; 
         }
      }
      else if (cmdWords[subCmdIndex].equals(CMD_CLEAR))
      {
         // BF CLEAR ...

         cmdPrefix = ": bf clear";

         int minClearWords = 3;
         if (cmdWords.length < minClearWords)
         {
            return INVALID_NUMBER_OF_ARGS + cmdPrefix;
         }
                
         int clearTypeIndex = 2;
         if (cmdWords[clearTypeIndex].equals(CMD_BF_ANTS))
         {
            return handleBeamClearAntsCmd(cmdWords);
         }
         else if (cmdWords[clearTypeIndex].equals(CMD_BF_COORDS))
         {
            return handleBeamClearCoordsCmd(cmdWords);
         }
         else if (cmdWords[clearTypeIndex].equals(CMD_BF_NULLS))
         {
            // For now, do nothing
            return CMD_OK;
         }
         else
         {
            return ERROR_PREFIX + "don't know how to " + CMD_CLEAR + " "
               + cmdWords[clearTypeIndex]; 
         }
      }
      else if (cmdWords[subCmdIndex].equals(CMD_ADD))
      {
         String cmdName = ": bf add null";
         // bf add null beamxxx <coordsys> <coord1> <coord2>
         
         int nExpectedWords = 7;
         if (cmdWords.length != nExpectedWords)
         {
            return INVALID_NUMBER_OF_ARGS + cmdName;

         }

         int coordSysIndex = 4;
         String coordSys = cmdWords[coordSysIndex];

         if (! coordSys.matches(COORD_SYS_AZEL)
             && ! coordSys.matches(COORD_SYS_J2000)
             && ! coordSys.matches(COORD_SYS_GAL))
         {
             return INVALID_COORD_SYS + cmdName;
         }
         
         // TBD more arg validation

         // TBD take command action here

         return CMD_OK;
      }
      else if (cmdWords[subCmdIndex].equals(CMD_BF_CAL))
      {
         String cmdName = ": bf cal";

         // bf cal <delay|phase|freq> integrate <secs> cycles <count>
         int nExpectedWords = 7;
         if (cmdWords.length != nExpectedWords)
         {
            return INVALID_NUMBER_OF_ARGS + cmdName;
         }

         int calTypeIndex = 2;
         if (cmdWords[calTypeIndex].equals(CAL_TYPE_DELAY))
         {
            // do nothing
         }
         else if (cmdWords[calTypeIndex].equals(CAL_TYPE_PHASE))
         {
            // do nothing
         }
         else if (cmdWords[calTypeIndex].equals(CAL_TYPE_FREQ))
         {
            // do nothing
         }
         else
         {
            return INVALID_ARGUMENT + " " + cmdWords[calTypeIndex];
         }
            
         String status = copyBeamPointCmdsToStatus();
         if (status.equals(CMD_OK))
         {
            return READY_PREFIX + CMD_BF_CAL;
         }
         return status;
      }
      else if (cmdWords[subCmdIndex].equals(CMD_BF_POINT))
      {
         String status = copyBeamPointCmdsToStatus();
         if (status.equals(CMD_OK))
         {
            return READY_PREFIX + CMD_BF_POINT;
         }
         return status;
      }
      else if (cmdWords[subCmdIndex].equals(CMD_BF_RESET))
      {
         return READY_PREFIX + CMD_BF_RESET;
      }
      else if (cmdWords[subCmdIndex].equals(CMD_BF_STOP))
      {
         // do nothing
         return CMD_OK;
      }
      else if (cmdWords[subCmdIndex].equals(CMD_BF_OBSLEN))
      {
         return UNIMPLEMENTED_COMMAND;
      }
      else if (cmdWords[subCmdIndex].equals(CMD_BF_INIT))
      {
         // do nothing
         return READY_PREFIX + CMD_BF_INIT;
      }
      else if (cmdWords[subCmdIndex].equals(CMD_BF_AUTOATTEN))
      {
         // do nothing

         // TBD in reality this takes some time, may want to sleep a bit
         return READY_PREFIX + CMD_BF_AUTOATTEN;
      }
      else
      {
         return ERROR_PREFIX + "unknown beamformer command: " 
            + cmdWords[subCmdIndex];
      }
   }

   /**
    * Set up the IF tuning with the sky frequency indicated by freqMhzString.
    * @param val command string containing tuning & desired frequency
    * @return string indicated success or error
    */
   private String tune(TuningStatus status, String freqMhzString)
   {
      String cmdName = ": tune";

      try
      {
	 // parse the frequency parameter
	 double freqMhz = Double.parseDouble(freqMhzString);
	 if (freqMhz < 1.0 || freqMhz > MAX_TUNE_FREQ_MHZ) {
	    return OUT_OF_RANGE + cmdName;
	 }

	 status.skyFreqMhz = freqMhz;

	 System.out.println("Freq tuning set to " + freqMhz + ". ");
	 return CMD_OK;
      }
      catch (NumberFormatException nfe)
      {
	 return INVALID_ARGUMENT + cmdName;
      }

   }

   /**
    * Set up the IF tuning attenuation with the values indicated
    * @param val command string containing tuning & desired frequency
    * @return string indicated success or error
    */
   private String attn(TuningStatus status, String attnXdbString,
		       String attnYdbString)
   {
      String cmdName = ": attn";

      try
      {
	 // parse the attn parameter
	 int attnXdb = Integer.parseInt(attnXdbString);
	 if (attnXdb < 0 || attnXdb > MAX_ATTN_DB) {
	    return OUT_OF_RANGE + cmdName;
	 }

	 int attnYdb = Integer.parseInt(attnYdbString);
	 if (attnYdb < 0 || attnYdb > MAX_ATTN_DB) {
	    return OUT_OF_RANGE + cmdName;
	 }

	 status.attnXdb = attnXdb;
	 status.attnYdb = attnYdb;

	 System.out.println("attn set to " + attnXdb + " " + attnYdb);
	 return CMD_OK;
      }
      catch (NumberFormatException nfe)
      {
	 return INVALID_ARGUMENT + cmdName;
      }

   }



   /**
    * Set up the Monitoring period in seconds indicated by val.
    * @param val command string containing desired period 
    * @return string indicated success or error
    */
   private String monitorPeriod(String period)
   {
      String cmdName = ": monitor";

      try
      {
	 int periodSecs = Integer.parseInt(period.trim());
	 if (periodSecs < 1 || periodSecs > 99999)
         {
            return OUT_OF_RANGE + cmdName;
         }
	 /*
	   TBD:
	   - Handle periodSecs=0 case for no monitor output
	   - Add mutex around modification of mStatusInterval?
	   - wake up monitor thread so that it picks up the
	   new interval (could be in a long sleep)?
	 */ 

	 mStatusInterval = periodSecs * 1000;

	 System.out.println("Monitor period set to " + periodSecs + " secs. ");
	 return CMD_OK;
      }
      catch (NumberFormatException nfe)
      {
	 return INVALID_ARGUMENT + cmdName;
      }
      catch (IndexOutOfBoundsException iobe)
      {
	 return INVALID_NUMBER_OF_ARGS + cmdName;
      }

   }

   private String handleSetObslen(String[] parms)
   {
      String cmdName = ": bf set obslen";

      // BF set obslen <secs>
      int nExpectedWords = 4;
      if (parms.length != nExpectedWords)
      {
         return INVALID_NUMBER_OF_ARGS + cmdName;
      }
      
      int newObsLenSecs;
      int obsLenIndex = 3;
      try
      {
         newObsLenSecs = Integer.parseInt(parms[obsLenIndex]);
         if (newObsLenSecs < 1)
         {
            return INVALID_ARGUMENT + cmdName + " " + parms[obsLenIndex]
               + ", must be >= 1 ";
         }
      }
      catch (Exception ex)
      {
         return INVALID_ARGUMENT + cmdName + " " + parms[obsLenIndex];
      }
      
      mObsLenSecs = newObsLenSecs;
      
      return CMD_OK;
   }


   private String handleSetNullType(String[] parms)
   {
      String cmdName = ": bf set NULLTYPE <AXIAL | PROJECTION | NONE> ";

      int nExpectedWords = 4;
      if (parms.length != nExpectedWords)
      {
         return INVALID_NUMBER_OF_ARGS + cmdName;
      }
      
      int nullTypeIndex = 3;
      String nullType = parms[nullTypeIndex];
      
      if (nullType.equals("AXIAL") ||
          nullType.equals("PROJECTION") ||
          nullType.equals("NONE"))
      {
         return CMD_OK;
      }

      return INVALID_ARGUMENT + cmdName + " " + nullType;
   }

   
   private String handleListObsLen()
   {
      return "" + mObsLenSecs;
   }

   /**
    * Set the telescope azimuth and elevation, both as degrees
    * @param string versions of az,el
    * @return a string indicating success or error
    */
   private String setAzEl(Pointing pointing, String azDegString, String elDegString)
   {
      String cmdName = ": set azel";

      try
      {
	 // parse az
	 double az = Double.parseDouble(azDegString);
	 if (az < 0.0 || az >= 360.0) 
         {
            return OUT_OF_RANGE + cmdName;
         }
	 pointing.azDeg = az;

	 // parse el
	 double el = Double.parseDouble(elDegString);
	 if (el < 0.0 || el > 90.00000)
         {
            return OUT_OF_RANGE + cmdName;
         }
	 pointing.elDeg = el;

	 pointing.commandedCoordSystem = COORD_SYS_AZEL;

	 System.out.println("(Az, El) set to (" + az + ", " + el + "). ");
	 return CMD_OK;
      }
      catch (NumberFormatException nfe)
      {
	 return INVALID_ARGUMENT + cmdName;
      }
   }

   /**
    * Set the telescope (RA, Dec) in J2000 coordinates.
    * ra comes in as hours, dec as degs
    * ra is converted to degrees for internal use
    * @param string versions of ra dec to set J2000
    * @return a string indicating success or error
    */
   private String setJ2000(Pointing pointing, String raHoursString, String decDegString)
   {
      try
      {
	 // parse ra
	 double ra = Double.parseDouble(raHoursString);
	 if (ra < 0.0 || ra >= 24.0) return OUT_OF_RANGE;
	 pointing.raHours = ra;

	 // parse dec
	 double dec = Double.parseDouble(decDegString);
	 if (dec < -90.0 || dec > 90.0) return OUT_OF_RANGE;
	 pointing.decDeg = dec;

	 pointing.commandedCoordSystem = COORD_SYS_J2000;

	 System.out.println("(RA, Dec) set to (" + ra + ", " + dec + "). ");
	 return CMD_OK;
      }
      catch (NumberFormatException nfe)
      {
	 return INVALID_ARGUMENT;
      }
   }

   /**
    * Set the telescope position in Galactic longitude and latitude.
    * @param coord strings to set Gal
    * @return a string indicating success or error
    */
   private String setGal(Pointing pointing, String glongDegString, String glatDegString)
   {
      try
      {
	 // parse longitude
	 double lon = Double.parseDouble(glongDegString);
	 if (lon < 0.0 || lon > 360.0) return OUT_OF_RANGE;
	 pointing.glongDeg = lon;

	 // parse latitude
	 double lat = Double.parseDouble(glatDegString);
	 if (lat < -90.0 || lat > 90.0) return OUT_OF_RANGE;
	 pointing.glatDeg = lat;

	 pointing.commandedCoordSystem = COORD_SYS_GAL;

	 System.out.println("(glong, glat) set to (" + lon + ", " + lat + "). ");
	 return CMD_OK;
      }
      catch (NumberFormatException nfe)
      {
	 return INVALID_ARGUMENT;
      }
   }


   /*
     Get the primary beam pointing status for the subarray
     associated with the specified synth beam.
   */
   private String primaryBeamStatusString(String beamName)
   {
      SubarrayModel model = (SubarrayModel) mSubarrayModelMap.get(beamName);

      // generate a status string
      StringBuffer stat = new StringBuffer("");

      stat.append(model.mCoordSys);
      stat.append(' ');

      stat.append("AZ ");
      stat.append(mAngleFormatter.format(model.mAz));
      stat.append(" deg ");

      stat.append("EL ");
      stat.append(mAngleFormatter.format(90 - model.mZen));
      stat.append(" deg ");

      stat.append("RA ");
      double degPerHour = 15;
      stat.append(mAngleFormatter.format(model.mRA/degPerHour));
      stat.append(" hr ");

      stat.append("DEC ");
      stat.append(mAngleFormatter.format(model.mDec));
      stat.append(" deg ");

      stat.append("GLONG ");
      stat.append(mAngleFormatter.format(model.mGlong));
      stat.append(" deg ");

      stat.append("GLAT ");
      stat.append(mAngleFormatter.format(model.mGlat));
      stat.append(" deg ");

      return stat.toString();
   }


   private String beamStatusString(Pointing status)
   {
      // generate a status string
      StringBuffer stat = new StringBuffer("");

      stat.append(status.commandedCoordSystem);
      stat.append(' ');

      stat.append("AZ ");
      stat.append(mAngleFormatter.format(status.azDeg));
      stat.append(" deg ");

      stat.append("EL ");
      stat.append(mAngleFormatter.format(status.elDeg));
      stat.append(" deg ");

      stat.append("RA ");
      stat.append(mAngleFormatter.format(status.raHours));
      stat.append(" hr ");

      stat.append("DEC ");
      stat.append(mAngleFormatter.format(status.decDeg));
      stat.append(" deg ");

      stat.append("GLONG ");
      stat.append(mAngleFormatter.format(status.glongDeg));
      stat.append(" deg ");

      stat.append("GLAT ");
      stat.append(mAngleFormatter.format(status.glatDeg));
      stat.append(" deg ");

      return stat.toString();
   }


   private String subarrayStatusString(String beamName) {

      SubarrayModel model = (SubarrayModel) mSubarrayModelMap.get(beamName);

      //System.out.println("model[" + beamName + "] subarray: " + model.getAntNamesList());

      StringBuffer stat = new StringBuffer("");

      stat.append("NANTS ");
      stat.append(model.mNumDishesTotal);

      stat.append(" NSHAREDPOINTING ");
      stat.append(model.mNumDishesSharedPointing);

      stat.append(" NTRACK ");
      stat.append(model.mNumDishesTrack);
            
      stat.append(" NSLEW ");
      stat.append(model.mNumDishesSlew);
            
      stat.append(" NSTOP ");
      stat.append(model.mNumDishesStop);
            
      stat.append(" NOFFLINE ");
      stat.append(model.mNumDishesOffline);
            
      stat.append(" NERROR ");
      stat.append(model.mNumDishesDriveError);

      stat.append(" WRAP ");
      stat.append(model.mWrap);
      
      stat.append(" ZFOCUS ");
      stat.append(mFreqFormatter.format(model.mZfocusFreqMhz));
      stat.append(" MHz");
      
      stat.append(" GCERROR ");
      stat.append(mAngleFormatter.format(model.mError));
      stat.append(" deg");

      return stat.toString();
   }


   /**
    * Generate a status string using current variable settings.
    * @return a new status string
    */
   private String statusString()
   {
      // here is output template

      /*
        ARRAY: <HH:MM:SS UTC>

        // For each beam (32 total):
	// Name is BEAM<pol><tuning><number>
	// but sort order is: tuning={A-D}, number={1-4}, pol={X,Y}
	// i.e., BEAMXA1, BEAMYA1, BEAMXA2, BEAMYA2, ... BEAMXD4, BEAMYD4

        BEAMxxx: SUBARRAY: NANTS xx NSHAREDPOINTING xx NTRACK xx NSLEW xx \
        NSTOP xx NOFFLINE xx NERROR xx WRAP <0 | 1>  \
        ZFOCUS xxxx.x MHz GCERROR xxx deg \r\n

        BEAMxxx: PRIMARY: <AZEL | J2000 | GAL> AZ xxx deg EL xxx deg \
        RA xxx hr DEC xxx deg GLONG xxx deg GLAT xxx deg \r\n

        BEAMxxx: SYNTH: <AZEL | J2000 | GAL> AZ xxx deg EL xxx deg \
        RA xxx hr DEC xxx deg GLONG xxx deg GLAT xxx deg \r\n

        BEAMxxx: IF: SKYFREQ xxxxx.xxxxxx MHz \r\n

        // For each tuning (4 total):
        TUNING{A-D}: SKYFREQ xxxxx.xxxxxx MHz \r\n

        END \r\n
      */

      // generate a status string
      StringBuffer stat = new StringBuffer("");

      // General array information
      stat.append("ARRAY: ");
      stat.append(timeString());
      stat.append(" UTC ");
      stat.append("\r\n");

      // TBD - determine allocation scheme
      //if (mIsAllocated) stat.append("ALLOC");
      //else              stat.append("DEALLOC");


      for (int i=0; i<mBeamNames.length; ++i)
      {
         // subarray status
         stat.append(mBeamNames[i]);
         stat.append(": SUBARRAY: ");
         stat.append(subarrayStatusString(mBeamNames[i]));
         stat.append("\r\n");

         // primary pointing (of subarray associated with synth beam)
	 stat.append(mBeamNames[i]);
	 stat.append(": PRIMARY: ");

         // primary beam pointing
         stat.append(primaryBeamStatusString(mBeamNames[i]));
         stat.append("\r\n");

         // synth beam pointing
	 stat.append(mBeamNames[i]);
	 stat.append(": SYNTH: ");
	 Pointing beamStatus = mBeamStatusStorage.get(mBeamNames[i]);
	 stat.append(beamStatusString(beamStatus));
	 stat.append("\r\n");

         // TBD IF Chain associated with synth beam
	 stat.append(mBeamNames[i]);
	 stat.append(": IF:");
         stat.append(" SKYFREQ 99999.999 MHz");
         stat.append(" ATTN " + getAttnDb() + " DB");
	 stat.append("\r\n");
      }

      // tunings
      for (int tuningIndex=0; tuningIndex<mTuningNames.length; ++tuningIndex)
      {
	 String name = "TUNING" + mTuningNames[tuningIndex] + ": ";
	 stat.append(name);

	 TuningStatus tuningStatus =
	    mTuningStatusStorage.get(mTuningNames[tuningIndex]);

	 stat.append("SKYFREQ ");
	 stat.append(mFreqFormatter.format(tuningStatus.skyFreqMhz));
	 stat.append(" MHz ");

	 stat.append("\r\n");
      }

      stat.append("END\r\n");
      stat.append("\r\n");

      return stat.toString();
   }

   /**
    * Generate a time tag for current time.
    * @return a string representing the current time
    */
   private String timeString()
   {
      long time = System.currentTimeMillis();

      mCalendar.setTimeInMillis(time);

      int hour = mCalendar.get(Calendar.HOUR_OF_DAY);
      int minute = mCalendar.get(Calendar.MINUTE);
      int second = mCalendar.get(Calendar.SECOND);

      return mTimeFormatter.format(hour) + ":" 
	 + mTimeFormatter.format(minute) + ":" 
	 + mTimeFormatter.format(second);
   }

   /**
    * Sleep for a little while.
    * @param millis how long to sleep in milliseconds.
    */
   private void sleepLocal(long millis)
   {
      try
      {
	 Thread.sleep(millis);
      }
      catch (InterruptedException ie)
      {
      }
   }

   // beam format: beam{pol}{tuning}{number}
   // eg for 'beamxa1' returns 'X'
   public String getPolFromBeamName(String beamName)
   {
      int polIndex = 4;
      String pol = beamName.substring(polIndex, polIndex+1);
      
      return pol.toUpperCase();
   }

   /*
     Meeus, 17.2
     Good for angles less than about 0.16 deg
   */
   double smallAngleGcErrorDeg(double az1Deg, double el1Deg,
                               double az2Deg, double el2Deg)
   {
      double azDiffDeg = az1Deg - az2Deg;
      double elDiffDeg = el1Deg - el2Deg;

      double deltaRaCosAvgDec = azDiffDeg *
         Math.cos(Math.toRadians(elDiffDeg/2));

      double gcErrorDeg = Math.sqrt(
         deltaRaCosAvgDec * deltaRaCosAvgDec +
         elDiffDeg * elDiffDeg);

      return gcErrorDeg;
   }


   /*
      Split out comma separated names.
      Verify that names match [ant]<number><letter>.
    */
   private boolean isValidAntList(String antList)
   {
       String[] names = antList.toLowerCase().trim().split(",");
       if (names.length == 0)
       {
          return false;
       }

       String antNameRegex = "(ant)?\\d\\D";
       for (int i = 0; i < names.length; ++i)
       {
          if (! names[i].matches(antNameRegex))
          {
             return false;
          }
       }
       
       return true;
   }

   public void runTestSuite()
   {
      System.out.println("running test suite");

      double az1Deg = 80;
      double el1Deg = 40;
      double az2Deg = 80;
      double el2Deg = 40.1;
      double gcErrorDeg = smallAngleGcErrorDeg(az1Deg, el1Deg,
                                               az2Deg, el2Deg);
      System.out.println("small gc error: " + gcErrorDeg);

      az1Deg = 80.05;
      el2Deg = el1Deg;

      gcErrorDeg = smallAngleGcErrorDeg(az1Deg, el1Deg,
                                               az2Deg, el2Deg);

      System.out.println("small gc error: " + gcErrorDeg);

      assert !isValidAntList("");
      assert isValidAntList("ant1a,ant2b");
      assert isValidAntList("ant1z");
      assert isValidAntList("1a,1b,1c");
      assert !isValidAntList("a1");
      assert !isValidAntList("a11");
      assert !isValidAntList("foobar");

      assert getPolFromBeamName("beamxa1").matches("X");

      //TBD: this currently does nothing, rework allocation scheme
      assert dispatchCommand("allocate").matches(CMD_OK);

      assert dispatchCommand("zfocus ant1z 1222").startsWith(
         SUBARRAY_NOT_ASSIGNED_TO_ANY_BEAMS);
      assert dispatchCommand("bf set ants beamxa1 bad,ant,list").startsWith(INVALID_ANTLIST);
      assert dispatchCommand("bf set ants beamxa1 ant1z").matches(CMD_OK);
      assert dispatchCommand("zfocus ant1z 1222").matches(CMD_OK);
      assert dispatchCommand("zfocus bad,ant,list 1222").startsWith(INVALID_ANTLIST);
      assert dispatchCommand("zfocus   ant1z   1222  ").matches(CMD_OK);  // extra white space
      assert dispatchCommand("zfocus ant1z badfreq").startsWith(INVALID_ARGUMENT);

      assert dispatchCommand("bf set").startsWith(INVALID_NUMBER_OF_ARGS);
      assert dispatchCommand("bf set ants not-enough-args").startsWith(INVALID_NUMBER_OF_ARGS);
      assert dispatchCommand("bf set foobar").startsWith("ERROR");
      assert dispatchCommand("bf set ants bad-beam ant3d,ant3e").startsWith(INVALID_BEAM);
      assert dispatchCommand("bf set ants beamxa1 ant3d,ant3e").matches(CMD_OK);

      assert dispatchCommand("bf list ants beamxa1").toLowerCase().matches("beamxa1: ant3d,ant3e");
      assert dispatchCommand("bf list foobar").startsWith("ERROR");
      assert dispatchCommand("bf list ants bad-beam").startsWith(INVALID_BEAM);

      assert dispatchCommand("bf clear ants beamxa1").matches(CMD_OK);
      assert dispatchCommand("bf list ants beamxa1").toLowerCase().matches("beamxa1: ");   

      // point subarray:

      assert dispatchCommand("point ant5e azel 100 5").startsWith(
         SUBARRAY_NOT_ASSIGNED_TO_ANY_BEAMS);

      assert dispatchCommand("bf set ants beamxa1 ant5e").matches(CMD_OK);
      assert dispatchCommand("point ant5e azel 100 5").matches(CMD_OK);
      assert dispatchCommand("point ant5e j2000 10.5 -20").matches(CMD_OK);
      assert dispatchCommand("point ant5e gal 122 10").matches(CMD_OK);

      assert dispatchCommand("point bad,ant,list gal 122 10").startsWith(INVALID_ANTLIST);
      assert dispatchCommand("point ant5e azel badarg 5").startsWith(INVALID_ARGUMENT);
      assert dispatchCommand("point ant5e j2000 badarg -20").startsWith(INVALID_ARGUMENT);
      assert dispatchCommand("point ant5e bad-coord-sys 10.5 -20").startsWith(INVALID_COORD_SYS);

      assert dispatchCommand("point").startsWith(INVALID_NUMBER_OF_ARGS);

      // antgroups
      assert dispatchCommand("antgroup").startsWith(INVALID_NUMBER_OF_ARGS);
      assert dispatchCommand("antgroup set").startsWith(INVALID_NUMBER_OF_ARGS);

      assert dispatchCommand("antgroup set primary 1a,1b,1c").matches(CMD_OK);
      assert dispatchCommand("antgroup list primary").matches("PRIMARY: 1a,1b,1c");
      assert dispatchCommand("antgroup set primary foo,fred").startsWith(INVALID_ANTLIST);

      assert dispatchCommand("antgroup set xpol 1a,1b").matches(CMD_OK);
      assert dispatchCommand("antgroup list xpol").matches("XPOL: 1a,1b");

      assert dispatchCommand("antgroup set ypol 1b,1c").matches(CMD_OK);
      assert dispatchCommand("antgroup list ypol").matches("YPOL: 1b,1c");

      assert dispatchCommand("antgroup clear primary").matches(CMD_OK);
      assert dispatchCommand("antgroup list primary").matches("PRIMARY: ");

      assert dispatchCommand("antgroup autoselect sefd 1000 freq 8400").startsWith(READY_PREFIX);
      assert dispatchCommand("antgroup autoselect").startsWith(INVALID_NUMBER_OF_ARGS);

      // assign ant groups, use them in ant commands
      assert dispatchCommand("antgroup clear all").matches(CMD_OK);
      assert dispatchCommand("antgroup set xpol 1a,1b").matches(CMD_OK);
      assert dispatchCommand("antgroup set primary 1a,1b,1c").matches(CMD_OK);

      assert dispatchCommand("bf set ants beamxa1 antgroup").matches(CMD_OK);
      assert dispatchCommand("bf list ants beamxa1").toLowerCase().matches("beamxa1: 1a,1b");
      assert dispatchCommand("bf set ants beamza1 antgroup").startsWith(INVALID_BEAM);
      assert dispatchCommand("bf set ants beamya1 antgroup").startsWith(ANTGROUP_UNASSIGNED);

      assert dispatchCommand("antgroup clear all").matches(CMD_OK);
      assert dispatchCommand("point antgroup j2000 10.5 -20").startsWith(ANTGROUP_UNASSIGNED);
      assert dispatchCommand("antgroup set primary 2a,2b,2c").matches(CMD_OK);
      assert dispatchCommand("point antgroup j2000 10.5 -20").startsWith(SUBARRAY_NOT_ASSIGNED_TO_ANY_BEAMS);

      // Use a primary list that overlaps that previously assigned to a beam
      assert dispatchCommand("antgroup set primary 1a,1b,1c,1d").matches(CMD_OK);
      assert dispatchCommand("point antgroup j2000 10.5 -20").matches(CMD_OK);
      assert dispatchCommand("stop antgroup").matches(CMD_OK);
      assert dispatchCommand("wrap antgroup 1").matches(CMD_OK);
      assert dispatchCommand("zfocus antgroup 1222").matches(CMD_OK);

      // set beam coords:

      assert dispatchCommand("bf set coords beamxa1 azel 20 10").matches(CMD_OK);
      assert dispatchCommand("bf set coords beamyd4 azel 20 10").matches(CMD_OK);
      assert dispatchCommand("bf set coords beamxa1 j2000 10.5 -20").matches(CMD_OK);
      assert dispatchCommand("bf set coords beamxa1 gal 122 10").matches(CMD_OK);

      assert dispatchCommand("bf set coords badbeamname azel 20 10").startsWith(INVALID_BEAM);
      assert dispatchCommand("bf set coords beamyd4 bad-coord-sys 20 10").startsWith(INVALID_COORD_SYS);
      assert dispatchCommand("bf set coords beamyd4 azel badarg 10").startsWith(INVALID_ARGUMENT);
      assert dispatchCommand("bf set coords beamyd4 j2000 badarg 10").startsWith(INVALID_ARGUMENT);
      assert dispatchCommand("bf set coords beamyd4 gal badarg 10").startsWith(INVALID_ARGUMENT);

      assert dispatchCommand("bf set coords beamyd4 azel 40 50").matches(CMD_OK);

      assert dispatchCommand("bf list coords beamyd4").startsWith("BEAMYD4 AZEL AZ 40.000000 deg EL 50.000000 deg RA .000000 hr DEC .000000 deg GLONG .000000 deg GLAT .000000 deg");

      assert dispatchCommand("bf set coords beamyd4 j2000 5 25").matches(CMD_OK);
      assert dispatchCommand("bf list coords beamyd4").startsWith("BEAMYD4 J2000 AZ .000000 deg EL .000000 deg RA 5.000000 hr DEC 25.000000 deg GLONG .000000 deg GLAT .000000 deg");

      assert dispatchCommand("bf set coords beamyd4 gal 100 -12").matches(CMD_OK);
      assert dispatchCommand("bf list coords beamyd4").startsWith("BEAMYD4 GAL AZ .000000 deg EL .000000 deg RA .000000 hr DEC .000000 deg GLONG 100.000000 deg GLAT -12.000000 deg");

      assert dispatchCommand("bf set nulltype none").matches(CMD_OK);
      assert dispatchCommand("bf set nulltype foobar").startsWith(INVALID_ARGUMENT);

      assert dispatchCommand("bf clear nulls all").matches(CMD_OK);
      assert dispatchCommand("bf set nulltype projection").matches(CMD_OK);
      assert dispatchCommand("bf add null beamyd4 j2000 5 25").matches(CMD_OK);
      assert dispatchCommand("bf add null beamyd4 azel 30 50").matches(CMD_OK);
      assert dispatchCommand("bf add null beamyd4 gal 100 10").matches(CMD_OK);
      assert dispatchCommand("bf add null beamyd4 badcoordsys 100 10").startsWith(INVALID_COORD_SYS);
      assert dispatchCommand("bf list nulls").startsWith(UNIMPLEMENTED_COMMAND);

      assert dispatchCommand("bf clear coords beamyd4").matches(CMD_OK);
      assert dispatchCommand("bf list coords beamyd4").startsWith("BEAMYD4 UNINIT AZ .000000 deg EL .000000 deg RA .000000 hr DEC .000000 deg GLONG .000000 deg GLAT .000000 deg");

      assert dispatchCommand("bf autoatten").startsWith(READY_PREFIX);

      assert dispatchCommand("tune a 1500").matches(CMD_OK);
      assert dispatchCommand("tune d 1600").matches(CMD_OK);
      assert dispatchCommand("tune a badarg").startsWith(INVALID_ARGUMENT);
      assert dispatchCommand("tune badtuning 1500").startsWith(INVALID_TUNING_NAME);

      assert dispatchCommand("attn a 10 20").matches(CMD_OK);
      assert dispatchCommand("attn a badarg 20").startsWith(INVALID_ARGUMENT);
      assert dispatchCommand("attn b").startsWith(INVALID_NUMBER_OF_ARGS);
      assert dispatchCommand("attn badtuning 10 20").startsWith(INVALID_TUNING_NAME);
      assert dispatchCommand("attn a 1000 20").startsWith(OUT_OF_RANGE);

      assert dispatchCommand("stop").startsWith(INVALID_NUMBER_OF_ARGS);

      assert dispatchCommand("bf set ants beamxa1 ant3d,ant3e").matches(CMD_OK);
      assert dispatchCommand("stop ant3d,ant3e").matches(CMD_OK);
      assert dispatchCommand("stop bad,ant,list").startsWith(INVALID_ANTLIST);

      assert dispatchCommand("stow").startsWith(INVALID_NUMBER_OF_ARGS);
      assert dispatchCommand("bf set ants beamxa1 ant3d,ant3e").matches(CMD_OK);
      assert dispatchCommand("stow ant3d,ant3e").matches(CMD_OK);

      assert dispatchCommand("lnaon ant3d,ant3e").matches(CMD_OK);
      assert dispatchCommand("pamset ant3d,ant3e").matches(CMD_OK);

      assert dispatchCommand("monitor 9").matches(CMD_OK);
      assert dispatchCommand("monitor badarg").startsWith(INVALID_ARGUMENT);
      assert dispatchCommand("monitor").startsWith(INVALID_NUMBER_OF_ARGS);

      assert dispatchCommand("wrap").startsWith(INVALID_NUMBER_OF_ARGS);
      assert dispatchCommand("bf set ants beamxa1 ant3d,ant3e").matches(CMD_OK);
      assert dispatchCommand("wrap ant3d,ant3e 1").matches(CMD_OK);
      assert dispatchCommand("wrap bad,ant,list 1").startsWith(INVALID_ANTLIST);
      assert dispatchCommand("wrap ant3d,ant3e badarg").startsWith(INVALID_ARGUMENT);

      assert dispatchCommand("badcommand").startsWith(UNRECOGNIZED_COMMAND);

      assert dispatchCommand("bf stop").matches(CMD_OK);
      assert dispatchCommand("bf reset").startsWith(READY_PREFIX);

      assert dispatchCommand("bf cal ").startsWith(INVALID_NUMBER_OF_ARGS);

      assert dispatchCommand("bf cal delay integrate 20 cycles 4").startsWith(READY_PREFIX);
      assert dispatchCommand("bf cal foobar integrate 20 cycles 4").startsWith(INVALID_ARGUMENT);

      assert dispatchCommand("bf set obslen 220").matches(CMD_OK);
      assert dispatchCommand("bf list obslen").matches("220");

      assert dispatchCommand("bf set attn beamyd4 5").matches(CMD_OK);
      assert dispatchCommand("bf list attn beamyd4").matches("5.0");

      assert dispatchCommand("bf list config").startsWith("BEAM");

      assert dispatchCommand("bf init").startsWith(READY_PREFIX);

      assert dispatchCommand("bf clear coords all").startsWith(CMD_OK);
      assert dispatchCommand("bf point").startsWith(NO_COORDS_ASSIGNED_TO_BEAMS);
      assert dispatchCommand("bf set coords beamyd4 azel 15 58").startsWith(CMD_OK);
      assert dispatchCommand("bf point").startsWith(READY_PREFIX);

      //TBD: this currently does nothing, rework allocation scheme
      assert dispatchCommand("deallocate").matches(CMD_OK);
   }

   public static void main(String[] args) throws IOException
   {
      try
      {
	 CmdLineOptions options = new CmdLineOptions();
	 if ( ! options.parseCommandLineArgs(args) ) {
	    System.exit(1);
	 }

	 System.out.println("command port is " + options.getCommandPort());
	 System.out.println("status port is " + options.getStatusPort());

	 AtaControlSim sim = new AtaControlSim(
	    options.getCommandPort(), options.getStatusPort());

	 if (options.runTestSuite) {
            try {
               sim.runTestSuite();
            }
            catch(AssertionError ae)
            {
               ae.printStackTrace();
            }
            finally {
               System.exit(1);
            }
	 }
      }
      catch (Exception ex)
      {
	 ex.printStackTrace();
      }
   }

}