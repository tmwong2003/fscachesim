#!/usr/bin/perl -w
#
# RCS:         $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/scripts/cumul2jgraph.pl,v 1.2 2000/10/28 22:22:17 tmwong Exp $
# Description: 
# Author:      T.M. Wong <tmwong+@cs.cmu.edu>

$arraySize = 32768;
$arraySizeMB = $arraySize * 4 / 1024;

for ($i = 1; $i < 8; $i++) {
    $probSize = $arraySize * $i / 8;
    $probSizeMB = $probSize * 4 / 1024;
    $protSizeMB = $arraySizeMB - $probSizeMB;

    $filename = "httpd-LRU-LRU-SLRU-16-$probSizeMB-$protSizeMB";

    $cmdline = "./fscachesim -m -s $probSize mambo/httpd/httpd.server.* | tee $filename";

    print "$cmdline\n";
    `$cmdline`;
}
