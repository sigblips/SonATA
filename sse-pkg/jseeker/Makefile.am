################################################################################
#
# File:    Makefile.am
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


MYSQL_CONNECTOR = mysql-connector-java-3.0.14-bin.jar
JFREECHART = jfreechart-1.0.6.jar
JCOMMON = jcommon-1.0.10.jar
PKG_DATADISPLAYS_DIR = ./opensonata/dataDisplays

DISTRIBUTED_JARS = \
	AbsoluteLayout.jar \
	plot.jar \
	$(MYSQL_CONNECTOR) \
        $(JFREECHART) \
	$(JCOMMON) \
	jtransforms-2.3.jar \
	jsap.jar

DISTRIBUTED_PROG_WRAPPERS = \
	waterfallDisplay \
	waterfallDisplayHeadless \
	baselineDisplay \
	baselineImage \
	filesender \
	filereceiver \
	obsHistoryDisplay \
	stxhistogramDisplay \
	atacontrolsim

EXTRA_DIST = \
	ATACasperPacket.java \
	WaterfallDisplay.java \
	EnvReader.java	\
	WaterfallDisplayManifest.txt \
	BaselineDisplay.java \
	BaselineDisplayManifest.txt \
	BaselineImage.java \
	BaselineImageManifest.txt \
	FileSender.java \
	FileSenderManifest.txt \
	FileReceiver.java \
	FileReceiverManifest.txt \
	ObsHistoryDisplay.java \
	ObsHistoryDisplayManifest.txt \
	ReadoutPlot.java \
	ReadoutListener.java \
	STXHistogramDisplay.java \
	FormattedDocument.java \
	STXHistogramDisplayManifest.txt \
	DBConnectionDialog.java \
	DataDisplay.java \
	AtaControlSim.java \
	AtaControlSimManifest.txt \
	setilogo.jpg \
	$(DISTRIBUTED_PROG_WRAPPERS) \
	$(DISTRIBUTED_JARS)

CREATED_JARS = \
	AtaControlSim.jar \
	WaterfallDisplay.jar \
	BaselineDisplay.jar \
	BaselineImage.jar \
	FileSender.jar \
	FileReceiver.jar \
	ObsHistoryDisplay.jar \
	STXHistogramDisplay.jar

bin_SCRIPTS = \
	$(DISTRIBUTED_PROG_WRAPPERS) \
	$(CREATED_JARS) \
	$(DISTRIBUTED_JARS)

# Set the javac flags so that the source files can
# be located in some place other than the current directory, 
# and that the generated .class files are always put in the currect 
# directory.  This is required to support a "VPATH" build, such 
# as 'make distcheck'.
# Note that each source file must also be prefixed with $(srcdir)
# to make this work.

# use -source 1.4 to compile with asserts
JVC = javac -source 1.4
JVCFLAGS = -sourcepath $(srcdir) -d . -classpath $(PKG_DATADISPLAYS_DIR):$(srcdir)/AbsoluteLayout.jar:$(srcdir)/plot.jar:$(srcdir)/$(MYSQL_CONNECTOR):$(srcdir)/jtransforms-2.3.jar:$(srcdir)/jsap.jar:$(srcdir)/$(JFREECHART):$(srcdir)/$(JCOMMON)

JVCFLAGSNOLIBS = -sourcepath $(srcdir) -d . -classpath $(srcdir)

all: waterfalljar baselinejar baselineimagejar filesenderjar filereceiverjar obshistorydisplayjar atacontrolsimjar

check: all

