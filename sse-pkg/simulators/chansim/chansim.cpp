/*******************************************************************************

 File:    chansim.cpp
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
  Channelizer simulator main
 */
#include "ace/Reactor.h"
#include "SseSock.h"
#include "Channelizer.h"
#include "SseProxy.h"
#include "SseUtil.h"
#include "ssePorts.h"
#include "SseException.h"
#include "CmdLineParser.h"

using namespace std;

const string sseHostArg("-ssehost");
const string ssePortArg("-sseport");
const string nameArg("-name");
const string totalChansArg("-totchans");
const string outChansArg("-outchans");
const string outBandwidthArg("-outbw");

void initCmdLineArgs(CmdLineParser &parser)
{
   const string &noDescrip("");
   const string defaultSseHost("localhost");
   parser.addStringOption(sseHostArg, defaultSseHost, noDescrip);

   int defaultSsePort(SseUtil::strToInt(DEFAULT_CHANNELIZER_PORT));
   parser.addIntOption(ssePortArg, defaultSsePort, noDescrip);

   string defaultName("chan1");
   parser.addStringOption(nameArg, defaultName, noDescrip);

   int defaultTotalChans(256);
   parser.addIntOption(totalChansArg, defaultTotalChans, "total number of channels");

   int defaultOutChans(172);
   parser.addIntOption(outChansArg, defaultOutChans, "number of output channels");

   double defaultOutBandwidthMhz(20);  
   parser.addDoubleOption(outBandwidthArg, defaultOutBandwidthMhz, "output bandwidth MHz");
}

int main(int argc, char * argv[])
{
   CmdLineParser parser;
   initCmdLineArgs(parser);
   if (! parser.parse(argc, argv))
   {
      cerr << parser.getErrorText() << endl;
      cerr << parser.getUsage();
      exit(1);
   }

   try 
   {
      int ssePort(parser.getIntOption(ssePortArg));
      string sseHostname(parser.getStringOption(sseHostArg));
       
      // put all the ace event handlers on the heap
      // for easier cleanup management

      // Connect to sse via 
      // direct tcp socket connect using the above host & port.

      u_short socketPort(ssePort);
      string socketHost(sseHostname);

      SseSock *ssesock = new SseSock(socketPort, socketHost.c_str());
      ssesock->connect_to_server();

      SseProxy *proxy = new SseProxy(ssesock->sockstream());

      // Register the socket input event handler for reading
      ACE_Reactor::instance()->register_handler(
         proxy, ACE_Event_Handler::READ_MASK);

      string name(parser.getStringOption(nameArg));
      int totalChans(parser.getIntOption(totalChansArg));
      int outputChans(parser.getIntOption(outChansArg));
      double outBwMhz(parser.getDoubleOption(outBandwidthArg));

      // put this on the heap also, since it contains an ACE event handler
      /*Channelizer * chanzer = */
      new Channelizer(proxy, name, totalChans, outputChans, outBwMhz);

      ACE_Reactor::run_event_loop();

      // Done running.  Unregister the event handler so that the
      // reactor can cleanly exit.

      ACE_Reactor::instance()->remove_handler(
             proxy,
             ACE_Event_Handler::READ_MASK);

      // Shut down the Reactor explicitly, so that the
      // registered inputHandlers can properly clean up 

      ACE_Reactor::close_singleton();

   } 
   catch (SseException &exception)
   {
      cerr << exception.descrip() << endl;

      exit(1);
   }
   catch (...)
   {
      cerr << "chansim: caught unknown exception" << endl;
      exit(1);
   }
   return 0;

}


