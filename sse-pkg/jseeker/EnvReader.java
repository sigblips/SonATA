/*******************************************************************************

 File:    EnvReader.java
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
 * Reads the environment variables..
 *
 * Project: OpenSonATA
 * Version: 1.0
 * Author:  Jon Richards (current maintainer)
 *          The OpenSonATA code is the result of many programmers over many
 *          years.
 *
 * This class displays read in the environment variables.
 *
 * Copyright 2010 The SETI Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0 Unless required by
 * applicable law or agreed to in writing, software distributed
 * under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * Attribution must be: “Licensed through SETI” in all published
 * uses of the software including analytics based on the software,
 * combined and merged software, papers, articles, books, reports,
 * web pages, etc.
 */

/**
 * @file EnvReader.java
 *
 * This class displays read in the environment variables.
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

import java.io.*;
import java.util.*;

/**
 * Reads in the environment variables.
 */
public class EnvReader
{
    private Map envMap;

    /**
     * Constructor.
     */
    public EnvReader()
    {
        envMap = getEnvironment();   
    }

    // returns the value of the given environment variable,
    // or null if it does not exist.
    /**
     * Get the value of an environment variable.
     *
     * @param envVarName the name ov the environment variable.
     * @return the string value of the environment variable, or null if
     * it does not exist.
     */
    public String getEnvVar(String envVarName)
    {
        return (String) envMap.get(envVarName);
    }

    /**
     * Read the environment variables into a HashMap.
     *
     * @return a hashMapp containing the varname:value as key:value pairs.
     */
    private Map getEnvironment()
    {
        Map map = new HashMap();

        try 
        {
            // OS-specific command: (UNIX)
            String cmd[] = { "/bin/sh", "-c", "env" };
            Process pid = Runtime.getRuntime().exec(cmd);

            BufferedReader in =
                new BufferedReader(
                        new InputStreamReader(
                            pid.getInputStream()));

            for (;;) 
            {

                // read through all the lines, breaking them
                // into name, value pairs,
                // and store them in the map.

                String line = in.readLine();
                if (line == null) break;
                int p = line.indexOf("=");
                if (p != -1) 
                {
                    String key = line.substring(0, p);
                    String value = line.substring(p+1);
                    map.put(key, value);
                }

            }

        }
        catch ( IOException e)
        {
            System.out.println("getEnvironment IO Exception");
        }


        return map;
    }


    /**
     * The main entry point. Used mainly for testing. Prints out
     * the environment varibale.
     *
     * @param args the command line argument array.
     */
    public static void main(String[] args) throws IOException
    {
        if (args.length > 0)
        {
            String envVarName = args[0];
            EnvReader env = new EnvReader();

            String value = env.getEnvVar(envVarName);
            if (value == null) {
                System.out.println(envVarName + " not found");
            }
            else {
                System.out.println(envVarName + "=" + value);
            }
        }
        else
        {
            System.out.println("give env var to display");
        }
    }

}
