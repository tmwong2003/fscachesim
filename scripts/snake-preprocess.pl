#!/usr/bin/perl -w
#
# RCS:         $Header: $
# Description: Preprocessing script to convert snake SRT-formatted files into 
#              fscachesim files.
# Author:      T.M. Wong <tmwong+@cs.cmu.edu>

use strict;
use vars qw($glblProgname $glblSRT2txtOptions $glblSectorSize $opt_d);
use Getopt::Std;

$glblProgname = $0;

$glblSRT2txtOptions = "-h -n -p \"%t %d %s %l\" -s 'operation = read'";
$glblSectorSize = 256;

sub usage {
  print "$glblProgname: $glblProgname [-d day] SRT_filename\n";
  exit(0);
}

if (!getopts('d:')) {
  usage();
}

if (!defined($opt_d)) {
  $opt_d = 0;
}

if ($#ARGV < 0) {
  usage();
}

open(SRTFILE, "SRT2txt $glblSRT2txtOptions $ARGV[0] |")
  or die("%glblProgname: $1");

while (<SRTFILE>) {
  my (@line) = split;
  $line[2] =~ s/^0+//;
  my ($devID) = hex($line[2]); # dev ID is in hex

  # Offset value is 1/2 the physical offset; also, offset value is a sector
  # offset, and not a byte offset.

  my ($offset) = $line[3] * 2 * $glblSectorSize;

  # Convert the date format into the number of seconds since the start of the
  # run. The time from SRT2txt is "days hours:minutes:seconds".

  my (@time) = split(/:/, $line[1]);
  my ($timeInSeconds) = ((($opt_d*24 + $time[0])*60 + $time[1])*60 + $time[2]);

  print "$timeInSeconds $devID $offset $line[4]\n";
}

close(SRTFILE)
  or die("%glblProgname: $1");
