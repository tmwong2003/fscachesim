/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/StoreCache.cc,v 1.13 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <list>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#include "Block.hh"
#include "IORequest.hh"

#include "StoreCacheSimple.hh"

using Block::block_t;

/**
 * This cache maintains one ejection queue. The head of the queue is the
 * eject-me-next block. Hence, for LRU and MRU we add blocks at the tail
 * and head respectively.
 */
void
StoreCacheSimple::BlockCache(const IORequest& inIOReq,
			     const block_t inBlock,
			     list<IORequest>& outIOReqs)
{
  // See if the block is cached.

  if (cache.isCached(inBlock)) {

    // Eject the block (we will re-cache it later).

    cache.blockGet(inBlock);

    switch (inIOReq.opGet()) {
    case Demote:
      demoteHitsPerOrig[inIOReq.origGet()]++;
      demoteHits++;
      break;
    case Read:
      readHitsPerOrig[inIOReq.origGet()]++;
      readHits++;
      break;
    default:
      abort();
    }
  }
  else {

    // Eject the front block if the cache is full.

    if (cache.isFull()) {
      block_t demoteBlock;

      cache.blockGetAtHead(demoteBlock);

      // If necessary, create a Demote I/O.

      if (demotePolicy == DemoteDemand) {
	outIOReqs.push_back(IORequest(inIOReq.origGet(),
				      Demote,
				      inIOReq.timeIssuedGet(),
				      demoteBlock.objID,
				      demoteBlock.blockID * blockSize,
				      blockSize));
      }
    }

    switch (inIOReq.opGet()) {
    case Demote:
      demoteMissesPerOrig[inIOReq.origGet()]++;
      demoteMisses++;
      break;
    case Read:
      readMissesPerOrig[inIOReq.origGet()]++;
      readMisses++;

      // Create a new IORequest to pass on to the next-level node.

      outIOReqs.push_back(IORequest(inIOReq.origGet(),
				    Read,
				    inIOReq.timeIssuedGet(),
				    inIOReq.objIDGet(),
				    inBlock.blockID * blockSize,
				    blockSize));

      break;
    default:
      abort();
    }
  }

  switch (ejectPolicy) {
  case LRU:
    cache.blockPutAtTail(inBlock);
    break;
  case MRU:
    if (inIOReq.opGet() == Demote) {
      // Demoted blocks always go at the eject-me-last end.

      cache.blockPutAtTail(inBlock);
    }
    else {
      cache.blockPutAtHead(inBlock);
    }
    break;
  default:
    // Wow - we should not get here!

    abort();
  }
}
