/*
  RCS:          $Header: $
  Description:  
  Author:       T.M. Wong <tmwong@cs.cmu.edu>
*/

#ifndef _BLOCKCACHE_HH_
#define _BLOCKCACHE_HH_

#include <map>

extern "C" {
#include "top-down-size-splay.h"
}

class Block
{
public:
  uint16_t devID;
  uint32_t objectID;
  uint32_t blockID;
};

struct BlockLessThan
{
  bool operator()(const Block block1, const Block block2) const
    {
      return (block1.objectID < block2.objectID ||
	      (block1.objectID == block2.objectID &&
	       block1.blockID < block2.blockID));
    }
};

typedef map<Block, uint32_t, BlockLessThan> BlockMap;
typedef map<Block, uint32_t, BlockLessThan>::iterator BlockMapIter;

struct uint32LessThan
{
  bool operator()(const uint32_t i1, const uint32_t i2) const
    {
      return (i1 < i2);
    }
};

typedef map<uint32_t, uint32_t, uint32LessThan> uint32Map;
typedef map<uint32_t, uint32_t, uint32LessThan>::iterator uint32MapIter;

class BlockCache
{
private:
  uint32_t bytesPerBlock;

  BlockMap blockTimestampMap;
  uint32_t blockTimestampClock;

  BlockMap freqMap;

  uint32Map LRUMap;
  uint32_t  LRUDemandMisses;

  Tree *LRUTree;

private:
  // Copy constructors - declared private and never defined

  BlockCache(const BlockCache&);
  BlockCache& operator=(const BlockCache&);

public:
  BlockCache(unsigned long inCacheSize,
	     unsigned int inBytesPerBlock) :
    bytesPerBlock(inBytesPerBlock),
    blockTimestampMap(),
    blockTimestampClock(0),
    freqMap(),
    LRUMap(),
    LRUDemandMisses(0),
    LRUTree(NULL)
    { ; };

  // Process incoming I/O requests

  bool IORequestHandle(IORequest &inIOReq);

  // Output statistics

  void StatisticsShow();
};

#endif /* _BLOCKCACHE_HH_ */
