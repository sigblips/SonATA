#!/usr/bin/env python
################################################################################
#
# File:    verifySonATATestSignal.py
# Project: OpenSonATA
# Authors: The OpenSonATA code is the result of many programmers
#          over many years
#
# Copyright 2011 The SETI Institute
#
# OpenSonATA is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# OpenSonATA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
# 
# Implementers of this code are requested to include the caption
# "Licensed through SETI" with a link to setiQuest.org.
# 
# For alternate licensing arrangements, please contact
# The SETI Institute at www.seti.org or setiquest.org. 
#
################################################################################

# -*-python-*-  (emacs mode)

#***********************************
# Save this for historical reference
# if we ever get another Test Signal
#***********************************

# verify that SonATA test signal is present in all dxs
# for the activity that follows the timestampBeforeAct (unix timestamp)

import MySQLdb
import sys

class VerifySonATATestSignal:

    def __init__(self):
        self.errorCount = 0
        self.warningCount = 0

    def printError(self, errorStr):
        print "Error:", errorStr
        self.errorCount = self.errorCount + 1

    def printWarning(self, warnStr):
        print "Warning:", warnStr
        self.warningCount = self.warningCount + 1

    def printSummary(self):
        print ""
        print "================================"
        print "Summary: Warnings: %d Errors: %d" % (self.warningCount, self.errorCount)
        print "================================"

    def exitStatus(self):
        if self.errorCount > 0:
            return 1
        else:
            return 0

    def printTitle(self, title):
        print ""
        print title
        print "==============="
	
    def setUp(self):
    
        # connect to database
        hostName="localhost"
        dbName="sonata_autotest"
        userName="sonata"
        self.db = MySQLdb.connect(host=hostName, user=userName, db=dbName)
        
        # create a cursor
        self.cursor = self.db.cursor()

    def tearDown(self):

        self.db.close()        

    def testActivityRanOk(self):

        self.printTitle("testActivityRanOk")

        # verify that the activity ran, successfully
        query="select id, validObservation, comments from Activities where ts > FROM_UNIXTIME(%s)" % timestampBeforeAct
        self.cursor.execute(query)
        result = self.cursor.fetchall()

        # make sure there's only one activity
        nActsFound=len(result)
        #print "nActsFound=" ,nActsFound

        if nActsFound == 0:
            self.printError("did not find an activity after the given time")
            return

        if nActsFound > 1:
            self.printError("found more activities than expected: %s " % nActsFound)
            return

        #print "found " , len(result), " rows "

        row = result[0];
        #print "row is ", row

        #actId=row[0]
        validObs=row[1]
        comments=row[2]
        #print "actId= ", actId, " validObs=", validObs

        if validObs != "Yes":
            self.printError("activity does not have valid status, comment=" + comments)
        else:
            print "OK"

    def testAllExpectedDxsRanOk(self):

        self.printTitle("testAllExpectedDxsRanOk")

        query="select DxIntrinsics.dxHostName, ActivityUnits.validObservation, ActivityUnits.comments from DxIntrinsics, ActivityUnits where ActivityUnits.dxIntrinsicsId = DxIntrinsics.id and ActivityUnits.startOfDataCollection > FROM_UNIXTIME(%s)" % timestampBeforeAct

        print "query= ", query

        self.cursor.execute(query)
        result = self.cursor.fetchall()

        print "result = ", result
        nFoundDxs=len(result)
        nExpectedDxs=len(globalExpectedDxs)

        if nFoundDxs != nExpectedDxs:
            self.printWarning("expected %d dxs, found %d dxs " % (nExpectedDxs, nFoundDxs))

        if nFoundDxs == 0:
            self.printError("Did not find results for any dxs")
            return

        # find out which dxs out of the expected ones did not show up
        dxNameCol=0
        notFoundDxs=globalExpectedDxs[:]
        for record in result:
            dxNameInDb = record[dxNameCol]
            #print "dxNameInDb" , dxNameInDb
            if dxNameInDb in notFoundDxs:
                notFoundDxs.remove(dxNameInDb)
            else:
                self.printWarning("found unexpected dx: %s" % dxNameInDb)

        if len(notFoundDxs) > 0:
            self.printWarning("did not find results from dxs: %s" % notFoundDxs)

        # check that all activity units reported in as valid
        validObsCol=1
        commentsCol=2
        validObsCount=0
        for record in result:
            dxNameInDb = record[dxNameCol]
            validObs = record[validObsCol]
            if (validObs != "Yes"):
                self.printWarning("%s reported an invalid observation: " % dxNameInDb)
                print record[commentsCol]
            else:
                validObsCount=validObsCount+1

        if validObsCount == 0:
            self.printError("No dxs had valid observation status")
            return
        else:
            print "OK"

    def testAllDxsSawTestSignal(self):

        self.printTitle("testAllDxsSawTestSignal")

        expectedTestFreqMhz=1420.8001
        #expectedTestFreqMhz=1501.123456
        expectedFreqTolMhz=0.000050
        query="select dxNumber, type, pol from CandidateSignals where rfFreq > %f and rfFreq < %f and ts > FROM_UNIXTIME(%s) and reason='Confrm'" % (expectedTestFreqMhz - expectedFreqTolMhz, expectedTestFreqMhz + expectedFreqTolMhz, timestampBeforeAct)

        print "query= ", query

        self.cursor.execute(query)
        result = self.cursor.fetchall()

        print "result = ", result
        nFoundDxs=len(result)
        nExpectedDxs=len(globalExpectedDxs)

        if nFoundDxs != nExpectedDxs:
            self.printWarning("expected %d dxs, found %d" % (nExpectedDxs, nFoundDxs))

        if nFoundDxs == 0:
            self.printError("Did not find test signal for any dxs")
            return
        
        # find out which dxs did and did not see the test signal
        dxNumberCol=0
        #typeCol=1
        polCol=2
        dxsThatDidNotSeeSignal=globalExpectedDxs[:]
        dxsThatSawSignal=[]
        for record in result:
            dxNumberInDb = record[dxNumberCol]
            dxNameInDb="dx%s" % dxNumberInDb
            dxsThatSawSignal.append(dxNameInDb)
            if dxNameInDb in dxsThatDidNotSeeSignal:
                dxsThatDidNotSeeSignal.remove(dxNameInDb)
            else:
                self.printWarning("found unexpected dx: %s" % dxNameInDb)

            # verify that signal was seen in both pols
            pol = record[polCol]
            if pol != "both":
                self.printWarning("test signal seen only in %s pol for %s" % (pol, dxNameInDb))

        # summarize results
        print "dxs that saw test signal: ", dxsThatSawSignal
        if len(dxsThatDidNotSeeSignal) > 0:
	        self.printWarning("dxs that did NOT see the test signal: %s " % dxsThatDidNotSeeSignal)

        print "OK"
            
if __name__ == '__main__':

    if len(sys.argv) >= 2:
        timestampBeforeAct=sys.argv[1]
    else:
        print "check for SonATA test results"
        print "usage: %s <unix timestamp before test activity results>" % sys.argv[0] 
        sys.exit(1)

    globalExpectedDxs=["dx1000", "dx1001", "dx1002", "dx1003", "dx1004", "dx1005", "dx1006", "dx1007", "dx2000", "dx2001", "dx2002", "dx2003", "dx2004", "dx2005", "dx2006", "dx2007"]


    #print "timestampBeforeAct=" , timestampBeforeAct

    test = VerifySonATATestSignal()
    test.setUp()
    test.testActivityRanOk()
    test.testAllExpectedDxsRanOk()
    test.testAllDxsSawTestSignal()
    test.tearDown()
    test.printSummary()

    sys.exit(test.exitStatus())
                