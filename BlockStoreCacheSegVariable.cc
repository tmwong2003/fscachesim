/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheSegVariable.cc,v 1.2 2001/07/19 02:53:34 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Cache.hh"
#include "CacheGhost.hh"
#include "IORequest.hh"

#include "BlockStoreCacheSegVariable.hh"

int
BlockStoreCacheSegVariable::blockGetCascade(Block inBlock)
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
BlockStoreCacheSegVariable::blockPutAtSegCascade(Block inBlock,
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

BlockStoreCacheSegVariable::BlockStoreCacheSegVariable(const char *inName,
						       uint32_t inBlockSize,
						       uint32_t inCacheSize,
						       int inSegCount) :
  BlockStore(inName, inBlockSize),
  cacheSegCount(inSegCount),
  ghost(inCacheSize)
{
  uint32_t cacheSegSize = inCacheSize / cacheSegCount;
  uint32_t cacheSizeUnallocated = inCacheSize;

  cacheSegs = new Cache *[cacheSegCount];
  segHits = new uint32_t[cacheSegCount];
  for (int i = 0; i < cacheSegCount; i++) {
    uint32_t thisSegSize = (i < cacheSegCount - 1 ?
			    cacheSegSize :
			    cacheSizeUnallocated);

    cacheSegs[i] = new Cache(thisSegSize);
    segHits[i] = 0;

    cacheSizeUnallocated -= thisSegSize;
  }
  if (cacheSizeUnallocated != 0) {
    abort();
  }
}

BlockStoreCacheSegVariable::BlockStoreCacheSegVariable(const char *inName,
						       uint32_t inBlockSize,
						       uint32_t inCacheSize,
						       int inSegCount,
						       double inSegBase) :
  BlockStore(inName, inBlockSize),
  cacheSegCount(inSegCount),
  ghost(inCacheSize)
{
  cacheSegs = new Cache *[cacheSegCount];
  segHits = new uint32_t[cacheSegCount];

  // Tail segment is the largest.

  uint32_t cacheSizeUnallocated = inCacheSize;
  double segFracTotal = 0;
  for (int i = 0; i < cacheSegCount; i++) {
    // Compute fraction of entire cache. The last segment (the tail) is the
    // largest.

    double thisSegFrac = 
	log((double)(cacheSegCount + 1 - i) / (cacheSegCount - i)) /
	log(inSegBase);
    uint32_t thisSegSize = (i < cacheSegCount - 1 ?
			    (uint32_t)(inCacheSize * thisSegFrac) :
			    cacheSizeUnallocated);

    cacheSegs[i] = new Cache(thisSegSize);
    segHits[i] = 0;
    fprintf(stderr, "%d %lf %ld\n", i, thisSegFrac, thisSegSize);
    cacheSizeUnallocated -= thisSegSize;
    segFracTotal += thisSegFrac;
  }
  if (cacheSizeUnallocated != 0 || (int)(segFracTotal) != 1) {
    abort();
  }
}

BlockStoreCacheSegVariable::~BlockStoreCacheSegVariable()
{
  for (int i = 0; i < cacheSegCount; i++) {
    delete cacheSegs[i];
  }
  delete segHits;
  delete cacheSegs;
}

bool
BlockStoreCacheSegVariable::IORequestDown(const IORequest& inIOReq,
				    list<IORequest>& outIOReqList)
{
  Block block = {0, inIOReq.objectIDGet(), inIOReq.blockOffsetGet(blockSize)};
  IORequestOp_t op = inIOReq.opGet();
  uint32_t reqBlockLength = inIOReq.blockLengthGet(blockSize);

  for (uint32_t i = 0; i < reqBlockLength; i++) {
    // See if we have cached this block.

    int blockSeg = blockGetCascade(block);
    if (blockSeg != cacheSegCount) {
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

      blockPutAtSegCascade(block, cacheSegCount - 1);
    }
    else {
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

      // Determine which bin the block will go into, and cache
      // it. Remember, the 0th segment is the closest to head of the queue
      // (i.e. the LRU end).

      int insertSeg = (int)(cacheSegCount * ghost.probGet(op));
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
    ghost.blockPut(op, block);

    block.blockID++;
  }

  return (true);
}

void
BlockStoreCacheSegVariable::statisticsReset()
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
BlockStoreCacheSegVariable::statisticsShow() const
{
  printf("Statistics for BlockStoreCacheSegVariable.%s\n", nameGet());

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

  ghost.statisticsShow();
}
