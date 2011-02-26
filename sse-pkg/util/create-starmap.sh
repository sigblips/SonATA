#!/bin/sh
################################################################################
#
# File:    create-starmap.sh
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


# Create a starmap image using the pp3, tex, and ImageMagick packages.
# http://pp3.sourceforge.net/

setGlobalVars()
{
   #set -x
   progName=`basename $0`  

   outFile="starmap.gif"

   # default to Big Dipper
   mapCenterRaHours="12.000"
   mapCenterDecDeg="65.000"

   # by default, set the target coords so they're off the plot
   target1RaHours="0.0"
   target1DecDeg="999"  
   target1Name="T1"

   target2RaHours="0.0"
   target2DecDeg="999"  
   target2Name="T2"

   target3RaHours="0.0"
   target3DecDeg="999"  
   target3Name="T3"

   mapWidthCm="19"
   mapHeightCm="12"
   mapScaleDegPerCm="2"

   # ATA beamsize at 1Ghz
   primaryBeamsizeDeg="3.5"

   mapWorkingName="starmap"
   pp3InputFile="${mapWorkingName}.pp3"
   mapTexPreamble="${mapWorkingName}-preamble.tex"

   tmpdir="/tmp/create-starmap-workdir$$" 

   # debug
   #tmpdir="/tmp/create-starmap-workdir"

   pwd="$PWD"

   # RGB colors
   black="0 0 0"
   green="0 1 0"
   red="1 0 0"
   white="1 1 1"
   yellow="1 1 0"
   cyan="0 1 1"
   deepBlue="0.05 0.57 1.0"

   displayFancyCrosshair="false"

   showPrimaryBeam="false"

   # command line arg names
   raHoursArgName="-rahours"
   decDegArgName="-decdeg"
   crosshairArgName="-crosshair"
   showPrimaryBeamArgName="-showprimarybeam"
   primaryBeamsizeDegArgName="-primarybeamsizedeg"
   target1RaHoursArgName="-target1rahours"
   target1DecDegArgName="-target1decdeg"
   target2RaHoursArgName="-target2rahours"
   target2DecDegArgName="-target2decdeg"
   target3RaHoursArgName="-target3rahours"
   target3DecDegArgName="-target3decdeg"
   degPerCmArgName="-degpercm"
   outfileArgName="-outfile"
}


usage()
{
   echo "usage: $progName [${raHoursArgName} <map center RA>] [${decDegArgName} <map center Dec>] [${crosshairArgName}] [${showPrimaryBeamArgName}] [${primaryBeamsizeDegArgName} <beamsize, default=${primaryBeamsizeDeg}> ] [${target1RaHoursArgName} <RA> -target1decdeg <dec>]  [-target2rahours <RA> -target2decdeg <dec>] [-target3rahours <RA> -target3decdeg <dec>] [${degPerCmArgName} <map scale degrees per cm, default=${mapScaleDegPerCm}>] [${outfileArgName} <starmap.gif>] [-help]"
   echo "Creates a starmap image in GIF format."
}

help()
{
   usage

   echo "${raHoursArgName}: map center RA"
   echo "${decDegArgName}: map center Dec"
   echo "${crosshairArgName}: draw crosshair at map center"
   echo "${showPrimaryBeamArgName}: draw a circle around the map center showing the primary beam"
   echo "${primaryBeamsizeDegArgName}: size of primary beam in degrees"
   echo "${target1RaHoursArgName} ${target1DecDegArgName}: show target 1 at given RA, dec"
   echo "${target2RaHoursArgName} ${target2DecDegArgName}: show target 2 at given RA, dec"
   echo "${target3RaHoursArgName} ${target3DecDegArgName}: show target 3 at given RA, dec"
   echo "${degPerCmArgName}: set map scale to degrees per cm"
   echo "${outfileArgName}: map filename"
}

