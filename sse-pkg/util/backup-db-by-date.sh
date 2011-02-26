#!/bin/sh
################################################################################
#
# File:    backup-db-by-date.sh
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


# Backup the data from one or all of the databases on a mysql
# server which have tables containing a timestamp column 'ts'
# that matches a given date (default is 'yesterday').
# Each database will have a separate backup file in the output dir,
# with a filename like: '<dbname>.YYYY-MM-DD.mysql-db-backup.tar.gz'
# Output dir must already exist.

# Inside the .tar.gz file, each individual database table is stored
# in a file with a name like:
#   <columnName>.YYYY-MM-DD.txt
# The file format is compatible with the 'mysqlimport' program
# and/or the mysql 'load data local infile' command.
# The first line of each output file has the column headers in it.
# NULL values are stored as \N.

# Example table output file:
# FarCities.2007-05-22.txt
# id  ts                     name        population
# 1   2007-03-20 00:19:16    Tokyo       4000
# 2   2007-03-20 00:19:16    Dallas      5000
# 3   2007-03-20 00:19:16    Green Bay   \N

# To load these files back into a database:
# mysqlimport --local --ignore-lines=1 -h <dbhost> <dbName> <files...>
# or
# mysql> load data local infile <file1> into table <table1> ignore 1 lines;
# mysql> load data local infile <file2> into table <table2> ignore 1 lines;
# ...

# default values
dbHost=""
dbNameRequested=""
backupDateRequested="yesterday"
outDir="."

# command line arg names
dbHostArgName="-dbhost"
dbNameArgName="-dbname"
backupDateArgName="-date"
outDirArgName="-outdir"
helpArgName="-help"

timeCol='ts'

usage()
{

   echo "usage: $0 $dbHostArgName <database host> [$dbNameArgName <database name> (default=all db)]  [$backupDateArgName <YYYY-MM-DD, default=yesterday>]  [$outDirArgName <dir> (default=./)] "
  echo "Backup one days worth of data from one or more databases."
  echo "Output is one file per table bundled into a single gzipped tar file per database."

}

# process command line args
while [ "$1" ]
do
    if [ "$1" = $dbHostArgName ]
    then
        if [ $# -lt 2 ]
        then
            echo "missing argument for $dbHostArgName"
            exit 1
        fi

        shift
        dbHost="$1"

    elif [ "$1" = $dbNameArgName ]
    then
        if [ $# -lt 2 ]
        then
            echo "missing argument for $dbNameArgName"
            exit 1
        fi
        shift
        dbNameRequested="$1"
    elif [ "$1" = $backupDateArgName ]
    then
        if [ $# -lt 2 ]
        then
            echo "missing argument for $backupDateArgName"
            exit 1
        fi

        shift
        backupDateRequested="$1"
    elif [ "$1" = $outDirArgName ]
    then
        if [ $# -lt 2 ]
        then
            echo "missing argument for $outDirArgName"
            exit 1
        fi

        shift
        outDir="$1"
    elif [ "$1" = $helpArgName ]
    then
        usage
        exit
    else
        echo "Invalid argument: $1"
        usage
        exit
    fi
    shift
done

# verify arguments

if [ "$dbHost" = "" ]
then
    echo "$0 error: must give mysql hostname"
    usage
    exit
fi

if [ ! -d "$outDir" ]
then
    echo "$0 Error: output dir '$outDir' does not exist"
    exit
fi

# Use GNU date to validate the iso8601 date format (YYYY-MM-DD)
backupDate=`date -d "${backupDateRequested}" +'%Y-%m-%d'`
if [ "$?" -ne "0" ]
then
   echo "$0: invalid backup date: ${backupDateRequested}, must be YYYY-MM-DD or valid GNU date format"
   exit
fi


# set up defaults

if [ "$dbNameRequested" = "" ]
then

   # all databases
   # look up all the databases in the server that have at least one 
   # table with a $timeCol (time) column:

   dbList=`mysql -h ${dbHost} --skip-column-names \
      -e "select distinct table_schema from columns where column_name='$timeCol'" \
      information_schema`
   #echo "dbList: $dbList"

   if [ "$dbList" = "" ]
   then
       echo "$0: no databases found that have tables with a '$timeCol' column"
       exit
   fi

else

   dbList="$dbNameRequested"
fi

tmpDir="/tmp/db-dump-backup.${USER}.$$"

# snapshot data for each db
for dbName in $dbList
do

   outFile="${outDir}/${dbName}.${backupDate}.mysql-db-backup"

   #echo "dbName: $dbName"
   #echo "dbHost: $dbHost"
   #echo "backupDate: $backupDate"
   #echo "outFile: $outFile"

   # look up all the tables in the database that have a '$timeCol' column
   tables=`mysql -h ${dbHost} --skip-column-names -e \
      "select table_name from columns where table_schema='$dbName' \
      and column_name = '$timeCol'" information_schema`

   if [ "$tables" = "" ]
   then
       echo "$0: database '$dbName' has no tables with a '$timeCol' column, no data extracted"
       continue
   fi

   mkdir $tmpDir

   for table in $tables 
   do
      # Dump data from specified day.
      # Convert NULL to \N so that it gets read back properly in the
      # mysqlimport program and 'load data local infile' mysql command

      #echo $table

      tableOutfile="${tmpDir}/${table}.${backupDate}.txt"

      mysql -h ${dbHost} -e "select * from ${table} \
         where $timeCol >= '$backupDate' and \
         $timeCol <= '$backupDate 23:59:59'" \
         ${dbName} | sed -e "s/NULL/\\\N/g" > ${tableOutfile}

   done

   tableFiles=`cd $tmpDir; ls *.txt`

   # bundle them up
   outFile="${outFile}.tar"
   tar cf ${outFile} -C ${tmpDir} ${tableFiles}
   gzip -f ${outFile}

   # cleanup
   rm -fr ${tmpDir}

done