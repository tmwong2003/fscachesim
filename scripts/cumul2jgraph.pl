#!/usr/bin/perl -w
#
# RCS:         $Header: $
# Description: Convert a raw LRU cumulative file to a .jgraph file.
# Author:      T.M. Wong <tmwong+@cs.cmu.edu>

use strict;
use vars qw($glblProgname $glblSRT2txtOptions $glblSectorSize $opt_d);
use Getopt::Std;

$glblProgname = $0;

sub usage {
  print "$glblProgname: $glblProgname workload sectors LRU-cumul_filename\n";
  exit(0);
}

if ($#ARGV < 2) {
  usage();
}

open(CUMULFILE, "<$ARGV[2]")
  or die("%glblProgname: $1");

print <<EOF;
newgraph
  legend defaults font Helvetica
  title y 1.1 font Helvetica : Cumulative hit fraction vs. LRU stack depth - $ARGV[0]

xaxis
  hash_labels font Helvetica
  label font Helvetica : Stack depth ($ARGV[1])

yaxis
  hash_labels font Helvetica
  label font Helvetica : Cumulative fraction of accesses
  max 1.0

(* the cumulative curve *)

newcurve
  linetype solid
  marktype none
  pts
EOF

while (<CUMULFILE>) {
  print $_;
}

close(CUMULFILE)
  or die("%glblProgname: $1");
