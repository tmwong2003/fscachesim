/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheSegVariableMulti.cc,v 1.3 2001/07/19 20:24:53 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Cache.hh"
#include "CacheGhost.hh"
#include "IORequest.hh"

#include "BlockStoreCacheSegVariableMulti.hh"

int
BlockStoreCacheSegVariableMulti::blockGetCascade(Block inBlock)
{
  int retval = cacheSegCount;

  for (int i = 0; i < cacheSegCount; i++) {
    if (cacheSegs[i]->isCached(inBlock)) {
      cacheSegs[i]->blockGet(inBlock);
      segHits[i]++;
      retval = i;
      break;
    }
  }

  return (retval);
}

void
BlockStoreCacheSegVariableMulti::blockPutAtSegCascade(Block inBlock,
						 int inSeg)
{
  Block cascadeEjectBlock = inBlock;
  bool cascadeEjectFlag = true;

  for (int i = inSeg; i >= 0 && cascadeEjectFlag; i--) {
    Block ejectBlock = {0, 0, 0};

    cascadeEjectFlag = false;
    if (cacheSegs[i]->isFull()) {
      cacheSegs[i]->blockGetAtHead(ejectBlock);
      cascadeEjectFlag = true;
    }

    // Cache the incoming block at the tail of the queue.

    cacheSegs[i]->blockPutAtTail(cascadeEjectBlock);
    cascadeEjectBlock = ejectBlock;
  }
}

BlockStoreCacheSegVariableMulti::
BlockStoreCacheSegVariableMulti(const char *inName,
				uint32_t inBlockSize,
				uint32_t inCacheSize,
				int inSegCount) :
  BlockStore(inName, inBlockSize),
  cacheSegCount(inSegCount),
  ghostMap(),
  ghostCacheSize(inCacheSize)
{
  uint32_t cacheSegSize = inCacheSize / cacheSegCount;
  uint32_t cacheSizeRemain = inCacheSize;

  cacheSegs = new Cache *[cacheSegCount];
  segHits = new uint32_t[cacheSegCount];
  for (int i = 0; i < cacheSegCount; i++) {
    uint32_t thisSegSize = (i < cacheSegCount - 1 ?
			    cacheSegSize :
			    cacheSizeRemain);

    cacheSegs[i] = new Cache(thisSegSize);
    segHits[i] = 0;

    cacheSizeRemain -= thisSegSize;
  }
  if (cacheSizeRemain != 0) {
    abort();
  }
}

// For the exponential size segments, each segment is inSegMultiplier times
// the size of the previous segment.

BlockStoreCacheSegVariableMulti::
BlockStoreCacheSegVariableMulti(const char *inName,
				uint32_t inBlockSize,
				uint32_t inCacheSize,
				int inSegCount,
				int inSegMultiplier) :
  BlockStore(inName, inBlockSize),
  cacheSegCount(inSegCount),
  ghostMap(),
  ghostCacheSize(inCacheSize)
{
  cacheSegs = new Cache *[cacheSegCount];
  segHits = new uint32_t[cacheSegCount];

  // Track how much cache space remains after allocating space for each
  // segment. We will sweep remaining space into the tail segment.

  uint32_t cacheSizeRemain = inCacheSize;

  // Each cache segment size is a fraction of the cache size. Determine the
  // denominator of the fractions. For example, if each segment is twice
  // the size of the previous segment, and there are four segments, the
  // denominator is:
  //
  // 8 4 2 1 = 15
  //
  // Thus, we divide the cache size into fifteen shares, and distribute
  // shares accordingly.

  int cacheShareCount = 0;
  for (int i = 0; i < cacheSegCount; i++) {
    cacheShareCount += ((int)pow(inSegMultiplier, i));
  }
  uint32_t cacheShareSize = inCacheSize / cacheShareCount;

  for (int i = 0; i < cacheSegCount; i++) {
    // Assign shares to this segment. The last (tail) segment gets the most
    // shares, and also any slop hasn't already been allocated due to
    // round-off.

    uint32_t thisSegSize = (i < cacheSegCount - 1 ?
			    ((int)(pow(inSegMultiplier, i))) * cacheShareSize :
			    cacheSizeRemain);

    fprintf(stderr, "%d %u\n", i, thisSegSize);

    cacheSegs[i] = new Cache(thisSegSize);
    segHits[i] = 0;
    cacheSizeRemain -= thisSegSize;
  }

  // Make sure we assigned all of the cache.

  if (cacheSizeRemain != 0) {
    abort();
  }
}

