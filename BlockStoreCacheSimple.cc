/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCache.cc,v 1.13 2001/11/20 02:20:13 tmwong Exp $
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

#include "BlockStoreCacheSimple.hh"

using Block::block_t;

// This cache maintains one ejection queue, the head of which is the
// eject-me-next block. Hence, for LRU and MRU we add blocks at the tail
// and head respectively.

bool
BlockStoreCacheSimple::IORequestDown(const IORequest& inIOReq,
				     list<IORequest>& outIOReqList)
{
  block_t block = {inIOReq.objIDGet(), inIOReq.blockOffGet(blockSize)};
  uint64_t reqBlockLen = inIOReq.blockLenGet(blockSize);

  // Log incoming request if desired.

  if (logRequestFlag) {
    printf("%lf %llu %llu %llu\n",
	   inIOReq.timeIssuedGet(),
	   inIOReq.objIDGet(),
	   inIOReq.offGet(),
	   inIOReq.lenGet());
  }

  for (uint64_t i = 0; i < reqBlockLen; i++) {
    // See if the block is cached.

    if (cache.isCached(block)) {

      // Eject the block (we will re-cache it later).

      cache.blockGet(block);

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
	  outIOReqList.push_back(IORequest(inIOReq.origGet(),
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

	outIOReqList.push_back(IORequest(inIOReq.origGet(),
					 Read,
					 inIOReq.timeIssuedGet(),
					 inIOReq.objIDGet(),
					 block.blockID * blockSize,
					 blockSize));

	break;
      default:
	abort();
      }
    }

    switch (ejectPolicy) {
    case LRU:
      cache.blockPutAtTail(block);
      break;
    case MRU:
      if (inIOReq.opGet() == Demote) {
	// Demoted blocks always go at the eject-me-last end.

	cache.blockPutAtTail(block);
      }
      else {
	cache.blockPutAtHead(block);
      }
      break;
    default:
      // Wow - we should not get here!

      abort();
    }

    block.blockID++;
  }

  return (true);
}
