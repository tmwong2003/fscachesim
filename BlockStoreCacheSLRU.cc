/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreCacheSLRU.cc,v 1.4 2001/11/20 02:20:13 tmwong Exp $
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <list>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#include "Block.hh"
#include "IORequest.hh"

#include "BlockStoreCacheSLRU.hh"

using Block::block_t;

/**
 * Receive an incoming I/O request sent down from a higher-level block
 * store or request generator.
 * 
 * When a block first arrives (regardless of whether they were read in or
 * demoted down), it goes into the probationary cache. If some subsequent
 * request hits the block, the block moves it into the protected cache.
 */
bool
BlockStoreCacheSLRU::IORequestDown(const IORequest& inIOReq,
				   list<IORequest>& outIOReqList)
{
  block_t block = {inIOReq.objIDGet(), inIOReq.blockOffGet(blockSize)};
  uint64_t reqBlockLen = inIOReq.blockLenGet(blockSize);

  for (uint64_t i = 0; i < reqBlockLen; i++) {
    if (protCache.isCached(block)) {

      // Block is in the protected cache.

      // Eject the block (we will re-cache it later).

      protCache.blockGet(block);

      switch (inIOReq.opGet()) {
      case Demote:
	protDemoteHitsPerOrig[inIOReq.origGet()]++;
	demoteHitsPerOrig[inIOReq.origGet()]++;
	demoteHits++;
	break;
      case Read:
	protReadHitsPerOrig[inIOReq.origGet()]++;
	readHitsPerOrig[inIOReq.origGet()]++;
	readHits++;
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
	block_t protToProbBlock;

	protCache.blockGetAtHead(protToProbBlock);
	probCache.blockPutAtTail(protToProbBlock);
	protToProbXfersPerOrig[inIOReq.origGet()]++;
      }

      switch (inIOReq.opGet()) {
      case Demote:
	probDemoteHitsPerOrig[inIOReq.origGet()]++;
	demoteHitsPerOrig[inIOReq.origGet()]++;
	demoteHits++;
	break;
      case Read:
	probReadHitsPerOrig[inIOReq.origGet()]++;
	readHitsPerOrig[inIOReq.origGet()]++;
	readHits++;
	break;
      default:
	abort();
      }

      protCache.blockPutAtTail(block);
      probToProtXfersPerOrig[inIOReq.origGet()]++;
    }
    else {

      // Block isn't cached.

      // Eject the front block of the probationary cache if it is full.

      if (probCache.isFull()) {
	block_t ejectBlock;

	probCache.blockGetAtHead(ejectBlock);
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
					 0,
					 inIOReq.objIDGet(),
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
BlockStoreCacheSLRU::statisticsReset()
{
  protDemoteHitsPerOrig.clear();
  protReadHitsPerOrig.clear();

  probDemoteHitsPerOrig.clear();
  probReadHitsPerOrig.clear();

  probToProtXfersPerOrig.clear();
  protToProbXfersPerOrig.clear();

  BlockStoreCache::statisticsReset();
}

void
BlockStoreCacheSLRU::statisticsShow() const
{
  printf("Statistics for BlockStoreCacheSLRU.%s\n", nameGet());

  for (Char::Counter::const_iterator i = probReadHitsPerOrig.begin();
       i != probReadHitsPerOrig.end();
       i++) {
    printf("Block probationary read hits for %s %llu\n", i->first, i->second);
  }
  for (Char::Counter::const_iterator i = protReadHitsPerOrig.begin();
       i != protReadHitsPerOrig.end();
       i++) {
    printf("Block protected read hits for %s %llu\n", i->first, i->second);
  }
  for (Char::Counter::const_iterator i = probToProtXfersPerOrig.begin();
       i != probToProtXfersPerOrig.end();
       i++) {
    printf("Block prob-to-prot transfers for %s %llu\n", i->first, i->second);
  }
  for (Char::Counter::const_iterator i = protToProbXfersPerOrig.begin();
       i != protToProbXfersPerOrig.end();
       i++) {
    printf("Block prot-to-prob transfers for %s %llu\n", i->first, i->second);
  }
}
