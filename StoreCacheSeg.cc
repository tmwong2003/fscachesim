/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/StoreCacheSeg.cc,v 1.5 2001/11/20 02:20:13 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Block.hh"
#include "IORequest.hh"

#include "StoreCacheSeg.hh"

using Block::block_t;

int
StoreCacheSeg::blockGetCascade(const block_t& inBlock)
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
StoreCacheSeg::blockPutAtSegCascade(const block_t& inBlock,
				    int inSeg)
{
  block_t cascadeEjectBlock = inBlock;
  bool cascadeEjectFlag = true;

  for (int i = inSeg; i >= 0 && cascadeEjectFlag; i--) {
    block_t ejectBlock = {0, 0};

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

StoreCacheSeg::StoreCacheSeg(const char *inName,
			     Store *inNextStore,
			     uint64_t inBlockSize,
			     uint64_t inSize,
			     int inSegCount,
			     bool inNormalizeFlag) :
  StoreCache(inName, inNextStore, inBlockSize),
  cacheSegCount(inSegCount),
  ghost(inName, inSize, inNormalizeFlag)
{
  uint64_t cacheSegSize = inSize / cacheSegCount;
  uint64_t cacheSizeRemain = inSize;

  cacheSegs = new Cache *[cacheSegCount];
  segHits = new uint64_t[cacheSegCount];
  for (int i = 0; i < cacheSegCount; i++) {
    uint64_t thisSegSize = (i < cacheSegCount - 1 ?
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

StoreCacheSeg::StoreCacheSeg(const char *inName,
			     Store *inNextStore,
			     uint64_t inBlockSize,
			     uint64_t inSize,
			     int inSegCount,
			     int inSegMultiplier,
			     bool inNormalizeFlag) :
  StoreCache(inName, inNextStore, inBlockSize),
  cacheSegCount(inSegCount),
  ghost(inName, inSize, inNormalizeFlag)
{
  cacheSegs = new Cache *[cacheSegCount];
  segHits = new uint64_t[cacheSegCount];

  // Track how much cache space remains after allocating space for each
  // segment. We will sweep remaining space into the tail segment.

  uint64_t cacheSizeRemain = inSize;

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
  uint64_t cacheShareSize = inSize / cacheShareCount;

  for (int i = 0; i < cacheSegCount; i++) {
    // Assign shares to this segment. The last (tail) segment gets the most
    // shares, and also any slop hasn't already been allocated due to
    // round-off.

    uint64_t thisSegSize = (i < cacheSegCount - 1 ?
			    ((int)(pow(inSegMultiplier, i))) * cacheShareSize :
			    cacheSizeRemain);

    fprintf(stderr, "%d %llu\n", i, thisSegSize);

    cacheSegs[i] = new Cache(thisSegSize);
    segHits[i] = 0;
    cacheSizeRemain -= thisSegSize;
  }

  // Make sure we assigned all of the cache.

  if (cacheSizeRemain != 0) {
    abort();
  }
}

StoreCacheSeg::~StoreCacheSeg()
{
  for (int i = 0; i < cacheSegCount; i++) {
    delete cacheSegs[i];
  }
  delete segHits;
  delete cacheSegs;
}

void
StoreCacheSeg::BlockCache(const IORequest& inIOReq,
			  const block_t& inBlock,
			  list<IORequest>& outIOReqs)
{
  const char* reqOrig = inIOReq.origGet();
  IORequestOp_t op = inIOReq.opGet();

  // See if we have cached this block.

  int blockSeg = blockGetCascade(inBlock);
  if (blockSeg != cacheSegCount) {
    switch (op) {
    case Demote:
      demoteHitsPerOrig[reqOrig]++;
      demoteHits++;
      break;
    case Read:
      readHitsPerOrig[reqOrig]++;
      readHits++;
      ghost.probUpdate(inBlock);
      break;
    default:
      abort();
    }

    // For now, cache the block at the MRU end of the actual LRU queue.

    blockPutAtSegCascade(inBlock, cacheSegCount - 1);
  }
  else {
    switch (op) {
    case Demote:
      demoteMissesPerOrig[reqOrig]++;
      demoteMisses++;
      break;
    case Read:
      readMissesPerOrig[reqOrig]++;
      readMisses++;
      ghost.probUpdate(inBlock);

      // Create a new IORequest to pass on to the next-level node.

      outIOReqs.push_back(IORequest(reqOrig,
				    Read,
				    inIOReq.timeIssuedGet(),
				    inIOReq.objIDGet(),
				    inBlock.blockID * blockSize,
				    blockSize));
      break;
    default:
      abort();
    }

    // Determine which bin the block will go into, and cache
    // it. Remember, the 0th segment is the closest to head of the queue
    // (i.e. the LRU end).

    int insertSeg = (int)(cacheSegCount * ghost.probGet(op));
    if (insertSeg == cacheSegCount) {
      insertSeg--;
    }
    blockPutAtSegCascade(inBlock, insertSeg);
  }
  ghost.blockPut(op, inBlock);
}

void
StoreCacheSeg::statisticsReset()
{
  for (int i = 0; i < cacheSegCount; i++) {
    segHits[i] = 0;
  }

  StoreCache::statisticsReset();
}

void
StoreCacheSeg::statisticsShow() const
{
  printf("Statistics for StoreCacheSeg.%s\n", nameGet());

  uint64_t segHitsTotal = 0;
  for (int i = cacheSegCount - 1; i >= 0; i--) {
    printf("Hits in segment %d %llu\n", i, segHits[i]);
    segHitsTotal += segHits[i];
  }
  printf("Total hits in segments %llu\n", segHitsTotal);

  ghost.statisticsShow();
  StoreCache::statisticsShow();
}