parseArgs()
{
   minArgs=6

   # walk through the args
   while [ "$1" ] 
   do
      case $1 in

      "${raHoursArgName}")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing map center ra hours value"
            exit 1
         else
            mapCenterRaHours="$1"
         fi
        ;;

      "${decDegArgName}")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing map center dec deg value"
            exit 1
         else
            mapCenterDecDeg="$1"
         fi
        ;;

      "${crosshairArgName}")
         displayFancyCrosshair="true"
        ;;

      "${showPrimaryBeamArgName}")
         showPrimaryBeam="true"
        ;;

      "${primaryBeamsizeDegArgName}")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing primary beamsize deg value"
            exit 1
         else
             primaryBeamsizeDeg="$1"
         fi
        ;;

      "${target1RaHoursArgName}")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing target1 ra hours value"
            exit 1
         else
            target1RaHours="$1"
         fi
        ;;

      "-target1decdeg")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing target1 dec deg value"
            exit 1
         else
            target1DecDeg="$1"
         fi
        ;;

      "-target2rahours")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing target2 ra hours value"
            exit 1
         else
            target2RaHours="$1"
         fi
        ;;

      "-target2decdeg")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing target2 dec deg value"
            exit 1
         else
            target2DecDeg="$1"
         fi
        ;;

      "-target3rahours")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing target3 ra hours value"
            exit 1
         else
            target3RaHours="$1"
         fi
        ;;

      "-target3decdeg")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing target3 dec deg value"
            exit 1
         else
            target3DecDeg="$1"
         fi
        ;;

      "${degPerCmArgName}")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing deg per cm value"
            exit 1
         else
            mapScaleDegPerCm="$1"
         fi
        ;;
     
      "${outfileArgName}")
         shift
         if [ "$1" = "" ]
         then
            echo "$progName: missing outfile name"
            exit 1
         else
            outFile="$1"
         fi
        ;;

      "-help")
         help
         exit 1
         ;;

       *)
         echo "$progName: unknown option: $1"
         usage
         exit 1
       ;;
     esac

     shift

   done

}


