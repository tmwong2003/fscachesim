#!/usr/bin/perl -w
#
# RCS:         $Header: $
# Description: Wrapper script for fscachesim
# Author:      T.M. Wong <tmwong+@cs.cmu.edu>

# Usage:
#
# fscachesim-run.pl TRACESET CLIENTSIZE ARRAYSIZE ARRAYTYPES...
#
# where:
#
# TRACESET:   One of the trace sets from the [Wong2002]. Valid sets:
#             db2 httpd openmail
# CLIENTSIZE: The size of the simulated client caches, in MB.
# ARRAYSIZE:  The size of the simulated array cache, in MB.
# ARRAYTYPES: A space-separated list of arraytypes to simulate. The script will
#             run fscachesim and create a separate result file for each array
#             type. Valid types:
#             LRU MRULRU NSEGEXP NSEGUNI RSEGEXP RSEGUNI
#
# Warnings:
#
# The script does not any input parameter validation.
#
# The script always simulates demoting clients for all array types except LRU.

use strict;

use "fscachesim-config.pl";

my (%demoteTable) =
  (
   "LRU", "",
   "MRULRU", "-d",
   "NSEGEXP", "-d",
   "NSEGUNI", "-d",
   "RSEGEXP", "-d",
   "RSEGUNI", "-d"
  );

my (%trTypeTable) =
  (
   "db2", "-m",
   "httpd", "-m",
   "httpd-test", "-m",
   "openmail", ""
  );

my (%trFilesTable) =
  (
   "db2", "db2.server.0.trace db2.server.1.trace db2.server.2.trace db2.server.3.trace db2.server.4.trace db2.server.5.trace db2.server.6.trace db2.server.7.trace",
   "httpd", "httpd.server.1.trace httpd.server.2.trace httpd.server.3.trace httpd.server.4.trace httpd.server.5.trace httpd.server.6.trace httpd.server.7.trace",
   "httpd-test", "httpd.server.1.trace",
   "openmail", "i3125om1.fscachesim i3125om2.fscachesim i3125om3.fscachesim i3125om4.fscachesim i3125om5.fscachesim i3125om6.fscachesim"
  );

sub runSim {
  my ($sim, $clientSize, $arraySize, @arrayTypes) = @_;

  my ($trFiles) = trFilesGet($sim, $configTracePathTable{$sim});
  my ($trType) = $trTypeTable{$sim};

  my ($arrayType);
  foreach $arrayType (@arrayTypes) {
    my ($demoteFlag) = $demoteTable{$arrayType};

    my ($cmdline) =
      "$configBin " .
      "$demoteFlag " .
      "$trType " .
      "-o $configResultPath/$sim " .
      "$arrayType " .
      "$clientSize $arraySize " .
      "$trFiles";

    print "$cmdline\n";
    `$cmdline`;
  }
}

sub trFilesGet{
  my ($sim, $trPath) = @_;
  my ($trFiles) = "";
  my ($i);

  foreach $i (split(/\s/,$trFilesTable{$sim})) {
    $trFiles .= "$trPath/$i ";
  }

  return ($trFiles);
}

runSim(@ARGV);
