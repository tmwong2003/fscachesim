/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/BlockStoreCache.cc,v 1.1 2000/09/22 16:15:38 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "IORequest.hh"

#include "BlockStoreCache.hh"

// This cache maintains one ejection queue, the head of which is the
// eject-me-next block. Hence, for LRU and MRU we add blocks at the tail
// and head respectively.

bool
BlockStoreCache::IORequestDown(const IORequest& inIOReq,
			       list<IORequest>& outIOReqList)
{
  Block block = {0, inIOReq.objectIDGet(), inIOReq.blockOffsetGet(blockSize)};
  uint32_t reqBlockLength = inIOReq.blockLengthGet(blockSize);

  for (uint32_t i = 0; i < reqBlockLength; i++) {
    // See if the block is cached.

    CacheIndexIter blockIter = cacheIndex.find(block);
    if (blockIter != cacheIndex.end()) {
      switch (inIOReq.opGet()) {
      case Demote:
	blockDemoteHits++;
	break;
      case Read:
	blockReadHits++;
	break;
      default:
	abort();
      }

      // Pull it out of the list.

      cache.erase(blockIter->second);
    }
    else {
      switch (inIOReq.opGet()) {
      case Demote:
	blockDemoteMisses++;
	break;
      case Read:
	blockReadMisses++;
	break;
      default:
	abort();
      }

      // If the cache is full, eject the front block.

      if (cacheBlocks == cacheSize) {
	// If necessary, create a Demote I/O.

	if (cacheDemotePolicy == DemoteDemand) {
	  Block demoteBlock = *cache.begin();

	  outIOReqList.push_back(IORequest(Demote,
					   demoteBlock.devID,
					   demoteBlock.objectID,
					   demoteBlock.blockID * blockSize,
					   blockSize));
	}

	cacheIndex.erase(cache.front());
	cache.pop_front();
      }
      else {
	cacheBlocks++;
      }

      // Create a new IORequest to pass on to the next-level node.

      outIOReqList.push_back(IORequest(Read,
				       0,
				       inIOReq.objectIDGet(),
				       block.blockID * blockSize,
				       blockSize));
    }

    switch (cacheReplPolicy) {
    case LRU:
      cache.push_back(block);
      cacheIndex[block] = --cache.end();
      break;
    case MRU:
      if (inIOReq.opGet() == Demote) {
	// Demoted blocks always go at the eject-me-last end.

	cache.push_back(block);
	cacheIndex[block] = --cache.end();
      }
      else {
	cache.push_front(block);
	cacheIndex[block] = cache.begin();
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

void
BlockStoreCache::statisticsShow() const
{
  printf("Block demote hits %u\n", blockDemoteHits);
  printf("Block demote misses %u\n", blockDemoteMisses);
  printf("Block read hits %u\n", blockReadHits);
  printf("Block read misses %u\n", blockReadMisses);
}