compileall:
	$(JVC) $(JVCFLAGS) $(srcdir)/*.java

jseekerall: compileall


# ----- AtaControlSim -----

AtaControlSimTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/AtaControlSim.java  

atacontrolsimjar: AtaControlSimTarget
	jar cmf $(srcdir)/AtaControlSimManifest.txt AtaControlSim.jar $(PKG_DATADISPLAYS_DIR)/AtaControlSim.class

AtaControlSim.jar:  atacontrolsimjar


# ----- WaterfallDisplay -----

WaterfallDisplayTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/WaterfallDisplay.java $(srcdir)/EnvReader.java $(srcdir)/ReadoutPlot.java $(srcdir)/ReadoutListener.java $(srcdir)/ATACasperPacket.java

waterfalljar: WaterfallDisplayTarget
	jar cmf $(srcdir)/WaterfallDisplayManifest.txt WaterfallDisplay.jar $(PKG_DATADISPLAYS_DIR)/WaterfallDisplay*.class $(PKG_DATADISPLAYS_DIR)/EnvReader.class  $(PKG_DATADISPLAYS_DIR)/ReadoutPlot*.class $(PKG_DATADISPLAYS_DIR)/ReadoutListener.class $(PKG_DATADISPLAYS_DIR)/ATACasperPacket.class

WaterfallDisplay.jar:  waterfalljar

# ----- BaselineDisplay -----

BaselineDisplayTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/BaselineDisplay.java $(srcdir)/ReadoutPlot.java $(srcdir)/ReadoutListener.java $(srcdir)/EnvReader.java

baselinejar: BaselineDisplayTarget
	jar cmf $(srcdir)/BaselineDisplayManifest.txt BaselineDisplay.jar $(PKG_DATADISPLAYS_DIR)/BaselineDisplay*.class $(PKG_DATADISPLAYS_DIR)/EnvReader.class $(PKG_DATADISPLAYS_DIR)/ReadoutPlot*.class $(PKG_DATADISPLAYS_DIR)/ReadoutListener.class

BaselineDisplay.jar: baselinejar

# ----- BaselineImage -----

BaselineImageTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/BaselineImage.java

baselineimagejar: BaselineImageTarget
	jar cmf $(srcdir)/BaselineImageManifest.txt BaselineImage.jar $(PKG_DATADISPLAYS_DIR)/BaselineImage*.class 

BaselineImage.jar: baselineimagejar


# ----- BinTail -----

BinTailTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/BinTail.java  

bintailjar: BinTailTarget
	jar cmf $(srcdir)/BinTailManifest.txt BinTail.jar BinTail*.class 

BinTail.jar: bintailjar

# ----- FileSender -----

FileSenderTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/FileSender.java  

filesenderjar: FileSenderTarget
	jar cmf $(srcdir)/FileSenderManifest.txt FileSender.jar $(PKG_DATADISPLAYS_DIR)/FileSender*.class 

FileSender.jar: filesenderjar

# ----- FileReceiver -----

FileReceiverTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/FileReceiver.java  

filereceiverjar: FileReceiverTarget
	jar cmf $(srcdir)/FileReceiverManifest.txt FileReceiver.jar $(PKG_DATADISPLAYS_DIR)/FileReceiver*.class 

FileReceiver.jar: filereceiverjar

# ----- ObsHistoryDisplay -----

ObsHistoryDisplayTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/ObsHistoryDisplay.java $(srcdir)/ReadoutPlot.java $(srcdir)/ReadoutListener.java $(srcdir)/EnvReader.java

obshistorydisplayjar: ObsHistoryDisplayTarget
	jar cmf $(srcdir)/ObsHistoryDisplayManifest.txt ObsHistoryDisplay.jar $(PKG_DATADISPLAYS_DIR)/ObsHistoryDisplay*.class $(PKG_DATADISPLAYS_DIR)/EnvReader.class $(PKG_DATADISPLAYS_DIR)/ReadoutPlot*.class $(PKG_DATADISPLAYS_DIR)/ReadoutListener.class

ObsHistoryDisplay.jar: obshistorydisplayjar

# ----- ActivitySummary -----

ActivitySummaryTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/ActivitySummary.java $(srcdir)/DBConnectionDialog.java 

activitysummaryjar: ActivitySummaryTarget
	jar cmf $(srcdir)/ActivitySummaryManifest.txt ActivitySummary.jar \
          ActivitySummary*.class  \
          DBApplicationInterface*.class DBConnectionDialog*.class

ActivitySummary.jar: activitysummaryjar

# ----- SeekerReports -----

SeekerReportsTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/SeekerReports.java $(srcdir)/DBConnectionDialog.java 

seekerreportsjar: SeekerReportsTarget
	jar cmf $(srcdir)/SeekerReportsManifest.txt SeekerReports.jar \
          SeekerReports*.class  \
          SRPreferencesDialog*.class \
          DBApplicationInterface*.class DBConnectionDialog*.class

SeekerReports.jar: seekerreportsjar

# ----- TSS2NSS -----

TSS2NSSTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/TSS2NSS.java $(srcdir)/DBConnectionDialog.java 

tss2nssjar: TSS2NSSTarget
	jar cmf $(srcdir)/TSS2NSSManifest.txt TSS2NSS.jar \
          TSS2NSS*.class  \
          DBApplicationInterface*.class DBConnectionDialog*.class

TSS2NSS.jar: tss2nssjar

# ----- BirdieMaskGen -----

BirdieMaskGenTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/BirdieMaskGen.java $(srcdir)/DBConnectionDialog.java 

birdiemaskjar: BirdieMaskGenTarget
	jar cmf $(srcdir)/BirdieMaskGenManifest.txt BirdieMaskGen.jar \
           BirdieMaskGen*.class \
           DBApplicationInterface*.class DBConnectionDialog*.class

BirdieMaskGen.jar: birdiemaskjar

# ----- PermRFIMaskGen -----

PermRFIMaskGenTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/PermRFIMaskGen.java $(srcdir)/DBConnectionDialog.java 

permrfimaskjar: PermRFIMaskGenTarget
	jar cmf $(srcdir)/PermRFIMaskGenManifest.txt PermRFIMaskGen.jar \
          PermRFIMaskGen*.class  \
          DBApplicationInterface*.class DBConnectionDialog*.class

PermRFIMaskGen.jar: permrfimaskjar

# ----- STX Histogram -----

STXHistogramDisplayTarget:
	$(JVC) $(JVCFLAGS) $(srcdir)/STXHistogramDisplay.java $(srcdir)/FormattedDocument.java 

stxhistogramjar: STXHistogramDisplayTarget
	jar cmf $(srcdir)/STXHistogramDisplayManifest.txt STXHistogramDisplay.jar $(PKG_DATADISPLAYS_DIR)/STXHistogram*.class $(PKG_DATADISPLAYS_DIR)/FormattedDocument.class

STXHistogramDisplay.jar: stxhistogramjar


# cleanup 
CLEAN_FILES = $(PKG_DATADISPLAYS_DIR)/*.class $(CREATED_JARS) 

clean:
	rm -f $(CLEAN_FILES)

distclean:
	rm -f $(CLEAN_FILES) Makefile
