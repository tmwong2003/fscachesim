/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheSegmented.cc,v 1.3 2001/07/17 01:56:26 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>
#include <stdlib.h>

#include "Cache.hh"
#include "IORequest.hh"

#include "BlockStoreCacheSegmented.hh"

// Segemented cache operation.
//
// 1. When blocks first arrive in the cache (regardless of whether they
// were read in or demoted down), they go into the probationary queue.
//
// 2. If some request hits the block, the cache moves it into the protected
// queue.

bool
BlockStoreCacheSegmented::IORequestDown(const IORequest& inIOReq,
					list<IORequest>& outIOReqList)
{
  Block block = {0, inIOReq.objectIDGet(), inIOReq.blockOffsetGet(blockSize)};
  uint64_t reqBlockLength = inIOReq.blockLengthGet(blockSize);

  for (uint64_t i = 0; i < reqBlockLength; i++) {
    if (protCache.isCached(block)) {

      // Block is in the protected cache.

      // Eject the block (we will re-cache it later).

      protCache.blockGet(block);

      switch (inIOReq.opGet()) {
      case Demote:
	protDemoteHitsMap[inIOReq.originatorGet()]++;
	blockDemoteHits++;
	break;
      case Read:
	protReadHitsMap[inIOReq.originatorGet()]++;
	blockReadHits++;
	break;
      default:
	abort();
      }

      // Re-cache the block at the end of the protected cache.

      protCache.blockPutAtTail(block);
    }
    else if (probCache.isCached(block)) {

      // Block is in the probationary cache.

      // Eject the block (we will re-cache it later).

      probCache.blockGet(block);

      // Move a block to the probationary cache if the protected cache is
      // full.

      if (protCache.isFull()) {
	Block protToProbBlock;

	protCache.blockGetAtHead(protToProbBlock);
	probCache.blockPutAtTail(protToProbBlock);
	protToProbXfersMap[inIOReq.originatorGet()]++;
      }

      switch (inIOReq.opGet()) {
      case Demote:
	probDemoteHitsMap[inIOReq.originatorGet()]++;
	blockDemoteHits++;
	break;
      case Read:
	probReadHitsMap[inIOReq.originatorGet()]++;
	blockReadHits++;
	break;
      default:
	abort();
      }

      protCache.blockPutAtTail(block);
      probToProtXfersMap[inIOReq.originatorGet()]++;
    }
    else {

      // Block isn't cached.

      // Eject the front block of the probationary cache if it is full.

      if (probCache.isFull()) {
	Block ejectBlock;

	probCache.blockGetAtHead(ejectBlock);
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
					 0,
					 inIOReq.objectIDGet(),
					 block.blockID * blockSize,
					 blockSize));
	break;
      default:
	abort();
      }

      // Cache the block at the end of the probationary cache.

      probCache.blockPutAtTail(block);
    }

    block.blockID++;
  }

  return (true);
}

void
BlockStoreCacheSegmented::statisticsReset()
{
  protDemoteHitsMap.clear();
  protReadHitsMap.clear();

  probDemoteHitsMap.clear();
  probReadHitsMap.clear();

  blockDemoteMissesMap.clear();
  blockReadMissesMap.clear();

  probToProtXfersMap.clear();
  protToProbXfersMap.clear();

  BlockStore::statisticsReset();
}

void
BlockStoreCacheSegmented::statisticsShow() const
{
  printf("Statistics for BlockStoreCache.%s\n", nameGet());
  for (StatMapConstIter i = probReadHitsMap.begin();
       i != probReadHitsMap.end();
       i++) {
    printf("Block probationary read hits for %s %llu\n", i->first, i->second);
  }
  for (StatMapConstIter i = protReadHitsMap.begin();
       i != protReadHitsMap.end();
       i++) {
    printf("Block protected read hits for %s %llu\n", i->first, i->second);
  }
  for (StatMapConstIter i = probToProtXfersMap.begin();
       i != probToProtXfersMap.end();
       i++) {
    printf("Block prob-to-prot transfers for %s %llu\n", i->first, i->second);
  }
  for (StatMapConstIter i = protToProbXfersMap.begin();
       i != protToProbXfersMap.end();
       i++) {
    printf("Block prot-to-prob transfers for %s %llu\n", i->first, i->second);
  }
  for (StatMapConstIter i = blockReadMissesMap.begin();
       i != blockReadMissesMap.end();
       i++) {
    printf("Block read misses for %s %llu\n", i->first, i->second);
  }
  printf("Block demote hits %llu\n", blockDemoteHits);
  printf("Block demote misses %llu\n", blockDemoteMisses);
  printf("Block read hits %llu\n", blockReadHits);
  printf("Block read misses %llu\n", blockReadMisses);
}
