/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheSegmented.cc,v 1.1 2001/07/04 17:49:30 tmwong Exp $
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
// 1. Demoted blocks go to the tail of the protected LRU queue.
// 2. Read blocks go to the tail of the probationary LRU queue.

bool
BlockStoreCacheSegmented::IORequestDown(const IORequest& inIOReq,
					list<IORequest>& outIOReqList)
{
  Block block = {0, inIOReq.objectIDGet(), inIOReq.blockOffsetGet(blockSize)};
  uint32_t reqBlockLength = inIOReq.blockLengthGet(blockSize);

  for (uint32_t i = 0; i < reqBlockLength; i++) {
    if (protCache.isCached(block)) {

      // Block is in the protected cache. Eject the block (we will
      // re-cache it later).

      protCache.blockGet(block);

      switch (inIOReq.opGet()) {
      case Demote:
	protDemoteHitsMap[inIOReq.originatorGet()]++;
	blockDemoteHits++;

	// Re-cache the block at the end of the protected cache.

	protCache.blockPutAtTail(block);
	break;
      case Read:
	protReadHitsMap[inIOReq.originatorGet()]++;
	blockReadHits++;

	// If the probationary cache is full, eject the front
	// block. Then, re-cache the block at the end of the
	// probationary cache.

	if (probCache.isFull()) {
	  Block ejectBlock;

	  probCache.blockGetAtHead(ejectBlock);
	}
	probCache.blockPutAtTail(block);
	break;
      default:
	abort();
      }
    }
    else if (probCache.isCached(block)) {

      // Block is in the probationary cache. Eject the block (we will
      // re-cache it later).

      probCache.blockGet(block);

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

      // If the protected cache is full, move a block to the
      // probationary cache.

      if (protCache.isFull()) {
	Block protToProbBlock;

	protCache.blockGetAtHead(protToProbBlock);
	probCache.blockPutAtTail(protToProbBlock);
	protToProbXfersMap[inIOReq.originatorGet()]++;
      }

      protCache.blockPutAtTail(block);
      probToProtXfersMap[inIOReq.originatorGet()]++;
#if 0
      // Old behavior keeps only demoted blocks in the protected queue.

      switch (inIOReq.opGet()) {
      case Demote:
	probDemoteHitsMap[inIOReq.originatorGet()]++;
	blockDemoteHits++;

	// If the protected cache is full, move a block to the
	// probationary cache.

	if (protCache.isFull()) {
	  Block protToProbBlock;

	  protCache.blockGetAtHead(protToProbBlock);
	  probCache.blockPutAtTail(protToProbBlock);
	  protToProbXfersMap[inIOReq.originatorGet()]++;
	}

	// Re-cache the block at the end of the protected cache.

	protCache.blockPutAtTail(block);
	probToProtXfersMap[inIOReq.originatorGet()]++;
	break;
      case Read:
	probReadHitsMap[inIOReq.originatorGet()]++;
	blockReadHits++;

	// Re-cache the block at the end of the probationary cache.

	probCache.blockPutAtTail(block);
	break;
      default:
	abort();
      }
#endif /* 0 */
    }
    else {

      // Block isn't cached.

      switch (inIOReq.opGet()) {
      case Demote:
	blockDemoteMissesMap[inIOReq.originatorGet()]++;
	blockDemoteMisses++;

	// If this is a demoted block, and the protected cache is
	// full, move a block to the probationary cache. Eject the
	// head of the probationary cache if necessary.

	if (protCache.isFull()) {
	  Block protToProbBlock;

	  protCache.blockGetAtHead(protToProbBlock);
	  if (probCache.isFull()) {
	    Block ejectBlock;

	    probCache.blockGetAtHead(ejectBlock);
	  }
	  probCache.blockPutAtTail(protToProbBlock);
	  protToProbXfersMap[inIOReq.originatorGet()]++;
	}
	protCache.blockPutAtTail(block);
	break;

      case Read:
	blockReadMissesMap[inIOReq.originatorGet()]++;
	blockReadMisses++;

	// If the probationary cache is full, eject the front block.

	if (probCache.isFull()) {
	  Block ejectBlock;

	  probCache.blockGetAtHead(ejectBlock);
	}
	probCache.blockPutAtTail(block);

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
    printf("Block probationary read hits for %s %u\n", i->first, i->second);
  }
  for (StatMapConstIter i = protReadHitsMap.begin();
       i != protReadHitsMap.end();
       i++) {
    printf("Block protected read hits for %s %u\n", i->first, i->second);
  }
  for (StatMapConstIter i = probToProtXfersMap.begin();
       i != probToProtXfersMap.end();
       i++) {
    printf("Block prob-to-prot transfers for %s %u\n", i->first, i->second);
  }
  for (StatMapConstIter i = protToProbXfersMap.begin();
       i != protToProbXfersMap.end();
       i++) {
    printf("Block prot-to-prob transfers for %s %u\n", i->first, i->second);
  }
  for (StatMapConstIter i = blockReadMissesMap.begin();
       i != blockReadMissesMap.end();
       i++) {
    printf("Block read misses for %s %u\n", i->first, i->second);
  }
  printf("Block demote hits %u\n", blockDemoteHits);
  printf("Block demote misses %u\n", blockDemoteMisses);
  printf("Block read hits %u\n", blockReadHits);
  printf("Block read misses %u\n", blockReadMisses);
}
