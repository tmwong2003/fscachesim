/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/Cvs/fscachesim/BlockStoreInfinite.cc,v 1.4 2000/10/26 16:14:24 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong+@cs.cmu.edu>
*/

#include <stdio.h>

#include "IORequest.hh"

#include "BlockStoreInfinite.hh"

bool
BlockStoreInfinite::IORequestDown(const IORequest& inIOReq,
				  list<IORequest>& outIOReqList)
{
  Block block = {0, inIOReq.objectIDGet(), inIOReq.blockOffsetGet(blockSize)};
  uint32_t reqBlockLength = inIOReq.blockLengthGet(blockSize);

  for (uint32_t i = 0; i < reqBlockLength; i++) {
    // See if the block is cached.

    BlockMapIter blockIter = blockTimestampMap.find(block);
    if (blockIter != blockTimestampMap.end()) {
      uint32_t blockTimestamp = blockIter->second;
      uint32_t blockLRUDepth;
      uint32MapIter LRUIter;

      blockReadHits++;

      // Splay to find the LRU depth.

      LRUTree = Splay_splay(blockTimestamp, LRUTree);
      blockLRUDepth = node_size(LRUTree->left);
      LRUTree = Splay_delete(blockTimestamp, LRUTree);

      // node_size() returns the count between the current sector and the
      // LRU one - the opposite of what we want.

      blockLRUDepth = blockTimestampMap.size() - blockLRUDepth;

      // Increment the touch count at this depth.

      LRUIter = LRUMap.find(blockLRUDepth);
      LRUMap[blockLRUDepth] = (LRUIter != LRUMap.end() ?
			       ++(LRUIter->second) :
			       1);
    }
    else {
      blockReadMisses++;
    }

    LRUTree = Splay_insert(blockTimestampClock, LRUTree);
    blockTimestampMap[block] = blockTimestampClock;

    // Increment the access count for this sector.

    blockIter = freqMap.find(block);
    freqMap[block] = (blockIter != freqMap.end() ?
		      ++(blockIter->second) :
		      1);

    // Increment the sector access clock.

    blockTimestampClock++;

    block.blockID++;
  }

  return (true);
}

void
BlockStoreInfinite::statisticsReset()
{
  // A reset for a cache doesn't wipe the infinite cache clean - it just
  // clears the LRU stack depth and freq. statistics.

  LRUMap.clear();
  freqMap.clear();

  // And of course, reset the parent.

  BlockStore::statisticsReset();
}

void
BlockStoreInfinite::statisticsShow() const
{
  printf("Block access frequency:\n");
  statisticsFreqShow();
  printf("LRU stack depth hits:\n");
  statisticsLRUShow();
  statisticsSummaryShow();
}

void
BlockStoreInfinite::statisticsFreqShow() const
{
  for (BlockMapConstIter i = freqMap.begin(); i != freqMap.end(); i++) {
    printf("%u,%u %d\n", i->first.objectID, i->first.blockID, i->second);
  }
  fflush(stdout);
}

void
BlockStoreInfinite::statisticsLRUShow() const
{
  for (uint32MapConstIter i = LRUMap.begin(); i != LRUMap.end(); i++) {
    // Convert the x-axis to kilobytes instead of block size - the
    // latter is not an intuitive measure of size.

    printf("%d %d\n", (i->first * blockSize / (1024 * 1024)), i->second);
  }
  fflush(stdout);
}

void
BlockStoreInfinite::statisticsLRUCumulShow() const
{
  uint32_t cumul = 0;
  uint32_t cumulTotal = blockReadMisses;

  // cumulTotal doesn't start at zero since we need to account for
  // compulsory misses.

  for (uint32MapConstIter i = LRUMap.begin(); i != LRUMap.end(); i++) {
    cumulTotal += i->second;
  }
  for (uint32MapConstIter i = LRUMap.begin(); i != LRUMap.end(); i++) {
    // Convert the x-axis to kilobytes instead of block size - the
    // latter is not an intuitive measure of size.

    cumul += i->second;
    printf("%d %4.3f\n", (i->first * blockSize / (1024 * 1024)), ((double)cumul / cumulTotal));
  }
  fflush(stdout);
}

void
BlockStoreInfinite::statisticsSummaryShow() const
{
  printf("Block hits %u\n", blockReadHits);
  printf("Block misses %u\n", blockReadMisses);
  fflush(stdout);
}
