/*
  RCS:          $Header: $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#include <stdio.h>

#include "IORequest.hh"

#include "BlockCache.hh"

bool
BlockCache::IORequestHandle(IORequest &inIOReq)
{
  uint32_t offset = inIOReq.offsetGet();
  uint32_t length = inIOReq.lengthGet();

  // Fill out the size of the request to whole blocks. The following
  // expression is length + starting fill + ending fill.

  length = (length + (offset % bytesPerBlock) +
	    ((length + offset) % bytesPerBlock ?
	     bytesPerBlock - ((length + offset) % bytesPerBlock) : 0));

  Block block = {0, inIOReq.objectIDGet(), offset / bytesPerBlock};
  for (uint32_t i = 0; i < length / bytesPerBlock; i++) {
    // See if the block is cached.

    BlockMapIter blockIter = blockTimestampMap.find(block);
    if (blockIter != blockTimestampMap.end()) {
      uint32_t blockTimestamp = blockIter->second;
      uint32_t blockLRUDepth;
      uint32MapIter LRUIter;

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
      // We count uncached sectors as being at the infinite LRU depth.

      LRUDemandMisses++;
    }

    LRUTree = Splay_insert(blockTimestampClock, LRUTree);
    blockTimestampMap[block] = blockTimestampClock;

    // Increment the access count for this sector.

//     freq_iter = freq_map.find(sect);
//     freq_map[sect] = (freq_iter != freq_map.end() ?
// 		      ++(freq_iter->second) :
// 		      1);

    // Increment the sector access clock.

    blockTimestampClock++;

    block.blockID++;
  }

  return (true);
}

void
BlockCache::StatisticsShow()
{
  for (uint32MapIter i = LRUMap.begin(); i != LRUMap.end(); i++) {
    printf("%d %d\n", i->first, i->second);
  }
  printf("Demand misses %d\n", LRUDemandMisses);
}
