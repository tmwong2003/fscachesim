# RCS:         $Header: $
# Description: Template file for fscachesim-run.pl configuration file.
# Author:      T.M. Wong <tmwong+@cs.cmu.edu>

# Fully-qualified fscachesim binary name.

my ($configBin) = "/path/fscachesim";

# Path to trace files, for each trace set.

my (%configTracePathTable) =
  (
   "db2", "/path/db2",
   "httpd", "/path/httpd",
   "openmail", "/path/openmail"
  );

# Path into which to dump result files. Results file names have the form:
#
# $configResultsPath/traceSet-DEMOTETYPE}-{ARRAYTYPE}-{CLIENTSIZE}-{ARRAYSIZE}
#
# where
#
# DEMOTETYPE : NONE, DEMOTE
# ARRAYTYPE : LRU, MRULRU, NSEGEXP, NSEGUNI, RSEGEXP, RSEGUNI
# CLIENTSIZE : (in MB)
# ARRAYSIZE : (in MB)

my ($configResultPath) = "/path/results";
