/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCache.cc,v 1.13 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "Char.hh"

#include "BlockStoreCache.hh"

void
BlockStoreCache::statisticsReset()
{
  demoteHitsPerOrig.clear();
  demoteMissesPerOrig.clear();

  readHitsPerOrig.clear();
  readMissesPerOrig.clear();

  BlockStore::statisticsReset();
}

void
BlockStoreCache::statisticsShow() const
{
  printf("Statistics for BlockStoreCache.%s\n", nameGet());

  for (Char::Counter::const_iterator i = demoteHitsPerOrig.begin();
       i != demoteHitsPerOrig.end();
       i++) {
    printf("Demote hits for %s %llu\n", i->first, i->second);
  }
  for (Char::Counter::const_iterator i = demoteMissesPerOrig.begin();
       i != demoteMissesPerOrig.end();
       i++) {
    printf("Demote misses for %s %llu\n", i->first, i->second);
  }

  printf("Demote hits %llu\n", demoteHits);
  printf("Demote misses %llu\n", demoteMisses);

  for (Char::Counter::const_iterator i = readHitsPerOrig.begin();
       i != readHitsPerOrig.end();
       i++) {
    printf("Read hits for %s %llu\n", i->first, i->second);
  }
  for (Char::Counter::const_iterator i = readMissesPerOrig.begin();
       i != readMissesPerOrig.end();
       i++) {
    printf("Read misses for %s %llu\n", i->first, i->second);
  }

  printf("Read hits %llu\n", readHits);
  printf("Read misses %llu\n", readMisses);
}
