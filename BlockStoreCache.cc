/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCache.cc,v 1.12 2001/07/21 00:07:28 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "Cache.hh"
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
  uint64_t reqBlockLength = inIOReq.blockLengthGet(blockSize);

  // Log incoming request if desired.

  if (logRequestFlag) {
    printf("%lf %llu %llu %llu\n",
	   inIOReq.timeIssuedGet(),
	   inIOReq.objectIDGet(),
	   inIOReq.offsetGet(),
	   inIOReq.lengthGet());
  }

  for (uint64_t i = 0; i < reqBlockLength; i++) {
    // See if the block is cached.

    if (cache.isCached(block)) {

      // Eject the block (we will re-cache it later).

      cache.blockGet(block);

      switch (inIOReq.opGet()) {
      case Demote:
	blockDemoteHitsMap[inIOReq.originatorGet()]++;
	blockDemoteHits++;
	break;
      case Read:
	blockReadHitsMap[inIOReq.originatorGet()]++;
	blockReadHits++;
	break;
      default:
	abort();
      }
    }
    else {

      // Eject the front block if the cache is full.

      if (cache.isFull()) {
	Block demoteBlock;

	cache.blockGetAtHead(demoteBlock);

	// If necessary, create a Demote I/O.

	if (cacheDemotePolicy == DemoteDemand) {
	  outIOReqList.push_back(IORequest(inIOReq.originatorGet(),
					   Demote,
					   demoteBlock.devID,
					   demoteBlock.objectID,
					   demoteBlock.blockID * blockSize,
					   blockSize));
	}
      }

      switch (inIOReq.opGet()) {
      case Demote:
	blockDemoteMissesMap[inIOReq.originatorGet()]++;
	blockDemoteMisses++;
	break;
      case Read:
	blockReadMissesMap[inIOReq.originatorGet()]++;
	blockReadMisses++;

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
    }

    switch (cacheReplPolicy) {
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

void
BlockStoreCache::statisticsReset()
{
  blockDemoteHitsMap.clear();
  blockDemoteMissesMap.clear();

  blockReadHitsMap.clear();
  blockReadMissesMap.clear();

  BlockStore::statisticsReset();
}

void
BlockStoreCache::statisticsShow() const
{
  printf("Statistics for BlockStoreCache.%s\n", nameGet());

  for (StatMapConstIter i = blockDemoteHitsMap.begin();
       i != blockDemoteHitsMap.end();
       i++) {
    printf("Demote hits for %s %llu\n", i->first, i->second);
  }
  for (StatMapConstIter i = blockDemoteMissesMap.begin();
       i != blockDemoteMissesMap.end();
       i++) {
    printf("Demote misses for %s %llu\n", i->first, i->second);
  }

  printf("Demote hits %llu\n", blockDemoteHits);
  printf("Demote misses %llu\n", blockDemoteMisses);

  for (StatMapConstIter i = blockReadHitsMap.begin();
       i != blockReadHitsMap.end();
       i++) {
    printf("Read hits for %s %llu\n", i->first, i->second);
  }
  for (StatMapConstIter i = blockReadMissesMap.begin();
       i != blockReadMissesMap.end();
       i++) {
    printf("Read misses for %s %llu\n", i->first, i->second);
  }

  printf("Read hits %llu\n", blockReadHits);
  printf("Read misses %llu\n", blockReadMisses);
}
