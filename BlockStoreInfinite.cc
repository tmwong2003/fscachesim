/*
  RCS:          $Header: /afs/cs.cmu.edu/user/tmwong/pdl-62/Cvs/fscachesim/BlockStoreInfinite.cc,v 1.1 2000/09/22 16:15:39 tmwong Exp $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
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
BlockStoreInfinite::statisticsShow() const
{
  for (uint32MapConstIter i = LRUMap.begin(); i != LRUMap.end(); i++) {
    printf("%d %d\n", i->first, i->second);
  }
  printf("Frequency:\n");
  for (BlockMapConstIter i = freqMap.begin(); i != freqMap.end(); i++) {
    printf("%u,%u %d\n", i->first.objectID, i->first.blockID, i->second);
  }
  printf("Block hits %u\n", blockReadHits);
  printf("Block misses %u\n", blockReadMisses);
}
