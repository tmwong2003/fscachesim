#!/usr/bin/perl -w
#
# RCS:         $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/scripts/SLRU.pl,v 1.3 2001/07/06 03:46:14 tmwong Exp $
# Description: 
# Author:      T.M. Wong <tmwong+@cs.cmu.edu>

use strict;

sub runSim {
    my ($arraySize, $traces) = @_;
    my ($clientSize) = $arraySize / 8;
    my ($filename, $cmdline);

    # Baseline.

    $filename = "$traces-LRU-LRU-NONE-$clientSize-$arraySize";
    $cmdline = "./fscachesim -m $clientSize $arraySize mambo/$traces/$traces.server.* | tee $filename";

    print "$cmdline\n";
    `$cmdline`;

    # Prototype.

    $filename = "$traces-LRU-MRU-LRU-$clientSize-$arraySize";
    $cmdline = "./fscachesim -d -m $clientSize $arraySize mambo/$traces/$traces.server.* | tee $filename";

    print "$cmdline\n";
    `$cmdline`;

    # Run the SLRU-* tests.

    my ($i);
    for ($i = 1; $i < 8; $i++) {
	my($probSize) = $arraySize * $i / 8;
	my($protSize) = $arraySize - $probSize;

	# First, SLRU-NONE

	$filename = "$traces-LRU-SLRU-NONE-$clientSize-$probSize-$protSize";
	$cmdline = "./fscachesim -m -s $probSize $clientSize $arraySize mambo/$traces/$traces.server.* | tee $filename";

	print "$cmdline\n";
	`$cmdline`;

	# Then, SLRU-SLRU

	$filename = "$traces-LRU-SLRU-SLRU-$clientSize-$probSize-$protSize";
	$cmdline = "./fscachesim -d -m -s $probSize $clientSize $arraySize mambo/$traces/$traces.server.* | tee $filename";

	print "$cmdline\n";
	`$cmdline`;
    }
}

runSim($ARGV[0], $ARGV[1]);