create_tex_preamble()
{
cat <<EOF > ${mapTexPreamble}
   % --- increase font sizes for better readability
   % --- available sizes:
   % --- \tiny, \scriptsize, \footnotesize, \small, \normalsize, 
   % --- \large, \Large, \LARGE, \huge, \Huge
   
   \usepackage{relsize}

   \renewcommand*{\TextLabel}[1]{\LARGE #1}
   \renewcommand*{\Label}[1]{\small #1}
   \renewcommand*{\TicMark}[1]{\large #1}
   \renewcommand*{\FlexLabel}[1]{\Large #1}
EOF
}

# create map pp3 file input

create_pp3_input_file()
{

cat <<EOF > ${pp3InputFile}

   filename latex_preamble ${mapTexPreamble}

   switch eps_output on
   filename output ${mapWorkingName}.tex

   set fontsize 12

   set center_rectascension $mapCenterRaHours
   set center_declination $mapCenterDecDeg

   set box_width $mapWidthCm
   set box_height $mapHeightCm

   set grad_per_cm $mapScaleDegPerCm

   color background $black
   color grid 0.6 0.6 0.6

   # labels: stars, galaxies, etc
   color labels $deepBlue

   # text_labels: constellation names, user labels
   color text_labels $red

   # constellation boundaries
   color boundaries 0.8 0.8 0.0
   color highlighted_boundaries 0.8 0.8 0.0

   # constellation drawings
   color constellation_lines 0 0.8 0

   switch colored_stars off
   color stars $white
   color nebulae $white
   color ecliptic 0.8 0.1333 0
   switch milky_way off
   color milky_way 0.5 0.5 1

   objects_and_labels

   # set names of common stars
   set_label_text ORI 19 "\\\large Rigel"
   set_label_text ORI 58 "\\\large Betelgeuse"
   set_label_text CMA 9 "\\\large Sirius"
   set_label_text CMI 10 "\\\large Procyon"
   set_label_text AUR 13 "\\\large Capella"
   set_label_text PER 33 "\\\large Mirfak"
   set_label_text PER 26 "\\\large Algol"
   set_label_text TAU 87 "\\\large Aldebaran"
   set_label_text CET 68 "\\\large Mira"
   set_label_text LEO 32 "\\\large Regulus"
   set_label_text LEO 94 "\\\large Denebola"
   set_label_text VIR 67 "\\\large Spica"
   set_label_text BOO 16 "\\\large Arcturus"
   set_label_text UMI 1 "\\\large Polaris"
   set_label_text UMA 50 "\\\large Dubhe"
   set_label_text UMA 77 "\\\large Alioth"
   set_label_text UMA 85 "\\\large Alkaid"
   set_label_text CVN 12 "\\\large Cor Caroli"
   set_label_text CYG 50 "\\\large Deneb"
   set_label_text LYR 3 "\\\large Vega"
   set_label_text AQL 53 "\\\large Altair"
   set_label_text GEM 66 "\\\large Castor"
   set_label_text GEM 78 "\\\large Pollux"
   set_label_text AND 21 "\\\large Sirrah"
   set_label_text PSA 24 "\\\large Fomalhaut"
   set_label_text SCO 21 "\\\large Antares"
   set_label_text HD 128620 "\\\large Toliman"
   set_label_text HD 158427 "\\\large Choo"  
   set_label_text HD 108248 "\\\large Acrux"
   set_label_text HD 45348 "\\\large Canopus"
   set_label_text HD 10144 "\\\large Achernar"
   set_label_text SCO 35 "\\\large Shaula"
   set_label_text ORI 24 "\\\large Bellatrix"

   # define constellation labels
   text "Canes Venatici" at 12.3 35 along declination towards W ;
   text "Canis Major" at 6.3 -25 along declination towards SW ;
   text "Canis Major" at 7.4 -13 along declination towards SE ;
   text "Canis Minor" at 7.4 0.5 along declination towards NW ;
   text "Canis Minor" at 8 9 along declination towards SE ;
   text "Coma Berenices" at 13.3 31 along declination towards SE ;
   text "Coma Berenices" at 13.6 15 along declination ;
   text "Corona Australis" at 18.1 -38 along declination towards SW ;
   text "Corona Australis" at 19.2 -44.5 along declination ;
   text "Corona Borealis" at 16.4 39 along declination towards SE ;
   text "Leo Minor" at 10.05 28.5 along declination towards NW ;
   text "Piscis Austrinus" at 22.3 -35.5 along declination towards N ;
   text "Ursa Major" at 11 50 along declination towards N ;
   text "Ursa Major" at 12 30 along declination ;
   text "Ursa Major" at 12 42 along declination towards E ;
   text "Ursa Major" at 14.4 62 along declination towards SE ;
   text "Ursa Major" at 8.65 55.5 along declination towards W ;
   text "Ursa Major" at 9.4 42 along declination towards NW ;
   text "Ursa Minor" at 13.1 70 along declination towards NW ;
   text Andromeda at 0.5 46 along declination towards S ;
   text Andromeda at 2.05 36.25 along declination ;
   text Andromeda at 2.6 45.5 along declination towards E ;
   text Andromeda at 23.1 36 along declination towards NW ;
   text Antlia at 10 -30 along declination towards N ;
   text Antlia at 11 -40 along declination ;
   text Apus at 14 -71 along declination towards SW ;
   text Apus at 16 -71 along declination towards S ;
   text Apus at 16 -75 along declination towards N ;
   text Apus at 18.1 -68 along declination towards SE ;
   text Aquarius at 20.7 2 along declination towards SW ;
   text Aquarius at 21.7 -2 along declination towards SE ;
   text Aquarius at 22.05 -23.5 along declination towards NW ;
   text Aquarius at 23.9 -23.5 along declination ;
   text Aquarius at 23.9 -4.5 along declination towards SE ;
   text Aquila at 19.05 -10 along declination towards W ;
   text Aquila at 20 14 along declination towards S ;
   text Aquila at 20.6 -7.5 along declination ;
   text Ara at 16.9 -62 along declination towards W ;
   text Ara at 17 -47 along declination towards S ;
   text Ara at 17.7 -67 along declination ;
   text Ara at 18.1 -47 along declination towards SE ;
   text Ara at 18.1 -56 along declination ;
   text Ari at 1.8 25 along declination towards SW ;
   text Aries at 1.85 16 along declination towards W ;
   text Aries at 2.5 20 along declination towards N ;
   text Aries at 3 30.5 along declination towards S ;
   text Aries at 3.3 11.5 along declination ;
   text Auriga at 4.9 29 along declination towards NW ;
   text Auriga at 5 50 along declination towards NW ;
   text Auriga at 6.5 30 along declination towards E ;
   text Auriga at 6.6 53.5 along declination towards SE ;
   text Auriga at 7.4 44 along declination towards SE ;
   text Bootes at 13.7 9 along declination towards NW ;
   text Bootes at 13.95 24 along declination towards W ;
   text Bootes at 14.2 42 along declination towards W ;
   text Bootes at 15.75 42 along declination towards E ;
   text Caelum at 4.9 -46 along declination ;
   text Caelum at 5.05 -30 along declination towards SE ;
   text Cam at 12 77 along declination towards NW ;
   text Cam at 3.4 68.5 along declination towards SW ;
   text Cam at 5.1 53.5 along declination ;
   text Camelopardalis at 14 83 along declination towards SE ;
   text Camelopardalis at 3.3 61 along declination towards W ;
   text Camelopardalis at 8.1 62.5 along declination ;
   text Cancer at 8 15 along declination towards W ;
   text Cancer at 8.2 32 along declination towards SW ;
   text Cancer at 8.5 24 along declination towards S ;
   text Cap at 21.9 -9 along declination towards SE ;
   text Cap at 21.94 -24 along declination ;
   text Capricornus at 20.4 -9 along declination towards S ;
   text Capricornus at 21 -20 along declination towards S ;
   text Capricornus at 21.4 -27 along declination ;
   text Car at 11.3 -69 along declination towards E ;
   text Car at 6.1 -46 along declination towards W ;
   text Car at 6.1 -51.5 along declination towards W ;
   text Carina at 11.2 -69 along declination towards E ;
   text Carina at 6.6 -57.5 along declination towards NW ;
   text Carina at 7 -55 along declination towards W ;
   text Carina at 7 -63.5 along declination towards NW ;
   text Carina at 8.5 -56 along declination towards S ;
   text Carina at 9.1 -75 along declination towards NW ;
   text Cassiopeia at 0 55 along declination towards S ;
   text Cassiopeia at 3.1 60 along declination ;
   text Cen at 11.15 -41 along declination towards W ;
   text Cen at 11.2 -50 along declination towards SW ;
   text Cen at 11.4 -61 along declination towards W ;
   text Cen at 14.6 -56.5 along declination towards SE ;
   text Cen at 15 -40 along declination ;
   text Centaurus at 11.2 -36 along declination towards SW ;
   text Centaurus at 12.7 -46 along declination towards N ;
   text Centaurus at 12.7 -54 along declination towards N ;
   text Centaurus at 14.6 -64 along declination towards NE ;
   text Centaurus at 15 -31 along declination towards SE ;
   text Cep at 22.9 57.5 along declination ;
   text Cepheus at 0.2 69 along declination towards E ;
   text Cepheus at 20.8 56 along declination towards NW ;
   text Cepheus at 5 82 along declination ;
   text Cet at 0 -24 along declination towards NW ;
   text Cetus at 1.5 -1 along declination towards S ;
   text Cetus at 1.7 -24 along declination ;
   text Cetus at 2.2 9 along declination towards SW ;
   text Cetus at 2.7 -6.5 along declination towards E ;
   text Chamaeleon at 10.15 -76 along declination towards S ;
   text Chamaeleon at 11.5 -76 along declination towards SE ;
   text Chamaeleon at 13.8 -76 along declination towards SE ;
   text Chamaeleon at 7.9 -82 along declination towards NW ;
   text Circinus at 13.9 -69 along declination towards W ;
   text Circinus at 14.4 -68 along declination towards N ;
   text Circinus at 14.7 -56 along declination towards SW ;
   text Circinus at 14.85 -70 along declination ;
   text Circinus at 15.45 -56 along declination towards SE ;
   text Columba at 5.1 -30 along declination towards SW ;
   text Columba at 5.1 -42.5 along declination towards NW ;
   text Columba at 6.6 -42 along declination ;
   text Corvus at 12 -12 along declination towards SW ;
   text Corvus at 12.9 -12 along declination towards SE ;
   text Crater at 10.95 -25 along declination towards NW ;
   text Crater at 11.9 -20 along declination ;
   text Crux at 12 -56 along declination towards SW ;
   text Cygnus at 19.3 45 along declination towards NW ;
   text Cygnus at 20 31 along declination towards NW ;
   text Cygnus at 20.4 52 along declination towards S ;
   text Cygnus at 21.9 40 along declination towards E ;
   text Cygnus at 22 55 along declination towards SE ;
   text Del at 20.3 9.5 along declination towards NW ;
   text Del at 20.9 4 along declination ;
   text Del at 21.1 19.5 along declination towards SE ;
   text Delphinus at 20.4 19.5 along declination towards SW ;
   text Delphinus at 20.95 9 along declination towards E ;
   text Dor at 4.5 -49 along declination towards SE ;
   text Dor at 4.7 -69 along declination towards NW ;
   text Dor at 6.4 -67.5 along declination towards E ;
   text Dorado at 4.7 -65 along declination towards W ;
   text Dorado at 6.5 -67.5 along declination towards E ;
   text Draco at 12.3 65 along declination towards NW ;
   text Draco at 15.8 65 along declination towards W ;
   text Draco at 17.1 52 along declination ;
   text Draco at 18.4 50 along declination towards NW ;
   text Draco at 20.5 75 along declination towards SE ;
   text Draco at 9.5 74 along declination towards NW ;
   text Equ at 21 3.5 along declination towards NW ;
   text Equuleus at 21 12 along declination towards SW ;
   text Eri at 1.5 -55 along declination towards W ;
   text Eri at 2.2 -55 along declination towards E ;
   text Eridanus at 2.8 -2 along declination towards SW ;
   text Eridanus at 3.4 -45 along declination ;
   text Eridanus at 4 -20 along declination ;
   text Eridanus at 4.3 -39 along declination ;
   text Eridanus at 5.1 -10 along declination ;
   text Fornax at 1.8 -36 along declination towards W ;
   text Fornax at 2 -25 along declination towards SW ;
   text Fornax at 3.5 -38 along declination ;
   text Gemini at 6.1 27 along declination towards W ;
   text Gemini at 6.45 15 along declination towards W ;
   text Gemini at 7 30 along declination towards N ;
   text Gemini at 7.9 14 along declination ;
   text Gemini at 8.1 32 along declination towards SE ;
   text Grus at 21.5 -37 along declination towards SW ;
   text Grus at 21.6 -49 along declination towards NW ;
   text Grus at 22.2 -56 along declination towards NW ;
   text Grus at 23.4 -37 along declination towards SE ;
   text Her at 15.85 49 along declination towards NW ;
   text Hercules at 18.1 48 along declination towards SE ;
   text Hercules at 15.9 40.5 along declination towards NW ;
   text Hercules at 17.5 20 along declination towards N ;
   text Hercules at 18.85 25 along declination towards SE ;
   text Hercules at 18.9 13.5 along declination ;
   text Hor at 2.35 -65 along declination towards W ;
   text Hor at 4.2 -48 along declination ;
   text Horologium at 2.3 -54 along declination towards SW ;
   text Horologium at 3.5 -56 along declination ;
   text Horologium at 4.3 -46 along declination ;
   text Hya at 8.25 6 along declination towards SW ;
   text Hydra at 10 -20 along declination towards S ;
   text Hydra at 10.9 -22 along declination towards E ;
   text Hydra at 12.2 -26 along declination towards S ;
   text Hydra at 13 -29 along declination towards NW ;
   text Hydra at 14.7 -29.5 along declination towards N ;
   text Hydra at 8.5 -17 along declination towards NW ;
   text Hydra at 9.2 -24 along declination towards NW ;
   text Hydra at 9.65 3 along declination towards E ;
   text Hydrus at 1.45 -59 along declination towards SW ;
   text Hydrus at 1.45 -70 along declination towards W ;
   text Hydrus at 3.3 -81.5 along declination ;
   text Hydrus at 3.8 -67.5 along declination towards S ;
   text Hyi at 4.5 -68 along declination towards SE ;
   text Ind at 20.55 -56 along declination towards W ;
   text Ind at 23.4 -73.7 along declination ;
   text Indus at 20.5 -45.5 along declination towards SW ;
   text Indus at 21.5 -58.5 along declination towards N ;
   text Indus at 22 -50 along declination towards SE ;
   text Indus at 22.1 -60 along declination towards SE ;
   text Indus at 22.5 -74 along declination towards N ;
   text Lacerta at 22.9 36 along declination ;
   text Lacerta at 22.9 56 along declination towards SE ;
   text Leo at 10.2 7 along declination towards N ;
   text Leo at 10.55 27 along declination towards SE ;
   text Leo at 11 0 along declination towards W ;
   text Leo at 11.6 -6 along declination ;
   text Leo at 11.9 17 along declination towards E ;
   text Leo at 11.9 27  along declination towards SE ;
   text Leo at 9.5 18 along declination towards W ;
   text Leo at 9.9 31 along declination towards SE ;
   text Lepus at 5.5 -12 along declination towards S ;
   text Lepus at 6.1 -26 along declination ;
   text Libra at 14.4 -24.5 along declination towards NW ;
   text Libra at 14.5 -9 along declination towards SW ;
   text Libra at 15.1 -29 along declination towards NW ;
   text Libra at 16 -19 along declination ;
   text Lupus at 15.1 -54.5 along declination ;
   text Lupus at 15.1 -55 along declination ;
   text Lupus at 15.8 -48 along declination ;
   text Lupus at 16.1 -35 along declination towards E ;
   text Lupus at 16.2 31 along declination towards SE ;
   text Lynx at 6.8 50.5 along declination towards NW ;
   text Lynx at 7.9 34 along declination towards NW ;
   text Lynx at 8 41 along declination towards N ;
   text Lynx at 8.5 59 along declination towards SE ;
   text Lynx at 9 34 along declination towards N ;
   text Lynx at 9.3 46 along declination towards SE ;
   text Lyra at 18.35 47 along declination towards SW ;
   text Lyra at 19.4 34 along declination towards E ;
   text Mensa at 3.7 -79 along declination towards W ;
   text Mensa at 4.65 -71 along declination towards SW ;
   text Mensa at 7.4 -76 along declination towards SE ;
   text Mensa at 7.5 -76 along declination towards SE ;
   text Mic at 21.4 -28 along declination towards SE ;
   text Mic at 21.4 -44 along declination ;
   text Microscopium at 20.5 -28 along declination towards SW ;
   text Monoceros at 6 -10 along declination  towards NW ;
   text Monoceros at 7 11 along declination towards SE ;
   text Monoceros at 8.15 -10.5 along declination ;
   text Musca at 11.4 -70 along declination towards NW ;
   text Musca at 11.4 -71 along declination towards W ;
   text Musca at 11.4 -75 along declination towards NW ;
   text Musca at 13.8 -68.5 along declination towards E ;
   text Musca at 13.8 -73 along declination towards E ;
   text Norma at 15.6 -56 along declination towards W ;
   text Norma at 16 -50 along declination towards S ;
   text Norma at 16.5 -43 along declination towards SE ;
   text Norma at 16.5 -57 along declination towards E ;
   text Norma at 16.5 -59.5 along declination ;
   text Octans at 0 -74.5 along declination towards SE ;
   text Octans at 18.4 -75.5 along declination towards SW ;
   text Oph at 16.05 -7.5 along declination towards NW ;
   text Oph at 17.7 -25.5 along declination towards E ;
   text Ophiuchus at 16.6 -20 along declination towards NW ;
   text Ophiuchus at 17.3 1 along declination towards N ;
   text Ophiuchus at 18 10 along declination towards NW ;
   text Ori at 6 22.5 along declination towards SE ;
   text Orion at 4.8 -3 along declination towards NW ;
   text Orion at 4.8 15 along declination towards SW ;
   text Orion at 5.25 -10.05 along declination towards NW ;
   text Orion at 6.3 -3.5 along declination towards NE ;
   text Orion at 6.35 13 along declination towards E ;
   text Pavo at 17.8 -57.5 along declination towards SW ;
   text Pavo at 18.5 -58 along declination towards S ;
   text Pavo at 19 -74 along declination towards N ;
   text Pavo at 21 -74 along declination ;
   text Pavo at 21.4 -60 along declination towards SE ;
   text Pavo at 21.4 -63 along declination towards E ;
   text Peg at 21.2 19.5 along declination towards SW ;
   text Pegasus at 0.05 31 along declination towards SE ;
   text Pegasus at 21.2 13.5 along declination towards NW ;
   text Pegasus at 22.5 35 along declination towards S ;
   text Pegasus at 23.55 34.5 along declination towards SE ;
   text Pegasus at 23.8 10 along declination ;
   text Perseus at 2.4 52 along declination towards N ;
   text Perseus at 3 32 along declination towards NW ;
   text Perseus at 4.6 31.5 along declination ;
   text Perseus at 4.7 51 along declination ;
   text Phe at 2.3 -47 along declination ;
   text Phoenix at 1.4 -57 along declination ;
   text Phoenix at 2.2 -40 along declination towards SE ;
   text Phoenix at 23.5 -57 along declination towards NW ;
   text Pic at 6.8 -63.5 along declination ;
   text Pictor at 4.6 -47 along declination towards SW ;
   text Pictor at 4.6 -53 along declination towards NW ;
   text Pictor at 5 -43.5 along declination towards SW ;
   text Pictor at 5. -43.5 along declination towards SW ;
   text Pictor at 5.6 -60 along declination towards NW ;
   text Pictor at 6 -49 along declination ;
   text Pictor at 6.8 -63.5 along declination ;
   text Pisces at 0 5 along declination towards S ;
   text Pisces at 1 10 along declination towards N ;
   text Pisces at 1.4 3.5 along declination towards N ;
   text Pisces at 1.7 28 along declination towards SE ;
   text Pisces at 2.05 10 along declination towards SE ;
   text Pisces at 23 -1 along declination towards NW ;
   text PsA at 21.5 -25.5 along declination towards SW ;
   text PsA at 23.1 -26 along declination towards SE ;
   text Pup at 6.1 -50 along declination towards NW ;
   text Puppis at 6.1 -47 along declination towards W ;
   text Puppis at 6.2 -43.5 along declination towards SW ;
   text Puppis at 7.4 -50 along declination towards N ;
   text Puppis at 7.5 -11.5 along declination towards SW ;
   text Puppis at 8.4 -28 along declination towards E ;
   text Puppis at 8.4 -40.5 along declination towards SE ;
   text Pyxis at 8.5 -25 along declination towards W ;
   text Pyxis at 9.1 -20 along declination towards SE ;
   text Ret at 3.4 -58 along declination towards SW ;
   text Ret at 4.3 -57 along declination towards SE ;
   text Reticulum at 4.5 -66.5 along declination ;
   text Reticulum at 4.6 -66.5 along declination ;
   text Sagitta at 19.1 18 along declination towards W ;
   text Sagitta at 20.3 18 along declination towards E ;
   text Sagittarius at 19 -17 along declination towards SE ;
   text Sagittarius at 19 -36 along declination towards N ;
   text Sagittarius at 19.5 -12.5 along declination towards S ;
   text Sagittarius at 20 -44.5 along declination towards N ;
   text Scl at 23.2 -35.5 along declination towards NW ;
   text Scorpius at 15.85 -29 along declination towards NW ;
   text Scorpius at 16.2 -32 along declination towards W ;
   text Scorpius at 16.6 -45 along declination towards NW ;
   text Scorpius at 17.45 -34 along declination towards S ;
   text Sculptor at 1.6 -26 along declination towards SE ;
   text Sculptor at 1.7 -35 along declination towards E ;
   text Sculptor at 23.2 -26 along declination towards SW ;
   text Scutum at 18.95 -15 along declination ;
   text Serpens at 16.1 10 along declination towards E ;
   text Ser at 18.25 -10 along declination towards N ;
   text Serpens at 15.25 25 along declination towards SW ;
   text Serpens at 17.3 -14 along declination towards W ;
   text Serpens at 18 -15 along declination towards N ;
   text Serpens at 18.67 -3.5 along declination ;
   text Serpens at 18.9 6 along declination towards SE ;
   text Sextans at 10.3 -3 along declination towards S ;
   text Sgr at 17.75 -27 along declination towards W ;
   text Sgr at 20.1 -18 along declination towards E ;
   text Tau at 5.9 27 along declination towards SE ;
   text Taurus at 3.6 30 along declination towards SW ;
   text Taurus at 4.4 0.8 along declination ;
   text Taurus at 4.8 29 along declination towards SE ;
   text Taurus at 5.3 17 along declination towards N ;
   text Tel at 18.3 -51 along declination towards W ;
   text Tel at 20.4 -51 along declination towards E ;
   text Tel at 20.45 -45.5 along declination towards SE ;
   text Telescopium at 18.3 -56 along declination towards NW ;
   text Telescopium at 19.2 -46 along declination towards S ;
   text TrA at 16.1 -61 along declination towards S ;
   text TrA at 16.5 -61 along declination towards SE ;
   text TrA at 16.9 -64 along declination towards SE ;
   text Tri at 1.6 35 along declination towards SW ;
   text Triangulum at 2.5 28.5 along declination ;
   text Tuc at 22.2 -66.5 along declination towards NW ;
   text Tucana at 1.3 -58 along declination towards SE ;
   text Tucana at 22.2 -66.5 along declination towards NW ;
   text Tucana at 23.4 -56.5 along declination towards SE ;
   text Tucana at 23.5 -69 along declination towards W ;
   text UMa at 14.1 49.5 along declination towards SE ;
   text UMa at 8.3 72 along declination towards SW ;
   text Vela at 10 -41 along declination towards S ;
   text Vela at 10 -50 along declination towards N ;
   text Vela at 8.5 -41 along declination towards W ;
   text Vela at 8.95 -56.5 along declination towards NW ;
   text Virgo at 11.65 10 along declination towards SW ;
   text Virgo at 11.7 -6 along declination towards NW ;
   text Virgo at 13 -20 along declination towards W ;
   text Virgo at 13 14 along declination towards SW ;
   text Virgo at 13.5 5 along declination towards S ;
   text Virgo at 14.7 -7.5 along declination ;
   text Vol at 6.7 -64.5 along declination towards SW ;
   text Vol at 6.7 -65 along declination towards SW ;
   text Volans at 6.7 -65 along declination towards SW ;
   text Volans at 8.8 -75 along declination ;
   text Vulpecula at 19.6 21 along declination towards N ;
   text Vulpecula at 20 28.5 along declination towards SW ;
   text Vulpecula at 21 20.5 along declination towards N ;

   # reset text color
   text "" at 0 0 color 1 1 1;
 
EOF

}

add_grid_labels()
{
   # grid labels
   # RA

   echo "# RA grid labels" >> ${pp3InputFile}
   raIntervalHours=1
   decMinDeg=-35
   decMaxDeg=90
   decStepDeg=20
   decDeg=${decMinDeg}
   while [ ${decDeg} -lt ${decMaxDeg} ]
   do
      echo "text \"\$#3\$h\" at 0 $decDeg along declination" >> ${pp3InputFile}
      echo "   tics rectascension ${raIntervalHours} towards E ;" >> ${pp3InputFile}
      decDeg=`expr ${decDeg} + ${decStepDeg}`
   done

   # dec labels
   echo "# Dec grid labels" >> ${pp3InputFile}
   decIntervalDeg=10
   raMinHours=0
   raMaxHours=24
   raStepHours=2
   raHours=${raMinHours}
   while [ ${raHours} -lt ${raMaxHours} ]
   do
      echo "text \"\$#5\$d\" at ${raHours}.5 0 along declination" >> ${pp3InputFile}
      echo "   tics declination ${decIntervalDeg} towards S ;" >> ${pp3InputFile}
      raHours=`expr ${raHours} + ${raStepHours}`
   done

}

# Use bc to perform a calculation, returning the result
calculate()
{
   formula=$*
   numDecimalDigits=5

   echo "scale = ${numDecimalDigits}; ${formula}" | bc
}

draw_crosshair_lines()
{
   raHours=$1
   decDeg=$2
   lineWidth=$3
   length=$4

   # crosshair lines
   echo "text \psline[linewidth=${lineWidth}](-${length},0) at $raHours $decDeg color $deepBlue;" >>  ${pp3InputFile}

   echo "text \psline[linewidth=${lineWidth}](0,${length}) at $raHours $decDeg color $deepBlue;" >>  ${pp3InputFile}

   echo "text \psline[linewidth=${lineWidth}](${length},0) at $raHours $decDeg color $deepBlue;" >>  ${pp3InputFile}

   echo "text \psline[linewidth=${lineWidth}](0,-${length}) at $raHours $decDeg color $deepBlue;" >>  ${pp3InputFile}

}


add_ata_primary_beam_fancy_crosshair()
{
   #draw crosshair at map center
   lineWidthCm="0.06"
   lengthCm="1.5"

   draw_crosshair_lines $mapCenterRaHours $mapCenterDecDeg $lineWidthCm $lengthCm

   # crosshair circles
   outerCircleRadius=`calculate ${length}*0.8`

   echo "text \pscircle[linestyle=solid]{${outerCircleRadius}} at $mapCenterRaHours $mapCenterDecDeg color $deepBlue; " >> ${pp3InputFile}

   innerCircleRadius=`calculate ${length}*0.33`

   echo "text \pscircle[linestyle=solid]{${innerCircleRadius}} at $mapCenterRaHours $mapCenterDecDeg color $deepBlue; " >> ${pp3InputFile}

}

add_ata_primary_beam_simple_crosshair()
{
   lineWidthCm="0.06"
   lengthCm="0.2"

   draw_crosshair_lines $mapCenterRaHours $mapCenterDecDeg $lineWidthCm $lengthCm
}

draw_circle()
{
   centerRaHours=$1
   centerDecDeg=$2
   diamDeg=$3

   # Convert to cm
   radiusDeg=`calculate ${diamDeg}/2.0`
   radiusCm=`calculate ${radiusDeg}/${mapScaleDegPerCm}`

   echo "text \pscircle[linestyle=solid]{${radiusCm}}
 at $centerRaHours $centerDecDeg color $deepBlue; " >> ${pp3InputFile}

}

# write target name and draw circle around it
add_target()
{
   raHours=$1
   decDeg=$2
   name=$3

   echo "text \"$name\" at $raHours $decDeg color $red ;" >> ${pp3InputFile}

   # Estimate size of target (synthesized) beam.
   # Primary-to-synthesized beam ratio should be about 32/1 for ata42, 
   # and 115/1 for ATA350.

   primaryToTargetBeamsizeRatio="32"
   targetBeamDiamDeg=`calculate ${primaryBeamsizeDeg}/${primaryToTargetBeamsizeRatio}`
   draw_circle $raHours $decDeg $targetBeamDiamDeg
}

add_target_indicators()
{
    add_target $target1RaHours $target1DecDeg $target1Name
    add_target $target2RaHours $target2DecDeg $target2Name
    add_target $target3RaHours $target3DecDeg $target3Name
}

add_ata_primary_beam_circle()
{
   # Draw circle showing ATA primary beam
   # Beamsize: 3.5 deg diameter at 1 GHZ

   draw_circle $mapCenterRaHours $mapCenterDecDeg $primaryBeamsizeDeg
}


cleanup() 
{
   rm -fr $tmpdir
}

#----------------------------

# cleanup if ctrl-c is issued, or the script exits with an error
trap cleanup INT QUIT ABRT ILL HUP KILL ALRM TERM

setGlobalVars

parseArgs $@

mkdir $tmpdir
cd $tmpdir

create_tex_preamble

create_pp3_input_file

add_grid_labels

if [ $displayFancyCrosshair = "true" ]
then
   add_ata_primary_beam_fancy_crosshair
fi

if [ $showPrimaryBeam = "true" ]
then
    add_ata_primary_beam_circle

    add_ata_primary_beam_simple_crosshair
fi


add_target_indicators

pp3OutputFile=${mapWorkingName}-pp3-output.txt

pp3 ${pp3InputFile} > ${pp3OutputFile} 2>&1
status=$?

if [ $status = 1 ]
then
   # error
   cat ${pp3OutputFile}
fi

# use ImageMagick for format conversion
convert ${mapWorkingName}.eps ${mapWorkingName}.gif

# go back to working dir, just in case outfile is not a full path
cd $pwd

cp ${tmpdir}/${mapWorkingName}.gif $outFile

cleanup