/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/StoreCache.cc,v 1.3 2002/02/15 18:17:30 tmwong Exp $
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <list>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */
#include <stdio.h>
#include <stdlib.h>

#include "Block.hh"
#include "Char.hh"
#include "IORequest.hh"
#include "Store.hh"

#include "StoreCache.hh"

using Block::block_t;

bool
StoreCache::IORequestDown(const IORequest& inIOReq)
{
  block_t block = {inIOReq.objIDGet(), inIOReq.blockOffGet(blockSize)};
  list<IORequest> newIOReqs;

  // Log incoming request if desired.

  if (logRequestFlag) {
    printf("%lf %llu %llu %llu\n",
	   inIOReq.timeIssuedGet(),
	   inIOReq.objIDGet(),
	   inIOReq.offGet(),
	   inIOReq.lenGet());
  }

  // See if each block is cached; if not, make space for it and send an I/O
  // request to the lower-level storage device.

  for (uint64_t i = 0; i < inIOReq.blockLenGet(blockSize); i++) {
    BlockCache(inIOReq, block, newIOReqs);
    block.blockID++;
  }

  // Send I/O requests to the next storage device.

  if (nextStore) {
    for (list<IORequest>::iterator i = newIOReqs.begin();
	 i != newIOReqs.end();
	 i++) {

      nextStore->IORequestDown(*i);
    }
  }

  return (true);
}

void
StoreCache::statisticsReset()
{
  demoteHitsPerOrig.clear();
  demoteMissesPerOrig.clear();

  readHitsPerOrig.clear();
  readMissesPerOrig.clear();

  Store::statisticsReset();
}

void
StoreCache::statisticsShow() const
{
  printf("{StoreCache.%s\n", nameGet());

  printf("\t{blockSize=%llu}\n", blockSizeGet());

  printf("\t{readHitsPerOrig ");
  for (Char::Counter::const_iterator i = readHitsPerOrig.begin();
       i != readHitsPerOrig.end();
       i++) {
    printf("{%s=%llu} ", i->first, i->second);
  }
  printf("}\n");
  printf("\t{readMissesPerOrig ");
  for (Char::Counter::const_iterator i = readMissesPerOrig.begin();
       i != readMissesPerOrig.end();
       i++) {
    printf("{%s=%llu} ", i->first, i->second);
  }
  printf("}\n");

  printf("\t{read {hits=%llu} {misses=%llu} }\n", readHits, readMisses);

  printf("\t{demoteHitsPerOrig ");
  for (Char::Counter::const_iterator i = demoteHitsPerOrig.begin();
       i != demoteHitsPerOrig.end();
       i++) {
    printf("{%s=%llu} ", i->first, i->second);
  }
  printf("}\n");
  printf("\t{demoteMissesPerOrig ");
  for (Char::Counter::const_iterator i = demoteMissesPerOrig.begin();
       i != demoteMissesPerOrig.end();
       i++) {
    printf("{%s=%llu} ", i->first, i->second);
  }
  printf("}\n");

  printf("\t{demote {hits=%llu} {misses=%llu} }\n", demoteHits, demoteMisses);

  printf("}\n");
}
