/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheGhost.cc,v 1.1 2001/07/18 03:16:35 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "Cache.hh"
#include "CacheGhost.hh"
#include "IORequest.hh"

#include "BlockStoreCacheGhost.hh"

bool
BlockStoreCacheGhost::IORequestDown(const IORequest& inIOReq,
				    list<IORequest>& outIOReqList)
{
  Block block = {0, inIOReq.objectIDGet(), inIOReq.blockOffsetGet(blockSize)};
  IORequestOp_t op = inIOReq.opGet();
  uint32_t reqBlockLength = inIOReq.blockLengthGet(blockSize);

  for (uint32_t i = 0; i < reqBlockLength; i++) {
    // See if we have cached this block.

    if (cache.isCached(block)) {

      // Eject the block (we will re-cache it later).

      cache.blockGet(block);

      // Update actual cache statistics.

      switch (op) {
      case Demote:
	demoteHitsMap[inIOReq.originatorGet()]++;
	blockDemoteHits++;
	break;
      case Read:
	readHitsMap[inIOReq.originatorGet()]++;
	blockReadHits++;
	ghost.probUpdate(block);
	break;
      default:
	abort();
      }

      // For now, cache the block at the MRU end of the actual LRU queue.

      cache.blockPutAtTail(block);
    }
    else {
      // Update actual cache statistics.

      switch (op) {
      case Demote:
	demoteMissesMap[inIOReq.originatorGet()]++;
	blockDemoteMisses++;
	break;
      case Read:
	readMissesMap[inIOReq.originatorGet()]++;
	blockReadMisses++;
	ghost.probUpdate(block);

	// Create a new IORequest to pass on to the next-level node.

	outIOReqList.push_back(IORequest(inIOReq.originatorGet(),
					 Read,
					 inIOReq.timeIssuedGet(),
					 inIOReq.objectIDGet(),
					 block.blockID * blockSize,
					 blockSize));
	break;
      default:
	abort();
      }

      // Decide if we're going to cache this block or not. Of course, it's
      // stupid not to cache when we have space available.

      double prob = (double)rand() / RAND_MAX;
      if (!cache.isFull() || prob <= ghost.probGet(op)) {
	// Eject the LRU block from the actual LRU queue.

	if (cache.isFull()) {
	  Block ejectBlock;
	  cache.blockGetAtHead(ejectBlock);
	}

	// For now, cache the block at the MRU end of the actual LRU queue.

	cache.blockPutAtTail(block);

	switch (op) {
	case Demote:
	  demotesCached++;
	  break;
	case Read:
	  readsCached++;
	  break;
	default:
	  abort();
	}
      }
      else {
	switch (op) {
	case Demote:
	  demotesUncached++;
	  break;
	case Read:
	  readsUncached++;
	  break;
	default:
	  abort();
	}
      }
    }
    ghost.blockPut(op, block);

    block.blockID++;
  }

  return (true);
}

void
BlockStoreCacheGhost::statisticsReset()
{
  demoteHitsMap.clear();
  demoteMissesMap.clear();

  readHitsMap.clear();
  readMissesMap.clear();

  BlockStore::statisticsReset();
}

void
BlockStoreCacheGhost::statisticsShow() const
{
  printf("Statistics for BlockStoreCacheGhost.%s\n", nameGet());

  for (StatMapConstIter i = demoteHitsMap.begin();
       i != demoteHitsMap.end();
       i++) {
    printf("Demote hits for %s %u\n", i->first, i->second);
  }
  for (StatMapConstIter i = demoteMissesMap.begin();
       i != demoteMissesMap.end();
       i++) {
    printf("Demote misses for %s %u\n", i->first, i->second);
  }

  printf("Demote hits %u\n", blockDemoteHits);
  printf("Demote misses %u\n", blockDemoteMisses);

  printf("Demote cached %u\n", demotesCached);
  printf("Demote uncached %u\n", demotesUncached);

  for (StatMapConstIter i = readHitsMap.begin();
       i != readHitsMap.end();
       i++) {
    printf("Read hits for %s %u\n", i->first, i->second);
  }
  for (StatMapConstIter i = readMissesMap.begin();
       i != readMissesMap.end();
       i++) {
    printf("Read misses for %s %u\n", i->first, i->second);
  }

  printf("Read hits %u\n", blockReadHits);
  printf("Read misses %u\n", blockReadMisses);

  ghost.statisticsShow();

  printf("Read cached %u\n", readsCached);
  printf("Read uncached %u\n", readsUncached);
}
