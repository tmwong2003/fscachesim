#!/usr/bin/perl -w
#
# RCS:         $Header: $
# Description: Wrapper script for preprocessing a batch of snake SRT traces.
# Author:      T.M. Wong <tmwong+@cs.cmu.edu>

@snakeFiles = ("920523.srt.Z", "920524.srt.Z", "920525.srt.Z", "920526.srt.Z",
	       "920527.srt.Z", "920528.srt.Z", "920529.srt.Z",
	       "920530.srt.Z", "920531.srt.Z", "920601.srt.Z", "920602.srt.Z",
	       "920603.srt.Z", "920604.srt.Z", "920605.srt.Z");

$snakeTracePath = "/home/tmwong/pantheon/traces";

$day = 0;
for $i (@snakeFiles) {
  my ($commandLine) = "../scripts/snake-preprocess.pl -d $day $snakeTracePath/$i > data.$day";

  print "$commandLine\n";
  `$commandLine`;
  $day++;
}
