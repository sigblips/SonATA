
Mysql database files for SETI targets which are
compatible with the TargetCat table of seeker.schema.

Data sources:

1) The file seeker.nearest.load-data is included in this
directory. It is fairly small.

2) Other catalogs are available via download from the internet.
   The file is approximately 54MB and must be ungzipped and
   untarred.

   wget http://setiquest.org/sonata_files/catalogs_1.0.tar.gz
   tar -xzvf catalogs_1.0.tar.gz

File formats:

1. Files ending in .load-data should be loaded with
   the Mysql 'load data local infile' command, e.g.:
  
mysql -h <host> <dbname> -e "load data local infile '<file>.load-data' into 
table TargetCat fields terminated by ',' ignore 2 lines; show warnings;"


2. Files ending in .data can be loaded directly, e.g.:

    mysql -h <host> <dbname> < <file>.data

3. Data for the calibration source fluxes is primarily from the
VLA calibrators catalog: 

http://www.vla.nrao.edu/astro/calib/manual/csource.html

Sources that are not recommended for a particular frequency are
set to zero in the data files here.

================================
 mysql --local-infile=1 -h mozart sonatadb --show-warnings -e "load data local infile 'seeker.habcat.load-data' into table TargetCat fields terminated by ',' ignore 2 lines;"


