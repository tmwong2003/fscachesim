/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/BlockStoreInfinite.cc,v 1.1.1.1 2000/09/21 16:25:41 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#include <stdio.h>

#include "IORequest.hh"

#include "BlockStoreCache.hh"

bool
BlockStoreCache::IORequestDown(const IORequest& inIOReq,
			       list<IORequest>& outIOReqList)
{
  Block block = {0, inIOReq.objectIDGet(), inIOReq.blockOffsetGet(blockSize)};

  for (uint32_t i = 0; i < inIOReq.blockLengthGet(blockSize); i++) {
    // See if the block is cached.

    CacheIndexIter blockIter = cacheIndex.find(block);
    if (blockIter != cacheIndex.end()) {
      blockReadHits++;

      // Pull it out of the list.

      cache.erase(blockIter->second);
    }
    else {
      blockReadMisses++;

      // If the cache is full, eject the front block.

      if (cacheBlocks == cacheSize) {
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
    cache.push_back(block);
    cacheIndex[block] = --cache.end();

    block.blockID++;
  }

  return (true);
}

void
BlockStoreCache::StatisticsShow()
{
  printf("Block hits %u\n", blockReadHits);
  printf("Block misses %u\n", blockReadMisses);
}
