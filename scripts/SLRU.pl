#!/usr/bin/perl -w
#
# RCS:         $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/scripts/SLRU.pl,v 1.2 2001/07/06 01:41:47 tmwong Exp $
# Description: 
# Author:      T.M. Wong <tmwong+@cs.cmu.edu>

sub runSim {
    my ($arraySizeMB, $traceFamily) = @_;

    $arraySize = $arraySizeMB * 1024 / 4;

    for ($i = 1; $i < 8; $i++) {
	$probSize = $arraySize * $i / 8;
	$probSizeMB = $probSize * 4 / 1024;
	$protSizeMB = $arraySizeMB - $probSizeMB;

	$clientSizeMB = $arraySizeMB / 8;

	$filename = "$traceFamily-LRU-SLRU-NONE-$clientSizeMB-$probSizeMB-$protSizeMB";

	$cmdline = "./fscachesim -m -s $probSize mambo/$traceFamily/$traceFamily.server.* | tee $filename";

	print "$cmdline\n";
	`$cmdline`;
    }
}

runSim($ARGV[0], $ARGV[1]);