BlockStoreCacheSegVariableMulti::~BlockStoreCacheSegVariableMulti()
{
  for (GhostMapIter i = ghostMap.begin();
       i != ghostMap.end();
       i++) {
    delete i->second;
  }
  for (int i = 0; i < cacheSegCount; i++) {
    delete cacheSegs[i];
  }
  delete segHits;
  delete cacheSegs;
}

bool
BlockStoreCacheSegVariableMulti::IORequestDown(const IORequest& inIOReq,
					       list<IORequest>& outIOReqList)
{
  Block block = {0, inIOReq.objectIDGet(), inIOReq.blockOffsetGet(blockSize)};
  const char* reqOriginator = inIOReq.originatorGet();
  IORequestOp_t op = inIOReq.opGet();
  uint32_t reqBlockLength = inIOReq.blockLengthGet(blockSize);

  // If a ghost cache doesn't already exist for the originator, make one.

  GhostMapIter ghostIter = ghostMap.find(reqOriginator);
  if (ghostIter == ghostMap.end()) {
    fprintf(stderr,
	    "tmwong: Made ghost cache for originator %s\n",
	    reqOriginator);
    ghostMap[reqOriginator] = new CacheGhost(ghostCacheSize);
  }

  for (uint32_t i = 0; i < reqBlockLength; i++) {
    // See if we have cached this block.

    int blockSeg = blockGetCascade(block);
    if (blockSeg != cacheSegCount) {
      switch (op) {
      case Demote:
	demoteHitsMap[reqOriginator]++;
	blockDemoteHits++;
	break;
      case Read:
	readHitsMap[reqOriginator]++;
	blockReadHits++;

	// Per-client ghost caching.

	ghostMap[reqOriginator]->probUpdate(block);
	break;
      default:
	abort();
      }

      // For now, cache the block at the MRU end of the actual LRU queue.

      blockPutAtSegCascade(block, cacheSegCount - 1);
    }
    else {
      switch (op) {
      case Demote:
	demoteMissesMap[reqOriginator]++;
	blockDemoteMisses++;
	break;
      case Read:
	readMissesMap[reqOriginator]++;
	blockReadMisses++;
	ghostMap[reqOriginator]->probUpdate(block);

	// Create a new IORequest to pass on to the next-level node.

	outIOReqList.push_back(IORequest(reqOriginator,
					 Read,
					 inIOReq.timeIssuedGet(),
					 inIOReq.objectIDGet(),
					 block.blockID * blockSize,
					 blockSize));
	break;
      default:
	abort();
      }

      // Determine which bin the block will go into, and cache
      // it. Remember, the 0th segment is the closest to head of the queue
      // (i.e. the LRU end).

      int insertSeg = (int)(cacheSegCount *
			    ghostMap[reqOriginator]->probGet(op));
#if 0
      // Here's a heuristic: workloads that demote lots of blocks that are
      // already in the array get penalized.

      if (op == Demote && blockDemoteMisses > 0) {
	insertSeg = (double)insertSeg *
	  ((double)blockDemoteMisses / (blockDemoteHits + blockDemoteMisses));
      }
#endif /* 0 */
      if (insertSeg == cacheSegCount) {
	insertSeg--;
      }
      blockPutAtSegCascade(block, insertSeg);
    }
    ghostMap[reqOriginator]->blockPut(op, block);

    block.blockID++;
  }

  return (true);
}

void
BlockStoreCacheSegVariableMulti::statisticsReset()
{
  for (int i = 0; i < cacheSegCount; i++) {
    segHits[i] = 0;
  }

  demoteHitsMap.clear();
  demoteMissesMap.clear();

  readHitsMap.clear();
  readMissesMap.clear();

  BlockStore::statisticsReset();
}

void
BlockStoreCacheSegVariableMulti::statisticsShow() const
{
  printf("Statistics for BlockStoreCacheSegVariableMulti.%s\n", nameGet());

  uint32_t segHitsTotal = 0;
  for (int i = cacheSegCount - 1; i >= 0; i--) {
    printf("Hits in segment %d %ld\n", i, segHits[i]);
    segHitsTotal += segHits[i];
  }
  printf("Total hits in segments %ld\n", segHitsTotal);

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

  for (GhostMapConstIter i = ghostMap.begin();
       i != ghostMap.end();
       i++) {
    i->second->statisticsShow();
  }
}
